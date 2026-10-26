#ifndef CHROME_PLUS_SRC_UTILS_H_
#define CHROME_PLUS_SRC_UTILS_H_

#include <windows.h>

#include <cstdint>
#include <initializer_list>
#include <span>
#include <string>
#include <string_view>
#include <vector>

// Global variable declaration
extern HMODULE hInstance;

// Constants
consteval uint32_t GetMagicCode() {
  return 0x1603ABD9;
}

// Chrome command IDs
// https://source.chromium.org/chromium/chromium/src/+/main:chrome/app/chrome_command_ids.h?q=chrome_command_ids.h&ss=chromium%2Fchromium%2Fsrc
inline constexpr int IDC_NEW_TAB = 34014;
inline constexpr int IDC_CLOSE_TAB = 34015;
inline constexpr int IDC_SELECT_NEXT_TAB = 34016;
inline constexpr int IDC_SELECT_PREVIOUS_TAB = 34017;
inline constexpr int IDC_SHOW_TRANSLATE = 35009;
inline constexpr int IDC_WINDOW_CLOSE_OTHER_TABS = 35023;

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
void compression_html(std::string& html);

bool ReplaceStringInPlace(std::string& subject,
                          std::string_view search,
                          std::string_view replace);

bool ReplaceStringInPlace(std::wstring& subject,
                          std::wstring_view search,
                          std::wstring_view replace);

std::wstring QuoteSpaceIfNeeded(const std::wstring& str);

std::wstring JoinArgsString(const std::vector<std::wstring>& lines,
                            std::wstring_view delimiter);

// Memory and module search functions
std::span<uint8_t> SearchMemory(std::span<uint8_t> src,
                                std::span<const uint8_t> sub);

// Parse the INI file
std::wstring GetIniString(const wchar_t* section,
                          const wchar_t* key,
                          const wchar_t* default_value);
int GetIniInt(const wchar_t* section, const wchar_t* key, int default_value);

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
    log_file << log_content << L'\n';
  }
}
#else
inline void DebugLog(std::wstring_view, auto&&...) {}
#endif

// Window and message processing functions
HWND GetTopWnd(HWND hwnd);
void ExecuteCommand(int id, HWND hwnd = 0);
void LaunchCommands(const std::wstring& get_commands);
[[nodiscard]] bool IsChromeWindow(HWND hwnd);
// Returns nullptr when the window under `pt` is not part of Chrome.
[[nodiscard]] HWND GetChromeRootWindowAt(POINT pt);

// Presses `keys` in order, then releases them in the same order. Mouse
// buttons are sent as mouse input. Every event carries `GetMagicCode()`.
void SendKey(std::initializer_list<UINT> keys);

// Parse hotkey string like "Ctrl+Shift+A" into MAKELPARAM(modifiers, vk)
// Supports modifiers: shift, ctrl/control, alt, win
// Supports keys: F1-F24, A-Z, 0-9, arrow keys, special keys (esc, tab, etc.)
// Returns: LOWORD = modifiers, HIWORD = virtual key code
// no_repeat: if true, adds MOD_NOREPEAT flag (default for `RegisterHotKey`)
UINT ParseHotkeys(std::wstring_view keys, bool no_repeat = true);

#endif  // CHROME_PLUS_SRC_UTILS_H_
