#include "portable.h"

#include <windows.h>

#include <shellapi.h>

#include <string>
#include <string_view>
#include <vector>

#include "config.h"
#include "utils.h"

namespace {

constexpr auto kReservedArgsCount = 15;

// Construct new command line with portable mode.
std::wstring GetCommand(LPWSTR param) {
  int argc;
  LPWSTR* argv = CommandLineToArgvW(param, &argc);
  if (!argv) {
    return L"";
  }

  std::vector<std::wstring> args;
  args.reserve(argc + kReservedArgsCount);

  bool has_user_data_dir = false;
  bool has_disk_cache_dir = false;
  for (int i = 0; i < argc; ++i) {
    std::wstring_view arg_view(argv[i]);
    if (arg_view.find(L"--user-data-dir=") == 0) {
      has_user_data_dir = true;
    }
    if (arg_view.find(L"--disk-cache-dir=") == 0) {
      has_disk_cache_dir = true;
    }
  }

  int insert_pos = 0;
  for (int i = 0; i < argc; ++i) {
    std::wstring_view arg_view(argv[i]);
    if (arg_view == L"--" || arg_view == L"--single-argument") {
      break;
    }
    insert_pos = i;
  }
  for (int i = 0; i < argc; ++i) {
    // Preserve former arguments.
    if (i) {
      args.emplace_back(argv[i]);
    }

    // Append new arguments.
    if (i == insert_pos) {
      args.emplace_back(L"--portable");

      args.emplace_back(L"--disable-features=WinSboxNoFakeGdiInit");

      if (!has_user_data_dir) {
        if (auto userdata = config.GetUserDataDir(); !userdata.empty()) {
          args.emplace_back(L"--user-data-dir=" + userdata);
        }
      }
      if (!has_disk_cache_dir) {
        if (auto diskcache = config.GetDiskCacheDir(); !diskcache.empty()) {
          args.emplace_back(L"--disk-cache-dir=" + diskcache);
        }
      }

      // Get the command line and append parameters
      // Intercept and split the parameters starting with each --,
      // and then args.push_back multiple times
      // Repeat the above process until the -- sign no longer exists in the
      // string
      {
        const auto& cr_command_line = config.GetCommandLine();
        DebugLog(L"cr_command_line: {}", cr_command_line);
        std::wstring_view remaining_view(cr_command_line);
        while (true) {
          auto arg_start = remaining_view.find(L"--");
          if (arg_start == std::wstring_view::npos) {
            break;
          }
          remaining_view.remove_prefix(arg_start);

          auto arg_end = remaining_view.find(L" --", 1);
          if (arg_end == std::wstring_view::npos) {
            args.emplace_back(remaining_view);
            break;
          } else {
            args.emplace_back(remaining_view.substr(0, arg_end));
            remaining_view.remove_prefix(arg_end + 1);
          }
        }
      }
    }
  }
  LocalFree(argv);

  return JoinArgsString(args, L" ");
}

HANDLE hMutex = nullptr;
[[maybe_unused]] bool IsFirstRun() {
  DWORD pid = GetCurrentProcessId();
  std::wstring mutex_name =
      L"Global\\ChromePlusFirstRunMutex" + std::to_wstring(pid);
  hMutex = CreateMutexW(nullptr, TRUE, mutex_name.c_str());
  if (hMutex == nullptr || GetLastError() == ERROR_ALREADY_EXISTS) {
    return false;
  }
  return true;
}

}  // namespace

void Portable(LPWSTR param) {
  wchar_t path[MAX_PATH];
  ::GetModuleFileName(nullptr, path, MAX_PATH);

  std::wstring args = GetCommand(param);

  SHELLEXECUTEINFO sei = {0};
  sei.cbSize = sizeof(SHELLEXECUTEINFO);
  sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
  sei.lpVerb = L"open";
  sei.lpFile = path;
  sei.nShow = SW_SHOWNORMAL;

  sei.lpParameters = args.c_str();
  if (ShellExecuteEx(&sei)) {
    ExitProcess(0);
  }
}
