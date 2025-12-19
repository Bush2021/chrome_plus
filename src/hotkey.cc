#include "hotkey.h"

#include <windows.h>

#include <audiopolicy.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <tlhelp32.h>

#include <algorithm>
#include <cwctype>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "config.h"
#include "utils.h"

namespace {

using HotkeyAction = void (*)();

// Static variables for internal use
bool is_hide = false;
std::vector<HWND> hwnd_list;
std::unordered_map<std::wstring, bool> original_mute_states;
bool saved_any_session = false;
bool had_unmuted_session = false;
// Retry unmute briefly to catch late audio session creation.
constexpr UINT_PTR kUnmuteRetryTimerId = 1;
constexpr UINT kUnmuteRetryDelayMs = 200;
constexpr int kUnmuteRetryMax = 5;
int unmute_retry_left = 0;
bool unmute_watch_active = false;
bool unmute_watch_com_initialized = false;
bool unmute_watch_com_should_uninit = false;
IAudioSessionNotification* unmute_watch_notification = nullptr;
std::vector<IAudioSessionManager2*> unmute_watch_managers;

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

std::optional<std::wstring> GetSessionKey(IAudioSessionControl2* session2) {
  LPWSTR session_key = nullptr;
  if (SUCCEEDED(session2->GetSessionInstanceIdentifier(&session_key)) &&
      session_key) {
    std::wstring key(session_key);
    CoTaskMemFree(session_key);
    return key;
  }
  return std::nullopt;
}

bool ShouldUnmuteUnknownSession() {
  if (!saved_any_session) {
    return true;
  }
  return had_unmuted_session;
}

void ResetMuteStateTracking() {
  original_mute_states.clear();
  saved_any_session = false;
  had_unmuted_session = false;
}

bool IsUnmuteTrackingActive() {
  return unmute_retry_left > 0 || unmute_watch_active;
}

void ClearMuteStatesIfIdle() {
  if (!IsUnmuteTrackingActive()) {
    ResetMuteStateTracking();
  }
}

void AddAudioDevice(std::vector<IMMDevice*>& devices,
                    std::unordered_set<std::wstring>& seen_ids,
                    IMMDevice* device) {
  if (!device) {
    return;
  }
  LPWSTR device_id = nullptr;
  if (SUCCEEDED(device->GetId(&device_id)) && device_id) {
    if (seen_ids.insert(device_id).second) {
      devices.emplace_back(device);
    } else {
      device->Release();
    }
    CoTaskMemFree(device_id);
  } else {
    device->Release();
  }
}

std::vector<IMMDevice*> CollectAudioDevices(IMMDeviceEnumerator* enumerator) {
  std::vector<IMMDevice*> devices;
  if (!enumerator) {
    return devices;
  }
  std::unordered_set<std::wstring> seen_ids;
  constexpr ERole kRoles[] = {eConsole, eMultimedia, eCommunications};
  for (auto role : kRoles) {
    IMMDevice* device = nullptr;
    if (SUCCEEDED(
            enumerator->GetDefaultAudioEndpoint(eRender, role, &device))) {
      AddAudioDevice(devices, seen_ids, device);
    }
  }

  IMMDeviceCollection* collection = nullptr;
  if (SUCCEEDED(enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE,
                                               &collection)) &&
      collection) {
    UINT count = 0;
    if (SUCCEEDED(collection->GetCount(&count))) {
      for (UINT i = 0; i < count; ++i) {
        IMMDevice* device = nullptr;
        if (SUCCEEDED(collection->GetItem(i, &device))) {
          AddAudioDevice(devices, seen_ids, device);
        }
      }
    }
    collection->Release();
  }

  return devices;
}

void ProcessSessions(IAudioSessionManager2* manager,
                     const std::vector<DWORD>& pids,
                     bool set_mute,
                     bool save_mute_state) {
  if (!manager) {
    return;
  }
  IAudioSessionEnumerator* session_enumerator = nullptr;
  HRESULT hr = manager->GetSessionEnumerator(&session_enumerator);
  if (FAILED(hr) || !session_enumerator) {
    return;
  }

  int session_count = 0;
  session_enumerator->GetCount(&session_count);
  for (int i = 0; i < session_count; ++i) {
    IAudioSessionControl* session = nullptr;
    hr = session_enumerator->GetSession(i, &session);
    if (FAILED(hr) || !session) {
      continue;
    }
    IAudioSessionControl2* session2 = nullptr;
    hr = session->QueryInterface(__uuidof(IAudioSessionControl2),
                                 (void**)&session2);
    if (SUCCEEDED(hr) && session2) {
      DWORD session_pid = 0;
      if (FAILED(session2->GetProcessId(&session_pid))) {
        session2->Release();
        session->Release();
        continue;
      }

      for (DWORD pid : pids) {
        if (session_pid == pid) {
          auto session_key = GetSessionKey(session2);
          ISimpleAudioVolume* volume = nullptr;
          if (SUCCEEDED(session2->QueryInterface(__uuidof(ISimpleAudioVolume),
                                                 (void**)&volume))) {
            if (save_mute_state) {
              BOOL is_muted = FALSE;
              if (SUCCEEDED(volume->GetMute(&is_muted))) {
                saved_any_session = true;
                if (is_muted == FALSE) {
                  had_unmuted_session = true;
                }
                if (session_key) {
                  original_mute_states[*session_key] = (is_muted == TRUE);
                }
              }
            }

            if (set_mute) {
              volume->SetMute(TRUE, nullptr);
            } else {
              // Only unmute sessions we muted before. If the session key is not
              // recorded (e.g. session recreated), unmute to avoid stuck
              // system-level mute.
              bool should_unmute = true;
              if (session_key) {
                auto it = original_mute_states.find(*session_key);
                if (it != original_mute_states.end()) {
                  should_unmute = !it->second;
                }
              } else {
                should_unmute = ShouldUnmuteUnknownSession();
              }
              if (should_unmute) {
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
}

void MuteProcess(const std::vector<DWORD>& pids,
                 bool set_mute,
                 bool save_mute_state = false,
                 bool clear_state = true) {
  HRESULT hr = CoInitialize(nullptr);
  const bool should_uninit = (hr == S_OK || hr == S_FALSE);
  if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
    return;
  }
  IMMDeviceEnumerator* enumerator = nullptr;

  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                        IID_PPV_ARGS(&enumerator));
  if (FAILED(hr) || !enumerator) {
    goto Cleanup;
  }
  {
    auto devices = CollectAudioDevices(enumerator);
    for (auto* device_item : devices) {
      IAudioSessionManager2* manager = nullptr;
      hr = device_item->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL,
                                 nullptr, (void**)&manager);
      if (SUCCEEDED(hr) && manager) {
        ProcessSessions(manager, pids, set_mute, save_mute_state);
        manager->Release();
      }
      device_item->Release();
    }
  }

Cleanup:
  if (enumerator) {
    enumerator->Release();
  }
  if (should_uninit) {
    CoUninitialize();
  }

  if (!set_mute && clear_state) {
    ClearMuteStatesIfIdle();
  }
}

void StopUnmuteRetries(bool clear_state) {
  if (unmute_retry_left > 0) {
    KillTimer(nullptr, kUnmuteRetryTimerId);
  }
  unmute_retry_left = 0;
  if (clear_state) {
    ClearMuteStatesIfIdle();
  }
}

void StartUnmuteRetries() {
  StopUnmuteRetries(false);
  unmute_retry_left = kUnmuteRetryMax;
  if (unmute_retry_left <= 0) {
    return;
  }
  if (SetTimer(nullptr, kUnmuteRetryTimerId, kUnmuteRetryDelayMs, nullptr) ==
      0) {
    StopUnmuteRetries(true);
  }
}

bool EnsureUnmuteWatchComInitialized() {
  if (unmute_watch_com_initialized) {
    return true;
  }
  HRESULT hr = CoInitialize(nullptr);
  if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
    return false;
  }
  unmute_watch_com_initialized = true;
  unmute_watch_com_should_uninit = (hr == S_OK || hr == S_FALSE);
  return true;
}

class SessionNotification final : public IAudioSessionNotification {
 public:
  SessionNotification() : ref_count_(1) {}

  ULONG STDMETHODCALLTYPE AddRef() override {
    return static_cast<ULONG>(InterlockedIncrement(&ref_count_));
  }

  ULONG STDMETHODCALLTYPE Release() override {
    ULONG ref_count =
        static_cast<ULONG>(InterlockedDecrement(&ref_count_));
    if (ref_count == 0) {
      delete this;
    }
    return ref_count;
  }

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                           void** ppv_object) override {
    if (!ppv_object) {
      return E_POINTER;
    }
    if (riid == __uuidof(IUnknown) ||
        riid == __uuidof(IAudioSessionNotification)) {
      *ppv_object = static_cast<IAudioSessionNotification*>(this);
      AddRef();
      return S_OK;
    }
    *ppv_object = nullptr;
    return E_NOINTERFACE;
  }

  HRESULT STDMETHODCALLTYPE OnSessionCreated(
      IAudioSessionControl* new_session) override {
    if (!new_session || !unmute_watch_active || is_hide) {
      return S_OK;
    }
    IAudioSessionControl2* session2 = nullptr;
    HRESULT hr = new_session->QueryInterface(__uuidof(IAudioSessionControl2),
                                             (void**)&session2);
    if (FAILED(hr) || !session2) {
      return S_OK;
    }
    DWORD session_pid = 0;
    if (FAILED(session2->GetProcessId(&session_pid))) {
      session2->Release();
      return S_OK;
    }
    auto pids = GetAppPids();
    if (std::find(pids.begin(), pids.end(), session_pid) == pids.end()) {
      session2->Release();
      return S_OK;
    }

    auto session_key = GetSessionKey(session2);
    ISimpleAudioVolume* volume = nullptr;
    hr = session2->QueryInterface(__uuidof(ISimpleAudioVolume),
                                  (void**)&volume);
    if (SUCCEEDED(hr) && volume) {
      bool should_unmute = true;
      if (session_key) {
        auto it = original_mute_states.find(*session_key);
        if (it != original_mute_states.end()) {
          should_unmute = !it->second;
        }
      } else {
        should_unmute = ShouldUnmuteUnknownSession();
      }
      if (should_unmute) {
        volume->SetMute(FALSE, nullptr);
      }
      volume->Release();
    }
    session2->Release();
    return S_OK;
  }

 private:
  LONG ref_count_;
};

void UnregisterUnmuteWatch(bool clear_state) {
  for (auto* manager : unmute_watch_managers) {
    if (manager && unmute_watch_notification) {
      manager->UnregisterSessionNotification(unmute_watch_notification);
    }
    if (manager) {
      manager->Release();
    }
  }
  unmute_watch_managers.clear();
  if (unmute_watch_notification) {
    unmute_watch_notification->Release();
    unmute_watch_notification = nullptr;
  }
  if (unmute_watch_com_initialized && unmute_watch_com_should_uninit) {
    CoUninitialize();
  }
  unmute_watch_com_initialized = false;
  unmute_watch_com_should_uninit = false;
  unmute_watch_active = false;
  if (clear_state) {
    ClearMuteStatesIfIdle();
  }
}

void RegisterUnmuteWatch() {
  if (unmute_watch_active) {
    return;
  }
  if (!EnsureUnmuteWatchComInitialized()) {
    return;
  }
  unmute_watch_notification = new SessionNotification();

  IMMDeviceEnumerator* enumerator = nullptr;
  HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&enumerator));
  if (SUCCEEDED(hr) && enumerator) {
    auto devices = CollectAudioDevices(enumerator);
    for (auto* device_item : devices) {
      IAudioSessionManager2* manager = nullptr;
      hr = device_item->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL,
                                 nullptr, (void**)&manager);
      if (SUCCEEDED(hr) && manager) {
        if (SUCCEEDED(
                manager->RegisterSessionNotification(
                    unmute_watch_notification))) {
          unmute_watch_managers.emplace_back(manager);
        } else {
          manager->Release();
        }
      }
      device_item->Release();
    }
    enumerator->Release();
  }
  if (unmute_watch_managers.empty()) {
    if (unmute_watch_notification) {
      unmute_watch_notification->Release();
      unmute_watch_notification = nullptr;
    }
  }
  unmute_watch_active = true;
}

void StartUnmuteWatch() {
  UnregisterUnmuteWatch(false);
  RegisterUnmuteWatch();
}

void HandleUnmuteRetryTimer() {
  if (is_hide) {
    StopUnmuteRetries(true);
    return;
  }
  auto chrome_pids = GetAppPids();
  MuteProcess(chrome_pids, false, false, false);
  --unmute_retry_left;
  if (unmute_retry_left <= 0) {
    StopUnmuteRetries(true);
  }
}

void HideAndShow() {
  auto chrome_pids = GetAppPids();
  if (!is_hide) {
    StopUnmuteRetries(false);
    UnregisterUnmuteWatch(false);
    ResetMuteStateTracking();
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
    MuteProcess(chrome_pids, false, false, false);
    StartUnmuteRetries();
    StartUnmuteWatch();
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
        if (msg.message == WM_TIMER) {
          if (msg.wParam == kUnmuteRetryTimerId) {
            HandleUnmuteRetryTimer();
            continue;
          }
        }
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
