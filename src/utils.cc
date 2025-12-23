#include "utils.h"

#include <windows.h>

#include <shellapi.h>
#include <shlwapi.h>

#include <algorithm>
#include <cctype>
#include <cwctype>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#ifndef MOD_NOREPEAT
constexpr UINT kModNoRepeat = 0x4000;
#else
constexpr UINT kModNoRepeat = MOD_NOREPEAT;
#endif

// Global variable definitions
HMODULE hInstance = nullptr;

// Global constants - use functions to avoid static initialization order issues
const std::wstring& GetAppDir() {
  static std::wstring app_dir = []() {
    wchar_t path[MAX_PATH];
    ::GetModuleFileName(nullptr, path, MAX_PATH);
    ::PathRemoveFileSpec(path);
    return std::wstring(path);
  }();
  return app_dir;
}

const std::wstring& GetIniPath() {
  static std::wstring ini_path = GetAppDir() + L"\\chrome++.ini";
  return ini_path;
}

// String manipulation functions
// Specify the delimiter and wrapper to split the string.
std::vector<std::wstring> StringSplit(std::wstring_view str,
                                      const wchar_t delim,
                                      std::wstring_view enclosure) {
  std::vector<std::wstring> result;
  auto parts = std::views::split(str, delim);
  for (const auto& part : parts) {
    std::wstring_view part_sv(part.begin(), part.end());
    if (!enclosure.empty()) {
      if (!part_sv.empty() && part_sv.front() == enclosure.front()) {
        part_sv.remove_prefix(1);
      }
      if (!part_sv.empty() && part_sv.back() == enclosure.back()) {
        part_sv.remove_suffix(1);
      }
    }
    result.emplace_back(part_sv);
  }
  return result;
}

std::vector<std::string> StringSplit(std::string_view str,
                                     const char delim,
                                     std::string_view enclosure) {
  std::vector<std::string> result;
  auto parts = std::views::split(str, delim);
  for (const auto& part : parts) {
    std::string_view part_sv(part.begin(), part.end());
    if (!enclosure.empty()) {
      if (!part_sv.empty() && part_sv.front() == enclosure.front()) {
        part_sv.remove_prefix(1);
      }
      if (!part_sv.empty() && part_sv.back() == enclosure.back()) {
        part_sv.remove_suffix(1);
      }
    }
    result.emplace_back(part_sv);
  }
  return result;
}

UINT ParseHotkeys(std::wstring_view keys) {
  UINT mo = 0;
  UINT vk = 0;
  std::wstring temp(keys);
  std::vector<std::wstring> key_parts = StringSplit(temp, L'+');

  static const std::unordered_map<std::wstring, UINT> key_map = {
      {L"shift", MOD_SHIFT},  {L"ctrl", MOD_CONTROL}, {L"alt", MOD_ALT},
      {L"win", MOD_WIN},      {L"left", VK_LEFT},     {L"right", VK_RIGHT},
      {L"up", VK_UP},         {L"down", VK_DOWN},     {L"←", VK_LEFT},
      {L"→", VK_RIGHT},       {L"↑", VK_UP},          {L"↓", VK_DOWN},
      {L"esc", VK_ESCAPE},    {L"tab", VK_TAB},       {L"backspace", VK_BACK},
      {L"enter", VK_RETURN},  {L"space", VK_SPACE},   {L"prtsc", VK_SNAPSHOT},
      {L"scroll", VK_SCROLL}, {L"pause", VK_PAUSE},   {L"insert", VK_INSERT},
      {L"delete", VK_DELETE}, {L"end", VK_END},       {L"home", VK_HOME},
      {L"pageup", VK_PRIOR},  {L"pagedown", VK_NEXT},
  };

  for (auto& key : key_parts) {
    std::ranges::transform(key, key.begin(), ::towlower);

    if (key_map.contains(key)) {
      if (key == L"shift" || key == L"ctrl" || key == L"alt" || key == L"win") {
        mo |= key_map.at(key);
      } else {
        vk = key_map.at(key);
      }
    } else {
      TCHAR wch = key[0];
      if (key.length() == 1)  // Parse single characters A-Z, 0-9, etc.
      {
        if (isalnum(wch)) {
          vk = toupper(wch);
        } else {
          vk = LOWORD(VkKeyScan(wch));
        }
      } else if (wch == 'F' || wch == 'f')  // Parse the F1-F24 function keys.
      {
        if (isdigit(key[1])) {
          int fx = _wtoi(&key[1]);
          if (fx >= 1 && fx <= 24) {
            vk = VK_F1 + fx - 1;
          }
        }
      }
    }
  }

  mo |= kModNoRepeat;
  return MAKELPARAM(mo, vk);
}

// Compression html.
std::string& ltrim(std::string& s) {
  auto it = std::ranges::find_if_not(
      s, [](unsigned char c) { return std::isspace(c); });
  s.erase(s.begin(), it);
  return s;
}

std::string& rtrim(std::string& s) {
  auto reversed_view = s | std::views::reverse;
  auto it = std::ranges::find_if_not(
      reversed_view, [](unsigned char c) { return std::isspace(c); });
  s.erase(it.base(), s.end());
  return s;
}

std::string& trim(std::string& s) {
  return ltrim(rtrim(s));
}

void compression_html(std::string& html) {
  auto lines = StringSplit(html, '\n');
  html.clear();
  for (auto& line : lines) {
    html += "\n";
    html += trim(line);
  }
}

bool ReplaceStringInPlace(std::string& subject,
                          std::string_view search,
                          std::string_view replace) {
  bool find = false;
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
    find = true;
  }
  return find;
}

bool ReplaceStringInPlace(std::wstring& subject,
                          std::wstring_view search,
                          std::wstring_view replace) {
  bool find = false;
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::wstring::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
    find = true;
  }
  return find;
}

std::wstring QuoteSpaceIfNeeded(const std::wstring& str) {
  if (str.find(L' ') == std::wstring::npos)
    return str;

  std::wstring escaped(L"\"");
  for (auto c : str) {
    if (c == L'"') {
      escaped += L'"';
    }
    escaped += c;
  }
  escaped += L'"';
  return escaped;
}

std::wstring JoinArgsString(const std::vector<std::wstring>& lines,
                            std::wstring_view delimiter) {
  std::wstring text;
  bool first = true;
  for (auto& line : lines) {
    if (!first) {
      text += delimiter;
    } else {
      first = false;
    }
    text += QuoteSpaceIfNeeded(line);
  }
  return text;
}

// Search memory.
uint8_t* memmem(uint8_t* src, int n, const uint8_t* sub, int m) {
  return const_cast<uint8_t*>(FastSearch(src, n, sub, m));
}

std::wstring GetIniString(std::wstring_view section,
                          std::wstring_view key,
                          std::wstring_view default_value) {
  std::vector<TCHAR> buffer(100);
  DWORD bytesread = 0;
  do {
    bytesread = ::GetPrivateProfileStringW(
        section.data(), key.data(), default_value.data(), buffer.data(),
        static_cast<DWORD>(buffer.size()), GetIniPath().c_str());
    if (bytesread >= buffer.size() - 1) {
      buffer.resize(buffer.size() * 2);
    } else {
      break;
    }
  } while (true);

  return std::wstring(buffer.data());
}

std::wstring CanonicalizePath(const std::wstring& path) {
  TCHAR temp[MAX_PATH];
  ::PathCanonicalize(temp, path.data());
  return std::wstring(temp);
}

std::wstring GetAbsolutePath(const std::wstring& path) {
  wchar_t buffer[MAX_PATH];
  ::GetFullPathNameW(path.c_str(), MAX_PATH, buffer, nullptr);
  return buffer;
}

std::wstring ExpandEnvironmentPath(const std::wstring& path) {
  std::vector<wchar_t> buffer(MAX_PATH);
  size_t ExpandedLength = ::ExpandEnvironmentStrings(
      path.c_str(), &buffer[0], static_cast<DWORD>(buffer.size()));
  if (ExpandedLength > buffer.size()) {
    buffer.resize(ExpandedLength);
    ExpandedLength = ::ExpandEnvironmentStrings(
        path.c_str(), &buffer[0], static_cast<DWORD>(buffer.size()));
  }
  return std::wstring(&buffer[0], 0, ExpandedLength);
}

HWND GetTopWnd(HWND hwnd) {
  while (::GetParent(hwnd) && ::IsWindowVisible(::GetParent(hwnd))) {
    hwnd = ::GetParent(hwnd);
  }
  return hwnd;
}

void ExecuteCommand(int id, HWND hwnd) {
  if (hwnd == 0) {
    hwnd = GetForegroundWindow();
  }
  // hwnd = GetTopWnd(hwnd);
  // hwnd = GetForegroundWindow();
  // PostMessage(hwnd, WM_SYSCOMMAND, id, 0);
  ::SendMessageTimeoutW(hwnd, WM_SYSCOMMAND, id, 0, 0, 1000, 0);
}

void LaunchCommands(const std::wstring& get_commands) {
  auto commands = StringSplit(
      get_commands,
      L';');  // Quotes should not be used as they can cause errors with paths
              // that contain spaces. Since semicolons rarely appear in names
              // and commands, they are used as delimiters.
  if (commands.empty()) {
    return;
  }
  for (const auto& command : commands) {
    std::wstring expanded_path = ExpandEnvironmentPath(command);
    ReplaceStringInPlace(expanded_path, L"%app%", GetAppDir());

    // Using `start` launches the command in a new window asynchronously,
    // avoiding blocking Chrome's main thread. For more details:
    // https://github.com/Bush2021/chrome_plus/issues/130#issuecomment-2925782726
    // https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/start
    //  `cmd /c` ensures the command window exits after execution, preventing
    //  the "Not enough memory resources are available to process this command"
    //  error even when all commands run successfully.
    std::wstring cmd =
        LR"(start "chrome++ cmd" cmd /c ")" + expanded_path + LR"(")";
    _wsystem(cmd.c_str());
  }
}

bool IsFullScreen(HWND hwnd) {
  RECT windowRect;
  return (GetWindowRect(hwnd, &windowRect) &&
          (windowRect.left == 0 && windowRect.top == 0 &&
           windowRect.right == GetSystemMetrics(SM_CXSCREEN) &&
           windowRect.bottom == GetSystemMetrics(SM_CYSCREEN)));
}
