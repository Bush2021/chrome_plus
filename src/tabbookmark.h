#ifndef TABBOOKMARK_H_
#define TABBOOKMARK_H_

#include "iaccessible.h"

HHOOK mouse_hook = nullptr;

#define KEY_PRESSED 0x8000
bool IsPressed(int key) {
  return key && (::GetKeyState(key) & KEY_PRESSED) != 0;
}

bool IsNeedKeep(HWND hwnd, int32_t* ptr = nullptr) {
  bool keep_tab = false;

  NodePtr top_container_view = GetTopContainerView(hwnd);
  auto tab_count = GetTabCount(top_container_view);
  bool is_only_one_tab = (tab_count > 0 && tab_count <= 1);

  static auto last_closing_tab_tick = GetTickCount64();
  auto tick = GetTickCount64() - last_closing_tab_tick;
  last_closing_tab_tick = GetTickCount64();

  if (tick > 0 && tick <= 250 && tab_count <= 2) {
    is_only_one_tab = true;
  }

  keep_tab = is_only_one_tab;

  if (ptr) {
    *ptr = tick;
  }

  return keep_tab;
}

class IniConfig {
 public:
  IniConfig()
      : is_double_click_close(IsDoubleClickClose()),
        is_right_click_close(IsRightClickClose()),
        is_wheel_tab(IsWheelTab()),
        is_wheel_tab_when_press_right_button(IsWheelTabWhenPressRightButton()),
        is_bookmark_new_tab(IsBookmarkNewTab()),
        is_open_url_new_tab(IsOpenUrlNewTabFun()) {}

  bool is_double_click_close;
  bool is_right_click_close;
  bool is_wheel_tab;
  bool is_wheel_tab_when_press_right_button;
  std::string is_bookmark_new_tab;
  std::string is_open_url_new_tab;
};

IniConfig config;

// 滚轮切换标签页
bool handleMouseWheel(WPARAM wParam, LPARAM lParam, PMOUSEHOOKSTRUCT pmouse) {
  if (wParam != WM_MOUSEWHEEL ||
      (!config.is_wheel_tab && !config.is_wheel_tab_when_press_right_button)) {
    return false;
  }

  HWND hwnd = GetFocus();
  NodePtr top_container_view = GetTopContainerView(hwnd);

  PMOUSEHOOKSTRUCTEX pwheel = (PMOUSEHOOKSTRUCTEX)lParam;
  int zDelta = GET_WHEEL_DELTA_WPARAM(pwheel->mouseData);

  // 是否启用鼠标停留在标签栏时滚轮切换标签
  if (config.is_wheel_tab && IsOnTheTabBar(top_container_view, pmouse->pt)) {
    hwnd = GetTopWnd(hwnd);
    if (zDelta > 0) {
      ExecuteCommand(IDC_SELECT_PREVIOUS_TAB, hwnd);
    } else {
      ExecuteCommand(IDC_SELECT_NEXT_TAB, hwnd);
    }
    return true;
  }

  // 是否启用在任何位置按住右键时滚轮切换标签
  if (config.is_wheel_tab_when_press_right_button && IsPressed(VK_RBUTTON)) {
    hwnd = GetTopWnd(hwnd);
    if (zDelta > 0) {
      ExecuteCommand(IDC_SELECT_PREVIOUS_TAB, hwnd);
    } else {
      ExecuteCommand(IDC_SELECT_NEXT_TAB, hwnd);
    }
    return true;
  }

  return false;
}

// 双击关闭标签页
bool handleDblClk(WPARAM wParam, LPARAM lParam, PMOUSEHOOKSTRUCT pmouse) {
  if (wParam != WM_LBUTTONDBLCLK || !config.is_double_click_close) {
    return false;
  }

  HWND hwnd = WindowFromPoint(pmouse->pt);
  NodePtr top_container_view = GetTopContainerView(hwnd);

  bool is_on_one_tab = IsOnOneTab(top_container_view, pmouse->pt);
  bool is_only_one_tab = IsOnlyOneTab(top_container_view);

  if (is_on_one_tab) {
    if (is_only_one_tab) {
      ExecuteCommand(IDC_NEW_TAB);
      ExecuteCommand(IDC_SELECT_PREVIOUS_TAB);
      ExecuteCommand(IDC_CLOSE_TAB);
    } else {
      ExecuteCommand(IDC_CLOSE_TAB);
    }
    return true;
  }

  return false;
}

// 右键关闭标签页
bool handleRightClick(WPARAM wParam, LPARAM lParam, PMOUSEHOOKSTRUCT pmouse) {
  if (wParam != WM_RBUTTONUP || IsPressed(VK_SHIFT) ||
      !config.is_right_click_close) {
    return false;
  }

  HWND hwnd = WindowFromPoint(pmouse->pt);
  NodePtr top_container_view = GetTopContainerView(hwnd);

  bool is_on_one_tab = IsOnOneTab(top_container_view, pmouse->pt);
  bool keep_tab = IsNeedKeep(hwnd);

  if (is_on_one_tab) {
    if (keep_tab) {
      ExecuteCommand(IDC_NEW_TAB);
      ExecuteCommand(IDC_SELECT_PREVIOUS_TAB);
      ExecuteCommand(IDC_CLOSE_TAB);
    } else {
      SendKeys(VK_MBUTTON);
    }
    return true;
  }
  return false;
}

// 保留最后标签页 - 中键
bool handleMiddleClick(WPARAM wParam, LPARAM lParam, PMOUSEHOOKSTRUCT pmouse) {
  if (wParam != WM_MBUTTONUP) {
    return false;
  }

  HWND hwnd = WindowFromPoint(pmouse->pt);
  NodePtr top_container_view = GetTopContainerView(hwnd);

  bool is_on_one_tab = IsOnOneTab(top_container_view, pmouse->pt);
  bool keep_tab = IsNeedKeep(hwnd);

  if (is_on_one_tab && keep_tab) {
    ExecuteCommand(IDC_NEW_TAB);
    return true;
  }

  return false;
}

// 新标签页打开书签
bool handleBookmark(WPARAM wParam, LPARAM lParam, PMOUSEHOOKSTRUCT pmouse) {
  if (wParam != WM_LBUTTONUP || IsPressed(VK_CONTROL) || IsPressed(VK_SHIFT) ||
      config.is_bookmark_new_tab == "disabled") {
    return false;
  }

  HWND hwnd = WindowFromPoint(pmouse->pt);
  NodePtr top_container_view = GetTopContainerView(hwnd);

  bool is_on_bookmark = IsOnBookmark(top_container_view, pmouse->pt);
  bool is_on_new_tab = IsOnNewTab(top_container_view);

  if (top_container_view && is_on_bookmark && !is_on_new_tab) {
    if (config.is_bookmark_new_tab == "foreground") {
      SendKeys(VK_MBUTTON, VK_SHIFT);
    } else if (config.is_bookmark_new_tab == "background") {
      SendKeys(VK_MBUTTON);
    }
    return true;
  }

  return false;
}

// 新标签页打开书签文件夹中的书签
bool handleBookmarkMenu(WPARAM wParam, LPARAM lParam, PMOUSEHOOKSTRUCT pmouse) {
  if (wParam != WM_LBUTTONUP || IsPressed(VK_CONTROL) || IsPressed(VK_SHIFT) ||
      config.is_bookmark_new_tab == "disabled") {
    return false;
  }

  HWND hwnd_from_point = WindowFromPoint(pmouse->pt);
  HWND hwnd_from_keyboard = GetFocus();
  NodePtr top_container_view = GetTopContainerView(hwnd_from_keyboard);
  NodePtr menu_bar_pane = GetMenuBarPane(hwnd_from_point);

  bool is_on_menu_bookmark = IsOnMenuBookmark(menu_bar_pane, pmouse->pt);
  bool is_on_new_tab = IsOnNewTab(top_container_view);

  if (top_container_view && menu_bar_pane && is_on_menu_bookmark &&
      !is_on_new_tab) {
    if (config.is_bookmark_new_tab == "foreground") {
      DebugLog(L"MButton + Shift");
      SendKeys(VK_MBUTTON, VK_SHIFT);
    } else if (config.is_bookmark_new_tab == "background") {
      SendKeys(VK_MBUTTON);
    }
    return true;
  }

  return false;
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
  static bool wheel_tab_ing = false;
  static bool double_click_ing = false;

  if (nCode != HC_ACTION) {
    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
  }

  if (nCode == HC_ACTION) {
    PMOUSEHOOKSTRUCT pmouse = (PMOUSEHOOKSTRUCT)lParam;

    if (wParam == WM_MOUSEMOVE || wParam == WM_NCMOUSEMOVE) {
      return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
    }

    if (pmouse->dwExtraInfo == MAGIC_CODE) {
      // DebugLog(L"MAGIC_CODE %x", wParam);
      goto next;
    }

    if (wParam == WM_RBUTTONUP && wheel_tab_ing) {
      // DebugLog(L"wheel_tab_ing");
      wheel_tab_ing = false;
      return 1;
    }

    // if (wParam == WM_MBUTTONDOWN)
    //{
    //     //DebugLog(L"wheel_tab_ing");
    //     return 1;
    // }
    // if (wParam == WM_LBUTTONUP && double_click_ing)
    //{
    //     //DebugLog(L"double_click_ing");
    //     double_click_ing = false;
    //     return 1;
    // }

    if (handleMouseWheel(wParam, lParam, pmouse)) {
      return 1;
    }

    if (handleDblClk(wParam, lParam, pmouse)) {}

    if (handleRightClick(wParam, lParam, pmouse)) {
      return 1;
    }

    if (handleMiddleClick(wParam, lParam, pmouse)) {
      // return 1;
    }

    if (handleBookmark(wParam, lParam, pmouse)) {
      return 1;
    }

    if (handleBookmarkMenu(wParam, lParam, pmouse)) {
      return 1;
    }
  }
next:
  // DebugLog(L"CallNextHookEx %X", wParam);
  return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
}

bool handleTabPreserve(WPARAM wParam) {

  if (!(wParam == 'W' && IsPressed(VK_CONTROL) && !IsPressed(VK_SHIFT)) &&
      !(wParam == VK_F4 && IsPressed(VK_CONTROL))) {
    return 0;
  }

  HWND hwnd = GetFocus();

  if (!GetTopContainerView(hwnd)) {
    ExecuteCommand(IDC_FULLSCREEN);
  }

  if (!IsNeedKeep(hwnd)) {
    return 0;
  }

  ExecuteCommand(IDC_NEW_TAB);
  ExecuteCommand(IDC_SELECT_PREVIOUS_TAB);
  ExecuteCommand(IDC_CLOSE_TAB);
  return 1;
}
bool IsNeedOpenUrlInNewTab() {
  bool open_url_ing = false;

  NodePtr TopContainerView = GetTopContainerView(GetForegroundWindow());
  if (IsOmniboxFocus(TopContainerView)) {
    if (!IsOnNewTab(TopContainerView)) {
      open_url_ing = true;
    }
  }

  if (TopContainerView) {}

  return open_url_ing;
}

HHOOK keyboard_hook = nullptr;
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HC_ACTION && !(lParam & 0x80000000))  // pressed
  {

    if (handleTabPreserve(wParam)) {}

    bool open_url_ing = false;

    if (config.is_open_url_new_tab != "disabled" && wParam == VK_RETURN &&
        !IsPressed(VK_MENU)) {
      open_url_ing = IsNeedOpenUrlInNewTab();
    }

    if (open_url_ing) {
      if (config.is_open_url_new_tab == "foreground") {
        SendKeys(VK_MENU, VK_RETURN);
      } else if (config.is_open_url_new_tab == "background") {
        SendKeys(VK_SHIFT, VK_MENU, VK_RETURN);
      }
      return 1;
    }
  }
  return CallNextHookEx(keyboard_hook, nCode, wParam, lParam);
}

void TabBookmark() {
  mouse_hook =
      SetWindowsHookEx(WH_MOUSE, MouseProc, hInstance, GetCurrentThreadId());
  keyboard_hook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hInstance,
                                   GetCurrentThreadId());
}

#endif  // TABBOOKMARK_H_