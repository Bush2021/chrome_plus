#include "hotkey.h"

#include <windows.h>

#include <audiopolicy.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <tlhelp32.h>

#include <algorithm>
#include <cwctype>
#include <iterator>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

#include "config.h"
#include "utils.h"

namespace {

using HotkeyAction = void (*)();

// Static variables for internal use
bool is_hide = false;
std::vector<HWND> hwnd_list;
std::unordered_map<DWORD, bool> original_mute_states;

#define MOD_NOREPEAT 0x4000

UINT ParseHotkeys(std::wstring_view keys) {
  UINT mo = 0;
  UINT vk = 0;
  std::wstring temp(keys);
  std::vector<std::wstring> key_parts = StringSplit(temp, L'+');

  static const std::unordered_map<std::wstring, UINT> key_map = {
      {L"shift", MOD_SHIFT},  {L"ctrl", MOD_CONTROL}, {L"alt", MOD_ALT},
      {L"win", MOD_WIN},      {L"left", VK_LEFT},     {L"right", VK_RIGHT},
      {L"up", VK_UP},         {L"down", VK_DOWN},     {L"←", VK_LEFT},
      {L"→", VK_RIGHT},       {L"↑", VK_UP},          {L"↓", VK_DOWN},
      {L"esc", VK_ESCAPE},    {L"tab", VK_TAB},       {L"backspace", VK_BACK},
      {L"enter", VK_RETURN},  {L"space", VK_SPACE},   {L"prtsc", VK_SNAPSHOT},
      {L"scroll", VK_SCROLL}, {L"pause", VK_PAUSE},   {L"insert", VK_INSERT},
      {L"delete", VK_DELETE}, {L"end", VK_END},       {L"home", VK_HOME},
      {L"pageup", VK_PRIOR},  {L"pagedown", VK_NEXT},
  };

  for (auto& key : key_parts) {
    std::ranges::transform(key, key.begin(), ::towlower);

    if (key_map.contains(key)) {
      if (key == L"shift" || key == L"ctrl" || key == L"alt" || key == L"win") {
        mo |= key_map.at(key);
      } else {
        vk = key_map.at(key);
      }
    } else {
      TCHAR wch = key[0];
      if (key.length() == 1)  // Parse single characters A-Z, 0-9, etc.
      {
        if (isalnum(wch)) {
          vk = toupper(wch);
        } else {
          vk = LOWORD(VkKeyScan(wch));
        }
      } else if (wch == 'F' || wch == 'f')  // Parse the F1-F24 function keys.
      {
        if (isdigit(key[1])) {
          int fx = _wtoi(&key[1]);
          if (fx >= 1 && fx <= 24) {
            vk = VK_F1 + fx - 1;
          }
        }
      }
    }
  }

  mo |= MOD_NOREPEAT;

  return MAKELPARAM(mo, vk);
}

BOOL CALLBACK SearchChromeWindow(HWND hwnd, LPARAM lparam) {
  if (IsWindowVisible(hwnd)) {
    wchar_t buff[256];
    GetClassNameW(hwnd, buff, 255);
    if (wcscmp(buff, L"Chrome_WidgetWin_1") ==
        0)  // || wcscmp(buff, L"Chrome_WidgetWin_2")==0 || wcscmp(buff,
            // L"SysShadow")==0 )
    {
      DWORD pid;
      GetWindowThreadProcessId(hwnd, &pid);
      if (pid == GetCurrentProcessId()) {
        ShowWindow(hwnd, SW_HIDE);
        hwnd_list.emplace_back(hwnd);
      }
    }
  }
  return true;
}

std::vector<DWORD> GetAppPids() {
  std::vector<DWORD> pids;
  wchar_t current_exe_path[MAX_PATH];
  GetModuleFileNameW(nullptr, current_exe_path, MAX_PATH);
  wchar_t* exe_name = wcsrchr(current_exe_path, L'\\');
  if (exe_name) {
    ++exe_name;
  } else {
    exe_name = current_exe_path;
  }

  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snapshot == INVALID_HANDLE_VALUE) {
    return pids;
  }

  PROCESSENTRY32W pe32;
  pe32.dwSize = sizeof(PROCESSENTRY32W);

  if (Process32FirstW(snapshot, &pe32)) {
    do {
      if (_wcsicmp(pe32.szExeFile, exe_name) == 0) {
        pids.emplace_back(pe32.th32ProcessID);
      }
    } while (Process32NextW(snapshot, &pe32));
  }

  CloseHandle(snapshot);
  return pids;
}

void MuteProcess(const std::vector<DWORD>& pids,
                 bool set_mute,
                 bool save_mute_state = false) {
  CoInitialize(nullptr);
  IMMDeviceEnumerator* enumerator = nullptr;
  IMMDevice* device = nullptr;
  IAudioSessionManager2* manager = nullptr;
  IAudioSessionEnumerator* session_enumerator = nullptr;

  CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                   IID_PPV_ARGS(&enumerator));
  enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &device);
  device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr,
                   (void**)&manager);
  manager->GetSessionEnumerator(&session_enumerator);

  int session_count = 0;
  session_enumerator->GetCount(&session_count);
  for (int i = 0; i < session_count; ++i) {
    IAudioSessionControl* session = nullptr;
    session_enumerator->GetSession(i, &session);
    IAudioSessionControl2* session2 = nullptr;
    if (SUCCEEDED(session->QueryInterface(__uuidof(IAudioSessionControl2),
                                          (void**)&session2))) {
      DWORD session_pid = 0;
      session2->GetProcessId(&session_pid);

      for (DWORD pid : pids) {
        if (session_pid == pid) {
          ISimpleAudioVolume* volume = nullptr;
          if (SUCCEEDED(session2->QueryInterface(__uuidof(ISimpleAudioVolume),
                                                 (void**)&volume))) {
            if (save_mute_state) {
              BOOL is_muted;
              volume->GetMute(&is_muted);
              original_mute_states[pid] = (is_muted == TRUE);
            }

            if (set_mute) {
              volume->SetMute(TRUE, nullptr);
            } else {
              // Only unmute if the original state was not muted beforehand
              auto it = original_mute_states.find(pid);
              if (it != original_mute_states.end() && !it->second) {
                volume->SetMute(FALSE, nullptr);
              }
            }
            volume->Release();
          }
          break;
        }
      }
      session2->Release();
    }
    session->Release();
  }

  session_enumerator->Release();
  manager->Release();
  device->Release();
  enumerator->Release();
  CoUninitialize();
}

void HideAndShow() {
  auto chrome_pids = GetAppPids();
  if (!is_hide) {
    original_mute_states.clear();
    EnumWindows(SearchChromeWindow, 0);
    MuteProcess(chrome_pids, true, true);
  } else {
    for (auto r_iter = hwnd_list.rbegin(); r_iter != hwnd_list.rend();
         ++r_iter) {
      ShowWindow(*r_iter, SW_SHOW);
      SetWindowPos(*r_iter, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
      SetForegroundWindow(*r_iter);
      SetWindowPos(*r_iter, HWND_NOTOPMOST, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE);
      SetActiveWindow(*r_iter);
    }
    hwnd_list.clear();
    MuteProcess(chrome_pids, false);
  }
  is_hide = !is_hide;
}

void OnHotkey(HotkeyAction action) {
  action();
}

void Hotkey(std::wstring_view keys, HotkeyAction action) {
  if (keys.empty()) {
    return;
  } else {
    UINT flag = ParseHotkeys(keys.data());

    std::thread th([flag, action]() {
      RegisterHotKey(nullptr, 0, LOWORD(flag), HIWORD(flag));

      MSG msg;
      while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
          OnHotkey(action);
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    });
    th.detach();
  }
}

}  // anonymous namespace

UINT ParseTranslateKey() {
  const auto& translate_key = config.GetTranslateKey();
  if (translate_key.empty()) {
    return 0;
  }
  return ParseHotkeys(translate_key.c_str());
}

void GetHotkey() {
  const auto& boss_key = config.GetBossKey();
  if (!boss_key.empty()) {
    Hotkey(boss_key, HideAndShow);
  }
}
