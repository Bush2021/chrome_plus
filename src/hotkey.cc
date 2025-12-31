#include "hotkey.h"

#include <windows.h>

#include <audiopolicy.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <tlhelp32.h>
#include <wrl/client.h>

#include <algorithm>
#include <cwctype>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

#include "config.h"
#include "utils.h"

namespace {

using Microsoft::WRL::ComPtr;
using HotkeyAction = void (*)();

class ComInitializer {
 public:
  ComInitializer() {
    hr_ = CoInitialize(nullptr);
    initialized_ = SUCCEEDED(hr_) || hr_ == RPC_E_CHANGED_MODE;
    should_uninit_ = (hr_ == S_OK || hr_ == S_FALSE);
  }
  ~ComInitializer() {
    if (should_uninit_) {
      CoUninitialize();
    }
  }
  ComInitializer(const ComInitializer&) = delete;
  ComInitializer& operator=(const ComInitializer&) = delete;

  [[nodiscard]] bool IsInitialized() const { return initialized_; }

 private:
  HRESULT hr_ = E_FAIL;
  bool initialized_ = false;
  bool should_uninit_ = false;
};

// Static variables for internal use
bool is_hide = false;
std::vector<HWND> hwnd_list;
std::unordered_map<std::wstring, bool> original_mute_states;

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

std::optional<std::wstring> GetSessionKey(IAudioSessionControl2* session2) {
  if (!session2) {
    return std::nullopt;
  }
  LPWSTR session_id = nullptr;
  if (SUCCEEDED(session2->GetSessionInstanceIdentifier(&session_id)) &&
      session_id) {
    std::wstring key(session_id);
    CoTaskMemFree(session_id);
    return key;
  }
  return std::nullopt;
}

void ProcessAudioSession(IAudioSessionControl* session,
                         const std::vector<DWORD>& pids,
                         bool set_mute,
                         bool save_mute_state) {
  ComPtr<IAudioSessionControl2> session2;
  if (FAILED(session->QueryInterface(IID_PPV_ARGS(&session2)))) {
    return;
  }

  DWORD session_pid = 0;
  if (FAILED(session2->GetProcessId(&session_pid))) {
    return;
  }

  auto it = std::find(pids.begin(), pids.end(), session_pid);
  if (it == pids.end()) {
    return;
  }

  ComPtr<ISimpleAudioVolume> volume;
  if (FAILED(session2->QueryInterface(IID_PPV_ARGS(&volume)))) {
    return;
  }

  auto session_key = GetSessionKey(session2.Get());

  if (save_mute_state && session_key) {
    BOOL is_muted = FALSE;
    if (SUCCEEDED(volume->GetMute(&is_muted))) {
      original_mute_states[*session_key] = (is_muted == TRUE);
    }
  }

  if (set_mute) {
    volume->SetMute(TRUE, nullptr);
  } else {
    // Unmute logic:
    // - If we have recorded state for this session, respect it
    // - If session is new (not in our records), unmute it
    //   (it was likely created after hide, so it should be unmuted)
    bool should_unmute = true;
    if (session_key) {
      auto state_it = original_mute_states.find(*session_key);
      if (state_it != original_mute_states.end()) {
        should_unmute = !state_it->second;  // unmute only if was not muted
      }
    }
    if (should_unmute) {
      volume->SetMute(FALSE, nullptr);
    }
  }
}

void MuteProcess(const std::vector<DWORD>& pids,
                 bool set_mute,
                 bool save_mute_state = false) {
  ComInitializer com;
  if (!com.IsInitialized()) {
    return;
  }

  ComPtr<IMMDeviceEnumerator> enumerator;
  if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                              IID_PPV_ARGS(&enumerator)))) {
    return;
  }

  ComPtr<IMMDevice> device;
  if (FAILED(
          enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &device))) {
    return;
  }

  ComPtr<IAudioSessionManager2> manager;
  if (FAILED(
          device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr,
                           reinterpret_cast<void**>(manager.GetAddressOf())))) {
    return;
  }

  ComPtr<IAudioSessionEnumerator> session_enumerator;
  if (FAILED(manager->GetSessionEnumerator(&session_enumerator))) {
    return;
  }

  int session_count = 0;
  if (FAILED(session_enumerator->GetCount(&session_count))) {
    return;
  }

  for (int i = 0; i < session_count; ++i) {
    ComPtr<IAudioSessionControl> session;
    if (SUCCEEDED(session_enumerator->GetSession(i, &session)) && session) {
      ProcessAudioSession(session.Get(), pids, set_mute, save_mute_state);
    }
  }
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
    original_mute_states.clear();
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
