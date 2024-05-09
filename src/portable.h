#ifndef PORTABLE_H_
#define PORTABLE_H_

std::wstring QuoteSpaceIfNeeded(const std::wstring& str) {
  if (str.find(L' ') == std::wstring::npos)
    return std::move(str);

  std::wstring escaped(L"\"");
  for (auto c : str) {
    if (c == L'"')
      escaped += L'"';
    escaped += c;
  }
  escaped += L'"';
  return std::move(escaped);
}

std::wstring JoinArgsString(std::vector<std::wstring> lines,
                            const std::wstring& delimiter) {
  std::wstring text;
  bool first = true;
  for (auto& line : lines) {
    if (!first)
      text += delimiter;
    else
      first = false;
    text += QuoteSpaceIfNeeded(line);
  }
  return text;
}

// Construct new command line with portable mode.
std::wstring GetCommand(LPWSTR param) {
  std::vector<std::wstring> args;

  int argc;
  LPWSTR* argv = CommandLineToArgvW(param, &argc);

  int insert_pos = 0;
  for (int i = 0; i < argc; i++) {
    if (std::wstring(argv[i]).find(L"--") != std::wstring::npos ||
        std::wstring(argv[i]).find(L"--single-argument") !=
            std::wstring::npos) {
      break;
    }
    insert_pos = i;
  }
  for (int i = 0; i < argc; i++) {
    // Preserve former arguments.
    if (i)
      args.push_back(argv[i]);

    // Append new arguments.
    if (i == insert_pos) {
      args.push_back(L"--portable");

      {
        auto userdata = GetUserDataDir();
        args.push_back(L"--user-data-dir=" + userdata);
      }

      {
        auto diskcache = GetDiskCacheDir();
        args.push_back(L"--disk-cache-dir=" + diskcache);
      }

      // Get the command line and append parameters
      // Intercept and split the parameters starting with each --,
      // and then args.push_back multiple times
      // Repeat the above process until the -- sign no longer exists in the
      // string
      {
        auto cr_command_line = GetCrCommandLine();
        std::wstring temp = cr_command_line;
        while (true) {
          auto pos = temp.find(L"--");
          if (pos == std::wstring::npos) {
            break;
          } else {
            auto pos2 = temp.find(L" --", pos);
            if (pos2 == std::wstring::npos) {
              args.push_back(temp);
              break;
            } else {
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

void Portable(LPWSTR param) {
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
  if (ShellExecuteEx(&sei)) {
    ExitProcess(0);
  }
}

#endif  // PORTABLE_H_
