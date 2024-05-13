#ifndef CONFIG_H_
#define CONFIG_H_

const std::wstring kIniPath = GetAppDir() + L"\\chrome++.ini";

// 读取 ini 文件
bool IsIniExist() {
  if (PathFileExists(kIniPath.data())) {
    return true;
  }
  return false;
}

std::wstring GetIniString(const std::wstring& section,
                          const std::wstring& key,
                          const std::wstring& default_value) {
  std::vector<TCHAR> buffer(100);
  DWORD bytesread = 0;
  do {
    bytesread = ::GetPrivateProfileStringW(
        section.c_str(), key.c_str(), default_value.c_str(), buffer.data(),
        (DWORD)buffer.size(), kIniPath.c_str());
    // 如果字符串过长，则倍增缓冲区大小
    if (bytesread >= buffer.size() - 1) {
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

// 老板键
std::wstring GetBosskey() {
  return IsIniExist() ? GetIniString(L"General", L"Bosskey", L"")
                      : std::wstring(L"");
}

// 定义翻译快捷键
std::wstring GetTranslateKey() {
  return IsIniExist() ? GetIniString(L"General", L"TranslateKey", L"")
                      : std::wstring(L"");
}

// 保留最后一个标签
bool IsKeepLastTab() {
  return ::GetPrivateProfileIntW(L"Tabs", L"keep_last_tab", 1,
                                 kIniPath.c_str()) != 0;
}

// 双击关闭
bool IsDoubleClickClose() {
  return ::GetPrivateProfileIntW(L"Tabs", L"double_click_close", 1,
                                 kIniPath.c_str()) != 0;
}

// 单击右键关闭
bool IsRightClickClose() {
  return ::GetPrivateProfileIntW(L"Tabs", L"right_click_close", 0,
                                 kIniPath.c_str()) != 0;
}

// 鼠标停留在标签栏时滚轮切换标签
bool IsWheelTab() {
  return ::GetPrivateProfileIntW(L"Tabs", L"wheel_tab", 1, kIniPath.c_str()) !=
         0;
}

// 在任何位置按住右键时滚轮切换标签
bool IsWheelTabWhenPressRightButton() {
  return ::GetPrivateProfileIntW(L"Tabs", L"wheel_tab_when_press_rbutton", 1,
                                 kIniPath.c_str()) != 0;
}

// 地址栏输入网址在新标签页打开
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

// 书签在新标签页打开
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

// 新标签页不生效
bool IsNewTabDisable() {
  return ::GetPrivateProfileIntW(L"Tabs", L"new_tab_disable", 1,
                                 kIniPath.c_str()) != 0;
}

// 自定义禁用标签页名称
std::wstring GetDisableTabName() {
  return IsIniExist() ? GetIniString(L"Tabs", L"new_tab_disable_name", L"")
                      : L"";
}

#endif  // CONFIG_H_
