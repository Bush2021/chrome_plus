#ifndef CHROME_PLUS_SRC_UIA_H_
#define CHROME_PLUS_SRC_UIA_H_

#include <uiautomation.h>
#include <wrl/client.h>

#include <optional>
#include <string>
#include <vector>

struct TabHitResult {
  Microsoft::WRL::ComPtr<IUIAutomationElement> tab;
  int tab_count = 0;
  bool on_close_button = false;
};

[[nodiscard]] std::optional<TabHitResult>
FindTabHitResult(POINT pt, bool need_count, bool need_close_button);
[[nodiscard]] std::optional<int> FindTabCount(HWND hwnd);
[[nodiscard]] bool IsOnTabBar(POINT pt);
[[nodiscard]] bool IsOnBookmark(POINT pt);
[[nodiscard]] bool IsOmniboxFocused();
[[nodiscard]] bool IsOnNewTab(HWND hwnd,
                              const std::vector<std::wstring>& extra_tab_names);

#endif  // CHROME_PLUS_SRC_UIA_H_
