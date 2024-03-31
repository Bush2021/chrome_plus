#ifndef CONFIG_H_
#define CONFIG_H_

const std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";

// 尝试读取 ini 文件
bool IsIniExist() {
  if (PathFileExists(IniPath.data())) {
    return true;
  }
  return false;
}

// 如果 ini 存在，从中读取 CommandLine；如果 ini 不存在，或者存在，但是 CommandLine 为空，则返回空字符串
std::wstring GetCrCommandLine() {
  if (IsIniExist()) {
    std::vector<TCHAR> CommandLineBuffer(1024);  // 初始大小为 1024
    DWORD bytesRead = ::GetPrivateProfileStringW(
        L"General", L"CommandLine", L"", CommandLineBuffer.data(),
        CommandLineBuffer.size(), IniPath.c_str());

    // 如果读取的字符数接近缓冲区的大小，可能需要更大的缓冲区
    while (bytesRead >= CommandLineBuffer.size() - 1) {
      CommandLineBuffer.resize(CommandLineBuffer.size() * 2);
      bytesRead = ::GetPrivateProfileStringW(
          L"General", L"CommandLine", L"", CommandLineBuffer.data(),
          CommandLineBuffer.size(), IniPath.c_str());
    }

    return std::wstring(CommandLineBuffer.data());
  } else {
    return std::wstring(L"");
  }
}

// 读取 UserData 和 DiskCache
std::wstring GetDirPath(const std::wstring& dirType) {
  if (IsIniExist()) {
    std::wstring path = GetAppDir() + L"\\..\\" + dirType;
    TCHAR temp[MAX_PATH];
    ::PathCanonicalize(temp, path.data());

    if (!PathFileExists(IniPath.c_str())) {
      return GetAppDir() + L"\\..\\" + dirType;
    }

    TCHAR DirBuffer[MAX_PATH];
    ::GetPrivateProfileStringW(L"General", (dirType + L"Dir").c_str(), temp,
                               DirBuffer, MAX_PATH, IniPath.c_str());

    if (DirBuffer[0] == 0) {
      ::PathCanonicalize(DirBuffer, path.data());
    }

    std::wstring ExpandedPath = ExpandEnvironmentPath(DirBuffer);

    ReplaceStringIni(ExpandedPath, L"%app%", GetAppDir());
    std::wstring Dir;
    Dir = GetAbsolutePath(ExpandedPath);
    wcscpy(DirBuffer, Dir.c_str());

    return std::wstring(DirBuffer);
  } else {
    std::wstring path = GetAppDir() + L"\\..\\" + dirType;
    TCHAR temp[MAX_PATH];
    ::PathCanonicalize(temp, path.data());
    return temp;
  }
}

std::wstring GetUserDataDir() {
  return GetDirPath(L"Data");
}

std::wstring GetDiskCacheDir() {
  return GetDirPath(L"Cache");
}

// 如果启用老板键，则读取 ini 文件中的老板键设置；如果 ini 不存在或者该值为空，则返回空字符串
std::wstring GetBosskey() {
  if (IsIniExist()) {
    TCHAR BosskeyBuffer[100];
    ::GetPrivateProfileStringW(L"General", L"Bosskey", L"", BosskeyBuffer, 100,
                               IniPath.c_str());
    return std::wstring(BosskeyBuffer);
  } else {
    return std::wstring(L"");
  }
}

// 定义翻译快捷键
std::wstring GetTranslateKey() {
  if (IsIniExist()) {
    TCHAR TranslateKeyBuffer[100];
    ::GetPrivateProfileStringW(L"General", L"TranslateKey", L"",
                               TranslateKeyBuffer, 100, IniPath.c_str());
    return std::wstring(TranslateKeyBuffer);
  } else {
    return std::wstring(L"");
  }
}

// 保留最后一个标签
bool IsKeepLastTabFun() {
  if (::GetPrivateProfileIntW(L"Tabs", L"keep_last_tab", 1, IniPath.c_str()) ==
      0) {
    return false;
  }

  return true;
}

// 双击关闭
bool IsDblClkFun() {
  if (::GetPrivateProfileIntW(L"Tabs", L"double_click_close", 1,
                              IniPath.c_str()) == 0) {
    return false;
  }

  return true;
}

// 单击右键关闭
bool IsRClkFun() {
  if (::GetPrivateProfileIntW(L"Tabs", L"right_click_close", 0,
                              IniPath.c_str()) == 0) {
    return false;
  }

  return true;
}

// 鼠标停留在标签栏时滚轮切换标签
bool IsWheelTabFun() {
  if (::GetPrivateProfileIntW(L"Tabs", L"wheel_tab", 1, IniPath.c_str()) == 0) {
    return false;
  }

  return true;
}

// 在任何位置按住右键时滚轮切换标签
bool IsWheelTabWhenPressRButtonFun() {
  if (::GetPrivateProfileIntW(L"Tabs", L"wheel_tab_when_press_rbutton", 1,
                              IniPath.c_str()) == 0) {
    return false;
  }

  return true;
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
  if (::GetPrivateProfileIntW(L"Tabs", L"new_tab_disable", 1,
                              IniPath.c_str()) == 0) {
    return false;
  }

  return true;
}

#endif  // CONFIG_H_
