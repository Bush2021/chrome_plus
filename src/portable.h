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
// bool IsExistsPortable()
//{
//    std::wstring path = GetAppDir() + L"\\portable";
//    if (PathFileExists(path.data()))
//    {
//        return true;
//    }
//    return false;
//}

// bool IsNeedPortable()
//{
//     return true;
//     static bool need_portable = IsExistsPortable();
//     return need_portable;
// }

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

            {
                auto userdata = GetUserDataDir();

                wchar_t temp[MAX_PATH];
                wsprintf(temp, L"--user-data-dir=%s", userdata.c_str());
                args.push_back(temp);
            }

            {
                auto diskcache = GetDiskCacheDir();

                wchar_t temp[MAX_PATH];
                wsprintf(temp, L"--disk-cache-dir=%s", diskcache.c_str());
                args.push_back(temp);
            }

            args.push_back(L"--disable-features=RendererCodeIntegrity");

            // 获取命令行，然后追加参数
            // 如果存在 = 号，参数会被识别成值
            // 修改方法是截取拆分，然后多次 args.push_back
            // 首先检测是否存在 =，不存在按照原有方法处理
            // 若存在，以匹配到的第一个 = 为中心，向前匹配 -- 为开头，向后匹配空格为结尾，把这整一段提取出来，单独 push_back
            // 然后再把提取出来的部分从原有的字符串中删除，再 push_back 剩下的部分
            // 重复上述过程，直到字符串中不再存在 = 号
            if (GetCrCommandLine().length() > 0)
            {
                auto cr_command_line = GetCrCommandLine();

                std::wstring temp = cr_command_line;
                while (true)
                {
                    auto pos = temp.find(L"--");
                    if (pos == std::wstring::npos)
                    {
                        break;
                    }
                    else
                    {
                        auto pos2 = temp.find(L" --", pos);
                        if (pos2 == std::wstring::npos)
                        {
                            args.push_back(temp);
                            break;
                        }
                        else
                        {
                            args.push_back(temp.substr(pos, pos2 - pos));
                            temp = temp.substr(0, pos) + temp.substr(pos2 + 1);
                        }
                    }
                }
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