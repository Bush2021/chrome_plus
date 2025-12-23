#include "hotkey.h"

#include <windows.h>

#include <audiopolicy.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <tlhelp32.h>

#include <string>
#include <string_view>
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
HWND last_active_hwnd = nullptr;
HWND last_focus_hwnd = nullptr;
HWND bosskey_hwnd = nullptr;
ATOM bosskey_window_class = 0;
HotkeyAction bosskey_action = nullptr;
constexpr UINT kBossKeyHotkeyId = 1;

bool IsChromeWindow(HWND hwnd) {
  if (!hwnd) {
    return false;
  }
  wchar_t buff[256];
  GetClassNameW(hwnd, buff, 255);
  if (wcscmp(buff, L"Chrome_WidgetWin_1") != 0) {
    return false;
  }
  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  return pid == GetCurrentProcessId();
}

bool EnsureBossKeyWindow() {
  if (bosskey_hwnd) {
    return true;
  }
  if (bosskey_window_class == 0) {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wParam,
                        LPARAM lParam) -> LRESULT {
      if (msg == WM_HOTKEY && wParam == kBossKeyHotkeyId) {
        if (bosskey_action) {
          bosskey_action();
        }
        return 0;
      }
      return DefWindowProcW(hwnd, msg, wParam, lParam);
    };
    wc.hInstance = hInstance;
    wc.lpszClassName = L"ChromePlusBossKeyWindow";
    bosskey_window_class = RegisterClassExW(&wc);
    if (bosskey_window_class == 0) {
      if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        return false;
      }
      bosskey_window_class = 1;
    }
  }
  bosskey_hwnd = CreateWindowExW(0, L"ChromePlusBossKeyWindow", L"", 0, 0, 0, 0,
                                 0, HWND_MESSAGE, nullptr, hInstance, nullptr);
  return bosskey_hwnd != nullptr;
}

bool IsSameRootWindow(HWND child, HWND root) {
  if (!child || !root) {
    return false;
  }
  return GetAncestor(child, GA_ROOT) == GetAncestor(root, GA_ROOT);
}

HWND GetThreadFocusWindow(HWND root) {
  if (!IsWindow(root)) {
    return nullptr;
  }
  DWORD thread_id = GetWindowThreadProcessId(root, nullptr);
  GUITHREADINFO info = {};
  info.cbSize = sizeof(info);
  if (!GetGUIThreadInfo(thread_id, &info)) {
    return nullptr;
  }
  HWND focus = info.hwndFocus ? info.hwndFocus : info.hwndActive;
  if (!IsWindow(focus)) {
    return nullptr;
  }
  if (!IsSameRootWindow(focus, root)) {
    return nullptr;
  }
  if (!IsWindowVisible(focus) || !IsWindowEnabled(focus)) {
    return nullptr;
  }
  return focus;
}

HWND FindFocusableChromeChild(HWND parent) {
  struct EnumState {
    HWND best = nullptr;
  };
  EnumState state;
  EnumChildWindows(
      parent,
      [](HWND hwnd, LPARAM lparam) -> BOOL {
        auto* state = reinterpret_cast<EnumState*>(lparam);
        if (!IsWindowVisible(hwnd) || !IsWindowEnabled(hwnd)) {
          return true;
        }
        wchar_t cls[256];
        GetClassNameW(hwnd, cls, 255);
        if (wcscmp(cls, L"Chrome_RenderWidgetHostHWND") == 0 ||
            wcscmp(cls, L"Chrome_WidgetWin_0") == 0) {
          state->best = hwnd;
          return false;
        }
        if (!state->best) {
          state->best = hwnd;
        }
        return true;
      },
      reinterpret_cast<LPARAM>(&state));
  return state.best;
}

HWND SelectFocusTarget(HWND root, HWND preferred_focus) {
  if (preferred_focus && IsWindow(preferred_focus) &&
      IsSameRootWindow(preferred_focus, root) &&
      IsWindowVisible(preferred_focus) && IsWindowEnabled(preferred_focus)) {
    return preferred_focus;
  }
  HWND candidate = FindFocusableChromeChild(root);
  return candidate ? candidate : root;
}

void ForceForegroundWindow(HWND hwnd, HWND preferred_focus) {
  if (!IsWindow(hwnd)) {
    return;
  }
  if (IsIconic(hwnd)) {
    ShowWindow(hwnd, SW_RESTORE);
  } else {
    ShowWindow(hwnd, SW_SHOW);
  }

  SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  SetForegroundWindow(hwnd);
  SetActiveWindow(hwnd);
  HWND focus_target = SelectFocusTarget(hwnd, preferred_focus);
  SetFocus(focus_target);
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
    HWND foreground = GetForegroundWindow();
    last_active_hwnd = IsChromeWindow(foreground) ? foreground : nullptr;
    last_focus_hwnd =
        last_active_hwnd ? GetThreadFocusWindow(last_active_hwnd) : nullptr;
    original_mute_states.clear();
    EnumWindows(SearchChromeWindow, 0);
    MuteProcess(chrome_pids, true, true);
  } else {
    for (auto r_iter = hwnd_list.rbegin(); r_iter != hwnd_list.rend();
         ++r_iter) {
      ShowWindow(*r_iter, SW_SHOW);
      SetWindowPos(*r_iter, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
      SetWindowPos(*r_iter, HWND_NOTOPMOST, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE);
    }
    HWND target = IsWindow(last_active_hwnd) ? last_active_hwnd
                                             : (hwnd_list.empty()
                                                    ? nullptr
                                                    : hwnd_list.back());
    if (target) {
      ForceForegroundWindow(target, last_focus_hwnd);
    }
    hwnd_list.clear();
    MuteProcess(chrome_pids, false);
  }
  is_hide = !is_hide;
}

void Hotkey(std::wstring_view keys, HotkeyAction action) {
  if (keys.empty()) {
    return;
  }
  if (!EnsureBossKeyWindow()) {
    return;
  }
  UINT flag = ParseHotkeys(keys.data());
  bosskey_action = action;
  UnregisterHotKey(bosskey_hwnd, kBossKeyHotkeyId);
  RegisterHotKey(bosskey_hwnd, kBossKeyHotkeyId, LOWORD(flag), HIWORD(flag));
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
