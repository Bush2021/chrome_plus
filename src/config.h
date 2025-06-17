#ifndef CHROME_PLUS_SRC_CONFIG_H_
#define CHROME_PLUS_SRC_CONFIG_H_

#include <string>

class Config {
 public:
  static Config& Instance();

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
  Config();
  ~Config() = default;
  Config(const Config&) = delete;
  Config& operator=(const Config&) = delete;

  void LoadConfig();

  std::wstring LoadCommandLine();
  std::wstring LoadDirPath(const std::wstring& dir_type);
  std::wstring LoadBossKey();
  std::wstring LoadTranslateKey();
  std::string LoadOpenUrlNewTabMode();
  std::string LoadBookmarkNewTabMode();

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

extern const Config& config;

#endif  // CHROME_PLUS_SRC_CONFIG_H_
