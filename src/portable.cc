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

int FindArgumentInsertionPosition(int argc, LPWSTR* argv) {
  int insert_pos = 0;
  // Start at 1 to skip the program name
  for (int i = 1; i < argc; ++i) {
    std::wstring_view arg_view(argv[i]);
    if (arg_view == L"--" || arg_view == L"--single-argument") {
      break;
    }
    insert_pos = i;
  }
  return insert_pos;
}

std::vector<std::wstring> ParseConfiguredArgs(std::wstring_view args) {
  std::vector<std::wstring> result;
  while (true) {
    auto arg_start = args.find(L"--");
    if (arg_start == std::wstring_view::npos) {
      break;
    }
    args.remove_prefix(arg_start);
    auto arg_end = args.find(L" --", 1);
    if (arg_end == std::wstring_view::npos) {
      result.emplace_back(args);
      break;
    } else {
      result.emplace_back(args.substr(0, arg_end));
      args.remove_prefix(arg_end + 1);
    }
  }
  return result;
}

// Construct new command line with portable mode.
std::wstring GetCommand(LPWSTR param) {
  int argc;
  LPWSTR* argv = CommandLineToArgvW(param, &argc);
  if (!argv) {
    return L"";
  }

  std::vector<std::wstring> args;
  args.reserve(argc + kReservedArgsCount);

  for (int i = 0; i < argc; ++i) {
    // Preserve former arguments.
    if (i) {
      args.emplace_back(argv[i]);
    }

    // Append new arguments.
    if (i == FindArgumentInsertionPosition(argc, argv)) {
      const auto& config_args = config.GetCommandLine();
      DebugLog(L"config_args: {}", config_args);
      auto parsed_config_args = ParseConfiguredArgs(config_args);
      args.insert(args.end(), parsed_config_args.begin(),
                  parsed_config_args.end());

      {
        args.emplace_back(L"--portable");

        // The `--disable-features=ScriptStreamingForNonHTTP` flag is added to
        // address https://github.com/Bush2021/chrome_plus/issues/172. Google
        // Chrome receives field trial configurations from the variations
        // server, which can be inspected via
        // `chrome://version/?show-variations-cmd`. This mechanism causes
        // certain features (`base::Feature`) to be enabled or disabled
        // dynamically, leading to behavioral differences that may not be
        // reproducible across all environments. Adding `--enable-benchmarking`
        // can force all features to a fixed state, disabling randomization and
        // making it easier to diagnose whether an observed issue is caused by a
        // non-default `base::Feature` configuration.
        //
        // In this case, it was found that disabling either
        // `WebUIInProcessResourceLoading` or `ScriptStreamingForNonHTTP`
        // restores normal behavior. These features affect how Chrome WebUI
        // pages (such as `about:` or `chrome://`) load resources. See
        // https://issues.chromium.org/issues/362511750 and
        // https://chromium-review.googlesource.com/c/chromium/src/+/5868139 for
        // details. While these changes may improve performance,
        // `ScriptStreamingForNonHTTP` appears to only impact WebUI, so we
        // choose to disable this feature specifically. If this workaround
        // becomes ineffective in the future, more in-depth modifications may be
        // required.
        args.emplace_back(
            L"--disable-features=WinSboxNoFakeGdiInit,"
            L"ScriptStreamingForNonHTTP");
        bool has_user_data_dir = false;
        bool has_disk_cache_dir = false;
        for (const auto& arg : args) {
          std::wstring_view arg_view(arg);
          if (arg_view.find(L"--user-data-dir=") == 0) {
            has_user_data_dir = true;
          }
          if (arg_view.find(L"--disk-cache-dir=") == 0) {
            has_disk_cache_dir = true;
          }
        }

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
      }
    }
  }
  LocalFree(argv);

  return JoinArgsString(args, L" ");
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
