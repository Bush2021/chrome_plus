#include "utils.h"

#include <windows.h>

#include <shellapi.h>
#include <shlwapi.h>

#include <algorithm>
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ranges>
#include <string>
#include <vector>

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
    tokens.emplace_back(std::move(temp));
    start = end + 1;
  }
  std::string temp = text.substr(start);
  tokens.emplace_back(std::move(temp));
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

std::wstring JoinArgsString(std::vector<std::wstring> lines,
                            const std::wstring& delimiter) {
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

[[maybe_unused]]
uint8_t* SearchModuleRaw(HMODULE module, const uint8_t* sub, int m) {
  uint8_t* buffer = reinterpret_cast<uint8_t*>(module);

  PIMAGE_NT_HEADERS nt_header = reinterpret_cast<PIMAGE_NT_HEADERS>(
      buffer + reinterpret_cast<PIMAGE_DOS_HEADER>(buffer)->e_lfanew);
  PIMAGE_SECTION_HEADER section = reinterpret_cast<PIMAGE_SECTION_HEADER>(
      reinterpret_cast<char*>(nt_header) + sizeof(DWORD) +
      sizeof(IMAGE_FILE_HEADER) + nt_header->FileHeader.SizeOfOptionalHeader);

  for (int i = 0; i < nt_header->FileHeader.NumberOfSections; ++i) {
    if (strcmp(reinterpret_cast<const char*>(section[i].Name), ".text") == 0) {
      return memmem(buffer + section[i].PointerToRawData,
                    section[i].SizeOfRawData, sub, m);
      break;
    }
  }
  return nullptr;
}

[[maybe_unused]]
uint8_t* SearchModuleRaw2(HMODULE module, const uint8_t* sub, int m) {
  uint8_t* buffer = reinterpret_cast<uint8_t*>(module);

  PIMAGE_NT_HEADERS nt_header = reinterpret_cast<PIMAGE_NT_HEADERS>(
      buffer + reinterpret_cast<PIMAGE_DOS_HEADER>(buffer)->e_lfanew);
  PIMAGE_SECTION_HEADER section = reinterpret_cast<PIMAGE_SECTION_HEADER>(
      reinterpret_cast<char*>(nt_header) + sizeof(DWORD) +
      sizeof(IMAGE_FILE_HEADER) + nt_header->FileHeader.SizeOfOptionalHeader);

  for (int i = 0; i < nt_header->FileHeader.NumberOfSections; ++i) {
    if (strcmp(reinterpret_cast<const char*>(section[i].Name), ".rdata") == 0) {
      return memmem(buffer + section[i].PointerToRawData,
                    section[i].SizeOfRawData, sub, m);
      break;
    }
  }
  return nullptr;
}

[[maybe_unused]]
bool WriteMemory(PBYTE BaseAddress, PBYTE Buffer, DWORD nSize) {
  DWORD ProtectFlag = 0;
  if (VirtualProtectEx(GetCurrentProcess(), BaseAddress, nSize,
                       PAGE_EXECUTE_READWRITE, &ProtectFlag)) {
    memcpy(BaseAddress, Buffer, nSize);
    FlushInstructionCache(GetCurrentProcess(), BaseAddress, nSize);
    VirtualProtectEx(GetCurrentProcess(), BaseAddress, nSize, ProtectFlag,
                     &ProtectFlag);
    return true;
  }
  return false;
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

#if defined(_DEBUG)
void DebugLog(const wchar_t* format, ...) {
  va_list args;

  va_start(args, format);
  auto str = Format(format, args);
  va_end(args);

  str = Format(L"[chrome++] %s\n", str.c_str());

  std::string nstr = wstring_to_string(str);
  const char* cstr = nstr.c_str();

  FILE* fp = nullptr;
  std::wstring log_path = GetAppDir() + L"\\Chrome++_Debug.log";
  _wfopen_s(&fp, log_path.c_str(), L"a+");
  if (fp) {
    fwrite(cstr, strlen(cstr), 1, fp);
    fclose(fp);
  }
}
#else
void DebugLog(const wchar_t* format, ...) {}
#endif

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

[[maybe_unused]]
HANDLE RunExecute(const wchar_t* command, WORD show) {
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

void SendOneMouse(int mouse) {
  // Swap the left and right mouse buttons (if defined).
  if (::GetSystemMetrics(SM_SWAPBUTTON) == TRUE) {
    if (mouse == MOUSEEVENTF_RIGHTDOWN) {
      mouse = MOUSEEVENTF_LEFTDOWN;
    } else if (mouse == MOUSEEVENTF_RIGHTUP) {
      mouse = MOUSEEVENTF_LEFTUP;
    }
  }

  INPUT input[1];
  memset(input, 0, sizeof(input));

  input[0].type = INPUT_MOUSE;

  input[0].mi.dwFlags = mouse;
  input[0].mi.dwExtraInfo = MAGIC_CODE;
  ::SendInput(1, input, sizeof(INPUT));
}
