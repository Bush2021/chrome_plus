#ifndef CHROME_PLUS_SRC_UTILS_H_
#define CHROME_PLUS_SRC_UTILS_H_

// #include <cstdint>
#include <string>
#include <vector>

#include <windows.h>

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
std::wstring Format(const wchar_t* format, va_list args);
std::wstring Format(const wchar_t* format, ...);
std::string wstring_to_string(const std::wstring& wstr);

// Specify the delimiter and wrapper to split the string.
std::vector<std::wstring> StringSplit(const std::wstring& str,
                                      const wchar_t delim,
                                      const std::wstring& enclosure);

// HTML compression functions
std::string& ltrim(std::string& s);
std::string& rtrim(std::string& s);
std::string& trim(std::string& s);
std::vector<std::string> split(const std::string& text, char sep);
void compression_html(std::string& html);

bool ReplaceStringInPlace(std::string& subject,
                          const std::string& search,
                          const std::string& replace);

bool ReplaceStringInPlace(std::wstring& subject,
                          const std::wstring& search,
                          const std::wstring& replace);

std::wstring QuoteSpaceIfNeeded(const std::wstring& str);

std::wstring JoinArgsString(std::vector<std::wstring> lines,
                            const std::wstring& delimiter);

// Memory and module search functions
uint8_t* memmem(uint8_t* src, int n, const uint8_t* sub, int m);

uint8_t* SearchModuleRaw(HMODULE module, const uint8_t* sub, int m);
uint8_t* SearchModuleRaw2(HMODULE module, const uint8_t* sub, int m);

// Parse the INI file
std::wstring GetIniString(const std::wstring& section,
                          const std::wstring& key,
                          const std::wstring& default_value);

// Canonicalize the path
std::wstring CanonicalizePath(const std::wstring& path);

// Get the absolute path
std::wstring GetAbsolutePath(const std::wstring& path);

// Expand environment variables in the path
std::wstring ExpandEnvironmentPath(const std::wstring& path);

// Debug log function
void DebugLog(const wchar_t* format, ...);

// Window and message processing functions
HWND GetTopWnd(HWND hwnd);
void ExecuteCommand(int id, HWND hwnd = 0);
void LaunchCommands(const std::wstring& get_commands);
HANDLE RunExecute(const wchar_t* command, WORD show = SW_SHOW);
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
  SendInput((UINT)inputs.size(), &inputs[0], sizeof(INPUT));
}

// Send a single mouse operation
void SendOneMouse(int mouse);

#endif  // CHROME_PLUS_SRC_UTILS_H_
