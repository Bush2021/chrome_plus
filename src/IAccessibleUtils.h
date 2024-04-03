#ifndef IACCESSIBLEUTILS_H_
#define IACCESSIBLEUTILS_H_

#include <oleacc.h>
#pragma comment(lib, "oleacc.lib")

#include <wrl/client.h>
#include <thread>

using NodePtr = Microsoft::WRL::ComPtr<IAccessible>;

template <typename Function>
void GetAccessibleName(NodePtr node, Function f) {
  VARIANT self;
  self.vt = VT_I4;
  self.lVal = CHILDID_SELF;

  BSTR bstr = nullptr;
  if (S_OK == node->get_accName(self, &bstr)) {
    f(bstr);
    SysFreeString(bstr);
  }
}

template <typename Function>
void GetAccessibleDescription(NodePtr node, Function f) {
  VARIANT self;
  self.vt = VT_I4;
  self.lVal = CHILDID_SELF;

  BSTR bstr = nullptr;
  if (S_OK == node->get_accDescription(self, &bstr)) {
    f(bstr);
    SysFreeString(bstr);
  }
}

template <typename Function>
void GetAccessibleValue(NodePtr node, Function f) {
  VARIANT self;
  self.vt = VT_I4;
  self.lVal = CHILDID_SELF;

  BSTR bstr = nullptr;
  if (S_OK == node->get_accValue(self, &bstr)) {
    f(bstr);
    SysFreeString(bstr);
  }
}

template <typename Function>
void GetAccessibleSize(NodePtr node, Function f) {
  VARIANT self;
  self.vt = VT_I4;
  self.lVal = CHILDID_SELF;

  RECT rect;
  if (S_OK == node->accLocation(&rect.left, &rect.top, &rect.right,
                                &rect.bottom, self)) {
    auto [left, top, right, bottom] = rect;
    f({left, top, right + left, bottom + top});
  }
}

long GetAccessibleRole(NodePtr node) {
  VARIANT self;
  self.vt = VT_I4;
  self.lVal = CHILDID_SELF;

  VARIANT role;
  if (S_OK == node->get_accRole(self, &role)) {
    if (role.vt == VT_I4) {
      return role.lVal;
    }
  }
  return 0;
}

long GetAccessibleState(NodePtr node) {
  VARIANT self;
  self.vt = VT_I4;
  self.lVal = CHILDID_SELF;

  VARIANT state;
  if (S_OK == node->get_accState(self, &state)) {
    if (state.vt == VT_I4) {
      return state.lVal;
    }
  }
  return 0;
}

template <typename Function>
void TraversalAccessible(NodePtr node, Function f) {
  if (!node)
    return;

  long childCount = 0;
  if (S_OK != node->get_accChildCount(&childCount) || !childCount)
    return;

  auto nStep = childCount < 20 ? childCount : 20;
  for (auto i = 0; i < childCount;) {
    auto arrChildren = std::make_unique<VARIANT[]>(nStep);

    long nGetCount = 0;
    if (S_OK == AccessibleChildren(node.Get(), i, nStep, arrChildren.get(),
                                   &nGetCount)) {

      bool bDone = false;
      for (int j = 0; j < nGetCount; ++j) {
        if (arrChildren[j].vt != VT_DISPATCH) {
          continue;
        }
        if (bDone) {
          arrChildren[j].pdispVal->Release(); // 立刻释放，避免内存泄漏
          continue;
        }

        Microsoft::WRL::ComPtr<IDispatch> pDispatch = arrChildren[j].pdispVal;
        NodePtr pChild = nullptr;
        if (S_OK ==
            pDispatch->QueryInterface(IID_IAccessible, (void**)&pChild)) {
          if ((GetAccessibleState(pChild) & STATE_SYSTEM_INVISIBLE) == 0) {
            if (f(pChild)) {
              bDone = true;
            }
          }
        }
      }

      if (bDone) {
        return;
      }
    }

    i += nStep;

    if (i + nStep >= childCount) {
      nStep = childCount - i;
    }
  }
}

// 原 TraversalAccessible 函数，现在被使用步进的新函数替代，只保留 Raw 遍历功能
template <typename Function>
void TraversalRawAccessible(NodePtr node, Function f,
                            bool rawTraversal = false) {
  if (!node)
    return;

  long childCount = 0;
  if (S_OK != node->get_accChildCount(&childCount) || !childCount)
    return;

  std::vector<VARIANT> varChildren(childCount);
  if (S_OK != AccessibleChildren(node.Get(), 0, childCount, varChildren.data(),
                                 &childCount))
    return;

  for (const auto& varChild : varChildren) {
    if (varChild.vt != VT_DISPATCH)
      continue;

    Microsoft::WRL::ComPtr<IDispatch> dispatch = varChild.pdispVal;
    NodePtr child = nullptr;
    if (S_OK != dispatch->QueryInterface(IID_IAccessible, (void**)&child))
      continue;

    if (rawTraversal) {
      TraversalRawAccessible(child, f, true);
      if (f(child))
        break;
    } else {
      if ((GetAccessibleState(child) & STATE_SYSTEM_INVISIBLE) ==
          0)  // 只遍历可见节点
      {
        if (f(child))
          break;
      }
    }
  }
}

NodePtr FindElementWithRole(NodePtr node, long role) {
  NodePtr element = nullptr;
  if (node) {
    TraversalAccessible(node, [&](NodePtr child) {
      if (auto childRole = GetAccessibleRole(child); childRole == role) {
        element = child;
      } else {
        element = FindElementWithRole(child, role);
      }
      return element != nullptr;
    });
  }
  return element;
}

NodePtr GetParentElement(NodePtr child) {
  NodePtr element = nullptr;
  Microsoft::WRL::ComPtr<IDispatch> dispatch = nullptr;
  if (S_OK == child->get_accParent(&dispatch) && dispatch) {
    NodePtr parent = nullptr;
    if (S_OK == dispatch->QueryInterface(IID_IAccessible, (void**)&parent)) {
      element = parent;
    }
  }
  return element;
}

NodePtr GetTopContainerView(HWND hwnd) {
  NodePtr TopContainerView = nullptr;
  wchar_t name[MAX_PATH];
  if (GetClassName(hwnd, name, MAX_PATH) &&
      wcsstr(name, L"Chrome_WidgetWin_") == name) {
    NodePtr paccMainWindow = nullptr;
    if (S_OK == AccessibleObjectFromWindow(hwnd, OBJID_WINDOW,
                                           IID_PPV_ARGS(&paccMainWindow))) {
      NodePtr PageTabList =
          FindElementWithRole(paccMainWindow, ROLE_SYSTEM_PAGETABLIST);
      if (PageTabList) {
        TopContainerView = GetParentElement(PageTabList);
      }
      if (!TopContainerView) {
        DebugLog(L"GetTopContainerView failed");
      }
    }
  }
  return TopContainerView;
}

NodePtr GetMenuBarPane(HWND hwnd) {
  NodePtr MenuBarPane = nullptr;
  wchar_t name[MAX_PATH];
  if (GetClassName(hwnd, name, MAX_PATH) &&
      wcsstr(name, L"Chrome_WidgetWin_") == name) {
    NodePtr paccMainWindow = nullptr;
    if (S_OK == AccessibleObjectFromWindow(hwnd, OBJID_WINDOW,
                                           IID_PPV_ARGS(&paccMainWindow))) {
      NodePtr MenuBar =
          FindElementWithRole(paccMainWindow, ROLE_SYSTEM_MENUBAR);
      if (MenuBar) {
        MenuBarPane = GetParentElement(MenuBar);
      }
      if (!MenuBarPane) {
        DebugLog(L"GetBookmarkView failed");
      }
    }
  }
  return MenuBarPane;
}

NodePtr FindChildElement(NodePtr parent, long role, int skipcount = 0) {
  NodePtr element = nullptr;
  if (parent) {
    int i = 0;
    TraversalAccessible(parent,
                        [&element, &role, &i, &skipcount](NodePtr child) {
                          // DebugLog(L"当前 %d,%d", i, skipcount);
                          if (GetAccessibleRole(child) == role) {
                            if (i == skipcount) {
                              element = child;
                            }
                            i++;
                          }
                          return element != nullptr;
                        });
  }
  return element;
}

// 鼠标是否在某个标签上
bool IsOnOneTab(NodePtr top, POINT pt) {
  NodePtr PageTabList = FindElementWithRole(top, ROLE_SYSTEM_PAGETABLIST);
  if (!PageTabList) {
    return false;
  }

  NodePtr PageTab = FindElementWithRole(PageTabList, ROLE_SYSTEM_PAGETAB);
  if (!PageTab) {
    return false;
  }

  NodePtr PageTabPane = GetParentElement(PageTab);
  if (!PageTabPane) {
    return false;
  }

  bool flag = false;
  TraversalAccessible(PageTabPane, [&flag, &pt](NodePtr child) {
    if (GetAccessibleRole(child) != ROLE_SYSTEM_PAGETAB) {
      return false;
    }

    GetAccessibleSize(child, [&flag, &pt](RECT rect) {
      if (PtInRect(&rect, pt)) {
        flag = true;
      }
    });

    return flag;
  });

  return flag;
}

// 是否只有一个标签
bool IsOnlyOneTab(NodePtr top) {
  if (!IsKeepLastTabFun()) {
    return false;
  }

  NodePtr PageTabList = FindElementWithRole(top, ROLE_SYSTEM_PAGETABLIST);
  if (!PageTabList) {
    return false;
  }

  NodePtr PageTab = FindElementWithRole(PageTabList, ROLE_SYSTEM_PAGETAB);
  if (!PageTab) {
    return false;
  }

  NodePtr PageTabPane = GetParentElement(PageTab);
  if (!PageTabPane) {
    return false;
  }

  std::vector<NodePtr> children;
  TraversalAccessible(PageTabPane, [&children](NodePtr child) {
    children.push_back(child);
    return false;
  });

  auto tab_count =
      std::count_if(children.begin(), children.end(), [](NodePtr child) {
        auto role = GetAccessibleRole(child);
        auto state = GetAccessibleState(child);
        return role == ROLE_SYSTEM_PAGETAB ||
               (role == ROLE_SYSTEM_PAGETABLIST &&
                (state & STATE_SYSTEM_COLLAPSED));
      });

  return tab_count <= 1;
}

// 鼠标是否在标签栏上
bool IsOnTheTabBar(NodePtr top, POINT pt) {
  bool flag = false;
  NodePtr PageTabList = FindElementWithRole(top, ROLE_SYSTEM_PAGETABLIST);
  if (PageTabList) {
    GetAccessibleSize(PageTabList, [&flag, &pt](RECT rect) {
      if (PtInRect(&rect, pt)) {
        flag = true;
      }
    });
  }
  return flag;
}

// 从当前标签页的名称判断是否是新标签页
bool IsNameNewTab(NodePtr top) {

  std::vector<std::wstring> disableTabNames;
  std::wstring disableTabNamesStr = GetDisableTabName();
  std::wstring::size_type start = 0;
  std::wstring::size_type end = disableTabNamesStr.find(L',');
  while (end != std::wstring::npos) {
    std::wstring name = disableTabNamesStr.substr(start, end - start);
    if (!name.empty() && name.front() == L'"') {
      name.erase(0, 1);
    }
    if (!name.empty() && name.back() == L'"') {
      name.erase(name.size() - 1);
    }
    disableTabNames.push_back(name);
    start = end + 1;
    end = disableTabNamesStr.find(L',', start);
  }
  if (start < disableTabNamesStr.length()) {
    std::wstring name = disableTabNamesStr.substr(start);
    if (!name.empty() && name.front() == L'"') {
      name.erase(0, 1);
    }
    if (!name.empty() && name.back() == L'"') {
      name.erase(name.size() - 1);
    }
    disableTabNames.push_back(name);
  }

  bool flag = false;
  std::unique_ptr<wchar_t, decltype(&free)> new_tab_name(nullptr, free);
  NodePtr PageTabList = FindElementWithRole(top, ROLE_SYSTEM_PAGETABLIST);
  if (!PageTabList) {
    return false;
  }

  TraversalAccessible(PageTabList, [&new_tab_name](NodePtr child) {
    if (GetAccessibleRole(child) == ROLE_SYSTEM_PUSHBUTTON) {
      GetAccessibleName(child, [&new_tab_name](BSTR bstr) {
        new_tab_name.reset(_wcsdup(bstr));  // 保存从新建标签页按钮获取的名称
      });
    }
    return false;
  });

  NodePtr PageTab = FindElementWithRole(PageTabList, ROLE_SYSTEM_PAGETAB);
  if (!PageTab) {
    return false;
  }

  NodePtr PageTabPane = GetParentElement(PageTab);
  if (!PageTabPane) {
    return false;
  }

  TraversalAccessible(
      PageTabPane, [&flag, &new_tab_name, &disableTabNames](NodePtr child) {
        if (GetAccessibleState(child) & STATE_SYSTEM_SELECTED) {
          GetAccessibleName(
              child, [&flag, &new_tab_name, &disableTabNames](BSTR bstr) {
                std::wstring_view bstr_view(bstr);
                std::wstring_view new_tab_view(new_tab_name.get());
                flag = (bstr_view.find(new_tab_view) != std::wstring::npos);
                for (const auto& disableTabName : disableTabNames) {
                  if (bstr_view.find(disableTabName) != std::wstring::npos) {
                    flag = true;
                    break;
                  }
                }
              });
        }
        return false;
      });
  return flag;
}

// 从标签页的文档 value 判断是否是新标签页
bool IsDocNewTab() {
  bool flag = false;
  HWND hwnd = FindWindowEx(GetForegroundWindow(), nullptr,
                           L"Chrome_RenderWidgetHostHWND", nullptr);
  NodePtr paccMainWindow = nullptr;
  if (S_OK == AccessibleObjectFromWindow(hwnd, OBJID_WINDOW,
                                         IID_PPV_ARGS(&paccMainWindow))) {
    NodePtr Document =
        FindElementWithRole(paccMainWindow, ROLE_SYSTEM_DOCUMENT);
    if (Document) {
      // Document 的 accValue 需要添加启动参数 --force-renderer-accessibility 来获取
      GetAccessibleValue(Document, [&flag](BSTR bstr) {
        std::wstring_view bstr_view(bstr);
        flag =
            bstr_view.find(L"chrome://newtab") != std::wstring_view::npos ||
            bstr_view.find(L"chrome://new-tab-page") != std::wstring_view::npos;
      });
    }
  }
  return flag;
}

bool IsOnNewTab(NodePtr top) {
  if (!IsNewTabDisableFun()) {
    return false;
  }
  return IsNameNewTab(top) || IsDocNewTab();
}

// 鼠标是否在书签上
bool IsOnBookmark(NodePtr top, POINT pt) {
  if (!top) {
    return false;
  }

  bool flag = false;
  TraversalRawAccessible(
      top,
      [&flag, &pt](NodePtr child) {
        if (GetAccessibleRole(child) != ROLE_SYSTEM_PUSHBUTTON) {
          return false;
        }

        GetAccessibleSize(child, [&flag, &pt, &child](RECT rect) {
          if (!PtInRect(&rect, pt)) {
            return;
          }

          GetAccessibleDescription(child, [&flag](BSTR bstr) {
            std::wstring_view bstr_view(bstr);
            flag = bstr_view.find_first_of(L".:") != std::wstring_view::npos &&
                   bstr_view.substr(0, 11) != L"javascript:";
          });
        });

        return flag;
      },
      true);  // rawTraversal

  return flag;
}

// 鼠标是否在菜单栏的书签文件（夹）上
bool IsOnMenuBookmark(NodePtr top, POINT pt) {
  NodePtr MenuBar = FindElementWithRole(top, ROLE_SYSTEM_MENUBAR);
  if (!MenuBar) {
    return false;
  }

  NodePtr MenuItem = FindElementWithRole(MenuBar, ROLE_SYSTEM_MENUITEM);
  if (!MenuItem) {
    return false;
  }

  NodePtr MenuBarPane = GetParentElement(MenuItem);
  if (!MenuBarPane) {
    return false;
  }

  bool flag = false;
  TraversalAccessible(MenuBarPane, [&flag, &pt](NodePtr child) {
    if (GetAccessibleRole(child) != ROLE_SYSTEM_MENUITEM) {
      return false;
    }

    GetAccessibleSize(child, [&flag, &pt, &child](RECT rect) {
      if (!PtInRect(&rect, pt)) {
        return;
      }

      GetAccessibleDescription(child, [&flag](BSTR bstr) {
        std::wstring_view bstr_view(bstr);
        flag = bstr_view.find_first_of(L".:") != std::wstring_view::npos &&
               bstr_view.substr(0, 11) != L"javascript:";
      });
    });

    return flag;
  });

  return flag;
}

// 地址栏是否已经获得焦点
bool IsOmniboxFocus(NodePtr top) {
  NodePtr ToolBar = FindElementWithRole(top, ROLE_SYSTEM_TOOLBAR);
  if (!ToolBar) {
    return false;
  }

  NodePtr OmniboxEdit = FindElementWithRole(ToolBar, ROLE_SYSTEM_TEXT);
  if (!OmniboxEdit) {
    return false;
  }

  NodePtr ToolBarGroup = GetParentElement(OmniboxEdit);
  if (!ToolBarGroup) {
    return false;
  }

  bool flag = false;
  TraversalAccessible(ToolBarGroup, [&flag](NodePtr child) {
    if (GetAccessibleRole(child) != ROLE_SYSTEM_TEXT) {
      return false;
    }

    if (GetAccessibleState(child) & STATE_SYSTEM_FOCUSED) {
      flag = true;
    }

    return flag;
  });

  return flag;
}

#endif  // IACCESSIBLEUTILS_H_
