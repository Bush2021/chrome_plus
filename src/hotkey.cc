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
HWND last_active_hwnd = nullptr;
HWND last_focus_hwnd = nullptr;
HHOOK bosskey_hook = nullptr;
UINT bosskey_activate_msg = 0;
DWORD bosskey_hook_thread_id = 0;
ATOM bosskey_helper_class = 0;

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

void ForceForegroundWindow(HWND hwnd, HWND preferred_focus);
void ApplyFocusInUIThread(HWND hwnd, HWND preferred_focus);

bool EnsureBossKeyHelperClass() {
  if (bosskey_helper_class != 0) {
    return true;
  }
  WNDCLASSEXW wc = {};
  wc.cbSize = sizeof(wc);
  wc.lpfnWndProc = DefWindowProcW;
  wc.hInstance = hInstance;
  wc.lpszClassName = L"ChromePlusBossKeyHelper";
  bosskey_helper_class = RegisterClassExW(&wc);
  if (bosskey_helper_class == 0 &&
      GetLastError() == ERROR_CLASS_ALREADY_EXISTS) {
    bosskey_helper_class = 1;
  }
  return bosskey_helper_class != 0;
}

LRESULT CALLBACK BossKeyMsgProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HC_ACTION) {
    auto* msg = reinterpret_cast<MSG*>(lParam);
    if (msg && msg->message == bosskey_activate_msg) {
      HWND target = reinterpret_cast<HWND>(msg->wParam);
      HWND preferred_focus = reinterpret_cast<HWND>(msg->lParam);
      if (IsWindow(target)) {
        ApplyFocusInUIThread(target, preferred_focus);
      }
      msg->message = WM_NULL;
      msg->wParam = 0;
      msg->lParam = 0;
      return 1;
    }
  }
  return CallNextHookEx(bosskey_hook, nCode, wParam, lParam);
}

void EnsureBossKeyHook(DWORD thread_id) {
  if (bosskey_activate_msg == 0) {
    bosskey_activate_msg =
        RegisterWindowMessageW(L"ChromePlusBossKeyActivate");
  }
  if (thread_id == 0) {
    return;
  }
  if (bosskey_hook && bosskey_hook_thread_id == thread_id) {
    return;
  }
  if (bosskey_hook) {
    UnhookWindowsHookEx(bosskey_hook);
    bosskey_hook = nullptr;
    bosskey_hook_thread_id = 0;
  }
  bosskey_hook_thread_id = thread_id;
  bosskey_hook =
      SetWindowsHookEx(WH_GETMESSAGE, BossKeyMsgProc, hInstance, thread_id);
  if (!bosskey_hook) {
    bosskey_hook_thread_id = 0;
  }
}

bool PostFocusRequest(HWND target, HWND preferred_focus) {
  if (!target) {
    return false;
  }
  DWORD thread_id = GetWindowThreadProcessId(target, nullptr);
  EnsureBossKeyHook(thread_id);
  if (!bosskey_hook || bosskey_hook_thread_id == 0 ||
      bosskey_activate_msg == 0) {
    return false;
  }
  return PostThreadMessageW(bosskey_hook_thread_id, bosskey_activate_msg,
                            reinterpret_cast<WPARAM>(target),
                            reinterpret_cast<LPARAM>(preferred_focus)) != 0;
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

bool HasFocusInWindow(HWND root) {
  return GetThreadFocusWindow(root) != nullptr;
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

void SendActivateMessages(HWND hwnd, HWND focus_target) {
  DWORD_PTR result = 0;
  SendMessageTimeoutW(hwnd, WM_ACTIVATE, WA_ACTIVE, 0, SMTO_ABORTIFHUNG, 80,
                      &result);
  SendMessageTimeoutW(hwnd, WM_ACTIVATEAPP, TRUE, 0, SMTO_ABORTIFHUNG, 80,
                      &result);
  HWND target = focus_target ? focus_target : hwnd;
  SendMessageTimeoutW(target, WM_SETFOCUS, 0, 0, SMTO_ABORTIFHUNG, 80, &result);
}

void SendProbeKey(HWND hwnd) {
  if (!IsWindow(hwnd)) {
    return;
  }
  DWORD_PTR result = 0;
  SendMessageTimeoutW(hwnd, WM_KEYDOWN, VK_SHIFT, 0, SMTO_ABORTIFHUNG, 80,
                      &result);
  SendMessageTimeoutW(hwnd, WM_KEYUP, VK_SHIFT, 0, SMTO_ABORTIFHUNG, 80,
                      &result);
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

void ApplyFocusInUIThread(HWND hwnd, HWND preferred_focus) {
  if (!IsWindow(hwnd)) {
    return;
  }
  HWND focus_target = SelectFocusTarget(hwnd, preferred_focus);
  SetActiveWindow(hwnd);
  SetFocus(focus_target);
  SendActivateMessages(hwnd, focus_target);
  SendProbeKey(focus_target ? focus_target : hwnd);
}

void ActivateByNonClientClick(HWND hwnd) {
  RECT rect = {};
  if (!GetWindowRect(hwnd, &rect)) {
    return;
  }
  int x = rect.left + 20;
  int y = rect.top + 10;
  LPARAM lparam = MAKELPARAM(x, y);
  DWORD_PTR result = 0;
  SendMessageTimeoutW(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, lparam,
                      SMTO_ABORTIFHUNG, 80, &result);
  SendMessageTimeoutW(hwnd, WM_NCLBUTTONUP, HTCAPTION, lparam,
                      SMTO_ABORTIFHUNG, 80, &result);
}

void ActivateByMouseInput(HWND hwnd) {
  RECT rect = {};
  if (!GetWindowRect(hwnd, &rect)) {
    return;
  }
  POINT original = {};
  if (!GetCursorPos(&original)) {
    return;
  }
  int screen_width = GetSystemMetrics(SM_CXSCREEN);
  int screen_height = GetSystemMetrics(SM_CYSCREEN);
  if (screen_width <= 1 || screen_height <= 1) {
    return;
  }

  int x = rect.left + 20;
  int y = rect.top + 10;
  if (x < 0) {
    x = 0;
  } else if (x > screen_width - 1) {
    x = screen_width - 1;
  }
  if (y < 0) {
    y = 0;
  } else if (y > screen_height - 1) {
    y = screen_height - 1;
  }

  auto to_absolute = [](int value, int max_value) -> LONG {
    return static_cast<LONG>((value * 65535) / (max_value - 1));
  };

  INPUT inputs[4] = {};
  inputs[0].type = INPUT_MOUSE;
  inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
  inputs[0].mi.dx = to_absolute(x, screen_width);
  inputs[0].mi.dy = to_absolute(y, screen_height);

  inputs[1].type = INPUT_MOUSE;
  inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

  inputs[2].type = INPUT_MOUSE;
  inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;

  inputs[3].type = INPUT_MOUSE;
  inputs[3].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
  inputs[3].mi.dx = to_absolute(original.x, screen_width);
  inputs[3].mi.dy = to_absolute(original.y, screen_height);

  SendInput(4, inputs, sizeof(INPUT));
}

void ActivateByForegroundHelper(HWND hwnd, HWND focus_target) {
  if (!EnsureBossKeyHelperClass()) {
    return;
  }
  AllowSetForegroundWindow(ASFW_ANY);
  LockSetForegroundWindow(LSFW_UNLOCK);

  HWND helper =
      CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,
                      L"ChromePlusBossKeyHelper", L"", WS_POPUP, 0, 0, 1, 1,
                      nullptr, nullptr, hInstance, nullptr);
  if (!helper) {
    return;
  }
  SetLayeredWindowAttributes(helper, 0, 1, LWA_ALPHA);
  ShowWindow(helper, SW_SHOW);
  SetWindowPos(helper, HWND_TOPMOST, 0, 0, 1, 1,
               SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
  SetForegroundWindow(helper);
  SetActiveWindow(helper);
  SetFocus(helper);
  Sleep(10);

  SetForegroundWindow(hwnd);
  SetActiveWindow(hwnd);
  SetFocus(focus_target);
  SendActivateMessages(hwnd, focus_target);
  SendProbeKey(focus_target ? focus_target : hwnd);

  DestroyWindow(helper);
}

void ActivateByClientClick(HWND hwnd, HWND focus_target) {
  if (!IsWindow(hwnd)) {
    return;
  }
  HWND click_target = focus_target ? focus_target : hwnd;
  RECT rect = {};
  if (!GetClientRect(click_target, &rect)) {
    return;
  }
  POINT pt = {rect.left + 30, rect.top + 40};
  if (pt.x >= rect.right) {
    pt.x = rect.right > 1 ? rect.right - 1 : rect.left;
  }
  if (pt.y >= rect.bottom) {
    pt.y = rect.bottom > 1 ? rect.bottom - 1 : rect.top;
  }
  if (!ClientToScreen(click_target, &pt)) {
    return;
  }

  POINT original = {};
  if (!GetCursorPos(&original)) {
    return;
  }
  int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
  int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
  int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
  if (vw <= 1 || vh <= 1) {
    return;
  }

  auto to_absolute = [&](int value, int origin, int max_value) -> LONG {
    return static_cast<LONG>(((value - origin) * 65535) / (max_value - 1));
  };

  INPUT inputs[4] = {};
  inputs[0].type = INPUT_MOUSE;
  inputs[0].mi.dwFlags =
      MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK;
  inputs[0].mi.dx = to_absolute(pt.x, vx, vw);
  inputs[0].mi.dy = to_absolute(pt.y, vy, vh);

  inputs[1].type = INPUT_MOUSE;
  inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

  inputs[2].type = INPUT_MOUSE;
  inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;

  inputs[3].type = INPUT_MOUSE;
  inputs[3].mi.dwFlags =
      MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK;
  inputs[3].mi.dx = to_absolute(original.x, vx, vw);
  inputs[3].mi.dy = to_absolute(original.y, vy, vh);

  SendInput(4, inputs, sizeof(INPUT));
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

  HWND foreground = GetForegroundWindow();
  DWORD fg_thread = foreground ? GetWindowThreadProcessId(foreground, nullptr)
                               : 0;
  DWORD target_thread = GetWindowThreadProcessId(hwnd, nullptr);
  DWORD current_thread = GetCurrentThreadId();
  bool attached_fg_target = false;
  bool attached_current_target = false;
  bool attached_current_fg = false;

  if (fg_thread && target_thread && fg_thread != target_thread) {
    attached_fg_target = AttachThreadInput(fg_thread, target_thread, TRUE);
  }
  if (target_thread && target_thread != current_thread) {
    attached_current_target =
        AttachThreadInput(current_thread, target_thread, TRUE);
  }
  if (fg_thread && fg_thread != current_thread) {
    attached_current_fg = AttachThreadInput(current_thread, fg_thread, TRUE);
  }

  BringWindowToTop(hwnd);
  SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  AllowSetForegroundWindow(ASFW_ANY);
  LockSetForegroundWindow(LSFW_UNLOCK);
  SetForegroundWindow(hwnd);
  SetActiveWindow(hwnd);
  HWND focus_target = SelectFocusTarget(hwnd, preferred_focus);
  SetFocus(focus_target);
  SendActivateMessages(hwnd, focus_target);

  if (GetForegroundWindow() != hwnd) {
    SwitchToThisWindow(hwnd, TRUE);
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_MENU;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_MENU;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));
    SetForegroundWindow(hwnd);
    SetActiveWindow(hwnd);
    SetFocus(focus_target);
    SendActivateMessages(hwnd, focus_target);
  }
  if (GetForegroundWindow() != hwnd) {
    ActivateByNonClientClick(hwnd);
    SetForegroundWindow(hwnd);
    SetActiveWindow(hwnd);
    SetFocus(focus_target);
    SendActivateMessages(hwnd, focus_target);
  }
  if (GetForegroundWindow() != hwnd) {
    ActivateByMouseInput(hwnd);
    SetForegroundWindow(hwnd);
    SetActiveWindow(hwnd);
    SetFocus(focus_target);
    SendActivateMessages(hwnd, focus_target);
  }
  if (GetForegroundWindow() != hwnd) {
    ActivateByForegroundHelper(hwnd, focus_target);
  }
  SendProbeKey(focus_target ? focus_target : hwnd);
  if (!HasFocusInWindow(hwnd)) {
    ActivateByClientClick(hwnd, focus_target);
  }

  if (attached_current_fg) {
    AttachThreadInput(current_thread, fg_thread, FALSE);
  }
  if (attached_current_target) {
    AttachThreadInput(current_thread, target_thread, FALSE);
  }
  if (attached_fg_target) {
    AttachThreadInput(fg_thread, target_thread, FALSE);
  }
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
    Sleep(10);
    HWND target = IsWindow(last_active_hwnd) ? last_active_hwnd
                                             : (hwnd_list.empty()
                                                    ? nullptr
                                                    : hwnd_list.back());
    if (target) {
      ForceForegroundWindow(target, last_focus_hwnd);
      PostFocusRequest(target, last_focus_hwnd);
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
    EnsureBossKeyHook(GetCurrentThreadId());
    Hotkey(boss_key, HideAndShow);
  }
}
