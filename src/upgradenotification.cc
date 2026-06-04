#include "upgradenotification.h"

#include <windows.h>

#include <shlwapi.h>

#include <cstring>
#include <mutex>
#include <string>

#include "detours.h"

#include "config.h"
#include "utils.h"

namespace {

static auto RawRegOpenKeyExW = RegOpenKeyExW;
static auto RawRegQueryValueExW = RegQueryValueExW;

// Read the running browser version from the loaded `chrome.dll`'s embedded
// version resource, which is the same `VS_FIXEDFILEINFO` that
// `version_info::GetVersion` uses. The value `InstalledVersionPoller` compares
// the registry `pv` against
// (chrome/browser/upgrade_detector/installed_version_poller.cc).
//
// `FindResource`/`LoadResource` are `kernel32`, so this avoids
// `GetFileVersionInfo*` since chrome_plus ships as `version.dll` and proxies
// the real one.
std::wstring ComputeRunningChromeVersion() {
  HMODULE chrome_dll = GetModuleHandleW(L"chrome.dll");
  if (!chrome_dll) {
    return {};
  }
  HRSRC resource =
      FindResourceW(chrome_dll, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
  if (!resource) {
    return {};
  }
  const DWORD resource_size = SizeofResource(chrome_dll, resource);
  HGLOBAL loaded = LoadResource(chrome_dll, resource);
  const auto* data =
      loaded ? static_cast<const BYTE*>(LockResource(loaded)) : nullptr;
  if (!data || resource_size < sizeof(VS_FIXEDFILEINFO)) {
    return {};
  }

  // `VS_FIXEDFILEINFO` is DWORD-aligned inside the version resource and tagged
  // with a fixed signature; scan for it rather than hardcoding the offset.
  for (DWORD offset = 0; offset + sizeof(VS_FIXEDFILEINFO) <= resource_size;
       offset += sizeof(DWORD)) {
    const auto* info = reinterpret_cast<const VS_FIXEDFILEINFO*>(data + offset);
    if (info->dwSignature != 0xFEEF04BD) {
      continue;
    }
    if (info->dwFileVersionMS == 0 && info->dwFileVersionLS == 0) {
      return {};
    }
    return std::to_wstring(HIWORD(info->dwFileVersionMS)) + L'.' +
           std::to_wstring(LOWORD(info->dwFileVersionMS)) + L'.' +
           std::to_wstring(HIWORD(info->dwFileVersionLS)) + L'.' +
           std::to_wstring(LOWORD(info->dwFileVersionLS));
  }
  return {};
}

// The first `pv` read can arrive before `chrome.dll` is loaded, so keep
// retrying (cheaply) until it resolves rather than caching an empty result.
std::wstring RunningChromeVersion() {
  static std::mutex mutex;
  static std::wstring cached;
  std::lock_guard<std::mutex> lock(mutex);
  if (cached.empty()) {
    cached = ComputeRunningChromeVersion();
  }
  return cached;
}

// `InstallUtil::GetChromeVersion` reads `pv` (REG_SZ) under
// `...\Google\Update\Clients\{GUID}` (chrome/installer/util/install_util.cc).
// When a co-installed or policy-managed Chrome keeps that value ahead of the
// portable binary (or when it is missing), `InstalledVersionPoller` reports
// `kNormalUpdate`, which `upgrade_detector_impl.cc` surfaces as a false
// `UPGRADE_AVAILABLE_REGULAR`, and the browser shows "Chrome is out of date /
// Relaunch" (installed_version_poller.cc; upgrade_detector_impl.cc deliberately
// treats an unreadable installed version as an upgrade too). Relaunch never
// clears it because a portable binary cannot replace itself.
//
// Returning the running version makes the poller compute installed == running
// (`kNone` -> `UPGRADE_AVAILABLE_NONE`), clearing every surface at once. This
// only touches the installed-version comparison, so `OutdatedBuildDetector`
// (the build-age `UPGRADE_NEEDED_OUTDATED_INSTALL` warning) and the
// extension/component updaters are left intact (unlike
// `--disable-background-networking`, which suppresses the prompt by disabling
// all background networking).
LSTATUS APIENTRY MyRegQueryValueExW(HKEY hKey,
                                    LPCWSTR lpValueName,
                                    LPDWORD lpReserved,
                                    LPDWORD lpType,
                                    LPBYTE lpData,
                                    LPDWORD lpcbData) {
  if (lpValueName && lstrcmpiW(lpValueName, L"pv") == 0) {
    const std::wstring version = RunningChromeVersion();
    DebugLog(
        L"SuppressFalseUpgradeNotification: intercepted 'pv' read, "
        L"version='{}'",
        version);
    if (!version.empty()) {
      const DWORD size =
          static_cast<DWORD>((version.size() + 1) * sizeof(wchar_t));
      if (lpType) {
        *lpType = REG_SZ;
      }
      if (!lpData) {
        // Size probe: report the bytes needed, including the null terminator.
        if (lpcbData) {
          *lpcbData = size;
        }
        return ERROR_SUCCESS;
      }
      if (lpcbData) {
        if (*lpcbData < size) {
          *lpcbData = size;
          return ERROR_MORE_DATA;
        }
        std::memcpy(lpData, version.c_str(), size);
        *lpcbData = size;
        return ERROR_SUCCESS;
      }
      // A data buffer with no size pointer cannot be satisfied; fall through.
    }
  }
  return RawRegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData,
                             lpcbData);
}

// Make Chrome's installed-version probe succeed even when Google Update was
// never installed. `InstallUtil::GetChromeVersion` opens
// `...\Google\Update\Clients\{GUID}` before reading `pv`; on a portable install
// that key is absent, the open fails, the `pv` read never happens, and Chrome
// treats the unreadable version as an available upgrade. Hence, the false
// prompt persists with `MyRegQueryValueExW` never getting a chance to answer.
// When that specific open fails, hand back a stand-in handle (a duplicate of
// the requested root) so the `pv` read proceeds and is answered with the
// running version. Matching with a trailing separator keeps this off the
// sibling `ClientState` key.
LSTATUS APIENTRY MyRegOpenKeyExW(HKEY hKey,
                                 LPCWSTR lpSubKey,
                                 DWORD ulOptions,
                                 REGSAM samDesired,
                                 PHKEY phkResult) {
  const LSTATUS result =
      RawRegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
  if (result != ERROR_SUCCESS && phkResult && lpSubKey &&
      StrStrIW(lpSubKey, L"Google\\Update\\Clients\\")) {
    if (RawRegOpenKeyExW(hKey, L"", 0, samDesired, phkResult) ==
        ERROR_SUCCESS) {
      DebugLog(
          L"SuppressFalseUpgradeNotification: Clients key absent, substituting "
          L"stand-in handle");
      return ERROR_SUCCESS;
    }
  }
  return result;
}

}  // namespace

void SuppressFalseUpgradeNotification() {
  if (!config.IsSuppressFalseUpgradeNotification()) {
    return;
  }

  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach(reinterpret_cast<LPVOID*>(&RawRegOpenKeyExW),
               reinterpret_cast<void*>(MyRegOpenKeyExW));
  DetourAttach(reinterpret_cast<LPVOID*>(&RawRegQueryValueExW),
               reinterpret_cast<void*>(MyRegQueryValueExW));
  auto status = DetourTransactionCommit();
  if (status != NO_ERROR) {
    DebugLog(L"SuppressFalseUpgradeNotification hooks failed: {}", status);
  } else {
    DebugLog(L"SuppressFalseUpgradeNotification: hooks installed");
  }
}
