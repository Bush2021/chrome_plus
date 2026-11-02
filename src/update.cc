#include "update.h"

#include <windows.h>

#include <array>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "utils.h"

namespace {

// Rename command exit codes (chrome/installer/util/util_constants.h).
// `SwapNewChromeExeIfPresent` (chrome/browser/first_run/upgrade_util_win.cc)
// treats anything but `RENAME_SUCCESSFUL` as a failed swap.
constexpr int kRenameSuccessful = 23;
constexpr int kRenameFailed = 24;

// The updater writes this file last, holding only the staged version. It marks
// a build staged for Chrome++, as opposed to one Google's updater left for its
// own setup.exe.
constexpr wchar_t kStagedMarker[] = L"chrome++.staged";

std::wstring AppPath(std::wstring_view name) {
  return GetAppDir() + L"\\" + std::wstring(name);
}

bool Exists(const std::wstring& path) {
  return GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

// The version is also a directory name, so accept only four numeric parts.
bool IsChromeVersion(std::wstring_view version) {
  int parts = 0;
  for (auto part : std::views::split(version, L'.')) {
    const std::wstring_view number(part.begin(), part.end());
    if (number.empty() ||
        number.find_first_not_of(L"0123456789") != std::wstring_view::npos) {
      return false;
    }
    ++parts;
  }
  return parts == 4;
}

}  // namespace

std::optional<std::wstring> StagedChromeVersion() {
  HANDLE file =
      CreateFileW(AppPath(kStagedMarker).c_str(), GENERIC_READ,
                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                  nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return std::nullopt;
  }
  std::array<char, 64> buffer;
  DWORD size = 0;
  const BOOL read = ReadFile(file, buffer.data(),
                             static_cast<DWORD>(buffer.size()), &size, nullptr);
  CloseHandle(file);
  if (!read) {
    return std::nullopt;
  }
  std::string_view text(buffer.data(), size);
  text = text.substr(0, text.find_last_not_of("\r\n ") + 1);
  std::wstring version(text.begin(), text.end());
  if (!IsChromeVersion(version) || !Exists(AppPath(L"new_chrome.exe")) ||
      !Exists(AppPath(version + L"\\chrome.dll"))) {
    return std::nullopt;
  }
  return version;
}

std::wstring FinishUpdateCommandLine() {
  return L"\"" + AppPath(L"chrome.exe") + L"\" " + kFinishUpdateSwitch;
}

int FinishUpdate() {
  const std::optional<std::wstring> version = StagedChromeVersion();
  if (!version) {
    DebugLog(L"FinishUpdate: no staged build");
    return kRenameFailed;
  }

  std::vector<std::pair<std::wstring, std::wstring>> renamed;
  auto rename = [&renamed](std::wstring_view from, std::wstring_view to,
                           DWORD flags) {
    std::wstring source = AppPath(from);
    std::wstring target = AppPath(to);
    if (!MoveFileExW(source.c_str(), target.c_str(), flags)) {
      DebugLog(L"FinishUpdate: {} -> {} failed: {}", from, to, GetLastError());
      return false;
    }
    renamed.emplace_back(std::move(source), std::move(target));
    return true;
  };
  // Mirrors `RenameChromeExecutables` (chrome/installer/setup/setup_main.cc).
  // Windows lets a running executable be renamed but not replaced, so the
  // running build moves to the old_ names first, overwriting any left over
  // from an earlier swap.
  bool ok =
      rename(L"chrome.exe", L"old_chrome.exe", MOVEFILE_REPLACE_EXISTING) &&
      rename(L"new_chrome.exe", L"chrome.exe", 0);
  if (ok && Exists(AppPath(L"new_chrome_proxy.exe"))) {
    ok = (!Exists(AppPath(L"chrome_proxy.exe")) ||
          rename(L"chrome_proxy.exe", L"old_chrome_proxy.exe",
                 MOVEFILE_REPLACE_EXISTING)) &&
         rename(L"new_chrome_proxy.exe", L"chrome_proxy.exe", 0);
  }
  if (!ok) {
    for (const auto& [from, to] : std::views::reverse(renamed)) {
      MoveFileExW(to.c_str(), from.c_str(), 0);
    }
    return kRenameFailed;
  }
  DeleteFileW(AppPath(kStagedMarker).c_str());
  DebugLog(L"FinishUpdate: swapped in Chrome {}", *version);
  return kRenameSuccessful;
}
