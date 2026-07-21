#include "tabbookmark.h"

#include <windows.h>

#include <optional>

#include "config.h"
#include "inputhook.h"
#include "uia.h"
#include "utils.h"

namespace {

constexpr UINT kDefaultDpi = 96;
POINT lbutton_down_point = {-1, -1};

constexpr UINT_PTR kHoverTabTimerId = 0x68764254;  // 'hvBT'
// Non-null while a dwell timer is armed on that top-level window.
HWND hover_tab_root = nullptr;
// Screen position of the last WM_MOUSEMOVE handled by HandleHoverTab.
// Windows posts a synthetic same-position WM_MOUSEMOVE to the window under
// the cursor whenever the HWND arrangement beneath it may have changed
// (https://devblogs.microsoft.com/oldnewthing/20031001-00/?p=42343), and a
// tab switch does exactly that by showing/hiding
// `Chrome_RenderWidgetHostHWND`. Chromium drops such moves too, see
// `HWNDMessageHandler::OnMouseRange` in ui/views/win/hwnd_message_handler.cc.
std::optional<POINT> last_hover_move_point;
// Anchor of the last wheel-driven tab switch. While set, hover activation is
// suspended: rolling the wheel nudges the cursor a pixel or two, which would
// otherwise re-arm the dwell and select the tab still under the cursor,
// reverting the switch the user just made. Cleared once the cursor travels
// beyond the jitter threshold.
std::optional<POINT> wheel_switch_point;

enum class KeepTabTrigger {
  kRightClick = 0,
  kMiddleClick,
  kCloseButton,
  kKeyboardShortcut,
  kCount,
};

// This implements tick fault tolerance to prevent users from directly closing
// the window when they click too fast. Also uses pre-computed `tab_count` to
// avoid redundant `FindPageTabPane` traversal.
bool IsNeedKeep(int tab_count, KeepTabTrigger trigger) {
  // `tab_count` will be 0 if `config.IsKeepLastTab()` is false.
  if (tab_count == 0) {
    return false;
  }

  bool keep_tab = (tab_count == 1);
  static ULONGLONG
      last_closing_tab_ticks[static_cast<int>(KeepTabTrigger::kCount)] = {};
  const int trigger_index = static_cast<int>(trigger);
  const ULONGLONG now = GetTickCount64();
  const ULONGLONG tick = now - last_closing_tab_ticks[trigger_index];
  last_closing_tab_ticks[trigger_index] = now;
  if (tick > 50 && tick <= 250 && tab_count == 2) {
    keep_tab = true;
  }
  return keep_tab;
}

bool IsAnyMouseButtonPressed() {
  return IsKeyPressed(VK_LBUTTON) || IsKeyPressed(VK_RBUTTON) ||
         IsKeyPressed(VK_MBUTTON);
}

// https://github.com/Bush2021/chrome_plus/issues/226
UINT GetWindowDpiSafe(HWND hwnd) {
  // `GetDpiForWindow` requires Windows 10, version 1607 or later.
  const HMODULE user32 = GetModuleHandleW(L"user32.dll");
  if (!user32) {
    return kDefaultDpi;
  }
  using GetDpiForWindowFn = UINT(WINAPI*)(HWND);
  const auto fn = reinterpret_cast<GetDpiForWindowFn>(
      GetProcAddress(user32, "GetDpiForWindow"));
  if (!fn) {
    return kDefaultDpi;
  }
  const UINT dpi = fn(hwnd);
  return dpi ? dpi : kDefaultDpi;
}

void CancelHoverTabTimer() {
  if (!hover_tab_root) {
    return;
  }
  KillTimer(hover_tab_root, kHoverTabTimerId);
  hover_tab_root = nullptr;
}

void CALLBACK HoverTabTimerProc(HWND hwnd, UINT, UINT_PTR event_id, DWORD) {
  // SetTimer is periodic, not one-shot; kill it immediately so a dwell only
  // ever fires once, before any of the checks below can early-return.
  KillTimer(hwnd, event_id);
  // KillTimer does not flush a WM_TIMER already generated, so a stale fire
  // for a previously canceled window may still land here; do not clobber the
  // bookkeeping of a timer since armed on another window.
  if (hover_tab_root == hwnd) {
    hover_tab_root = nullptr;
  }

  // A stale fire may also land after a wheel switch already canceled the
  // timer; hover stays suspended until real cursor movement, so activating
  // here would revert the switch.
  if (wheel_switch_point) {
    return;
  }

  // The timer firing only proves the mouse went quiet; the window, cursor,
  // and capture state must all be re-checked here since they may have
  // changed since the timer was armed.
  if (IsAnyMouseButtonPressed() || GetCapture() != nullptr) {
    return;
  }

  POINT pt;
  if (!GetCursorPos(&pt)) {
    return;
  }

  const HWND point_window = WindowFromPoint(pt);
  const HWND point_root =
      point_window ? GetAncestor(point_window, GA_ROOT) : nullptr;
  if (point_root != hwnd) {
    return;
  }

  if (GetForegroundWindow() != hwnd) {
    return;
  }

  const auto hit = FindTabHitResult(pt, false, true);
  if (!hit || hit->on_close_button) {
    return;
  }

  SelectTab(*hit);
}

void HandleHoverTab(const MOUSEHOOKSTRUCT* pmouse) {
  if (!config.IsHoverTab()) {
    return;
  }

  // A same-position move is synthetic (see `last_hover_move_point`), not
  // hover intent; leave any pending dwell untouched.
  if (last_hover_move_point && last_hover_move_point->x == pmouse->pt.x &&
      last_hover_move_point->y == pmouse->pt.y) {
    return;
  }
  last_hover_move_point = pmouse->pt;

  if (wheel_switch_point) {
    // Scale by DPI the same way as the drag threshold in `HandleDrag`.
    constexpr UINT kWheelJitterThreshold = 8;
    const UINT dx = std::abs(pmouse->pt.x - wheel_switch_point->x);
    const UINT dy = std::abs(pmouse->pt.y - wheel_switch_point->y);
    const UINT dpi = GetWindowDpiSafe(pmouse->hwnd);
    const UINT threshold = MulDiv(kWheelJitterThreshold, dpi, kDefaultDpi);
    if (dx <= threshold && dy <= threshold) {
      return;
    }
    wheel_switch_point.reset();
  }

  if (IsAnyMouseButtonPressed() || GetCapture() != nullptr) {
    CancelHoverTabTimer();
    return;
  }

  // Moves over web content target the Chrome_RenderWidgetHostHWND child, so
  // this class gate keeps the hot path off the page area entirely with no
  // UIA involvement; tab-strip moves target the Chrome_WidgetWin_* frame
  // itself.
  if (!IsChromeWindow(pmouse->hwnd)) {
    CancelHoverTabTimer();
    return;
  }

  const HWND root = GetAncestor(pmouse->hwnd, GA_ROOT);
  if (!root) {
    CancelHoverTabTimer();
    return;
  }

  if (root != hover_tab_root) {
    CancelHoverTabTimer();
  }

  // Re-issuing SetTimer with the same hwnd/id restarts the countdown, which
  // is exactly the stationary-dwell semantics: any movement resets the
  // clock. A zero delay is clamped by the system to USER_TIMER_MINIMUM.
  if (SetTimer(root, kHoverTabTimerId, config.GetHoverTabDelay(),
               HoverTabTimerProc)) {
    hover_tab_root = root;
  } else {
    hover_tab_root = nullptr;
  }
}

// Use the mouse wheel to switch tabs
bool HandleMouseWheel(LPARAM lParam, const MOUSEHOOKSTRUCT* pmouse) {
  if (!config.IsWheelTab() && !config.IsWheelTabWhenPressRightButton()) {
    return false;
  }

  HWND hwnd = GetFocus();
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

  // If it is used to switch tabs when the right button is held.
  if (config.IsWheelTabWhenPressRightButton() && IsKeyPressed(VK_RBUTTON)) {
    return switch_tabs();
  }

  // If the mouse wheel is used to switch tabs when the mouse is on the tab bar.
  if (config.IsWheelTab() && IsOnTabBar(pmouse->pt)) {
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
  // Commands target the browser frame; child HWNDs under the cursor are common.
  if (hwnd) {
    if (const HWND root = GetAncestor(hwnd, GA_ROOT)) {
      hwnd = root;
    }
  }
  const auto hit = FindTabHitResult(pt, config.IsKeepLastTab(), true);
  if (!hit || hit->on_close_button) {
    return false;
  }
  if (hit->tab_count == 1) {
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
  const auto hit = FindTabHitResult(pt, config.IsKeepLastTab(), false);
  if (!hit) {
    return false;
  }
  if (IsNeedKeep(hit->tab_count, KeepTabTrigger::kRightClick)) {
    ExecuteCommand(IDC_NEW_TAB, hwnd);
    ExecuteCommand(IDC_WINDOW_CLOSE_OTHER_TABS, hwnd);
  } else {
    // Attempt new SendKey function which includes a `dwExtraInfo`
    // value (GetMagicCode()).
    SendKey(VK_MBUTTON);
  }
  return true;
}

// Preserve the last tab when the middle button is clicked on the tab.
bool HandleMiddleClick(const MOUSEHOOKSTRUCT* pmouse) {
  if (!config.IsKeepLastTab()) {
    return false;
  }

  const POINT pt = pmouse->pt;
  HWND hwnd = WindowFromPoint(pt);
  const auto hit = FindTabHitResult(pt, config.IsKeepLastTab(), false);
  if (!hit) {
    return false;
  }

  if (IsNeedKeep(hit->tab_count, KeepTabTrigger::kMiddleClick)) {
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
  const auto hit = FindTabHitResult(pt, config.IsKeepLastTab(), true);
  if (!hit || !hit->on_close_button) {
    return false;
  }
  if (!IsNeedKeep(hit->tab_count, KeepTabTrigger::kCloseButton)) {
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
  // https://source.chromium.org/chromium/chromium/src/+/main:ui/views/view.cc;l=127;drc=f67ec84a19893049f899352c79fa990872da2ff7
  constexpr UINT kDragThreshold = 8;
  const UINT dx = std::abs(pmouse->pt.x - lbutton_down_point.x);
  const UINT dy = std::abs(pmouse->pt.y - lbutton_down_point.y);
  if (dx <= kDragThreshold && dy <= kDragThreshold) {
    return false;
  }

  const UINT dpi = GetWindowDpiSafe(WindowFromPoint(pmouse->pt));
  // We scale it manually since `GetSystemMetricsForDpi` does not work for some
  // reason, see https://github.com/AvaloniaUI/Avalonia/issues/12112
  const UINT threshold = MulDiv(kDragThreshold, dpi, kDefaultDpi);
  const bool is_drag = (dx > threshold || dy > threshold);
  return is_drag;
}

// Open bookmarks in a new tab.
bool HandleBookmark(const MOUSEHOOKSTRUCT* pmouse) {
  const int mode = config.GetBookmarkNewTabMode();
  if (IsKeyPressed(VK_CONTROL) || IsKeyPressed(VK_SHIFT) || mode == 0) {
    return false;
  }

  const POINT pt = pmouse->pt;
  if (!IsOnBookmark(pt)) {
    return false;
  }

  if (!config.IsNewTabDisable() ||
      !IsOnNewTab(GetForegroundWindow(), config.GetDisableTabNames())) {
    // The middle-click below carries no coordinates, so Windows fires it at the
    // live cursor. The cursor can drift off `pt` between the mouse-up and this
    // injection when the hand keeps moving after release, which in a vertical
    // bookmark menu lands the click on the adjacent item. Re-anchor to the
    // validated point so the click opens the bookmark that was actually under
    // the cursor on release.
    SetCursorPos(pt.x, pt.y);
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
  // After a left double-click tab close, Windows still delivers the trailing
  // WM_LBUTTONUP of that gesture. Swallow that single UP only.
  // A sticky flag until the next LBUTTONDOWN is too wide: it can disrupt the
  // next window action (close, minimize, caption drag) on Chrome's custom
  // frame.
  static bool swallow_next_lbutton_up = false;
  // While set, ignore further LBUTTONDBLCLK on tabs so a burst of clicks
  // cannot close more than one tab when Windows omits LBUTTONDOWN between
  // consecutive DBLCLKs. Cleared on any new left-button down.
  static ULONGLONG ignore_lbutton_dblclk_until = 0;
  // Windows pairs two nearby clicks into one double-click even when the first
  // landed on the new-tab button (or other non-tab chrome) and the second on
  // a tab. Only treat DBLCLK as close when the preceding LBUTTONDOWN was on a
  // tab body.
  static bool last_lbutton_down_on_tab = false;

  switch (wParam) {
    case WM_MOUSEMOVE:
      HandleHoverTab(pmouse);
      return false;
    case WM_LBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
      CancelHoverTabTimer();
      swallow_next_lbutton_up = false;
      ignore_lbutton_dblclk_until = 0;
      if (wParam == WM_LBUTTONDOWN) {
        lbutton_down_point = pmouse->pt;
        const auto hit = FindTabHitResult(pmouse->pt, false, true);
        last_lbutton_down_on_tab = hit && !hit->on_close_button;
      } else {
        last_lbutton_down_on_tab = false;
      }
      return false;
    case WM_MBUTTONDOWN:
    case WM_NCMBUTTONDOWN:
      CancelHoverTabTimer();
      return false;
    case WM_RBUTTONDOWN:
    case WM_NCRBUTTONDOWN:
      CancelHoverTabTimer();
      return false;
    case WM_LBUTTONUP:
      if (swallow_next_lbutton_up) {
        swallow_next_lbutton_up = false;
        return true;
      }
      if (HandleDrag(pmouse)) {
        return false;
      } else if (HandleBookmark(pmouse)) {
        return true;
      } else if (HandleCloseButton(pmouse)) {
        return true;
      }
      return false;
    case WM_NCLBUTTONUP:
      if (swallow_next_lbutton_up) {
        swallow_next_lbutton_up = false;
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
      // An explicit wheel switch inside the dwell window must not be
      // overridden by the pending hover activation.
      CancelHoverTabTimer();
      if (HandleMouseWheel(lParam, pmouse)) {
        // Suspend hover re-arm until real cursor movement (see
        // `wheel_switch_point`).
        wheel_switch_point = pmouse->pt;
        // Mark it true only when a tab switch is performed via mouse wheel with
        // right button pressed. Otherwise, normal mouse wheel to switch tabs
        // will swallow irrelevant RBUTTONUP events, causing #198.
        wheel_tab_ing_with_rbutton = IsKeyPressed(VK_RBUTTON);
        return true;
      }
      return false;
    case WM_LBUTTONDBLCLK:
      if (GetTickCount64() < ignore_lbutton_dblclk_until) {
        // Burst DBLCLK only for tabs. Other clicks must pass through even
        // inside the system double-click interval.
        const auto hit = FindTabHitResult(pmouse->pt, false, true);
        if (hit && !hit->on_close_button) {
          swallow_next_lbutton_up = true;
          return true;
        }
        ignore_lbutton_dblclk_until = 0;
      }
      // Require the first click of the double-click pair to be on a tab.
      // Otherwise "new tab, then a quick click on a tab" becomes a system
      // DBLCLK and would close that tab.
      if (!last_lbutton_down_on_tab) {
        return false;
      }
      if (HandleDoubleClick(pmouse)) {
        // Swallow the double-click so Chrome does not process it on the
        // now-destroyed tab (prevents crash on vertical tabs, issue #220).
        // Only the orphan UP of this DBLCLK-UP pair is suppressed; the next
        // real left-button gesture must reach Chrome.
        swallow_next_lbutton_up = true;
        ignore_lbutton_dblclk_until =
            GetTickCount64() + GetDoubleClickTime();
        return true;
      }
      return false;
    case WM_MBUTTONUP:
      if (HandleMiddleClick(pmouse)) {
        return true;
      }
      return false;
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
      return false;
  }
  return false;
}

bool HandleKeepTab(WPARAM wParam) {
  if (!config.IsKeepLastTab()) {
    return false;
  }

  if (!(wParam == 'W' && IsKeyPressed(VK_CONTROL) && !IsKeyPressed(VK_SHIFT)) &&
      !(wParam == VK_F4 && IsKeyPressed(VK_CONTROL))) {
    return false;
  }

  HWND foreground = GetForegroundWindow();
  HWND root_owner = GetAncestor(foreground, GA_ROOTOWNER);
  HWND hwnd = root_owner ? root_owner : foreground;

  const auto tab_count = FindTabCount(hwnd);

  // Unknown tab count means this may be a popup or extension window without
  // visible tab UI. Do not apply keep-last-tab; let Chrome handle the close
  // normally.
  if (!tab_count.has_value()) {
    return false;
  }

  const bool need_keep =
      IsNeedKeep(tab_count.value(), KeepTabTrigger::kKeyboardShortcut);
  if (!need_keep) {
    return false;
  }

  ExecuteCommand(IDC_NEW_TAB, hwnd);
  ExecuteCommand(IDC_WINDOW_CLOSE_OTHER_TABS, hwnd);
  return true;
}

bool HandleOpenUrlNewTab(WPARAM wParam) {
  int mode = config.GetOpenUrlNewTabMode();
  if (mode == 0 || wParam != VK_RETURN || IsKeyPressed(VK_MENU)) {
    return false;
  }

  if (config.IsNewTabDisable() &&
      IsOnNewTab(GetForegroundWindow(), config.GetDisableTabNames())) {
    return false;
  }

  if (!IsOmniboxFocused()) {
    return false;
  }

  if (mode == 1) {
    SendKey(VK_MENU, VK_RETURN);
  } else if (mode == 2) {
    SendKey(VK_SHIFT, VK_MENU, VK_RETURN);
  }
  return true;
}

// Keyboard handler for tab and bookmark operations
bool TabBookmarkKeyboardHandler(WPARAM wParam, LPARAM lParam) {
  // Only handle key down events
  if (lParam & 0x80000000) {
    return false;
  }

  // Keyboard tab switching or typing during the dwell window supersedes
  // hover intent.
  CancelHoverTabTimer();

  if (HandleKeepTab(wParam)) {
    return true;
  }

  if (HandleOpenUrlNewTab(wParam)) {
    return true;
  }

  return false;
}

}  // namespace

void TabBookmark() {
  RegisterMouseHandler(TabBookmarkMouseHandler, HandlerPriority::kNormal);
  RegisterKeyboardHandler(TabBookmarkKeyboardHandler, HandlerPriority::kNormal);
}
