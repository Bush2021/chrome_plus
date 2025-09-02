#include "portable.h"

#include <windows.h>

#include <shellapi.h>

#include <string>
#include <string_view>
#include <vector>

#include "config.h"
#include "utils.h"

namespace {
// Note: As of Chromium M140, it seems that the `ScriptStreamingForNonHTTP`
// feature flag no longer works. We switch to using the
// `--disable-features=WebUIInProcessResourceLoading` flag instead.

// The `--disable-features=WebUIInProcessResourceLoading` flag is added to
// address https://github.com/Bush2021/chrome_plus/issues/172. Google Chrome
// receives field trial configurations from the variations server, which can be
// inspected via `chrome://version/?show-variations-cmd`. This mechanism causes
// certain features (`base::Feature`) to be enabled or disabled dynamically,
// leading to behavioral differences that may not be reproducible across all
// environments. Adding `--enable-benchmarking` can force all features to a
// fixed state, disabling randomization and making it easier to diagnose whether
// an observed issue is caused by a non-default `base::Feature` configuration.
//
// In this case, it was found that disabling `WebUIInProcessResourceLoading`
// restores normal behavior. This affects how Chrome WebUI pages (such as
// `about:` or `chrome://`) load resources. See
// https://issues.chromium.org/issues/362511750 and
// https://chromium-review.googlesource.com/c/chromium/src/+/5868139 for
// details. If this workaround becomes ineffective in the future, more in-depth
// modifications may be required.

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

        std::vector<std::wstring> final_args;
        std::wstring combined_features;
        const std::wstring disable_features_prefix = L"--disable-features=";

        bool has_user_data_dir = false;
        bool has_disk_cache_dir = false;

        for (const auto& arg : args) {
          if (arg.starts_with(disable_features_prefix)) {
            if (!combined_features.empty()) {
              combined_features.append(L",");
            }
            combined_features.append(
                arg.substr(disable_features_prefix.length()));
          } else {
            if (arg.starts_with(L"--user-data-dir=")) {
              has_user_data_dir = true;
            }
            if (arg.starts_with(L"--disk-cache-dir=")) {
              has_disk_cache_dir = true;
            }
            final_args.push_back(arg);
          }
        }

        // Rebuild the argument list with the final, single `--disable-features`
        // flag, since Chrome expects only one such flag.
        if (!combined_features.empty()) {
          combined_features.append(L",");
        }
        // See the comment at the start of the namespace for details on these.
        combined_features.append(
            L"WinSboxNoFakeGdiInit,WebUIInProcessResourceLoading");
        args = final_args;
        args.emplace_back(disable_features_prefix + combined_features);

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
