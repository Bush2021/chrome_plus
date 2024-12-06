#ifndef PORTABLE_H_
#define PORTABLE_H_

// Construct new command line with portable mode.
std::wstring GetCommand(LPWSTR param) {
  std::vector<std::wstring> args;

  int argc;
  LPWSTR* argv = CommandLineToArgvW(param, &argc);

  int insert_pos = 0;
  for (int i = 0; i < argc; ++i) {
    if (std::wstring(argv[i]).find(L"--") != std::wstring::npos ||
        std::wstring(argv[i]).find(L"--single-argument") !=
            std::wstring::npos) {
      break;
    }
    insert_pos = i;
  }
  for (int i = 0; i < argc; ++i) {
    // Preserve former arguments.
    if (i)
      args.push_back(argv[i]);

    // Append new arguments.
    if (i == insert_pos) {
      args.push_back(L"--portable");

      args.push_back(L"--disable-features=WinSboxNoFakeGdiInit");

      auto userdata = GetUserDataDir();
      if (!userdata.empty()) {
        args.push_back(L"--user-data-dir=" + userdata);
      }

      auto diskcache = GetDiskCacheDir();
      if (!diskcache.empty()) {
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

HANDLE hMutex = nullptr;
bool IsFirstRun() {
  DWORD pid = GetCurrentProcessId();
  std::wstring mutex_name =
      L"Global\\ChromePlusFirstRunMutex" + std::to_wstring(pid);
  hMutex = CreateMutexW(nullptr, TRUE, mutex_name.c_str());
  if (hMutex == nullptr || GetLastError() == ERROR_ALREADY_EXISTS) {
    return false;
  }
  return true;
}

void LaunchCommands(const std::wstring& get_commands,
                    int show_command,
                    std::vector<HANDLE>* program_handles) {
  auto commands = StringSplit(
      get_commands, L';',
      L"");  // Quotes should not be used as they can cause errors with paths
             // that contain spaces. Since semicolons rarely appear in names and
             // commands, they are used as delimiters.
  if (commands.empty()) {
    return;
  }
  for (const auto& command : commands) {
    std::wstring expanded_path = ExpandEnvironmentPath(command);
    ReplaceStringInPlace(expanded_path, L"%app%", GetAppDir());
    HANDLE handle = RunExecute(expanded_path.c_str(), show_command);
    if (program_handles != nullptr && handle != nullptr) {
      program_handles->push_back(handle);
    }
  }
}

void KillLaunchOnExit(std::vector<HANDLE>* program_handles) {
  if (IsKillLaunchOnExit() && program_handles != nullptr) {
    for (auto handle : *program_handles) {
      TerminateProcess(handle, 0);
    }
  }
}

void Portable(LPWSTR param) {
  bool first_run = IsFirstRun();
  auto launch_on_startup = GetLaunchOnStartup();
  auto launch_on_exit = GetLaunchOnExit();
  std::vector<HANDLE> program_handles = {nullptr};

  if (first_run && !launch_on_startup.empty()) {
    LaunchCommands(launch_on_startup, SW_SHOW, &program_handles);
  }

  wchar_t path[MAX_PATH];
  ::GetModuleFileName(nullptr, path, MAX_PATH);
  std::wstring args = GetCommand(param);

  SHELLEXECUTEINFO sei = {0};
  sei.cbSize = sizeof(SHELLEXECUTEINFO);
  sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
  sei.lpVerb = L"open";
  sei.lpFile = path;
  sei.nShow = SW_SHOWNORMAL;
  sei.lpParameters = args.c_str();

  if (ShellExecuteEx(&sei)) {
    if (first_run && !launch_on_exit.empty()) {
      // `WaitForSingleObject` causes IDM floating bar not to be displayed.
      // Hence, end users should be reminded to avoid using this feature until a
      // better method is implemented. See:
      // https://github.com/Bush2021/chrome_plus/issues/130
      WaitForSingleObject(sei.hProcess, INFINITE);
      CloseHandle(hMutex);
      KillLaunchOnExit(&program_handles);
      LaunchCommands(launch_on_exit, SW_HIDE, nullptr);
    }
    ExitProcess(0);
  }
}

#endif  // PORTABLE_H_
