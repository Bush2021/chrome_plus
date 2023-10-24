#include <oleacc.h>
#pragma comment(lib, "oleacc.lib")

#include <thread>
#include <wrl/client.h>

HHOOK mouse_hook = NULL;

using NodePtr = Microsoft::WRL::ComPtr<IAccessible>;

#define KEY_PRESSED 0x8000
bool IsPressed(int key)
{
    return key && (::GetKeyState(key) & KEY_PRESSED) != 0;
}

long GetAccessibleRole(NodePtr node)
{
    VARIANT self;
    self.vt = VT_I4;
    self.lVal = CHILDID_SELF;

    VARIANT role;
    if (S_OK == node->get_accRole(self, &role))
    {
        if (role.vt == VT_I4)
        {
            return role.lVal;
        }
    }
    return 0;
}

long GetAccessibleState(NodePtr node)
{
    VARIANT self;
    self.vt = VT_I4;
    self.lVal = CHILDID_SELF;

    VARIANT state;
    if (S_OK == node->get_accState(self, &state))
    {
        if (state.vt == VT_I4)
        {
            return state.lVal;
        }
    }
    return 0;
}

template <typename Function>
void TraversalAccessible(NodePtr node, Function f)
{
    long childCount = 0;
    if (node && S_OK == node->get_accChildCount(&childCount) && childCount)
    {
        VARIANT *varChildren = (VARIANT *)malloc(sizeof(VARIANT) * childCount);
        if (S_OK == AccessibleChildren(node.Get(), 0, childCount, varChildren, &childCount))
        {
            for (int i = 0; i < childCount; i++)
            {
                if (varChildren[i].vt == VT_DISPATCH)
                {
                    Microsoft::WRL::ComPtr<IDispatch> dispatch = varChildren[i].pdispVal;
                    NodePtr child = nullptr;
                    if (S_OK == dispatch->QueryInterface(IID_IAccessible, (void **)&child))
                    {
                        if ((GetAccessibleState(child) & STATE_SYSTEM_INVISIBLE) == 0) // 只遍历可见节点
                        {
                            if (f(child))
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }
        free(varChildren);
    }
}

template <typename Function>
void TraversalRawAccessible(NodePtr node, Function f)
{
    long childCount = 0;
    if (node && S_OK == node->get_accChildCount(&childCount) && childCount)
    {
        VARIANT *varChildren = (VARIANT *)malloc(sizeof(VARIANT) * childCount);
        if (S_OK == AccessibleChildren(node.Get(), 0, childCount, varChildren, &childCount))
        {
            for (int i = 0; i < childCount; i++)
            {
                if (varChildren[i].vt == VT_DISPATCH)
                {
                    Microsoft::WRL::ComPtr<IDispatch> dispatch = varChildren[i].pdispVal;
                    NodePtr child = nullptr;
                    if (S_OK == dispatch->QueryInterface(IID_IAccessible, (void **)&child))
                    {
                        // if ((GetAccessibleState(child) & STATE_SYSTEM_INVISIBLE) == 0) // 只遍历可见节点
                        {
                            if (f(child))
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }
        free(varChildren);
    }
}

NodePtr FindPageTabList(NodePtr node)
{
    NodePtr PageTabList = nullptr;
    if (node)
    {
        TraversalAccessible(node, [&](NodePtr child) {
            long role = GetAccessibleRole(child);
            if (role == ROLE_SYSTEM_PAGETABLIST)
            {
                PageTabList = child;
            }
            else if (role == ROLE_SYSTEM_PANE || role == ROLE_SYSTEM_TOOLBAR)
            {
                PageTabList = FindPageTabList(child);
            }
            return PageTabList;
        });
    }
    return PageTabList;
}

NodePtr FindPageTab(NodePtr node)
{
    NodePtr PageTab = nullptr;
    if (node)
    {
        TraversalAccessible(node, [&](NodePtr child) {
            long role = GetAccessibleRole(child);
            if (role == ROLE_SYSTEM_PAGETAB)
            {
                PageTab = child;
            }
            else if (role == ROLE_SYSTEM_PANE || role == ROLE_SYSTEM_TOOLBAR)
            {
                PageTab = FindPageTab(child);
            }
            return PageTab;
        });
    }
    return PageTab;
}

NodePtr GetParentElement(NodePtr child)
{
    NodePtr element = nullptr;
    Microsoft::WRL::ComPtr<IDispatch> dispatch = nullptr;
    if (S_OK == child->get_accParent(&dispatch) && dispatch)
    {
        NodePtr parent = nullptr;
        if (S_OK == dispatch->QueryInterface(IID_IAccessible, (void **)&parent))
        {
            element = parent;
        }
    }
    return element;
}

NodePtr GetTopContainerView(HWND hwnd)
{
    NodePtr TopContainerView = nullptr;
    wchar_t name[MAX_PATH];
    if (GetClassName(hwnd, name, MAX_PATH) && wcscmp(name, L"Chrome_WidgetWin_1") == 0)
    {
        NodePtr paccMainWindow = nullptr;
        if (S_OK == AccessibleObjectFromWindow(hwnd, OBJID_WINDOW, IID_PPV_ARGS(&paccMainWindow)))
        {
            NodePtr PageTabList = FindPageTabList(paccMainWindow);
            if (PageTabList)
            {
                TopContainerView = GetParentElement(PageTabList);
            }
            if (!TopContainerView)
            {
                DebugLog(L"FindTopContainerView failed");
            }
        }
    }
    return TopContainerView;
}

NodePtr FindChildElement(NodePtr parent, long role, int skipcount = 0)
{
    NodePtr element = nullptr;
    if (parent)
    {
        int i = 0;
        TraversalAccessible(parent, [&element, &role, &i, &skipcount](NodePtr child) {
            // DebugLog(L"当前 %d,%d", i, skipcount);
            if (GetAccessibleRole(child) == role)
            {
                if (i == skipcount)
                {
                    element = child;
                }
                i++;
            }
            return element != nullptr;
        });
    }
    return element;
}

template <typename Function>
void GetAccessibleSize(NodePtr node, Function f)
{
    VARIANT self;
    self.vt = VT_I4;
    self.lVal = CHILDID_SELF;

    RECT rect;
    if (S_OK == node->accLocation(&rect.left, &rect.top, &rect.right, &rect.bottom, self))
    {
        // rect.left = (int)((float)rect.left);
        // rect.top = (int)((float)rect.top);
        // rect.right = (int)((float)rect.right);
        // rect.bottom = (int)((float)rect.bottom);

        rect.right += rect.left;
        rect.bottom += rect.top;

        f(rect);
    }
}

// 鼠标是否在某个标签上
bool IsOnOneTab(NodePtr top, POINT pt)
{
    bool flag = false;
    NodePtr PageTabList = FindPageTabList(top);
    if (PageTabList)
    {
        NodePtr PageTab = FindPageTab(PageTabList);
        if (PageTab)
        {
            NodePtr PageTabPane = GetParentElement(PageTab);
            if (PageTabPane)
            {
                TraversalAccessible(PageTabPane, [&flag, &pt](NodePtr child) {
                    if (GetAccessibleRole(child) == ROLE_SYSTEM_PAGETAB)
                    {
                        GetAccessibleSize(child, [&flag, &pt](RECT rect) {
                            if (PtInRect(&rect, pt))
                            {
                                flag = true;
                            }
                        });
                    }
                    return flag;
                });
            }
        }
    }
    else
    {
        // if (top) DebugLog(L"IsOnOneTab failed");
    }
    return flag;
}

// 获取鼠标点击在第几个 Tab 上
int GetTabIndex(IAccessible *node, POINT pt)
{
    std::vector <RECT> tab_rects;
    TraversalAccessible(node, [&]
                        (NodePtr child) {
                            if (GetAccessibleRole(child) == ROLE_SYSTEM_PAGETAB)
                            {
                                GetAccessibleSize(child, [&]
                                                  (RECT rect) {
                                                      tab_rects.push_back(rect);
                                                  });
                            }
                            return false;
                        });
    std::sort(tab_rects.begin(), tab_rects.end(), [](auto &a, auto &b) {
        return a.left < b.left;
    });

    int index = 0;
    for (auto rect : tab_rects)
    {
        index++;
        if (PtInRect(&rect, pt))
        {
            break;
        }
    }

    if (index >= 9)
    {
        if (index == tab_rects.size())
        {
            index = 9;
        }
        else
        {
            index = 0;
        }

    }
    return index;
}

// 鼠标是否在某个未激活标签上（似乎没有正确实现）
bool IsOnOneInactiveTab(NodePtr top, POINT pt, int &index)
{
    bool flag = false;
    index = 0;
    NodePtr PageTabList = FindPageTabList(top);
    if (PageTabList)
    {
        TraversalAccessible(PageTabList, [&](NodePtr child) {
            if (GetAccessibleRole(child) == ROLE_SYSTEM_PAGETAB)
            {
                if (GetAccessibleState(child) & STATE_SYSTEM_SELECTED)
                {
                    // 跳过已经选中标签
                    return false;
                }
                GetAccessibleSize(child, [&](RECT rect) {
                    if (PtInRect(&rect, pt))
                    {
                        flag = true;
                        index = GetTabIndex(child.Get(), pt);
                    }
                });
            }
            if (flag)
            {
                return true;
            }
            return flag;
        });
    }
    else
    {
    }
    return flag;
}

// 是否只有一个标签
bool IsKeepLastTab = IsKeepLastTabFun();
bool IsOnlyOneTab(NodePtr top)
{
    if (!IsKeepLastTab)
    {
        return false;
    }
    else
    {
        NodePtr PageTabList = FindPageTabList(top);
        if (PageTabList)
        {
            // DebugLog(L"IsOnlyOneTab");
            long tab_count = 0;

            NodePtr PageTab = FindPageTab(PageTabList);
            if (PageTab)
            {
                NodePtr PageTabPane = GetParentElement(PageTab);
                if (PageTabPane)
                {
                    TraversalAccessible(PageTabPane, [&tab_count](NodePtr child) {
                        // if (GetAccessibleRole(child) == ROLE_SYSTEM_PAGETAB && GetChildCount(child))
                        if (GetAccessibleRole(child) == ROLE_SYSTEM_PAGETAB)
                        {
                            tab_count++;
                        }
                        return false;
                    });
                }
            }
            // DebugLog(L"closing %d,%d", closing, tab_count);
            return tab_count <= 1;
        }
        else
        {
            // if (top) DebugLog(L"IsOnlyOneTab failed");
        }
        return false;
    }
}

// 鼠标是否在标签栏上
bool IsOnTheTabBar(NodePtr top, POINT pt)
{
    bool flag = false;
    NodePtr PageTabList = FindPageTabList(top);
    if (PageTabList)
    {
        GetAccessibleSize(PageTabList, [&flag, &pt](RECT rect) {
            if (PtInRect(&rect, pt))
            {
                flag = true;
            }
        });
    }
    else
    {
        // if (top) DebugLog(L"IsOnTheTabBar failed");
    }
    return flag;
}

bool IsDblClk = IsDblClkFun();
bool IsRClk = IsRClkFun();
bool IsWheelTab = IsWheelTabFun();
bool IsWheelTabWhenPressRButton = IsWheelTabWhenPressRButtonFun();

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

                wheel_tab_ing = true;
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

            // 吞掉原来的右键消息
            SendOneMouse(MOUSEEVENTF_LEFTDOWN);
            SendOneMouse(MOUSEEVENTF_LEFTUP);
            std::thread th([]() {
                Sleep(500);
            });
            th.detach();

            bool isOnOneTab = IsOnOneTab(TopContainerView, pmouse->pt);
            bool isOnlyOneTab = IsOnlyOneTab(TopContainerView);
            bool IsOnOneInactiveTab = IsOnOneInactiveTab(TopContainerView, pmouse->pt, index);

            if (TopContainerView)
            {
            }

            // 右键关闭
            int index = 0;
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
                else if (IsOnOneInactiveTab)
                {
                    // 下面代码没有处理大于 9 个标签的情况，需要改进
                    // 要先切换到需要关闭的标签，所以需要修改代码
                    // 首先从之前的代码中获取当前标签的 index
                    // 然后 IDC_SELECT_TAB_* 时传入 * 为 index 的值（0-9）
                    // 然后把这个整体储存到一个变量 cur_tab 中
                    // 然后 ExecuteCommand(cur_tab);
                    // 最后执行 IDC_CLOSE_TAB
                    if (index >= 0 && index <= 7)
                    {
                        ExecuteCommand(IDC_SELECT_TAB_0 + index);
                        ExecuteCommand(IDC_CLOSE_TAB);
                        // 是否要回到原来的标签？如果要的话还需要确认当前标签的位置
                    }
                    else
                    {
                        // 可能需要处理 index 不在有效范围内的情况
                        ExecuteCommand(IDC_CLOSE_TAB);
                    }

                }
                else
                {
                    ExecuteCommand(IDC_CLOSE_TAB);
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
    }
    return CallNextHookEx(keyboard_hook, nCode, wParam, lParam);
}

void TabBookmark()
{
    mouse_hook = SetWindowsHookEx(WH_MOUSE, MouseProc, hInstance, GetCurrentThreadId());
    keyboard_hook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hInstance, GetCurrentThreadId());
}