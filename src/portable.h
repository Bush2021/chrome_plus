std::wstring QuoteSpaceIfNeeded(const std::wstring &str)
{
    if (str.find(L' ') == std::wstring::npos)
        return std::move(str);

    std::wstring escaped(L"\"");
    for (auto c : str)
    {
        if (c == L'"')
            escaped += L'"';
        escaped += c;
    }
    escaped += L'"';
    return std::move(escaped);
}

std::wstring JoinArgsString(std::vector<std::wstring> lines, const std::wstring &delimiter)
{
    std::wstring text;
    bool first = true;
    for (auto &line : lines)
    {
        if (!first)
            text += delimiter;
        else
            first = false;
        text += QuoteSpaceIfNeeded(line);
    }
    return text;
}

// 这段代码应该可以废弃了……
bool IsExistsPortable()
{
    std::wstring path = GetAppDir() + L"\\portable";
    if (PathFileExists(path.data()))
    {
        return true;
    }
    return false;
}

bool IsNeedPortable()
{
    return true;
    static bool need_portable = IsExistsPortable();
    return need_portable;
}

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

// 如果 ini 存在，读取 UserData 并配置，否则使用默认值
std::wstring GetUserDataDir()
{
    if (IsIniExist())
    {
        std::wstring IniDir = GetAppDir() + L"\\chrome++.ini";
        std::wstring path = GetAppDir() + L"\\..\\Data";
        TCHAR temp[MAX_PATH];
        ::PathCanonicalize(temp, path.data());

        if (!PathFileExists(IniDir.c_str()))
        {
            return GetAppDir() + L"\\..\\Data";
        }

        TCHAR UserDataBuffer[MAX_PATH];
        ::GetPrivateProfileStringW(L"General", L"DataDir", temp, UserDataBuffer, MAX_PATH, IniDir.c_str());
        if (UserDataBuffer[0] == 0)
        {
            ::PathCanonicalize(UserDataBuffer, path.data());
        }
        std::wstring expandedPath = ExpandEnvironmentPath(UserDataBuffer);

        // 替换 %app%
        ReplaceStringInPlace(expandedPath, L"%app%", GetAppDir());
        wcscpy(UserDataBuffer, expandedPath.c_str());

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
        std::wstring IniDir = GetAppDir() + L"\\chrome++.ini";
        std::wstring path = GetAppDir() + L"\\..\\Cache";
        TCHAR temp[MAX_PATH];
        ::PathCanonicalize(temp, path.data());

        if (!PathFileExists(IniDir.c_str()))
        {
            return GetAppDir() + L"\\..\\Cache";
        }

        TCHAR CacheDirBuffer[MAX_PATH];
        ::GetPrivateProfileStringW(L"General", L"CacheDir", temp, CacheDirBuffer, MAX_PATH, IniDir.c_str());
        if (CacheDirBuffer[0] == 0)
        {
            ::PathCanonicalize(CacheDirBuffer, path.data());
        }
        std::wstring expandedPath = ExpandEnvironmentPath(CacheDirBuffer);

        // 替换 %app%
        ReplaceStringInPlace(expandedPath, L"%app%", GetAppDir());
        wcscpy(CacheDirBuffer, expandedPath.c_str());

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

// 构造新命令行
std::wstring GetCommand(LPWSTR param)
{
    std::vector<std::wstring> args;

    int argc;
    LPWSTR *argv = CommandLineToArgvW(param, &argc);

    int insert_pos = 0;
    for (int i = 0; i < argc; i++)
    {
        if (wcscmp(argv[i], L"--") == 0)
        {
            break;
        }
        if (wcscmp(argv[i], L"--single-argument") == 0)
        {
            break;
        }
        insert_pos = i;
    }
    for (int i = 0; i < argc; i++)
    {
        // 保留原来参数
        if (i)
            args.push_back(argv[i]);

        // 追加参数
        if (i == insert_pos)
        {
            args.push_back(L"--portable");

            args.push_back(L"--no-first-run");

            args.push_back(L"--disable-features=RendererCodeIntegrity,FlashDeprecationWarning");

            // if (IsNeedPortable())
            {
                auto diskcache = GetDiskCacheDir();

                wchar_t temp[MAX_PATH];
                wsprintf(temp, L"--disk-cache-dir=%s", diskcache.c_str());
                args.push_back(temp);
            }
            {
                auto userdata = GetUserDataDir();

                wchar_t temp[MAX_PATH];
                wsprintf(temp, L"--user-data-dir=%s", userdata.c_str());
                args.push_back(temp);
            }
        }
    }
    LocalFree(argv);

    return JoinArgsString(args, L" ");
}

void Portable(LPWSTR param)
{
    wchar_t path[MAX_PATH];
    ::GetModuleFileName(NULL, path, MAX_PATH);

    std::wstring args = GetCommand(param);

    SHELLEXECUTEINFO sei = {0};
    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"open";
    sei.lpFile = path;
    sei.nShow = SW_SHOWNORMAL;

    sei.lpParameters = args.c_str();
    if (ShellExecuteEx(&sei))
    {
        ExitProcess(0);
    }
}
