#include "config.h"

#include <windows.h>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "utils.h"

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
  disable_tab_names_ = StringSplit(disable_tab_name_, L',', L"\"");

  // keymapping
  LoadKeyMappings();
}

void Config::LoadKeyMappings() {
  std::vector<wchar_t> buffer(4096);
  const DWORD chars_read = ::GetPrivateProfileSectionW(
      L"keymapping", buffer.data(), static_cast<DWORD>(buffer.size()),
      GetIniPath().c_str());

  if (chars_read == 0) {
    return;
  }

  const wchar_t* current = buffer.data();
  while (*current != L'\0') {
    const std::wstring_view line(current);
    current += line.length() + 1;

    const auto eq_pos = line.find(L'=');
    if (eq_pos == std::wstring_view::npos || eq_pos == 0) {
      continue;
    }

    std::wstring_view key = line.substr(0, eq_pos);
    std::wstring_view value = line.substr(eq_pos + 1);

    while (!key.empty() && (key.back() == L' ' || key.back() == L'\t')) {
      key.remove_suffix(1);
    }
    while (!value.empty() &&
           (value.front() == L' ' || value.front() == L'\t')) {
      value.remove_prefix(1);
    }

    if (!key.empty() && !value.empty()) {
      key_mappings_.emplace_back(std::wstring(key), std::wstring(value));
    }
  }
}

std::optional<std::wstring> Config::LoadDirPath(const std::wstring& dir_type) {
  std::wstring path = CanonicalizePath(GetAppDir() + L"\\..\\" + dir_type);
  std::wstring dir_key = dir_type + L"_dir";
  std::wstring dir_buffer = GetIniString(L"general", dir_key, path);

  if (dir_buffer == L"none") {
    return std::nullopt;
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

const Config& config = Config::Instance();
