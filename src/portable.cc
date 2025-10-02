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

bool IsWhitespace(wchar_t ch) {
  switch (ch) {
    case L' ':
    case L'\t':
    case L'\n':
    case L'\r':
      return true;
    default:
      return false;
  }
}

// This function ensures the found switch is a whole "word" by checking for
// whitespace or string boundaries before and after it. This prevents incorrect
// partial matches (e.g., finding "--foo" within "--foobar").
std::wstring_view::size_type FindStandaloneSwitch(
    std::wstring_view command_line,
    std::wstring_view flag) {
  auto pos = command_line.find(flag);
  while (pos != std::wstring_view::npos) {
    const bool at_start = pos == 0 || IsWhitespace(command_line[pos - 1]);
    const auto after = pos + flag.size();
    const bool at_end =
        after >= command_line.size() || IsWhitespace(command_line[after]);
    if (at_start && at_end) {
      return pos;
    }
    pos = command_line.find(flag, pos + flag.size());
  }
  return std::wstring_view::npos;
}

void TrimTrailingWhitespace(std::wstring& text) {
  while (!text.empty() && IsWhitespace(text.back())) {
    text.pop_back();
  }
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

// Split command line to extract `--single-argument` suffix if present.
std::pair<std::wstring, std::wstring> SplitSingleArgumentSwitch(
    const std::wstring& command_line) {
  constexpr std::wstring_view kSingleArgument = L"--single-argument";
  const auto single_argument_pos =
      FindStandaloneSwitch(command_line, kSingleArgument);

  if (single_argument_pos == std::wstring_view::npos) {
    return {command_line, L""};
  }

  std::wstring prefix = command_line.substr(0, single_argument_pos);
  std::wstring suffix = command_line.substr(single_argument_pos);
  TrimTrailingWhitespace(prefix);

  return {std::move(prefix), std::move(suffix)};
}

// Parse command line string into argument vector, skipping the executable name.
std::vector<std::wstring> ParseCommandLineArgs(const std::wstring& command_line,
                                               size_t reserve_extra = 15) {
  std::vector<std::wstring> args;
  if (command_line.empty()) {
    args.reserve(reserve_extra);
    return args;
  }

  int argc = 0;
  LPWSTR* argv = CommandLineToArgvW(command_line.c_str(), &argc);
  if (!argv) {
    args.reserve(reserve_extra);
    return args;
  }

  const size_t original_arg_count =
      argc > 0 ? static_cast<size_t>(argc - 1) : 0;
  args.reserve(original_arg_count + reserve_extra);
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }
  LocalFree(argv);

  return args;
}

// Separate arguments before and after the `--` sentinel.
std::pair<std::vector<std::wstring>, std::vector<std::wstring>>
SeparateSentinelArgs(std::vector<std::wstring> args) {
  size_t sentinel_index = args.size();
  for (size_t i = 0; i < args.size(); ++i) {
    if (args[i] == L"--") {
      sentinel_index = i;
      break;
    }
  }

  if (sentinel_index == args.size()) {
    return {std::move(args), {}};
  }

  std::vector<std::wstring> trailing_args(args.begin() + sentinel_index,
                                          args.end());
  args.erase(args.begin() + sentinel_index, args.end());

  return {std::move(args), std::move(trailing_args)};
}

// Merge and process arguments, combining `--disable-features` flags.
struct ProcessedArgs {
  std::vector<std::wstring> final_args;
  bool has_user_data_dir = false;
  bool has_disk_cache_dir = false;
};

ProcessedArgs ProcessAndMergeArgs(const std::vector<std::wstring>& args) {
  ProcessedArgs result;
  result.final_args.reserve(args.size() + 4);

  std::wstring combined_features;
  const std::wstring disable_features_prefix = L"--disable-features=";

  for (const auto& arg : args) {
    if (arg.starts_with(disable_features_prefix)) {
      if (!combined_features.empty()) {
        combined_features.append(L",");
      }
      combined_features.append(arg.substr(disable_features_prefix.length()));
    } else {
      if (arg.starts_with(L"--user-data-dir=")) {
        result.has_user_data_dir = true;
      }
      if (arg.starts_with(L"--disk-cache-dir=")) {
        result.has_disk_cache_dir = true;
      }
      result.final_args.push_back(arg);
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
  result.final_args.emplace_back(disable_features_prefix + combined_features);

  return result;
}

// Inject additional arguments based on config settings.
void InjectConfigPaths(std::vector<std::wstring>& args,
                       bool has_user_data_dir,
                       bool has_disk_cache_dir) {
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

// Reassemble final command line from arguments and suffix.
std::wstring ReassembleCommandLine(const std::vector<std::wstring>& args,
                                   const std::wstring& suffix) {
  std::wstring result = JoinArgsString(args, L" ");
  if (!suffix.empty()) {
    if (!result.empty()) {
      result.push_back(L' ');
    }
    result.append(suffix);
  }
  return result;
}

std::wstring GetCommand(LPWSTR param) {
  if (!param) {
    return L"";
  }

  // The `--single-argument` switch is a special case used by the Windows Shell
  // for file associations. Standard parsers like `CommandLineToArgvW` can
  // incorrectly split the argument that follows it (typically a file path with
  // spaces). To handle this, and consistent with Chromium's implementation
  // (https://github.com/chromium/chromium/blob/51ef426ae939dfa43c870ca1808a1c74dc46ce37/base/command_line.cc#L73),
  // we split the command line here. The part before the switch will be parsed
  // and modified, while the switch and its entire argument will be appended
  // verbatim at the end. Fix
  // https://github.com/Bush2021/chrome_plus/issues/181.
  auto [command_line, suffix] = SplitSingleArgumentSwitch(param);
  auto args = ParseCommandLineArgs(command_line);
  auto [main_args, trailing_args] = SeparateSentinelArgs(std::move(args));

  const auto& config_args = config.GetCommandLine();
  DebugLog(L"config_args: {}", config_args);
  auto parsed_config_args = ParseConfiguredArgs(config_args);
  main_args.insert(main_args.end(), parsed_config_args.begin(),
                   parsed_config_args.end());

  main_args.emplace_back(L"--portable");

  auto processed = ProcessAndMergeArgs(main_args);
  InjectConfigPaths(processed.final_args, processed.has_user_data_dir,
                    processed.has_disk_cache_dir);
  processed.final_args.insert(processed.final_args.end(), trailing_args.begin(),
                              trailing_args.end());
  return ReassembleCommandLine(processed.final_args, suffix);
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
