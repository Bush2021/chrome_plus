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
  return GetIniString(L"General", L"CommandLine", L"");
}

std::wstring CanonicalizePath(const std::wstring& path) {
  TCHAR temp[MAX_PATH];
  ::PathCanonicalize(temp, path.data());
  return std::wstring(temp);
}

std::wstring GetDirPath(const std::wstring& dirType) {
  std::wstring path = CanonicalizePath(GetAppDir() + L"\\..\\" + dirType);

  std::wstring DirBuffer(MAX_PATH, '\0');
  ::GetPrivateProfileStringW(L"General", (dirType + L"Dir").c_str(),
                             path.c_str(), &DirBuffer[0], MAX_PATH,
                             kIniPath.c_str());

  if (DirBuffer[0] == 0) {
    DirBuffer = path;
  }

  std::wstring ExpandedPath = ExpandEnvironmentPath(DirBuffer);

  ReplaceStringIni(ExpandedPath, L"%app%", GetAppDir());
  std::wstring Dir = GetAbsolutePath(ExpandedPath);

  return Dir;
}

std::wstring GetUserDataDir() {
  return GetDirPath(L"Data");
}

std::wstring GetDiskCacheDir() {
  return GetDirPath(L"Cache");
}

std::wstring GetBosskey() {
  return GetIniString(L"General", L"Bosskey", L"");
}

std::wstring GetTranslateKey() {
  return GetIniString(L"General", L"TranslateKey", L"");
}

// View password without verification
bool IsShowPassword() {
  return ::GetPrivateProfileIntW(L"General", L"ShowPassword", 1,
                                 kIniPath.c_str()) != 0;
}

// Force enable win32k
bool IsWin32K() {
  return ::GetPrivateProfileIntW(L"general", L"win32k", 0, kIniPath.c_str()) !=
         0;
}

bool IsKeepLastTab() {
  return ::GetPrivateProfileIntW(L"Tabs", L"keep_last_tab", 1,
                                 kIniPath.c_str()) != 0;
}

bool IsDoubleClickClose() {
  return ::GetPrivateProfileIntW(L"Tabs", L"double_click_close", 1,
                                 kIniPath.c_str()) != 0;
}

bool IsRightClickClose() {
  return ::GetPrivateProfileIntW(L"Tabs", L"right_click_close", 0,
                                 kIniPath.c_str()) != 0;
}

bool IsWheelTab() {
  return ::GetPrivateProfileIntW(L"Tabs", L"wheel_tab", 1, kIniPath.c_str()) !=
         0;
}

bool IsWheelTabWhenPressRightButton() {
  return ::GetPrivateProfileIntW(L"Tabs", L"wheel_tab_when_press_rbutton", 1,
                                 kIniPath.c_str()) != 0;
}

std::string IsOpenUrlNewTabFun() {
  int value = ::GetPrivateProfileIntW(L"Tabs", L"open_url_new_tab", 0,
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
  int value = ::GetPrivateProfileIntW(L"Tabs", L"open_bookmark_new_tab", 0,
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
  return ::GetPrivateProfileIntW(L"Tabs", L"new_tab_disable", 1,
                                 kIniPath.c_str()) != 0;
}

// Customize disabled tab page name
std::wstring GetDisableTabName() {
  return GetIniString(L"Tabs", L"new_tab_disable_name", L"");
}

#endif  // CONFIG_H_