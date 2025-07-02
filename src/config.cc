#include "config.h"

#include <windows.h>

#include <string>

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
  command_line_ = LoadCommandLine();
  launch_on_startup_ = GetIniString(L"general", L"launch_on_startup", L"");
  launch_on_exit_ = GetIniString(L"general", L"launch_on_exit", L"");
  user_data_dir_ = LoadDirPath(L"data");
  disk_cache_dir_ = LoadDirPath(L"cache");
  boss_key_ = LoadBossKey();
  translate_key_ = LoadTranslateKey();
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

std::wstring Config::LoadCommandLine() {
  auto commandLine = GetIniString(L"general", L"command_line", L"");
  if (!commandLine.empty()) {
    return commandLine;
  }
  return GetIniString(L"General", L"CommandLine", L"");  // Deprecated
}

std::wstring Config::LoadDirPath(const std::wstring& dir_type) {
  std::wstring path = CanonicalizePath(GetAppDir() + L"\\..\\" + dir_type);
  std::wstring dir_key = dir_type + L"_dir";
  std::wstring dir_buffer = GetIniString(L"general", dir_key, path);

  if (dir_buffer == L"none") {
    return L"";
  }

  if (dir_buffer.empty()) {  // Deprecated
    dir_key = dir_type + L"dir";
    dir_buffer = GetIniString(L"general", dir_key, path);
  }

  if (dir_buffer.empty()) {
    dir_buffer = path;
  }

  std::wstring expanded_path = ExpandEnvironmentPath(dir_buffer);
  ReplaceStringInPlace(expanded_path, L"%app%", GetAppDir());
  return GetAbsolutePath(expanded_path);
}

std::wstring Config::LoadBossKey() {
  auto key = GetIniString(L"general", L"boss_key", L"");
  if (!key.empty()) {
    return key;
  }
  return GetIniString(L"General", L"Bosskey", L"");  // Deprecated
}

std::wstring Config::LoadTranslateKey() {
  auto key = GetIniString(L"general", L"translate_key", L"");
  if (!key.empty()) {
    return key;
  }
  return GetIniString(L"General", L"TranslateKey", L"");  // Deprecated
}
std::string Config::LoadOpenUrlNewTabMode() {
  int value = ::GetPrivateProfileIntW(L"tabs", L"open_url_new_tab", 0,
                                      GetIniPath().c_str());
  switch (value) {
    case 1:
      return "foreground";
    case 2:
      return "background";
    default:
      return "disabled";
  }
}
std::string Config::LoadBookmarkNewTabMode() {
  int value = ::GetPrivateProfileIntW(L"tabs", L"open_bookmark_new_tab", 0,
                                      GetIniPath().c_str());
  switch (value) {
    case 1:
      return "foreground";
    case 2:
      return "background";
    default:
      return "disabled";
  }
}

const Config& config = Config::Instance();
