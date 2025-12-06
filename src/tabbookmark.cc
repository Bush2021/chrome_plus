#include "tabbookmark.h"

#include <windows.h>

#include "config.h"
#include "hotkey.h"
#include "iaccessible.h"
#include "utils.h"

namespace {

HHOOK mouse_hook = nullptr;
static POINT lbutton_down_point = {-1, -1};

#define KEY_PRESSED 0x8000
bool IsPressed(int key) {
  return key && (::GetKeyState(key) & KEY_PRESSED) != 0;
}

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
bool HandleMouseWheel(WPARAM wParam, LPARAM lParam, PMOUSEHOOKSTRUCT pmouse) {
  if (wParam != WM_MOUSEWHEEL ||
      (!config.IsWheelTab() && !config.IsWheelTabWhenPressRightButton())) {
    return false;
  }

  HWND hwnd = GetFocus();
  NodePtr top_container_view = GetTopContainerView(hwnd);

  PMOUSEHOOKSTRUCTEX pwheel = reinterpret_cast<PMOUSEHOOKSTRUCTEX>(lParam);
  int zDelta = GET_WHEEL_DELTA_WPARAM(pwheel->mouseData);

  auto switch_tabs = [&]() {
    hwnd = GetTopWnd(hwnd);
    if (zDelta > 0) {
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
  if (config.IsWheelTabWhenPressRightButton() && IsPressed(VK_RBUTTON)) {
    return switch_tabs();
  }

  return false;
}

// Double-click to close tab.
int HandleDoubleClick(WPARAM wParam, PMOUSEHOOKSTRUCT pmouse) {
  if (wParam != WM_LBUTTONDBLCLK || !config.IsDoubleClickClose()) {
    return 0;
  }

  POINT pt = pmouse->pt;
  HWND hwnd = WindowFromPoint(pt);
  NodePtr top_container_view = HandleFindBar(hwnd, pt);
  if (!top_container_view) {
    return 0;
  }

  bool is_on_one_tab = IsOnOneTab(top_container_view, pt);
  bool is_on_close_button = IsOnCloseButton(top_container_view, pt);
  bool is_only_one_tab = IsOnlyOneTab(top_container_view);
  if (!is_on_one_tab || is_on_close_button) {
    return 0;
  }
  if (is_only_one_tab) {
    ExecuteCommand(IDC_NEW_TAB, hwnd);
    ExecuteCommand(IDC_WINDOW_CLOSE_OTHER_TABS, hwnd);
  } else {
    ExecuteCommand(IDC_CLOSE_TAB, hwnd);
  }
  return 1;
}

// Right-click to close tab (Hold Shift to show the original menu).
int HandleRightClick(WPARAM wParam, PMOUSEHOOKSTRUCT pmouse) {
  if (wParam != WM_RBUTTONUP || IsPressed(VK_SHIFT) ||
      !config.IsRightClickClose()) {
    return 0;
  }

  POINT pt = pmouse->pt;
  HWND hwnd = WindowFromPoint(pt);
  NodePtr top_container_view = HandleFindBar(hwnd, pt);
  if (!top_container_view) {
    return 0;
  }

  if (IsOnOneTab(top_container_view, pt)) {
    if (IsNeedKeep(top_container_view)) {
      ExecuteCommand(IDC_NEW_TAB, hwnd);
      ExecuteCommand(IDC_WINDOW_CLOSE_OTHER_TABS, hwnd);
    } else {
      // Attempt new SendKey function which includes a `dwExtraInfo`
      // value (MAGIC_CODE).
      SendKey(VK_MBUTTON);
    }
    return 1;
  }
  return 0;
}

// Preserve the last tab when the middle button is clicked on the tab.
int HandleMiddleClick(WPARAM wParam, PMOUSEHOOKSTRUCT pmouse) {
  if (wParam != WM_MBUTTONUP) {
    return 0;
  }

  POINT pt = pmouse->pt;
  HWND hwnd = WindowFromPoint(pt);
  NodePtr top_container_view = HandleFindBar(hwnd, pt);
  if (!top_container_view) {
    return 0;
  }

  bool is_on_one_tab = IsOnOneTab(top_container_view, pt);
  bool keep_tab = IsNeedKeep(top_container_view);

  if (is_on_one_tab && keep_tab) {
    ExecuteCommand(IDC_NEW_TAB, hwnd);
    ExecuteCommand(IDC_WINDOW_CLOSE_OTHER_TABS, hwnd);
    return 1;
  }

  return 0;
}

// Check if mouse movement is a drag operation.
// Since `MouseProc` hook doesn't handle any drag-related events,
// this detection can return early to avoid interference.
bool HandleDrag(PMOUSEHOOKSTRUCT pmouse) {
  // Add drag detection logic for
  // https://github.com/Bush2021/chrome_plus/issues/152
  static int dragThresholdX = GetSystemMetrics(SM_CXDRAG);
  static int dragThresholdY = GetSystemMetrics(SM_CYDRAG);
  int dx = pmouse->pt.x - lbutton_down_point.x;
  int dy = pmouse->pt.y - lbutton_down_point.y;
  return (abs(dx) > dragThresholdX || abs(dy) > dragThresholdY);
}

// Open bookmarks in a new tab.
bool HandleBookmark(WPARAM wParam, PMOUSEHOOKSTRUCT pmouse) {
  int mode = config.GetBookmarkNewTabMode();
  if (wParam != WM_LBUTTONUP || IsPressed(VK_CONTROL) || IsPressed(VK_SHIFT) ||
      mode == 0) {
    return false;
  }

  POINT pt = pmouse->pt;
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

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
  static bool wheel_tab_ing_with_rbutton = false;
  if (nCode != HC_ACTION) {
    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
  }

  do {
    if (wParam == WM_MOUSEMOVE || wParam == WM_NCMOUSEMOVE) {
      break;
    }
    PMOUSEHOOKSTRUCT pmouse = reinterpret_cast<PMOUSEHOOKSTRUCT>(lParam);

    // Defining a `dwExtraInfo` value to prevent hook the message sent by
    // Chrome++ itself.
    if (pmouse->dwExtraInfo == MAGIC_CODE) {
      break;
    }

    // Record LBUTTONDOWN position
    if (wParam == WM_LBUTTONDOWN) {
      lbutton_down_point = pmouse->pt;
      break;
    }

    // Check for drag operations and return early if detected
    if (wParam == WM_LBUTTONUP && HandleDrag(pmouse)) {
      break;
    }

    // Swallow the first RBUTTONUP that follows a wheel-based tab switch to
    // suppress Chrome's context menu; the RBUTTONUP arrives after
    // WM_MOUSEWHEEL.
    if (wParam == WM_RBUTTONUP && wheel_tab_ing_with_rbutton) {
      wheel_tab_ing_with_rbutton = false;
      return 1;
    }

    if (HandleMouseWheel(wParam, lParam, pmouse)) {
      // Mark it true only when a tab switch is performed via mouse wheel with
      // right button pressed. Otherwise, normal mouse wheel to switch tabs will
      // swallow irrelevant RBUTTONUP events, causing #198.
      wheel_tab_ing_with_rbutton = IsPressed(VK_RBUTTON);
      return 1;
    }

    if (HandleDoubleClick(wParam, pmouse) != 0) {
      // Do not return 1. Returning 1 could cause the keep_tab to fail
      // or trigger double-click operations consecutively when the user
      // double-clicks on the tab page rapidly and repeatedly.
    }

    if (HandleRightClick(wParam, pmouse) != 0) {
      return 1;
    }

    if (HandleMiddleClick(wParam, pmouse) != 0) {
      return 1;
    }

    if (HandleBookmark(wParam, pmouse)) {
      return 1;
    }
  } while (0);
  return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
}

int HandleKeepTab(WPARAM wParam) {
  if (!(wParam == 'W' && IsPressed(VK_CONTROL) && !IsPressed(VK_SHIFT)) &&
      !(wParam == VK_F4 && IsPressed(VK_CONTROL))) {
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
  if (!(mode != 0 && wParam == VK_RETURN && !IsPressed(VK_MENU))) {
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
  if ((modifiers & MOD_SHIFT) && !IsPressed(VK_SHIFT)) {
    return 0;
  }
  if ((modifiers & MOD_CONTROL) && !IsPressed(VK_CONTROL)) {
    return 0;
  }
  if ((modifiers & MOD_ALT) && !IsPressed(VK_MENU)) {
    return 0;
  }
  if ((modifiers & MOD_WIN) && !IsPressed(VK_LWIN) && !IsPressed(VK_RWIN)) {
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

HHOOK keyboard_hook = nullptr;
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HC_ACTION && !(lParam & 0x80000000))  // pressed
  {
    if (HandleKeepTab(wParam) != 0) {
      return 1;
    }

    if (HandleOpenUrlNewTab(wParam) != 0) {
      return 1;
    }

    if (HandleTranslateKey(wParam) != 0) {
      return 1;
    }
  }
  return CallNextHookEx(keyboard_hook, nCode, wParam, lParam);
}

}  // namespace

void TabBookmark() {
  mouse_hook =
      SetWindowsHookEx(WH_MOUSE, MouseProc, hInstance, GetCurrentThreadId());
  keyboard_hook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hInstance,
                                   GetCurrentThreadId());
}
