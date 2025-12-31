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

void GetHotkey() {
  const auto& boss_key = config.GetBossKey();
  if (!boss_key.empty()) {
    Hotkey(boss_key, HideAndShow);
  }
}
