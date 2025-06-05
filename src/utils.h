#ifndef UTILS_H_
#define UTILS_H_

#include <algorithm>
#include <cctype>
#include <functional>
#include <string>
#include <vector>

#include <windows.h>
#include <Shlwapi.h>

#include "FastSearch.h"

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

// String manipulation function.
std::wstring Format(const wchar_t* format, va_list args) {
  std::vector<wchar_t> buffer;

  size_t length = _vscwprintf(format, args);

  buffer.resize((length + 1) * sizeof(wchar_t));

  _vsnwprintf_s(&buffer[0], length + 1, length, format, args);

  return std::wstring(&buffer[0]);
}

std::wstring Format(const wchar_t* format, ...) {
  va_list args;

  va_start(args, format);
  auto str = Format(format, args);
  va_end(args);

  return str;
}

std::string wstring_to_string(const std::wstring& wstr) {
  std::string strTo;
  auto szTo = new char[wstr.length() + 1];
  szTo[wstr.size()] = '\0';
  WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo,
                      static_cast<int>(wstr.length()), nullptr, nullptr);
  strTo = szTo;
  delete[] szTo;
  return strTo;
}

// Specify the delimiter and wrapper to split the string.
std::vector<std::wstring> StringSplit(const std::wstring& str,
                                      const wchar_t delim,
                                      const std::wstring& enclosure) {
  std::vector<std::wstring> result;
  std::wstring::size_type start = 0;
  std::wstring::size_type end = str.find(delim);
  while (end != std::wstring::npos) {
    std::wstring name = str.substr(start, end - start);
    if (!enclosure.empty() && !name.empty() &&
        name.front() == enclosure.front()) {
      name.erase(0, 1);
    }
    if (!enclosure.empty() && !name.empty() &&
        name.back() == enclosure.back()) {
      name.erase(name.size() - 1);
    }
    result.push_back(name);
    start = end + 1;
    end = str.find(delim, start);
  }
  if (start < str.length()) {
    std::wstring name = str.substr(start);
    if (!enclosure.empty() && !name.empty() &&
        name.front() == enclosure.front()) {
      name.erase(0, 1);
    }
    if (!enclosure.empty() && !name.empty() &&
        name.back() == enclosure.back()) {
      name.erase(name.size() - 1);
    }
    result.push_back(name);
  }
  return result;
}

// Compression html.
std::string& ltrim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                  [](int ch) { return !std::isspace(ch); }));
  return s;
}
std::string& rtrim(std::string& s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](int ch) { return !std::isspace(ch); })
              .base(),
          s.end());
  return s;
}

std::string& trim(std::string& s) {
  return ltrim(rtrim(s));
}

std::vector<std::string> split(const std::string& text, char sep) {
  std::vector<std::string> tokens;
  std::size_t start = 0, end = 0;
  while ((end = text.find(sep, start)) != std::string::npos) {
    std::string temp = text.substr(start, end - start);
    tokens.push_back(temp);
    start = end + 1;
  }
  std::string temp = text.substr(start);
  tokens.push_back(temp);
  return tokens;
}

void compression_html(std::string& html) {
  auto lines = split(html, '\n');
  html.clear();
  for (auto& line : lines) {
    html += "\n";
    html += trim(line);
  }
}

bool ReplaceStringInPlace(std::string& subject,
                          const std::string& search,
                          const std::string& replace) {
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
                          const std::wstring& search,
                          const std::wstring& replace) {
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
    return std::move(str);

  std::wstring escaped(L"\"");
  for (auto c : str) {
    if (c == L'"')
      escaped += L'"';
    escaped += c;
  }
  escaped += L'"';
  return std::move(escaped);
}

std::wstring JoinArgsString(std::vector<std::wstring> lines,
                            const std::wstring& delimiter) {
  std::wstring text;
  bool first = true;
  for (auto& line : lines) {
    if (!first)
      text += delimiter;
    else
      first = false;
    text += QuoteSpaceIfNeeded(line);
  }
  return text;
}

// Memory and module search functions.
// Search memory.
uint8_t* memmem(uint8_t* src, int n, const uint8_t* sub, int m) {
  return (uint8_t*)FastSearch(src, n, sub, m);
}

uint8_t* SearchModuleRaw(HMODULE module, const uint8_t* sub, int m) {
  uint8_t* buffer = (uint8_t*)module;

  PIMAGE_NT_HEADERS nt_header =
      (PIMAGE_NT_HEADERS)(buffer + ((PIMAGE_DOS_HEADER)buffer)->e_lfanew);
  PIMAGE_SECTION_HEADER section =
      (PIMAGE_SECTION_HEADER)((char*)nt_header + sizeof(DWORD) +
                              sizeof(IMAGE_FILE_HEADER) +
                              nt_header->FileHeader.SizeOfOptionalHeader);

  for (int i = 0; i < nt_header->FileHeader.NumberOfSections; ++i) {
    if (strcmp((const char*)section[i].Name, ".text") == 0) {
      return memmem(buffer + section[i].PointerToRawData,
                    section[i].SizeOfRawData, sub, m);
      break;
    }
  }
  return nullptr;
}

uint8_t* SearchModuleRaw2(HMODULE module, const uint8_t* sub, int m) {
  uint8_t* buffer = (uint8_t*)module;

  PIMAGE_NT_HEADERS nt_header =
      (PIMAGE_NT_HEADERS)(buffer + ((PIMAGE_DOS_HEADER)buffer)->e_lfanew);
  PIMAGE_SECTION_HEADER section =
      (PIMAGE_SECTION_HEADER)((char*)nt_header + sizeof(DWORD) +
                              sizeof(IMAGE_FILE_HEADER) +
                              nt_header->FileHeader.SizeOfOptionalHeader);

  for (int i = 0; i < nt_header->FileHeader.NumberOfSections; ++i) {
    if (strcmp((const char*)section[i].Name, ".rdata") == 0) {
      return memmem(buffer + section[i].PointerToRawData,
                    section[i].SizeOfRawData, sub, m);
      break;
    }
  }
  return nullptr;
}

// bool WriteMemory(PBYTE BaseAddress, PBYTE Buffer, DWORD nSize) {
//   DWORD ProtectFlag = 0;
//   if (VirtualProtectEx(GetCurrentProcess(), BaseAddress, nSize,
//                        PAGE_EXECUTE_READWRITE, &ProtectFlag)) {
//     memcpy(BaseAddress, Buffer, nSize);
//     FlushInstructionCache(GetCurrentProcess(), BaseAddress, nSize);
//     VirtualProtectEx(GetCurrentProcess(), BaseAddress, nSize, ProtectFlag,
//                      &ProtectFlag);
//     return true;
//   }
//   return false;
// }

// Path and file manipulation functions.
// Get the directory where the application is located.

bool isEndWith(const wchar_t* s, const wchar_t* sub) {
  if (!s || !sub)
    return false;
  size_t len1 = wcslen(s);
  size_t len2 = wcslen(sub);
  if (len2 > len1)
    return false;
  return !_memicmp(s + len1 - len2, sub, len2 * sizeof(wchar_t));
}

std::wstring GetAppDir() {
  wchar_t path[MAX_PATH];
  ::GetModuleFileName(nullptr, path, MAX_PATH);
  ::PathRemoveFileSpec(path);
  return path;
}

const std::wstring kAppDir = GetAppDir();
const std::wstring kIniPath = kAppDir + L"\\chrome++.ini";

// Prase the INI file.
std::wstring GetIniString(const std::wstring& section,
                          const std::wstring& key,
                          const std::wstring& default_value) {
  std::vector<TCHAR> buffer(100);
  DWORD bytesread = 0;
  do {
    bytesread = ::GetPrivateProfileStringW(
        section.c_str(), key.c_str(), default_value.c_str(), buffer.data(),
        (DWORD)buffer.size(), kIniPath.c_str());
    if (bytesread >= buffer.size() - 1) {
      buffer.resize(buffer.size() * 2);
    } else {
      break;
    }
  } while (true);

  return std::wstring(buffer.data());
}

// Canonicalize the path.
std::wstring CanonicalizePath(const std::wstring& path) {
  TCHAR temp[MAX_PATH];
  ::PathCanonicalize(temp, path.data());
  return std::wstring(temp);
}

// Get the absolute path.
std::wstring GetAbsolutePath(const std::wstring& path) {
  wchar_t buffer[MAX_PATH];
  ::GetFullPathNameW(path.c_str(), MAX_PATH, buffer, nullptr);
  return buffer;
}

// Expand environment variables in the path.
std::wstring ExpandEnvironmentPath(const std::wstring& path) {
  std::vector<wchar_t> buffer(MAX_PATH);
  size_t ExpandedLength = ::ExpandEnvironmentStrings(path.c_str(), &buffer[0],
                                                     (DWORD)buffer.size());
  if (ExpandedLength > buffer.size()) {
    buffer.resize(ExpandedLength);
    ExpandedLength = ::ExpandEnvironmentStrings(path.c_str(), &buffer[0],
                                                (DWORD)buffer.size());
  }
  return std::wstring(&buffer[0], 0, ExpandedLength);
}

// Debug log function.
void DebugLog(const wchar_t* format, ...) {
//   va_list args;

//   va_start(args, format);
//   auto str = Format(format, args);
//   va_end(args);

//   str = Format(L"[chrome++] %s\n", str.c_str());

//   std::string nstr = wstring_to_string(str);
//   const char* cstr = nstr.c_str();

//   FILE* fp = nullptr;
//   std::wstring logPath = kAppDir + L"\\Chrome++_Debug.log";
//   _wfopen_s(&fp, logPath.c_str(), L"a+");
//   if (fp) {
//     fwrite(cstr, strlen(cstr), 1, fp);
//     fclose(fp);
//   }
}

// Window and message processing functions.
HWND GetTopWnd(HWND hwnd) {
  while (::GetParent(hwnd) && ::IsWindowVisible(::GetParent(hwnd))) {
    hwnd = ::GetParent(hwnd);
  }
  return hwnd;
}

void ExecuteCommand(int id, HWND hwnd = 0) {
  if (hwnd == 0)
    hwnd = GetForegroundWindow();
  // hwnd = GetTopWnd(hwnd);
  // hwnd = GetForegroundWindow();
  // PostMessage(hwnd, WM_SYSCOMMAND, id, 0);
  ::SendMessageTimeoutW(hwnd, WM_SYSCOMMAND, id, 0, 0, 1000, 0);
}

void LaunchCommands(const std::wstring& get_commands) {
  auto commands = StringSplit(
      get_commands, L';',
      L"");  // Quotes should not be used as they can cause errors with paths
             // that contain spaces. Since semicolons rarely appear in names and
             // commands, they are used as delimiters.
  if (commands.empty()) {
    return;
  }
  for (const auto& command : commands) {
    std::wstring expanded_path = ExpandEnvironmentPath(command);
    ReplaceStringInPlace(expanded_path, L"%app%", kAppDir);

    // Using `start` launches the command in a new window asynchronously,
    // avoiding blocking Chrome's main thread. For more details:
    // https://github.com/Bush2021/chrome_plus/issues/130#issuecomment-2925782726
    // https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/start
    //  `cmd /c` ensures the command window exits after execution, preventing
    //  the "Not enough memory resources are available to process this command"
    //  error even when all commands run successfully.
    std::wstring cmd = LR"(start "chrome++ cmd" cmd /c ")" + expanded_path + LR"(")";
    _wsystem(cmd.c_str());
  }
}

[[maybe_unused]]
HANDLE RunExecute(const wchar_t* command, WORD show = SW_SHOW) {
  int nArgs = 0;
  std::vector<std::wstring> command_line;
  LPWSTR* szArglist = CommandLineToArgvW(command, &nArgs);
  for (int i = 0; i < nArgs; ++i) {
    command_line.push_back(QuoteSpaceIfNeeded(szArglist[i]));
  }
  LocalFree(szArglist);

  SHELLEXECUTEINFO ShExecInfo = {0};
  ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
  ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
  ShExecInfo.lpFile = command_line[0].c_str();
  ShExecInfo.nShow = show;

  std::wstring parameter;
  for (size_t i = 1; i < command_line.size(); ++i) {
    parameter += command_line[i];
    parameter += L" ";
  }
  if (command_line.size() > 1) {
    ShExecInfo.lpParameters = parameter.c_str();
  }
  if (ShellExecuteEx(&ShExecInfo)) {
    return ShExecInfo.hProcess;
  }
  return nullptr;
}

bool IsFullScreen(HWND hwnd) {
  RECT windowRect;
  return (GetWindowRect(hwnd, &windowRect) &&
          (windowRect.left == 0 && windowRect.top == 0 &&
           windowRect.right == GetSystemMetrics(SM_CXSCREEN) &&
           windowRect.bottom == GetSystemMetrics(SM_CYSCREEN)));
}

// Keyboard and mouse input functions.
// Send the combined key operation.
// class SendKeys {
//  public:
//   template <typename... T>
//   SendKeys(T... keys) {
//     std::vector<int> keys_ = {keys...};
//     for (auto& key : keys_) {
//       INPUT input = {0};
//       input.type = INPUT_KEYBOARD;
//       input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
//       input.ki.wVk = key;

//       // Correct the mouse message
//       switch (key) {
//         case VK_MBUTTON:
//           input.type = INPUT_MOUSE;
//           input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
//           break;
//       }

//       inputs_.push_back(input);
//     }

//     SendInput((UINT)inputs_.size(), &inputs_[0], sizeof(INPUT));
//   }
//   ~SendKeys() {
//     for (auto& input : inputs_) {
//       input.ki.dwFlags |= KEYEVENTF_KEYUP;

//       // Correct the mouse message
//       switch (input.ki.wVk) {
//         case VK_MBUTTON:
//           input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
//           break;
//       }
//     }

//     SendInput((UINT)inputs_.size(), &inputs_[0], sizeof(INPUT));
//   }

//  private:
//   std::vector<INPUT> inputs_;
// };

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

// Send a single key operation.
void SendOneMouse(int mouse) {
  // Swap the left and right mouse buttons (if defined).
  if (::GetSystemMetrics(SM_SWAPBUTTON) == TRUE) {
    if (mouse == MOUSEEVENTF_RIGHTDOWN)
      mouse = MOUSEEVENTF_LEFTDOWN;
    else if (mouse == MOUSEEVENTF_RIGHTUP)
      mouse = MOUSEEVENTF_LEFTUP;
  }

  INPUT input[1];
  memset(input, 0, sizeof(input));

  input[0].type = INPUT_MOUSE;

  input[0].mi.dwFlags = mouse;
  input[0].mi.dwExtraInfo = MAGIC_CODE;
  ::SendInput(1, input, sizeof(INPUT));
}

#endif  // UTILS_H_
