#include <windows.h>

#include <psapi.h>

#include <atomic>
#include <string>
#include <thread>

#include "detours.h"

#include "appid.h"
#include "config.h"
#include "green.h"
#include "hijack.h"
#include "hotkey.h"
#include "inputhook.h"
#include "keymapping.h"
#include "pakpatch.h"
#include "policies.h"
#include "portable.h"
#include "tabbookmark.h"
#include "upgradenotification.h"
#include "utils.h"
#include "version.h"

using Startup = int (*)();
constexpr DWORD kOwnershipPollMs = 50;
static std::atomic<bool> should_run_exit_cmd = false;
Startup ExeMain = nullptr;

void ChromePlus() {
  // Shortcut.
  SetAppId();

  // Portable hijack patch.
  MakeGreen();

  // Ignore enterprise policies.
  IgnorePolicies();

  // Initialize key mapping and translate key
  KeyMapping();

  // Enhancement of the address bar, tab, and bookmark.
  TabBookmark();

  // Install input hooks (must be called after all handlers are registered).
  InstallInputHooks();

  // Patch the pak file.
  PakPatch();

  // Suppress Chrome's false "out of date" upgrade notification.
  SuppressFalseUpgradeNotification();

  // Process the hotkey.
  GetHotkey();
}

// Returns the ID of the user data directory this process owns as a browser,
// empty when it doesn't own one yet. `ProcessSingleton::Create` in
// chrome/browser/process_singleton_win.cc titles a message window with the
// directory. `base/win/message_window.cc` names the class, which other windows
// of the process share, but their titles aren't directories.
std::wstring FindOwnedUserDataDirId() {
  constexpr wchar_t kWindowClass[] = L"Chrome_MessageWindow";
  for (HWND window =
           ::FindWindowExW(HWND_MESSAGE, nullptr, kWindowClass, nullptr);
       window;
       window = ::FindWindowExW(HWND_MESSAGE, window, kWindowClass, nullptr)) {
    DWORD process_id = 0;
    ::GetWindowThreadProcessId(window, &process_id);
    if (process_id != ::GetCurrentProcessId()) {
      continue;
    }
    wchar_t title[MAX_PATH] = {};
    ::GetWindowTextW(window, title, MAX_PATH);
    std::wstring directory_id = GetDirectoryId(title);
    if (!directory_id.empty()) {
      return directory_id;
    }
  }
  return L"";
}

// Every `--portable` process gets here, including one that only hands its
// command line to the browser on the same user data directory. The startup and
// exit commands belong to the browser, and a process that hands off exits
// without owning the directory, so this waits on a thread for that ownership.
// `ShutdownPostThreadsStop` in chrome/browser/lifetime/browser_shutdown.cc
// relaunches the browser while the old one is still exiting, so ownership also
// isn't enough: the old browser may not have finished its exit commands. The
// mutex orders the two. Its name comes from the directory's ID because Chrome
// locks a file inside the directory, which makes every spelling of its path one
// browser.
void RunStartupCommands() {
  std::thread([] {
    std::wstring directory_id = FindOwnedUserDataDirId();
    while (directory_id.empty()) {
      ::Sleep(kOwnershipPollMs);
      directory_id = FindOwnedUserDataDirId();
    }
    const std::wstring mutex_name = L"Local\\chrome++-launch-" + directory_id;
    const HANDLE mutex = ::CreateMutexW(nullptr, FALSE, mutex_name.c_str());
    if (!mutex || ::WaitForSingleObject(mutex, INFINITE) == WAIT_FAILED) {
      return;
    }
    should_run_exit_cmd = true;
    LaunchCommands(config.GetLaunchOnStartup());
    // A mutex goes with the thread that owns it, so this thread has to outlive
    // the browser. Its end abandons the mutex, and the next browser's wait
    // returns WAIT_ABANDONED, which here means only that the holder is gone.
    // https://learn.microsoft.com/en-us/windows/win32/sync/mutex-objects
    ::Sleep(INFINITE);
  }).detach();
}

void ChromePlusCommand(LPWSTR param) {
  if (!wcsstr(param, L"--portable")) {
    Portable(param);
  } else {
    ChromePlus();
    RunStartupCommands();
  }
}

int Loader() {
  // Only main interface.
  LPWSTR param = GetCommandLineW();
  if (!wcsstr(param, L"-type=")) {
    ChromePlusCommand(param);
  } else if (wcsstr(param, L"--type=renderer")) {
    // With in-process WebUI resource loading on (V1, crrev.com/c/5868139,
    // crbug.com/362511750), the browser fills `LocalResourceLoaderConfig`
    // (content/browser/webui/web_ui_impl.cc) and the renderer materializes
    // WebUI from its own inherited-handle `resources.pak`
    // (ui/base/resource/data_pack.cc), not over IPC. So the renderer must
    // patch that mapping itself to keep the injection from #172. We stopped
    // forcing V1 off because the field trial enabling
    // `InitialWebUISyncNavStartToCommit` (crrev.com/c/7778247) then made the
    // renderer CHECK that config (content/renderer/render_frame_impl.cc) and
    // crash (#263). Other sub-process types never serve WebUI, so skip them.
    PakPatch();
  }

  // Return to the main function.
  return ExeMain();
}

void InstallLoader() {
  // Get the address of the original entry point of the main module.
  MODULEINFO mi;
  GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &mi,
                       sizeof(MODULEINFO));
  ExeMain = reinterpret_cast<Startup>(mi.EntryPoint);

  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach(&ExeMain, Loader);
  auto status = DetourTransactionCommit();
  if (status != NO_ERROR) {
    DebugLog(L"InstallLoader failed: {}", status);
  }
}

__declspec(dllexport) void portable() {}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID pv) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(hModule);
    hInstance = hModule;

    // Maintain the original function of system DLLs.
    LoadSysDll(hModule);

    InstallLoader();
  } else if (dwReason == DLL_PROCESS_DETACH && ::should_run_exit_cmd) {
    LaunchCommands(config.GetLaunchOnExit());
    should_run_exit_cmd = false;
  }
  return TRUE;
}
