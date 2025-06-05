#ifndef CONFIG_H_
#define CONFIG_H_

class Config {
 public:
  static Config& Instance() {
    static Config instance;
    return instance;
  }

  // general
  const std::wstring& GetCommandLine() const { return command_line_; }
  const std::wstring& GetLaunchOnStartup() const { return launch_on_startup_; }
  const std::wstring& GetLaunchOnExit() const { return launch_on_exit_; }
  const std::wstring& GetUserDataDir() const { return user_data_dir_; }
  const std::wstring& GetDiskCacheDir() const { return disk_cache_dir_; }
  const std::wstring& GetBossKey() const { return boss_key_; }
  const std::wstring& GetTranslateKey() const { return translate_key_; }
  bool IsShowPassword() const { return show_password_; }
  bool IsWin32K() const { return win32k_; }

  // tabs
  bool IsKeepLastTab() const { return keep_last_tab_; }
  bool IsDoubleClickClose() const { return double_click_close_; }
  bool IsRightClickClose() const { return right_click_close_; }
  bool IsWheelTab() const { return wheel_tab_; }
  bool IsWheelTabWhenPressRightButton() const {
    return wheel_tab_when_press_rbutton_;
  }
  const std::string& GetOpenUrlNewTabMode() const { return open_url_new_tab_; }
  const std::string& GetBookmarkNewTabMode() const { return bookmark_new_tab_; }
  bool IsNewTabDisable() const { return new_tab_disable_; }
  const std::wstring& GetDisableTabName() const { return disable_tab_name_; }

 private:
  Config() { LoadConfig(); }
  ~Config() = default;
  Config(const Config&) = delete;
  Config& operator=(const Config&) = delete;

  void LoadConfig() {
    // general
    command_line_ = LoadCommandLine();
    launch_on_startup_ = GetIniString(L"general", L"launch_on_startup", L"");
    launch_on_exit_ = GetIniString(L"general", L"launch_on_exit", L"");
    user_data_dir_ = LoadDirPath(L"data");
    disk_cache_dir_ = LoadDirPath(L"cache");
    boss_key_ = LoadBossKey();
    translate_key_ = LoadTranslateKey();
    show_password_ = ::GetPrivateProfileIntW(L"general", L"show_password", 1,
                                             kIniPath.c_str()) != 0;
    win32k_ = ::GetPrivateProfileIntW(L"general", L"win32k", 0,
                                      kIniPath.c_str()) != 0;

    // tabs
    keep_last_tab_ = ::GetPrivateProfileIntW(L"tabs", L"keep_last_tab", 1,
                                             kIniPath.c_str()) != 0;
    double_click_close_ =
        ::GetPrivateProfileIntW(L"tabs", L"double_click_close", 1,
                                kIniPath.c_str()) != 0;
    right_click_close_ = ::GetPrivateProfileIntW(L"tabs", L"right_click_close",
                                                 0, kIniPath.c_str()) != 0;
    wheel_tab_ = ::GetPrivateProfileIntW(L"tabs", L"wheel_tab", 1,
                                         kIniPath.c_str()) != 0;
    wheel_tab_when_press_rbutton_ =
        ::GetPrivateProfileIntW(L"tabs", L"wheel_tab_when_press_rbutton", 1,
                                kIniPath.c_str()) != 0;
    open_url_new_tab_ = LoadOpenUrlNewTabMode();
    bookmark_new_tab_ = LoadBookmarkNewTabMode();
    new_tab_disable_ = ::GetPrivateProfileIntW(L"tabs", L"new_tab_disable", 1,
                                               kIniPath.c_str()) != 0;
    disable_tab_name_ = GetIniString(L"tabs", L"new_tab_disable_name", L"");
  }

  std::wstring LoadCommandLine() {
    auto commandLine = GetIniString(L"general", L"command_line", L"");
    if (!commandLine.empty()) {
      return commandLine;
    }
    return GetIniString(L"General", L"CommandLine", L"");  // Deprecated
  }

  std::wstring LoadDirPath(const std::wstring& dir_type) {
    std::wstring path = CanonicalizePath(kAppDir + L"\\..\\" + dir_type);
    std::wstring dir_key = dir_type + L"_dir";
    std::wstring dir_buffer = GetIniString(L"general", dir_key, path);

    if (dir_buffer == L"none") {
      return L"";
    }

    if (dir_buffer.empty()) {  // Deprecated
      dir_key = dir_type + L"dir";
      dir_buffer = GetIniString(L"general", dir_key, path);
    }

    if (dir_buffer.empty()) {
      dir_buffer = path;
    }

    std::wstring expanded_path = ExpandEnvironmentPath(dir_buffer);
    ReplaceStringInPlace(expanded_path, L"%app%", kAppDir);
    return GetAbsolutePath(expanded_path);
  }

  std::wstring LoadBossKey() {
    auto key = GetIniString(L"general", L"boss_key", L"");
    if (!key.empty()) {
      return key;
    }
    return GetIniString(L"General", L"Bosskey", L"");  // Deprecated
  }

  std::wstring LoadTranslateKey() {
    auto key = GetIniString(L"general", L"translate_key", L"");
    if (!key.empty()) {
      return key;
    }
    return GetIniString(L"General", L"TranslateKey", L"");  // Deprecated
  }

  std::string LoadOpenUrlNewTabMode() {
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

  std::string LoadBookmarkNewTabMode() {
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

 private:
  // general
  std::wstring command_line_;
  std::wstring launch_on_startup_;
  std::wstring launch_on_exit_;
  std::wstring user_data_dir_;
  std::wstring disk_cache_dir_;
  std::wstring boss_key_;
  std::wstring translate_key_;
  bool show_password_;
  bool win32k_;

  // tabs
  bool keep_last_tab_;
  bool double_click_close_;
  bool right_click_close_;
  bool wheel_tab_;
  bool wheel_tab_when_press_rbutton_;
  std::string open_url_new_tab_;
  std::string bookmark_new_tab_;
  bool new_tab_disable_;
  std::wstring disable_tab_name_;
};

const auto& config = Config::Instance();

#endif  // CONFIG_H_
