#ifndef CONFIG_H
#define CONFIG_H

// 尝试读取 ini 文件
bool IsIniExist()
{
    std::wstring path = GetAppDir() + L"\\chrome++.ini";
    if (PathFileExists(path.data()))
    {
        return true;
    }
    return false;
}

// 如果 ini 存在，从中读取 CommandLine；如果 ini 不存在，或者存在，但是 CommandLine 为空，则返回空字符串
std::wstring GetCrCommandLine()
{
  if (IsIniExist())
  {
    std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
    std::vector<TCHAR> CommandLineBuffer(1024); // 初始大小为 1024
    DWORD bytesRead = ::GetPrivateProfileStringW(L"General", L"CommandLine", L"", CommandLineBuffer.data(), CommandLineBuffer.size(), IniPath.c_str());

    // 如果读取的字符数接近缓冲区的大小，可能需要更大的缓冲区
    while (bytesRead >= CommandLineBuffer.size() - 1)
    {
      CommandLineBuffer.resize(CommandLineBuffer.size() * 2);
      bytesRead = ::GetPrivateProfileStringW(L"General", L"CommandLine", L"", CommandLineBuffer.data(), CommandLineBuffer.size(), IniPath.c_str());
    }

    return std::wstring(CommandLineBuffer.data());
  }
  else
  {
    return std::wstring(L"");
  }
}

// 如果 ini 存在，读取 UserData 并配置，否则使用默认值
std::wstring GetUserDataDir()
{
    if (IsIniExist())
    {
        std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
        // 修改 Chrome 默认 Data 路径
        std::wstring path = GetAppDir() + L"\\..\\Data";
        TCHAR temp[MAX_PATH];
        ::PathCanonicalize(temp, path.data());

        if (!PathFileExists(IniPath.c_str()))
        {
            return GetAppDir() + L"\\..\\Data";
        }

        TCHAR UserDataBuffer[MAX_PATH];
        ::GetPrivateProfileStringW(L"General", L"DataDir", temp, UserDataBuffer, MAX_PATH, IniPath.c_str());

        // 若 ini 中 DataDir 留空，则按照默认情况处理
        if (UserDataBuffer[0] == 0)
        {
            ::PathCanonicalize(UserDataBuffer, path.data());
        }

        std::wstring ExpandedPath = ExpandEnvironmentPath(UserDataBuffer);

        // 替换 %app%
        ReplaceStringIni(ExpandedPath, L"%app%", GetAppDir());
        std::wstring DataDir;
        DataDir = GetAbsolutePath(ExpandedPath);

        wcscpy(UserDataBuffer, DataDir.c_str());

        return std::wstring(UserDataBuffer);
    }
    else
    {
        std::wstring path = GetAppDir() + L"\\..\\Data";
        TCHAR temp[MAX_PATH];
        ::PathCanonicalize(temp, path.data());
        return temp;
    }
}

// 如果 ini 存在，读取 DiskCache 并配置，否则使用默认值
std::wstring GetDiskCacheDir()
{
    if (IsIniExist())
    {
        std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
        // 修改 Chrome 默认 Cache 路径
        std::wstring path = GetAppDir() + L"\\..\\Cache";
        TCHAR temp[MAX_PATH];
        ::PathCanonicalize(temp, path.data());

        if (!PathFileExists(IniPath.c_str()))
        {
            return GetAppDir() + L"\\..\\Cache";
        }

        TCHAR CacheDirBuffer[MAX_PATH];
        ::GetPrivateProfileStringW(L"General", L"CacheDir", temp, CacheDirBuffer, MAX_PATH, IniPath.c_str());

        // 若 ini 中 CacheDir 留空，则按照默认情况处理
        if (CacheDirBuffer[0] == 0)
        {
            ::PathCanonicalize(CacheDirBuffer, path.data());
        }

        std::wstring ExpandedPath = ExpandEnvironmentPath(CacheDirBuffer);

        // 替换 %app%
        ReplaceStringIni(ExpandedPath, L"%app%", GetAppDir());
        std::wstring CacheDir;
        CacheDir = GetAbsolutePath(ExpandedPath);
        wcscpy(CacheDirBuffer, CacheDir.c_str());

        return std::wstring(CacheDirBuffer);
    }
    else
    {
        std::wstring path = GetAppDir() + L"\\..\\Cache";
        TCHAR temp[MAX_PATH];
        ::PathCanonicalize(temp, path.data());
        return temp;
    }
}

// 如果启用老板键，则读取 ini 文件中的老板键设置；如果 ini 不存在或者该值为空，则返回空字符串
std::wstring GetBosskey()
{
    if (IsIniExist())
    {
        std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
        TCHAR BosskeyBuffer[100];
        ::GetPrivateProfileStringW(L"General", L"Bosskey", L"", BosskeyBuffer, 100, IniPath.c_str());
        return std::wstring(BosskeyBuffer);
    }
    else
    {
        return std::wstring(L"");
    }
}

// 定义翻译快捷键
std::wstring GetTranslateKey()
{
    if (IsIniExist())
    {
        std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
        TCHAR TranslateKeyBuffer[100];
        ::GetPrivateProfileStringW(L"General", L"TranslateKey", L"", TranslateKeyBuffer, 100, IniPath.c_str());
        return std::wstring(TranslateKeyBuffer);
    }
    else
    {
        return std::wstring(L"");
    }
}

// 保留最后一个标签
bool IsKeepLastTabFun()
{
    std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
    if (::GetPrivateProfileIntW(L"Tabs", L"keep_last_tab", 1, IniPath.c_str()) == 0)
    {
        return false;
    }

    return true;
}

// 双击关闭
bool IsDblClkFun()
{
    std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
    if (::GetPrivateProfileIntW(L"Tabs", L"double_click_close", 1, IniPath.c_str()) == 0)
    {
        return false;
    }

    return true;
}

// 单击右键关闭
bool IsRClkFun()
{
    std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
    if (::GetPrivateProfileIntW(L"Tabs", L"right_click_close", 0, IniPath.c_str()) == 0)
    {
        return false;
    }

    return true;
}

// 鼠标停留在标签栏时滚轮切换标签
bool IsWheelTabFun()
{
    std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
    if (::GetPrivateProfileIntW(L"Tabs", L"wheel_tab", 1, IniPath.c_str()) == 0)
    {
        return false;
    }

    return true;
}

// 在任何位置按住右键时滚轮切换标签
bool IsWheelTabWhenPressRButtonFun()
{
    std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
    if (::GetPrivateProfileIntW(L"Tabs", L"wheel_tab_when_press_rbutton", 1, IniPath.c_str()) == 0)
    {
        return false;
    }

    return true;
}

// 地址栏输入网址在新标签页打开
bool IsOpenUrlNewTabFun()
{
    std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
    if (::GetPrivateProfileIntW(L"Tabs", L"open_url_new_tab", 0, IniPath.c_str()) == 0)
    {
        return false;
    }

    return true;
}

// 书签在新标签页打开
bool IsBookmarkNewTabFun()
{
    std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
    if (::GetPrivateProfileIntW(L"Tabs", L"open_bookmark_new_tab", 0, IniPath.c_str()) == 0)
    {
        return false;
    }

    return true;
}

// 新标签页不生效
bool IsNewTabDisableFun()
{
    std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
    if (::GetPrivateProfileIntW(L"Tabs", L"new_tab_disable", 1, IniPath.c_str()) == 0)
    {
        return false;
    }

    return true;
}

#endif // CONFIG_H
