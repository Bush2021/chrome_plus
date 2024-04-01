#ifndef CONFIG_H_
#define CONFIG_H_

const std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";

// 读取 ini 文件
bool IsIniExist() {
  if (PathFileExists(IniPath.data())) {
    return true;
  }
  return false;
}

std::wstring GetIniString(const std::wstring& section, const std::wstring& key, const std::wstring& default_value) {
  std::vector<TCHAR> buffer(100);
  DWORD bytesRead = 0;
  do {
    bytesRead = ::GetPrivateProfileStringW(section.c_str(), key.c_str(), default_value.c_str(),
                                           buffer.data(), buffer.size(), IniPath.c_str());
    // 如果字符串过长，则倍增缓冲区大小
    if (bytesRead >= buffer.size() - 1) {
      buffer.resize(buffer.size() * 2);
    } else {
      break;
    }
  } while (true);

  return std::wstring(buffer.data());
}

// 追加参数
std::wstring GetCrCommandLine() {
  return IsIniExist() ? GetIniString(L"General", L"CommandLine", L"") : L"";
}

// 读取 UserData 和 DiskCache
std::wstring CanonicalizePath(const std::wstring& path) {
  TCHAR temp[MAX_PATH];
  ::PathCanonicalize(temp, path.data());
  return std::wstring(temp);
}

std::wstring GetDirPath(const std::wstring& dirType) {
  std::wstring path = CanonicalizePath(GetAppDir() + L"\\..\\" + dirType);

  if (!IsIniExist()) {
    return path;
  }

  std::wstring DirBuffer(MAX_PATH, '\0');
  ::GetPrivateProfileStringW(L"General", (dirType + L"Dir").c_str(), path.c_str(),
                             &DirBuffer[0], MAX_PATH, IniPath.c_str());

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

// 老板键
std::wstring GetBosskey() {
  return IsIniExist() ? GetIniString(L"General", L"Bosskey", L"") : std::wstring(L"");
}

// 定义翻译快捷键
std::wstring GetTranslateKey() {
  return IsIniExist() ? GetIniString(L"General", L"TranslateKey", L"") : std::wstring(L"");
}

// 保留最后一个标签
bool IsKeepLastTabFun() {
  return ::GetPrivateProfileIntW(L"Tabs", L"keep_last_tab", 1, IniPath.c_str()) != 0;
}

// 双击关闭
bool IsDblClkFun() {
  return ::GetPrivateProfileIntW(L"Tabs", L"double_click_close", 1, IniPath.c_str()) != 0;
}

// 单击右键关闭
bool IsRClkFun() {
  return ::GetPrivateProfileIntW(L"Tabs", L"right_click_close", 0, IniPath.c_str()) != 0;
}

// 鼠标停留在标签栏时滚轮切换标签
bool IsWheelTabFun() {
  return ::GetPrivateProfileIntW(L"Tabs", L"wheel_tab", 1, IniPath.c_str()) != 0;
}

// 在任何位置按住右键时滚轮切换标签
bool IsWheelTabWhenPressRButtonFun() {
  return ::GetPrivateProfileIntW(L"Tabs", L"wheel_tab_when_press_rbutton", 1, IniPath.c_str()) != 0;
}

// 地址栏输入网址在新标签页打开
std::string IsOpenUrlNewTabFun() {
  int value =
      ::GetPrivateProfileIntW(L"Tabs", L"open_url_new_tab", 0, IniPath.c_str());
  switch (value) {
    case 1:
      return "foreground";
    case 2:
      return "background";
    default:
      return "disabled";
  }
}

// 书签在新标签页打开
std::string IsBookmarkNewTabFun() {
  int value = ::GetPrivateProfileIntW(L"Tabs", L"open_bookmark_new_tab", 0,
                                      IniPath.c_str());
  switch (value) {
    case 1:
      return "foreground";
    case 2:
      return "background";
    default:
      return "disabled";
  }
}

// 新标签页不生效
bool IsNewTabDisableFun() {
  return ::GetPrivateProfileIntW(L"Tabs", L"new_tab_disable", 1, IniPath.c_str()) != 0;
}

// 自定义禁用标签页名称
std::wstring GetDisableTabName() {
  return IsIniExist() ? GetIniString(L"Tabs", L"new_tab_disable_name", L"") : L"";
}

#endif  // CONFIG_H_
