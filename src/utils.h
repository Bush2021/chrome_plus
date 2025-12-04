#ifndef CHROME_PLUS_SRC_UTILS_H_
#define CHROME_PLUS_SRC_UTILS_H_

#include <windows.h>

// #include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "fastsearch.h"

// Global variable declarations
extern HMODULE hInstance;

// Constants
#define MAGIC_CODE 0x1603ABD9

// Chrome command IDs
// https://source.chromium.org/chromium/chromium/src/+/main:chrome/app/chrome_command_ids.h?q=chrome_command_ids.h&ss=chromium%2Fchromium%2Fsrc
#define IDC_NEW_TAB 34014
#define IDC_CLOSE_TAB 34015
#define IDC_SELECT_NEXT_TAB 34016
#define IDC_SELECT_PREVIOUS_TAB 34017
#define IDC_SELECT_TAB_0 34018
#define IDC_SELECT_TAB_1 34019
#define IDC_SELECT_TAB_2 34020
#define IDC_SELECT_TAB_3 34021
#define IDC_SELECT_TAB_4 34022
#define IDC_SELECT_TAB_5 34023
#define IDC_SELECT_TAB_6 34024
#define IDC_SELECT_TAB_7 34025
#define IDC_SELECT_LAST_TAB 34026
#define IDC_FULLSCREEN 34030
#define IDC_SHOW_TRANSLATE 35009
#define IDC_WINDOW_CLOSE_OTHER_TABS 35023
#define IDC_CLOSE_FIND_OR_STOP 37003
#define IDC_UPGRADE_DIALOG 40024

// Global constants - use functions to avoid static initialization order issues
const std::wstring& GetAppDir();
const std::wstring& GetIniPath();

// String manipulation function declarations
// Specify the delimiter and wrapper to split the string.
std::vector<std::wstring> StringSplit(std::wstring_view str,
                                      const wchar_t delim,
                                      std::wstring_view enclosure = L"");
std::vector<std::string> StringSplit(std::string_view str,
                                     const char delim,
                                     std::string_view enclosure = "");

// HTML compression functions
std::string& ltrim(std::string& s);
std::string& rtrim(std::string& s);
std::string& trim(std::string& s);
void compression_html(std::string& html);

bool ReplaceStringInPlace(std::string& subject,
                          std::string_view search,
                          std::string_view replace);

bool ReplaceStringInPlace(std::wstring& subject,
                          std::wstring_view search,
                          std::wstring_view replace);

std::wstring QuoteSpaceIfNeeded(const std::wstring& str);

std::wstring JoinArgsString(std::vector<std::wstring> lines,
                            std::wstring_view delimiter);

// Memory and module search functions
uint8_t* memmem(uint8_t* src, int n, const uint8_t* sub, int m);

// Parse the INI file
std::wstring GetIniString(std::wstring_view section,
                          std::wstring_view key,
                          std::wstring_view default_value);

// Canonicalize the path
std::wstring CanonicalizePath(const std::wstring& path);

// Get the absolute path
std::wstring GetAbsolutePath(const std::wstring& path);

// Expand environment variables in the path
std::wstring ExpandEnvironmentPath(const std::wstring& path);

// Debug log function
#if defined(_DEBUG)
#include <filesystem>
#include <format>
#include <fstream>
#include <mutex>
template <typename... Args>
void DebugLog(std::wformat_string<Args...> fmt, Args&&... args) {
  static std::mutex log_mutex;
  std::lock_guard<std::mutex> lock(log_mutex);

  std::wstring log_content = std::format(
      L"[chrome++] {}", std::format(fmt, std::forward<Args>(args)...));

  std::filesystem::path log_path = GetAppDir();
  log_path /= L"Chrome++_Debug.log";

  if (std::wofstream log_file(log_path, std::ios::app); log_file.is_open()) {
    log_file.imbue(std::locale(""));
    log_file << log_content << std::endl;
  }
}
#else
inline void DebugLog(std::wstring_view, auto&&...) {}
#endif

// Window and message processing functions
HWND GetTopWnd(HWND hwnd);
void ExecuteCommand(int id, HWND hwnd = 0);
void LaunchCommands(const std::wstring& get_commands);
bool IsFullScreen(HWND hwnd);

// Keyboard and mouse input functions
// Template function for sending combined key operations - kept in header
template <typename... T>
void SendKey(T&&... keys) {
  std::vector<typename std::common_type<T...>::type> keys_ = {
      std::forward<T>(keys)...};
  std::vector<INPUT> inputs{};
  inputs.reserve(keys_.size() * 2);
  for (auto& key : keys_) {
    INPUT input = {0};
    // Adjust mouse messages
    switch (key) {
      case VK_RBUTTON:
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = GetSystemMetrics(SM_SWAPBUTTON) == TRUE
                               ? MOUSEEVENTF_LEFTDOWN
                               : MOUSEEVENTF_RIGHTDOWN;
        input.mi.dwExtraInfo = MAGIC_CODE;
        break;
      case VK_LBUTTON:
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = GetSystemMetrics(SM_SWAPBUTTON) == TRUE
                               ? MOUSEEVENTF_RIGHTDOWN
                               : MOUSEEVENTF_LEFTDOWN;
        input.mi.dwExtraInfo = MAGIC_CODE;
        break;
      case VK_MBUTTON:
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
        input.mi.dwExtraInfo = MAGIC_CODE;
        break;
      default:
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = (WORD)key;
        input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
        input.ki.dwExtraInfo = MAGIC_CODE;
        break;
    }
    inputs.emplace_back(std::move(input));
  }
  for (auto& key : keys_) {
    INPUT input = {0};
    // Adjust mouse messages
    switch (key) {
      case VK_RBUTTON:
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = GetSystemMetrics(SM_SWAPBUTTON) == TRUE
                               ? MOUSEEVENTF_LEFTUP
                               : MOUSEEVENTF_RIGHTUP;
        input.mi.dwExtraInfo = MAGIC_CODE;
        break;
      case VK_LBUTTON:
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = GetSystemMetrics(SM_SWAPBUTTON) == TRUE
                               ? MOUSEEVENTF_RIGHTUP
                               : MOUSEEVENTF_LEFTUP;
        input.mi.dwExtraInfo = MAGIC_CODE;
        break;
      case VK_MBUTTON:
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
        input.mi.dwExtraInfo = MAGIC_CODE;
        break;
      default:
        input.type = INPUT_KEYBOARD;
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        input.ki.wVk = (WORD)key;
        input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
        input.ki.dwExtraInfo = MAGIC_CODE;
        break;
    }
    inputs.emplace_back(std::move(input));
  }
  SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
}

#endif  // CHROME_PLUS_SRC_UTILS_H_
