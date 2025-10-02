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

// Construct new command line with portable mode.
std::wstring GetCommand(LPWSTR param) {
  if (!param) {
    return L"";
  }

  std::wstring command_line(param);
  std::wstring suffix;

  // The `--single-argument` switch is a special case used by the Windows Shell
  // for file associations. Standard parsers like `CommandLineToArgvW` can
  // incorrectly split the argument that follows it (typically a file path with
  // spaces). To handle this, and consistent with Chromium's implementation
  // (https://github.com/chromium/chromium/blob/51ef426ae939dfa43c870ca1808a1c74dc46ce37/base/command_line.cc#L73),
  // we split the command line here. The part before the switch will be parsed
  // and modified, while the switch and its entire argument will be appended
  // verbatim at the end. Fix
  // https://github.com/Bush2021/chrome_plus/issues/181.
  constexpr std::wstring_view kSingleArgumentSwitch = L"--single-argument";
  const auto single_argument_pos =
      FindStandaloneSwitch(command_line, kSingleArgumentSwitch);
  if (single_argument_pos != std::wstring_view::npos) {
    suffix = command_line.substr(single_argument_pos);
    command_line.erase(single_argument_pos);
  }

  TrimTrailingWhitespace(command_line);

  int argc = 0;
  LPWSTR* argv = nullptr;
  if (!command_line.empty()) {
    argv = CommandLineToArgvW(command_line.c_str(), &argc);
  }

  constexpr auto kReservedArgsCount = 15;
  std::vector<std::wstring> args;
  if (argv) {
    const size_t original_arg_count =
        argc > 0 ? static_cast<size_t>(argc - 1) : 0;
    args.reserve(original_arg_count + kReservedArgsCount);
    for (int i = 1; i < argc; ++i) {
      args.emplace_back(argv[i]);
    }
    LocalFree(argv);
  } else {
    args.reserve(kReservedArgsCount);
  }

  std::vector<std::wstring> trailing_args;
  size_t sentinel_index = args.size();
  for (size_t i = 0; i < args.size(); ++i) {
    if (args[i] == L"--") {
      sentinel_index = i;
      break;
    }
  }
  if (sentinel_index != args.size()) {
    trailing_args.assign(args.begin() + sentinel_index, args.end());
    args.erase(args.begin() + sentinel_index, args.end());
  }

  const auto& config_args = config.GetCommandLine();
  DebugLog(L"config_args: {}", config_args);
  auto parsed_config_args = ParseConfiguredArgs(config_args);
  args.insert(args.end(), parsed_config_args.begin(), parsed_config_args.end());

  args.emplace_back(L"--portable");

  std::vector<std::wstring> final_args;
  final_args.reserve(args.size() + trailing_args.size() + 4);
  std::wstring combined_features;
  const std::wstring disable_features_prefix = L"--disable-features=";

  bool has_user_data_dir = false;
  bool has_disk_cache_dir = false;

  for (const auto& arg : args) {
    if (arg.starts_with(disable_features_prefix)) {
      if (!combined_features.empty()) {
        combined_features.append(L",");
      }
      combined_features.append(arg.substr(disable_features_prefix.length()));
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
  final_args.emplace_back(disable_features_prefix + combined_features);

  if (!has_user_data_dir) {
    if (auto userdata = config.GetUserDataDir(); !userdata.empty()) {
      final_args.emplace_back(L"--user-data-dir=" + userdata);
    }
  }
  if (!has_disk_cache_dir) {
    if (auto diskcache = config.GetDiskCacheDir(); !diskcache.empty()) {
      final_args.emplace_back(L"--disk-cache-dir=" + diskcache);
    }
  }

  final_args.insert(final_args.end(), trailing_args.begin(),
                    trailing_args.end());

  std::wstring result = JoinArgsString(final_args, L" ");
  if (!suffix.empty()) {
    if (!result.empty()) {
      result.push_back(L' ');
    }
    result.append(suffix);
  }

  return result;
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
