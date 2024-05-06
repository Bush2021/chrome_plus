#include <windows.h>
#include <stdio.h>
#include <psapi.h>

HMODULE hInstance;

#define MAGIC_CODE 0x1603ABD9

#include "MinHook.h"
#include "version.h"

#include "hijack.h"
#include "utils.h"
#include "patch.h"
#include "config.h"
#include "tabbookmark.h"
#include "hotkey.h"
#include "portable.h"
#include "pakpatch.h"
#include "appid.h"
#include "green.h"

typedef int (*Startup)();
Startup ExeMain = NULL;

void ChromePlus() {
  // Shortcut.
  SetAppId();

  // Portable hajack patch.
  MakeGreen();

  // Enhancement of the address bar, tab, and bookmark.
  TabBookmark();

  // Patch the pak file.
  PakPatch();

  // Process the hotkey.
  GetHotkey();
}

void ChromePlusCommand(LPWSTR param) {
  if (!wcsstr(param, L"--portable")) {
    Portable(param);
  } else {
    ChromePlus();
  }
}

int Loader() {
  // Hard patch.
  MakePatch();

  // Only main interface.
  LPWSTR param = GetCommandLineW();
  // DebugLog(L"param %s", param);
  if (!wcsstr(param, L"-type=")) {
    ChromePlusCommand(param);
  }

  // Return to the main function.
  return ExeMain();
}

void InstallLoader() {
  // Get the address of the original entry point of the main module.
  MODULEINFO mi;
  GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &mi,
                       sizeof(MODULEINFO));
  PBYTE entry = (PBYTE)mi.EntryPoint;

  // Jump from the original entry to the loader.
  MH_STATUS status = MH_CreateHook(entry, Loader, (LPVOID*)&ExeMain);
  if (status == MH_OK) {
    MH_EnableHook(entry);
  } else {
    DebugLog(L"MH_CreateHook InstallLoader failed:%d", status);
  }
}
#define EXTERNC extern "C"

//
EXTERNC __declspec(dllexport) void portable() {}

EXTERNC BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID pv) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(hModule);
    hInstance = hModule;

    // Maintain the original function of system DLLs.
    LoadSysDll(hModule);

    // Install the loader after successfully initializing MinHook.
    MH_STATUS status = MH_Initialize();
    if (status == MH_OK) {
      InstallLoader();
    } else {
      DebugLog(L"MH_Initialize failed:%d", status);
    }
  }
  return TRUE;
}
