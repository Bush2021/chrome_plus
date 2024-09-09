#ifndef CONFIG_H_
#define CONFIG_H_

const std::wstring kIniPath = GetAppDir() + L"\\chrome++.ini";

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

std::wstring GetCrCommandLine() {
  auto commandLine = GetIniString(L"general", L"command_line", L"");
  if (!commandLine.empty()) {
    return commandLine;
  }
  return GetIniString(L"General", L"CommandLine", L""); // Deprecated
  return L"";
}

std::wstring CanonicalizePath(const std::wstring& path) {
  TCHAR temp[MAX_PATH];
  ::PathCanonicalize(temp, path.data());
  return std::wstring(temp);
}

std::wstring GetDirPath(const std::wstring& dirType) {
  std::wstring path = CanonicalizePath(GetAppDir() + L"\\..\\" + dirType);

  std::wstring DirBuffer(MAX_PATH, '\0');
  ::GetPrivateProfileStringW(L"general", (dirType + L"_dir").c_str(),
                             path.c_str(), &DirBuffer[0], MAX_PATH,
                             kIniPath.c_str());

  // Deprecated
  if (DirBuffer[0] == 0) {
    ::GetPrivateProfileStringW(L"general", (dirType + L"dir").c_str(),
                               path.c_str(), &DirBuffer[0], MAX_PATH,
                               kIniPath.c_str());
  }

  if (DirBuffer[0] == 0) {
    DirBuffer = path;
  }

  std::wstring ExpandedPath = ExpandEnvironmentPath(DirBuffer);

  ReplaceStringIni(ExpandedPath, L"%app%", GetAppDir());
  std::wstring Dir = GetAbsolutePath(ExpandedPath);

  return Dir;
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
  return GetIniString(L"General", L"TranslateKey", L""); // Deprecated
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