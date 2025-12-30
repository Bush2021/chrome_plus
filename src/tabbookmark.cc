#include "tabbookmark.h"

#include <windows.h>

#include "config.h"
#include "hotkey.h"
#include "iaccessible.h"
#include "inputhook.h"
#include "utils.h"

namespace {

POINT lbutton_down_point = {-1, -1};

// Compared with `IsOnlyOneTab`, this function additionally implements tick
// fault tolerance to prevent users from directly closing the window when
// they click too fast.
bool IsNeedKeep(const NodePtr& top_container_view) {
  if (!config.IsKeepLastTab()) {
    return false;
  }

  auto tab_count = GetTabCount(top_container_view);
  bool keep_tab = (tab_count == 1);

  static auto last_closing_tab_tick = GetTickCount64();
  auto tick = GetTickCount64() - last_closing_tab_tick;
  last_closing_tab_tick = GetTickCount64();

  if (tick > 50 && tick <= 250 && tab_count == 2) {
    keep_tab = true;
  }

  return keep_tab;
}

// When `top_container_view` is not found, the find-in-page bar may be open
// and focused. Use `IsOnFindBarPane` to check if the click occurred on the
// bar. If so, return nullptr to avoid interfering with find operations
// (#157). Otherwise, close the bar and retry finding `top_container_view`
// to fix issues where double-click and right-click close actions fail when
// the bar is open (#187). Closing the bar typically has no side effects,
// except that clicks on other tabs or bookmarks will also dismiss the bar
// when it is open.
NodePtr HandleFindBar(HWND hwnd, POINT pt) {
  NodePtr top_container_view = GetTopContainerView(hwnd);
  if (!top_container_view) {
    if (IsOnFindBarPane(pt)) {
      return nullptr;
    }
    ExecuteCommand(IDC_CLOSE_FIND_OR_STOP, hwnd);
    top_container_view = GetTopContainerView(hwnd);
    if (!top_container_view) {
      return nullptr;
    }
  }
  return top_container_view;
}

// Use the mouse wheel to switch tabs
bool HandleMouseWheel(LPARAM lParam, const MOUSEHOOKSTRUCT* pmouse) {
  if (!config.IsWheelTab() && !config.IsWheelTabWhenPressRightButton()) {
    return false;
  }

  HWND hwnd = GetFocus();
  const NodePtr top_container_view = GetTopContainerView(hwnd);

  const auto* pwheel = reinterpret_cast<const MOUSEHOOKSTRUCTEX*>(lParam);
  const int delta = GET_WHEEL_DELTA_WPARAM(pwheel->mouseData);

  auto switch_tabs = [&]() {
    hwnd = GetTopWnd(hwnd);
    if (delta > 0) {
      ExecuteCommand(IDC_SELECT_PREVIOUS_TAB, hwnd);
    } else {
      ExecuteCommand(IDC_SELECT_NEXT_TAB, hwnd);
    }
    return true;
  };

  // If the mouse wheel is used to switch tabs when the mouse is on the tab bar.
  if (config.IsWheelTab() && IsOnTheTabBar(top_container_view, pmouse->pt)) {
    return switch_tabs();
  }

  // If it is used to switch tabs when the right button is held.
  if (config.IsWheelTabWhenPressRightButton() && IsKeyPressed(VK_RBUTTON)) {
    return switch_tabs();
  }

  return false;
}

// Double-click to close tab.
bool HandleDoubleClick(const MOUSEHOOKSTRUCT* pmouse) {
  if (!config.IsDoubleClickClose()) {
    return false;
  }

  const POINT pt = pmouse->pt;
  HWND hwnd = WindowFromPoint(pt);
  const NodePtr top_container_view = HandleFindBar(hwnd, pt);
  if (!top_container_view) {
    return false;
  }

  bool is_on_one_tab = IsOnOneTab(top_container_view, pt);
  bool is_on_close_button = IsOnCloseButton(top_container_view, pt);
  if (!is_on_one_tab || is_on_close_button) {
    return false;
  }

  if (IsOnlyOneTab(top_container_view)) {
    ExecuteCommand(IDC_NEW_TAB, hwnd);
    ExecuteCommand(IDC_WINDOW_CLOSE_OTHER_TABS, hwnd);
  } else {
    ExecuteCommand(IDC_CLOSE_TAB, hwnd);
  }
  return true;
}

// Right-click to close tab (Hold Shift to show the original menu).
bool HandleRightClick(const MOUSEHOOKSTRUCT* pmouse) {
  if (IsKeyPressed(VK_SHIFT) || !config.IsRightClickClose()) {
    return false;
  }

  const POINT pt = pmouse->pt;
  HWND hwnd = WindowFromPoint(pt);
  const NodePtr top_container_view = HandleFindBar(hwnd, pt);
  if (!top_container_view) {
    return false;
  }

  if (IsOnOneTab(top_container_view, pt)) {
    if (IsNeedKeep(top_container_view)) {
      ExecuteCommand(IDC_NEW_TAB, hwnd);
      ExecuteCommand(IDC_WINDOW_CLOSE_OTHER_TABS, hwnd);
    } else {
      // Attempt new SendKey function which includes a `dwExtraInfo`
      // value (GetMagicCode()).
      SendKey(VK_MBUTTON);
    }
    return true;
  }
  return false;
}

// Preserve the last tab when the middle button is clicked on the tab.
bool HandleMiddleClick(const MOUSEHOOKSTRUCT* pmouse) {
  const POINT pt = pmouse->pt;
  HWND hwnd = WindowFromPoint(pt);
  const NodePtr top_container_view = HandleFindBar(hwnd, pt);
  if (!top_container_view) {
    return false;
  }

  const bool is_on_one_tab = IsOnOneTab(top_container_view, pt);
  const bool keep_tab = IsNeedKeep(top_container_view);

  if (is_on_one_tab && keep_tab) {
    ExecuteCommand(IDC_NEW_TAB, hwnd);
    ExecuteCommand(IDC_WINDOW_CLOSE_OTHER_TABS, hwnd);
    return true;
  }

  return false;
}

bool HandleCloseButton(const MOUSEHOOKSTRUCT* pmouse) {
  if (!config.IsKeepLastTab()) {
    return false;
  }

  const POINT pt = pmouse->pt;
  HWND hwnd = WindowFromPoint(pt);
  const NodePtr top_container_view = HandleFindBar(hwnd, pt);
  if (!top_container_view) {
    return false;
  }

  if (!IsOnOneTab(top_container_view, pt) ||
      !IsOnCloseButton(top_container_view, pt)) {
    return false;
  }

  if (!IsNeedKeep(top_container_view)) {
    return false;
  }
  ExecuteCommand(IDC_NEW_TAB, hwnd);
  ExecuteCommand(IDC_WINDOW_CLOSE_OTHER_TABS, hwnd);
  return true;
}

// Check if mouse movement is a drag operation.
// Since `MouseProc` hook doesn't handle any drag-related events,
// this detection can return early to avoid interference.
bool HandleDrag(const MOUSEHOOKSTRUCT* pmouse) {
  // Add drag detection logic for
  // https://github.com/Bush2021/chrome_plus/issues/152
  static const int kDragThresholdX = GetSystemMetrics(SM_CXDRAG);
  static const int kDragThresholdY = GetSystemMetrics(SM_CYDRAG);
  const int dx = pmouse->pt.x - lbutton_down_point.x;
  const int dy = pmouse->pt.y - lbutton_down_point.y;
  return (abs(dx) > kDragThresholdX || abs(dy) > kDragThresholdY);
}

// Open bookmarks in a new tab.
bool HandleBookmark(const MOUSEHOOKSTRUCT* pmouse) {
  const int mode = config.GetBookmarkNewTabMode();
  if (IsKeyPressed(VK_CONTROL) || IsKeyPressed(VK_SHIFT) || mode == 0) {
    return false;
  }

  const POINT pt = pmouse->pt;
  HWND hwnd = WindowFromPoint(pt);

  if (!IsOnBookmark(hwnd, pt)) {
    return false;
  }

  if (IsOnExpandedList(hwnd, pt)) {
    // This is only used to determine the expanded dropdown menu of the address
    // bar. When the mouse clicks on it, it may penetrate through to the
    // background, causing a misjudgment that it is on the bookmark. Related
    // issue: https://github.com/Bush2021/chrome_plus/issues/162
    return false;
  }

  NodePtr top_container_view = GetTopContainerView(
      GetFocus());  // Must use `GetFocus()`, otherwise when opening bookmarks
                    // in a bookmark folder (and similar expanded menus),
                    // `top_container_view` cannot be obtained, making it
                    // impossible to correctly determine `is_on_new_tab`. See
                    // #98.

  if (!IsOnNewTab(top_container_view)) {
    if (mode == 1) {
      SendKey(VK_MBUTTON, VK_SHIFT);
    } else if (mode == 2) {
      SendKey(VK_MBUTTON);
    }
    return true;
  }
  return false;
}

// Mouse handler for tab and bookmark operations
bool TabBookmarkMouseHandler(WPARAM wParam, LPARAM lParam) {
  PMOUSEHOOKSTRUCT pmouse = reinterpret_cast<PMOUSEHOOKSTRUCT>(lParam);

  static bool wheel_tab_ing_with_rbutton = false;

  switch (wParam) {
    case WM_LBUTTONDOWN:
      // Simply record the position of `LBUTTONDOWN` for drag detection
      lbutton_down_point = pmouse->pt;
      return false;
    case WM_LBUTTONUP:
      if (HandleDrag(pmouse)) {
        return false;
      } else if (HandleBookmark(pmouse)) {
        return true;
      } else if (HandleCloseButton(pmouse)) {
        return true;
      }
      return false;
    case WM_RBUTTONUP:
      if (wheel_tab_ing_with_rbutton) {
        // Swallow the first RBUTTONUP that follows a wheel-based tab switch to
        // suppress Chrome's context menu; the RBUTTONUP arrives after
        // WM_MOUSEWHEEL.
        wheel_tab_ing_with_rbutton = false;
        return true;
      } else if (HandleRightClick(pmouse)) {
        return true;
      }
      return false;
    case WM_MOUSEWHEEL:
      if (HandleMouseWheel(lParam, pmouse)) {
        // Mark it true only when a tab switch is performed via mouse wheel with
        // right button pressed. Otherwise, normal mouse wheel to switch tabs
        // will swallow irrelevant RBUTTONUP events, causing #198.
        wheel_tab_ing_with_rbutton = IsKeyPressed(VK_RBUTTON);
        return true;
      }
      return false;
    case WM_LBUTTONDBLCLK:
      if (HandleDoubleClick(pmouse)) {
        // Do not return true. Returning true could cause the keep_tab to fail
        // or trigger double-click operations consecutively when the user
        // double-clicks on the tab page rapidly and repeatedly.
      }
      return false;
    case WM_MBUTTONUP:
      if (HandleMiddleClick(pmouse)) {
        return true;
      }
      return false;
  }
  return false;
}

int HandleKeepTab(WPARAM wParam) {
  if (!(wParam == 'W' && IsKeyPressed(VK_CONTROL) && !IsKeyPressed(VK_SHIFT)) &&
      !(wParam == VK_F4 && IsKeyPressed(VK_CONTROL))) {
    return 0;
  }

  HWND hwnd = GetFocus();
  if (GetChromeWidgetWin(hwnd) == nullptr) {
    return 0;
  }

  if (IsFullScreen(hwnd)) {
    // Have to exit full screen to find the tab.
    ExecuteCommand(IDC_FULLSCREEN, hwnd);
  }

  HWND tmp_hwnd = hwnd;
  hwnd = GetAncestor(tmp_hwnd, GA_ROOTOWNER);
  ExecuteCommand(IDC_CLOSE_FIND_OR_STOP, tmp_hwnd);

  NodePtr top_container_view = GetTopContainerView(hwnd);
  if (!IsNeedKeep(top_container_view)) {
    return 0;
  }

  ExecuteCommand(IDC_NEW_TAB, hwnd);
  ExecuteCommand(IDC_WINDOW_CLOSE_OTHER_TABS, hwnd);
  return 1;
}

int HandleOpenUrlNewTab(WPARAM wParam) {
  int mode = config.GetOpenUrlNewTabMode();
  if (!(mode != 0 && wParam == VK_RETURN && !IsKeyPressed(VK_MENU))) {
    return 0;
  }

  NodePtr top_container_view = GetTopContainerView(GetForegroundWindow());
  if (IsOmniboxFocus(top_container_view) && !IsOnNewTab(top_container_view)) {
    if (mode == 1) {
      SendKey(VK_MENU, VK_RETURN);
    } else if (mode == 2) {
      SendKey(VK_SHIFT, VK_MENU, VK_RETURN);
    }
    return 1;
  }
  return 0;
}

int HandleTranslateKey(WPARAM wParam) {
  auto hotkey = ParseTranslateKey();
  if (hotkey == 0) {
    return 0;
  }

  auto vk = HIWORD(hotkey);
  auto modifiers = LOWORD(hotkey);
  if ((modifiers & MOD_SHIFT) && !IsKeyPressed(VK_SHIFT)) {
    return 0;
  }
  if ((modifiers & MOD_CONTROL) && !IsKeyPressed(VK_CONTROL)) {
    return 0;
  }
  if ((modifiers & MOD_ALT) && !IsKeyPressed(VK_MENU)) {
    return 0;
  }
  if ((modifiers & MOD_WIN) && !IsKeyPressed(VK_LWIN) &&
      !IsKeyPressed(VK_RWIN)) {
    return 0;
  }
  if (wParam != vk) {
    return 0;
  }

  ExecuteCommand(IDC_SHOW_TRANSLATE);
  keybd_event(VK_RIGHT, 0, 0, 0);
  keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
  return 1;
}

// Keyboard handler for tab and bookmark operations
bool TabBookmarkKeyboardHandler(WPARAM wParam, LPARAM lParam) {
  // Only handle key down events
  if (lParam & 0x80000000) {
    return false;
  }

  if (HandleKeepTab(wParam) != 0) {
    return true;
  }

  if (HandleOpenUrlNewTab(wParam) != 0) {
    return true;
  }

  if (HandleTranslateKey(wParam) != 0) {
    return true;
  }

  return false;
}

}  // namespace

void TabBookmark() {
  RegisterMouseHandler(TabBookmarkMouseHandler, HandlerPriority::kNormal);
  RegisterKeyboardHandler(TabBookmarkKeyboardHandler, HandlerPriority::kNormal);
}
