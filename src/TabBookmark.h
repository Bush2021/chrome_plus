#ifndef TABBOOKMARK_H_
#define TABBOOKMARK_H_

#include "IAccessibleUtils.h"

HHOOK mouse_hook = NULL;

#define KEY_PRESSED 0x8000
bool IsPressed(int key)
{
    return key && (::GetKeyState(key) & KEY_PRESSED) != 0;
}

class SendKeys
{
  public:
    template <typename... T>
    SendKeys(T... keys)
    {
        std::vector<int> keys_ = {keys...};
        for (auto &key : keys_)
        {
            INPUT input = {0};
            input.type = INPUT_KEYBOARD;
            input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
            input.ki.wVk = key;

            // 修正鼠标消息
            switch (key)
            {
            case VK_MBUTTON:
                input.type = INPUT_MOUSE;
                input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
                break;
            }

            inputs_.push_back(input);
        }

        SendInput((UINT)inputs_.size(), &inputs_[0], sizeof(INPUT));
    }
    ~SendKeys()
    {
        for (auto &input : inputs_)
        {
            input.ki.dwFlags |= KEYEVENTF_KEYUP;

            // 修正鼠标消息
            switch (input.ki.wVk)
            {
            case VK_MBUTTON:
                input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
                break;
            }
        }

        SendInput((UINT)inputs_.size(), &inputs_[0], sizeof(INPUT));
    }

  private:
    std::vector<INPUT> inputs_;
};

bool IsDblClk = IsDblClkFun();
bool IsRClk = IsRClkFun();
bool IsWheelTab = IsWheelTabFun();
bool IsWheelTabWhenPressRButton = IsWheelTabWhenPressRButtonFun();
bool IsBookmarkNewTab = IsBookmarkNewTabFun();

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    static bool wheel_tab_ing = false;
    static bool double_click_ing = false;

    if (nCode != HC_ACTION)
    {
        return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
    }

    if (nCode == HC_ACTION)
    {
        PMOUSEHOOKSTRUCT pmouse = (PMOUSEHOOKSTRUCT)lParam;

        if (wParam == WM_MOUSEMOVE || wParam == WM_NCMOUSEMOVE)
        {
            return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
        }

        if (pmouse->dwExtraInfo == MAGIC_CODE)
        {
            // DebugLog(L"MAGIC_CODE %x", wParam);
            goto next;
        }

        if (wParam == WM_RBUTTONUP && wheel_tab_ing)
        {
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

        if (wParam == WM_MOUSEWHEEL)
        {
            HWND hwnd = WindowFromPoint(pmouse->pt);
            NodePtr TopContainerView = GetTopContainerView(hwnd);

            PMOUSEHOOKSTRUCTEX pwheel = (PMOUSEHOOKSTRUCTEX)lParam;
            int zDelta = GET_WHEEL_DELTA_WPARAM(pwheel->mouseData);

            // 是否启用鼠标停留在标签栏时滚轮切换标签
            if (IsWheelTab && IsOnTheTabBar(TopContainerView, pmouse->pt))
            {
                hwnd = GetTopWnd(hwnd);
                if (zDelta > 0)
                {
                    ExecuteCommand(IDC_SELECT_PREVIOUS_TAB, hwnd);
                }
                else
                {
                    ExecuteCommand(IDC_SELECT_NEXT_TAB, hwnd);
                }

                // wheel_tab_ing = true;
                if (TopContainerView)
                {
                }
                // DebugLog(L"WM_MOUSEWHEEL");
                return 1;
            }

            // 是否启用在任何位置按住右键时滚轮切换标签
            if (IsWheelTabWhenPressRButton && IsPressed(VK_RBUTTON))
            {
                hwnd = GetTopWnd(hwnd);
                if (zDelta > 0)
                {
                    ExecuteCommand(IDC_SELECT_PREVIOUS_TAB, hwnd);
                }
                else
                {
                    ExecuteCommand(IDC_SELECT_NEXT_TAB, hwnd);
                }

                wheel_tab_ing = true;
                if (TopContainerView)
                {
                }
                // DebugLog(L"WM_MOUSEWHEEL");
                return 1;
            }
        }

        if (IsDblClk && wParam == WM_LBUTTONDBLCLK)
        {
            HWND hwnd = WindowFromPoint(pmouse->pt);
            NodePtr TopContainerView = GetTopContainerView(hwnd);

            bool isOnOneTab = IsOnOneTab(TopContainerView, pmouse->pt);
            bool isOnlyOneTab = IsOnlyOneTab(TopContainerView);

            if (TopContainerView)
            {
            }

            // 双击关闭
            if (isOnOneTab)
            {
                if (isOnlyOneTab)
                {
                    // DebugLog(L"keep_tab");
                    // ExecuteCommand(IDC_NEW_TAB, hwnd);
                    ExecuteCommand(IDC_NEW_TAB);
                    ExecuteCommand(IDC_SELECT_PREVIOUS_TAB);
                    ExecuteCommand(IDC_CLOSE_TAB);
                }
                else
                {
                    ExecuteCommand(IDC_CLOSE_TAB);
                }
            }
        }

        if (IsRClk && wParam == WM_RBUTTONUP && !IsPressed(VK_SHIFT))

        {
            HWND hwnd = WindowFromPoint(pmouse->pt);
            NodePtr TopContainerView = GetTopContainerView(hwnd);

            bool isOnOneTab = IsOnOneTab(TopContainerView, pmouse->pt);
            bool isOnlyOneTab = IsOnlyOneTab(TopContainerView);

            if (TopContainerView)
            {
            }

            // 右键关闭
            if (isOnOneTab)
            {

                if (isOnlyOneTab)
                {
                    // DebugLog(L"keep_tab");
                    // ExecuteCommand(IDC_NEW_TAB, hwnd);
                    ExecuteCommand(IDC_NEW_TAB);
                    ExecuteCommand(IDC_SELECT_PREVIOUS_TAB);
                    ExecuteCommand(IDC_CLOSE_TAB);
                    return 1;
                }
                else
                {
                    SendKeys(VK_MBUTTON);
                    return 1;
                }
            }
        }

        if (wParam == WM_MBUTTONUP)
        {
            HWND hwnd = WindowFromPoint(pmouse->pt);
            NodePtr TopContainerView = GetTopContainerView(hwnd);

            bool isOnOneTab = IsOnOneTab(TopContainerView, pmouse->pt);
            bool isOnlyOneTab = IsOnlyOneTab(TopContainerView);

            if (TopContainerView)
            {
            }

            if (isOnOneTab && isOnlyOneTab)
            {
                // DebugLog(L"keep_tab");
                // ExecuteCommand(IDC_NEW_TAB, hwnd);
                ExecuteCommand(IDC_NEW_TAB);
                // ExecuteCommand(IDC_SELECT_PREVIOUS_TAB);
                // ExecuteCommand(IDC_CLOSE_TAB);
            }
        }

        if (IsBookmarkNewTab && wParam == WM_LBUTTONUP && !IsPressed(VK_CONTROL) &&
            !IsPressed(VK_SHIFT))
        {
            HWND hwnd = WindowFromPoint(pmouse->pt);
            NodePtr TopContainerView = GetTopContainerView(hwnd);

            bool isOnBookmark = IsOnBookmark(TopContainerView, pmouse->pt);
            bool isOnNewTab = IsOnNewTab(TopContainerView);

            if (!isOnNewTab && TopContainerView && isOnBookmark)
            {
                DebugLog(L"isOnBookmark: = Shift+MButton");
                SendKeys(VK_MBUTTON, VK_SHIFT);
                return 1;
            }
        }
        if (IsBookmarkNewTab && wParam == WM_LBUTTONUP && !IsPressed(VK_CONTROL) &&
            !IsPressed(VK_SHIFT))
        {
            HWND hwnd_p = WindowFromPoint(pmouse->pt);
            HWND hwnd_k = GetFocus();
            NodePtr TopContainerView = GetTopContainerView(hwnd_k);
            NodePtr MenuBarPane = GetMenuBarPane(hwnd_p);

            bool isOnOneMenuBookmark = IsOnOneMenuBookmark(MenuBarPane, pmouse->pt);
            bool isOnNewTab = IsOnNewTab(TopContainerView);

            if (!isOnNewTab && MenuBarPane && isOnOneMenuBookmark)
            {
                DebugLog(L"isOnOneMenuBookmark: = Shift+MButton");
                SendKeys(VK_MBUTTON, VK_SHIFT);
                return 1;
            }
        }
    }
next:
    // DebugLog(L"CallNextHookEx %X", wParam);
    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
}

bool IsNeedKeep()
{
    bool keep_tab = false;

    NodePtr TopContainerView = GetTopContainerView(GetForegroundWindow());
    if (IsOnlyOneTab(TopContainerView))
    {
        keep_tab = true;
    }

    if (TopContainerView)
    {
    }

    return keep_tab;
}

bool IsNeedOpenUrlInNewTab()
{
    bool open_url_ing = false;

    NodePtr TopContainerView = GetTopContainerView(GetForegroundWindow());
    if (IsOmniboxFocus(TopContainerView))
    {
        if (!IsOnNewTab(TopContainerView))
        {
            open_url_ing = true;
        }
    }

    if (TopContainerView)
    {
    }

    return open_url_ing;
}

HHOOK keyboard_hook = NULL;
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && !(lParam & 0x80000000)) // pressed
    {
        bool keep_tab = false;

        if (wParam == 'W' && IsPressed(VK_CONTROL) && !IsPressed(VK_SHIFT))
        {
            keep_tab = IsNeedKeep();
        }
        if (wParam == VK_F4 && IsPressed(VK_CONTROL))
        {
            keep_tab = IsNeedKeep();
        }

        if (keep_tab)
        {
            ExecuteCommand(IDC_NEW_TAB);
            ExecuteCommand(IDC_SELECT_PREVIOUS_TAB);
            ExecuteCommand(IDC_CLOSE_TAB);
            return 1;
        }

        bool open_url_ing = false;
        bool IsOpenUrlNewTab = IsOpenUrlNewTabFun();

        if (IsOpenUrlNewTab && wParam == VK_RETURN && !IsPressed(VK_MENU))
        {
            open_url_ing = IsNeedOpenUrlInNewTab();
        }

        if (open_url_ing)
        {
            SendKeys(VK_MENU, VK_RETURN);
            return 1;
        }
    }
    return CallNextHookEx(keyboard_hook, nCode, wParam, lParam);
}

void TabBookmark()
{
    mouse_hook =
        SetWindowsHookEx(WH_MOUSE, MouseProc, hInstance, GetCurrentThreadId());
    keyboard_hook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hInstance,
                                     GetCurrentThreadId());
}

#endif // TABBOOKMARK_H_