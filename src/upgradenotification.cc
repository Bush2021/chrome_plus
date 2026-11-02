#include "upgradenotification.h"

#include <windows.h>

#include <shlwapi.h>

#include <atomic>
#include <cstring>
#include <mutex>
#include <string>
#include <string_view>

#include "detours.h"

#include "config.h"
#include "update.h"
#include "utils.h"

namespace {

auto RawRegOpenKeyExW = RegOpenKeyExW;
auto RawRegQueryValueExW = RegQueryValueExW;
auto RawRegCloseKey = RegCloseKey;

std::atomic<HKEY> rename_command_key{nullptr};

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

LSTATUS AnswerString(const std::wstring& value,
                     LPDWORD type,
                     LPBYTE data,
                     LPDWORD data_size) {
  const DWORD size = static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t));
  if (type) {
    *type = REG_SZ;
  }
  if (!data) {
    // Size probe: report the bytes needed, including the null terminator.
    if (data_size) {
      *data_size = size;
    }
    return ERROR_SUCCESS;
  }
  if (!data_size) {
    return ERROR_INVALID_PARAMETER;
  }
  if (*data_size < size) {
    *data_size = size;
    return ERROR_MORE_DATA;
  }
  std::memcpy(data, value.c_str(), size);
  *data_size = size;
  return ERROR_SUCCESS;
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
//
// With managed updates enabled, report a staged build instead so Chrome offers
// "Relaunch to update" for it, regardless of the suppression setting.
LSTATUS APIENTRY MyRegQueryValueExW(HKEY hKey,
                                    LPCWSTR lpValueName,
                                    LPDWORD lpReserved,
                                    LPDWORD lpType,
                                    LPBYTE lpData,
                                    LPDWORD lpcbData) {
  if (hKey && hKey == rename_command_key.load() && lpValueName &&
      lstrcmpiW(lpValueName, L"CommandLine") == 0) {
    return AnswerString(FinishUpdateCommandLine(), lpType, lpData, lpcbData);
  }
  if (lpValueName && lstrcmpiW(lpValueName, L"pv") == 0) {
    std::wstring version;
    if (config.IsManagedUpdates()) {
      version = StagedChromeVersion().value_or(L"");
    }
    if (version.empty()) {
      version = RunningChromeVersion();
    }
    DebugLog(L"UpgradeNotification: intercepted 'pv' read, version='{}'",
             version);
    if (!version.empty()) {
      return AnswerString(version, lpType, lpData, lpcbData);
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
// the requested root) so the `pv` read proceeds and `MyRegQueryValueExW`
// answers it. Matching with a trailing separator keeps this off the sibling
// `ClientState` key.
//
// On restart, a per-user Chrome swaps in a staged build by running the
// `CommandLine` under `...\Google\Update\Clients\{GUID}\Commands\cmd`
// (`SwapNewChromeExeIfPresent` in chrome/browser/first_run/upgrade_util_win.cc,
// `AppCommand::Initialize` in chrome/installer/util/app_command.cc). While a
// Chrome++ build is staged, that open gets a stand-in even when the key
// exists, since a real key belongs to an installed Google Chrome. The stand-in
// is a fresh handle rather than a predefined root, which `RegKey` never closes,
// so `MyRegCloseKey` sees when Chrome is done with it.
LSTATUS APIENTRY MyRegOpenKeyExW(HKEY hKey,
                                 LPCWSTR lpSubKey,
                                 DWORD ulOptions,
                                 REGSAM samDesired,
                                 PHKEY phkResult) {
  const bool clients_key =
      phkResult && lpSubKey && StrStrIW(lpSubKey, L"Google\\Update\\Clients\\");
  if (config.IsManagedUpdates() && clients_key && hKey == HKEY_CURRENT_USER &&
      std::wstring_view(lpSubKey).ends_with(L"\\Commands\\cmd") &&
      StagedChromeVersion()) {
    const LSTATUS result = RawRegOpenKeyExW(HKEY_CURRENT_USER, L"Software", 0,
                                            samDesired, phkResult);
    if (result == ERROR_SUCCESS) {
      rename_command_key.store(*phkResult);
    }
    DebugLog(L"UpgradeNotification: rename command stand-in: {}", result);
    return result;
  }
  const LSTATUS result =
      RawRegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
  if (result != ERROR_SUCCESS && clients_key) {
    if (RawRegOpenKeyExW(hKey, L"", 0, samDesired, phkResult) ==
        ERROR_SUCCESS) {
      DebugLog(
          L"UpgradeNotification: Clients key absent, substituting stand-in "
          L"handle");
      return ERROR_SUCCESS;
    }
  }
  return result;
}

LSTATUS APIENTRY MyRegCloseKey(HKEY hKey) {
  HKEY expected = hKey;
  rename_command_key.compare_exchange_strong(expected, nullptr);
  return RawRegCloseKey(hKey);
}

}  // namespace

void UpgradeNotification() {
  if (!config.IsManagedUpdates() &&
      !config.IsSuppressFalseUpgradeNotification()) {
    return;
  }

  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach(&RawRegOpenKeyExW, MyRegOpenKeyExW);
  DetourAttach(&RawRegQueryValueExW, MyRegQueryValueExW);
  DetourAttach(&RawRegCloseKey, MyRegCloseKey);
  auto status = DetourTransactionCommit();
  if (status != NO_ERROR) {
    DebugLog(L"UpgradeNotification hooks failed: {}", status);
  } else {
    DebugLog(L"UpgradeNotification: hooks installed");
  }
}
