#ifndef CONFIG_H_
#define CONFIG_H_

std::wstring GetCrCommandLine() {
  auto commandLine = GetIniString(L"general", L"command_line", L"");
  if (!commandLine.empty()) {
    return commandLine;
  }
  return GetIniString(L"General", L"CommandLine", L"");  // Deprecated
}
std::wstring GetLaunchOnStartup() {
  return GetIniString(L"general", L"launch_on_startup", L"");
}

bool IsKillLaunchOnExit() {
  return ::GetPrivateProfileIntW(L"general", L"kill_launch_on_exit", 0,
                                 kIniPath.c_str()) != 0;
}

std::wstring GetLaunchOnExit() {
  return GetIniString(L"general", L"launch_on_exit", L"");
}

std::wstring GetDirPath(const std::wstring& dir_type) {
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
  ReplaceStringIni(expanded_path, L"%app%", GetAppDir());
  std::wstring dir = GetAbsolutePath(expanded_path);
  return dir;
}

std::wstring GetUserDataDir() {
  return GetDirPath(L"data");
}

std::wstring GetDiskCacheDir() {
  return GetDirPath(L"cache");
}

std::wstring GetBosskey() {
  auto key = GetIniString(L"general", L"boss_key", L"");
  if (!key.empty()) {
    return key;
  }
  return GetIniString(L"General", L"Bosskey", L"");  // Deprecated
}

std::wstring GetTranslateKey() {
  auto key = GetIniString(L"general", L"translate_key", L"");
  if (!key.empty()) {
    return key;
  }
  return GetIniString(L"General", L"TranslateKey", L"");  // Deprecated
}

// View password without verification
bool IsShowPassword() {
  return ::GetPrivateProfileIntW(L"general", L"show_password", 1,
                                 kIniPath.c_str()) != 0;
}

// Force enable win32k
bool IsWin32K() {
  return ::GetPrivateProfileIntW(L"general", L"win32k", 0, kIniPath.c_str()) !=
         0;
}

bool IsKeepLastTab() {
  return ::GetPrivateProfileIntW(L"tabs", L"keep_last_tab", 1,
                                 kIniPath.c_str()) != 0;
}

bool IsDoubleClickClose() {
  return ::GetPrivateProfileIntW(L"tabs", L"double_click_close", 1,
                                 kIniPath.c_str()) != 0;
}

bool IsRightClickClose() {
  return ::GetPrivateProfileIntW(L"tabs", L"right_click_close", 0,
                                 kIniPath.c_str()) != 0;
}

bool IsWheelTab() {
  return ::GetPrivateProfileIntW(L"tabs", L"wheel_tab", 1, kIniPath.c_str()) !=
         0;
}

bool IsWheelTabWhenPressRightButton() {
  return ::GetPrivateProfileIntW(L"tabs", L"wheel_tab_when_press_rbutton", 1,
                                 kIniPath.c_str()) != 0;
}

std::string IsOpenUrlNewTabFun() {
  int value = ::GetPrivateProfileIntW(L"tabs", L"open_url_new_tab", 0,
                                      kIniPath.c_str());
  switch (value) {
    case 1:
      return "foreground";
    case 2:
      return "background";
    default:
      return "disabled";
  }
}

std::string IsBookmarkNewTab() {
  int value = ::GetPrivateProfileIntW(L"tabs", L"open_bookmark_new_tab", 0,
                                      kIniPath.c_str());
  switch (value) {
    case 1:
      return "foreground";
    case 2:
      return "background";
    default:
      return "disabled";
  }
}

bool IsNewTabDisable() {
  return ::GetPrivateProfileIntW(L"tabs", L"new_tab_disable", 1,
                                 kIniPath.c_str()) != 0;
}

// Customize disabled tab page name
std::wstring GetDisableTabName() {
  return GetIniString(L"tabs", L"new_tab_disable_name", L"");
}

#endif  // CONFIG_H_
