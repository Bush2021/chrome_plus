#include "portable.h"

#include <shellapi.h>

#include <vector>

#include "config.h"
#include "utils.h"

namespace {

// Construct new command line with portable mode.
std::wstring GetCommand(LPWSTR param) {
  std::vector<std::wstring> args;

  int argc;
  LPWSTR* argv = CommandLineToArgvW(param, &argc);

  bool has_user_data_dir = false;
  bool has_disk_cache_dir = false;
  for (int i = 0; i < argc; ++i) {
    std::wstring arg = argv[i];
    if (arg.find(L"--user-data-dir=") == 0) {
      has_user_data_dir = true;
    }
    if (arg.find(L"--disk-cache-dir=") == 0) {
      has_disk_cache_dir = true;
    }
  }

  int insert_pos = 0;
  for (int i = 0; i < argc; ++i) {
    if (std::wstring(argv[i]).find(L"--") != std::wstring::npos ||
        std::wstring(argv[i]).find(L"--single-argument") !=
            std::wstring::npos) {
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
        auto userdata = config.GetUserDataDir();
        if (!userdata.empty()) {
          args.emplace_back(L"--user-data-dir=" + userdata);
        }
      }
      if (!has_disk_cache_dir) {
        auto diskcache = config.GetDiskCacheDir();
        if (!diskcache.empty()) {
          args.emplace_back(L"--disk-cache-dir=" + diskcache);
        }
      }

      // Get the command line and append parameters
      // Intercept and split the parameters starting with each --,
      // and then args.push_back multiple times
      // Repeat the above process until the -- sign no longer exists in the
      // string
      {
        auto cr_command_line = config.GetCommandLine();
        DebugLog(L"cr_command_line: %s", cr_command_line.c_str());
        std::wstring temp = cr_command_line;
        while (true) {
          auto pos = temp.find(L"--");
          if (pos == std::wstring::npos) {
            break;
          } else {
            auto pos2 = temp.find(L" --", pos);
            if (pos2 == std::wstring::npos) {
              args.emplace_back(temp);
              break;
            } else {
              args.emplace_back(temp.substr(pos, pos2 - pos));
              temp = temp.substr(0, pos) + temp.substr(pos2 + 1);
            }
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
