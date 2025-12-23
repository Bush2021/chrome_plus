#include "config.h"

#include <windows.h>

#include <cwctype>
#include <string>
#include <vector>

#include "utils.h"

namespace {
constexpr UINT kModifierMask = MOD_SHIFT | MOD_CONTROL | MOD_ALT | MOD_WIN;

std::wstring Trim(std::wstring_view input) {
  size_t start = 0;
  size_t end = input.size();
  while (start < end && iswspace(input[start])) {
    ++start;
  }
  while (end > start && iswspace(input[end - 1])) {
    --end;
  }
  return std::wstring(input.substr(start, end - start));
}

bool EqualsIgnoreCase(std::wstring_view left, std::wstring_view right) {
  if (left.size() != right.size()) {
    return false;
  }
  for (size_t i = 0; i < left.size(); ++i) {
    if (towlower(left[i]) != towlower(right[i])) {
      return false;
    }
  }
  return true;
}

std::wstring StripInlineComment(std::wstring_view line) {
  bool in_quote = false;
  for (size_t i = 0; i < line.size(); ++i) {
    wchar_t ch = line[i];
    if (ch == L'"') {
      in_quote = !in_quote;
      continue;
    }
    if (!in_quote && ch == L';') {
      return Trim(line.substr(0, i));
    }
  }
  return Trim(line);
}

int CountBracketDelta(std::wstring_view text, bool* saw_open) {
  int delta = 0;
  bool in_quote = false;
  for (wchar_t ch : text) {
    if (ch == L'"') {
      in_quote = !in_quote;
      continue;
    }
    if (in_quote) {
      continue;
    }
    if (ch == L'[') {
      ++delta;
      if (saw_open) {
        *saw_open = true;
      }
    } else if (ch == L']') {
      --delta;
    }
  }
  return delta;
}

std::vector<std::wstring> ExtractQuotedStrings(std::wstring_view text) {
  std::vector<std::wstring> result;
  std::wstring current;
  bool in_quote = false;
  for (wchar_t ch : text) {
    if (ch == L'"') {
      if (in_quote) {
        result.emplace_back(Trim(current));
        current.clear();
      }
      in_quote = !in_quote;
      continue;
    }
    if (in_quote) {
      current.push_back(ch);
    }
  }
  return result;
}

std::wstring ConvertMultiByteToWide(const char* data, int size, UINT codepage) {
  if (size <= 0) {
    return L"";
  }
  int length = MultiByteToWideChar(codepage, 0, data, size, nullptr, 0);
  if (length <= 0) {
    return L"";
  }
  std::wstring result(length, L'\0');
  MultiByteToWideChar(codepage, 0, data, size, result.data(), length);
  return result;
}

std::wstring ReadFileAsWideString(const std::wstring& path) {
  HANDLE file = ::CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return L"";
  }
  DWORD size = ::GetFileSize(file, nullptr);
  if (size == INVALID_FILE_SIZE || size == 0) {
    ::CloseHandle(file);
    return L"";
  }

  std::vector<char> buffer(size);
  DWORD bytes_read = 0;
  if (!::ReadFile(file, buffer.data(), size, &bytes_read, nullptr)) {
    ::CloseHandle(file);
    return L"";
  }
  ::CloseHandle(file);
  if (bytes_read < size) {
    buffer.resize(bytes_read);
  }
  if (buffer.size() < 2) {
    return ConvertMultiByteToWide(buffer.data(),
                                  static_cast<int>(buffer.size()), CP_ACP);
  }

  const unsigned char b0 = static_cast<unsigned char>(buffer[0]);
  const unsigned char b1 = static_cast<unsigned char>(buffer[1]);
  if (b0 == 0xFF && b1 == 0xFE) {  // UTF-16LE
    size_t wchar_count = (buffer.size() - 2) / 2;
    const wchar_t* data =
        reinterpret_cast<const wchar_t*>(buffer.data() + 2);
    std::wstring text(data, data + wchar_count);
    while (!text.empty() && text.back() == L'\0') {
      text.pop_back();
    }
    return text;
  }
  if (b0 == 0xFE && b1 == 0xFF) {  // UTF-16BE
    std::wstring text;
    text.reserve((buffer.size() - 2) / 2);
    for (size_t i = 2; i + 1 < buffer.size(); i += 2) {
      wchar_t ch = (static_cast<unsigned char>(buffer[i]) << 8) |
                   static_cast<unsigned char>(buffer[i + 1]);
      text.push_back(ch);
    }
    while (!text.empty() && text.back() == L'\0') {
      text.pop_back();
    }
    return text;
  }
  if (buffer.size() >= 3 &&
      static_cast<unsigned char>(buffer[0]) == 0xEF &&
      static_cast<unsigned char>(buffer[1]) == 0xBB &&
      static_cast<unsigned char>(buffer[2]) == 0xBF) {
    return ConvertMultiByteToWide(buffer.data() + 3,
                                  static_cast<int>(buffer.size() - 3),
                                  CP_UTF8);
  }
  return ConvertMultiByteToWide(buffer.data(),
                                static_cast<int>(buffer.size()), CP_ACP);
}
}  // namespace

Config& Config::Instance() {
  static Config instance;
  return instance;
}

Config::Config() {
  LoadConfig();
}

void Config::LoadConfig() {
  // general
  command_line_ = GetIniString(L"general", L"command_line", L"");
  launch_on_startup_ = GetIniString(L"general", L"launch_on_startup", L"");
  launch_on_exit_ = GetIniString(L"general", L"launch_on_exit", L"");
  user_data_dir_ = LoadDirPath(L"data");
  disk_cache_dir_ = LoadDirPath(L"cache");
  boss_key_ = GetIniString(L"general", L"boss_key", L"");
  translate_key_ = GetIniString(L"general", L"translate_key", L"");
  key_mappings_ = LoadKeyMappings();
  show_password_ = ::GetPrivateProfileIntW(L"general", L"show_password", 1,
                                           GetIniPath().c_str()) != 0;
  win32k_ = ::GetPrivateProfileIntW(L"general", L"win32k", 0,
                                    GetIniPath().c_str()) != 0;

  // tabs
  keep_last_tab_ = ::GetPrivateProfileIntW(L"tabs", L"keep_last_tab", 1,
                                           GetIniPath().c_str()) != 0;
  double_click_close_ = ::GetPrivateProfileIntW(L"tabs", L"double_click_close",
                                                1, GetIniPath().c_str()) != 0;
  right_click_close_ = ::GetPrivateProfileIntW(L"tabs", L"right_click_close", 0,
                                               GetIniPath().c_str()) != 0;
  wheel_tab_ = ::GetPrivateProfileIntW(L"tabs", L"wheel_tab", 1,
                                       GetIniPath().c_str()) != 0;
  wheel_tab_when_press_rbutton_ =
      ::GetPrivateProfileIntW(L"tabs", L"wheel_tab_when_press_rbutton", 1,
                              GetIniPath().c_str()) != 0;
  open_url_new_tab_ = LoadOpenUrlNewTabMode();
  bookmark_new_tab_ = LoadBookmarkNewTabMode();
  new_tab_disable_ = ::GetPrivateProfileIntW(L"tabs", L"new_tab_disable", 1,
                                             GetIniPath().c_str()) != 0;
  disable_tab_name_ = GetIniString(L"tabs", L"new_tab_disable_name", L"");
}

std::wstring Config::LoadDirPath(const std::wstring& dir_type) {
  std::wstring path = CanonicalizePath(GetAppDir() + L"\\..\\" + dir_type);
  std::wstring dir_key = dir_type + L"_dir";
  std::wstring dir_buffer = GetIniString(L"general", dir_key, path);

  if (dir_buffer == L"none") {
    return L"";
  }

  if (dir_buffer.empty()) {
    dir_buffer = path;
  }

  std::wstring expanded_path = ExpandEnvironmentPath(dir_buffer);
  ReplaceStringInPlace(expanded_path, L"%app%", GetAppDir());
  return GetAbsolutePath(expanded_path);
}

int Config::LoadOpenUrlNewTabMode() {
  return ::GetPrivateProfileIntW(L"tabs", L"open_url_new_tab", 0,
                                 GetIniPath().c_str());
}
int Config::LoadBookmarkNewTabMode() {
  return ::GetPrivateProfileIntW(L"tabs", L"open_bookmark_new_tab", 0,
                                 GetIniPath().c_str());
}

std::vector<Config::KeyMapping> Config::LoadKeyMappings() {
  std::vector<KeyMapping> mappings;
  std::wstring ini_text = ReadFileAsWideString(GetIniPath());
  if (ini_text.empty()) {
    return mappings;
  }

  auto lines = StringSplit(ini_text, L'\n');
  bool in_section = false;
  bool collecting = false;
  bool saw_open = false;
  int bracket_depth = 0;
  std::wstring mapping_text;

  for (auto& raw_line : lines) {
    if (!raw_line.empty() && raw_line.back() == L'\r') {
      raw_line.pop_back();
    }
    std::wstring trimmed = Trim(raw_line);
    if (trimmed.empty() || trimmed.front() == L';') {
      continue;
    }

    if (!collecting && trimmed.front() == L'[') {
      size_t end = trimmed.find(L']');
      if (end == std::wstring::npos) {
        continue;
      }
      std::wstring section = Trim(trimmed.substr(1, end - 1));
      in_section = EqualsIgnoreCase(section, L"keymapping");
      continue;
    }

    if (!in_section) {
      continue;
    }

    if (!collecting) {
      size_t eq = trimmed.find(L'=');
      if (eq == std::wstring::npos) {
        continue;
      }
      std::wstring key = Trim(trimmed.substr(0, eq));
      if (!EqualsIgnoreCase(key, L"key_mapping")) {
        continue;
      }
      std::wstring value = StripInlineComment(trimmed.substr(eq + 1));
      mapping_text += value;
      mapping_text += L"\n";
      bracket_depth += CountBracketDelta(value, &saw_open);
      if (saw_open && bracket_depth <= 0) {
        break;
      }
      if (saw_open) {
        collecting = (bracket_depth > 0);
      }
    } else {
      std::wstring value = StripInlineComment(trimmed);
      mapping_text += value;
      mapping_text += L"\n";
      bracket_depth += CountBracketDelta(value, &saw_open);
      if (saw_open && bracket_depth <= 0) {
        break;
      }
    }
  }

  if (mapping_text.empty()) {
    return mappings;
  }

  auto parts = ExtractQuotedStrings(mapping_text);
  for (size_t i = 0; i + 1 < parts.size(); i += 2) {
    UINT from_hotkey = ParseHotkeys(parts[i]);
    UINT to_hotkey = ParseHotkeys(parts[i + 1]);
    UINT from_vk = HIWORD(from_hotkey);
    UINT to_vk = HIWORD(to_hotkey);
    if (from_vk == 0 || to_vk == 0) {
      continue;
    }
    KeyMapping mapping;
    mapping.from_modifiers = LOWORD(from_hotkey) & kModifierMask;
    mapping.from_vk = from_vk;
    mapping.to_modifiers = LOWORD(to_hotkey) & kModifierMask;
    mapping.to_vk = to_vk;
    mappings.emplace_back(mapping);
  }

  return mappings;
}

const Config& config = Config::Instance();
