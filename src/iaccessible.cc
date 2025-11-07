#include "iaccessible.h"

#include <windows.h>

#include <oleacc.h>

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "config.h"
#include "utils.h"

namespace {

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
void TraversalAccessible(NodePtr node, Function f, bool raw_traversal = false) {
  if (!node) {
    return;
  }

  long child_count = 0;
  if (S_OK != node->get_accChildCount(&child_count) || child_count == 0) {
    return;
  }

  auto step = child_count < 20 ? child_count : 20;
  auto arr_children = std::make_unique<VARIANT[]>(step);
  for (long i = 0; i < child_count;) {
    long get_count = 0;
    if (S_OK != AccessibleChildren(node.Get(), i, step, arr_children.get(),
                                   &get_count)) {
      return;
    }

    bool is_task_completed = false;
    for (long j = 0; j < get_count; ++j) {
      if (arr_children[j].vt != VT_DISPATCH) {
        continue;
      }

      if (is_task_completed) {
        continue;
      }

      NodePtr child_node = nullptr;
      if (S_OK != arr_children[j].pdispVal->QueryInterface(
                      IID_IAccessible, (void**)(&child_node))) {
        continue;
      }

      if (raw_traversal) {
        TraversalAccessible(child_node, f, true);
        if (f(child_node)) {
          is_task_completed = true;
        }
      } else {
        if ((GetAccessibleState(child_node) & STATE_SYSTEM_INVISIBLE) == 0) {
          if (f(child_node)) {
            is_task_completed = true;
          }
        }
      }
    }

    for (long k = 0; k < get_count; ++k) {
      VariantClear(&arr_children[k]);
    }

    if (is_task_completed) {
      return;
    }

    i += step;

    if (i + step >= child_count) {
      step = child_count - i;
    }
  }
}

NodePtr FindElementWithRole(NodePtr node, long role) {
  if (!node) {
    return nullptr;
  }
  NodePtr element = nullptr;
  TraversalAccessible(node, [&](NodePtr child) {
    if (auto childRole = GetAccessibleRole(child); childRole == role) {
      element = child;
    } else {
      element = FindElementWithRole(child, role);
    }
    return element != nullptr;
  });
  return element;
}

NodePtr FindPageTabList(NodePtr node) {
  if (!node) {
    return nullptr;
  }
  NodePtr page_tab_list = nullptr;
  TraversalAccessible(node, [&](NodePtr child) {
    if (auto role = GetAccessibleRole(child); role == ROLE_SYSTEM_PAGETABLIST) {
      page_tab_list = child;
    } else if (role == ROLE_SYSTEM_PANE || role == ROLE_SYSTEM_TOOLBAR) {
      // These two judgments must be retained, otherwise it will crash (#56)
      page_tab_list = FindPageTabList(child);
    }
    return page_tab_list;
  });
  return page_tab_list;
}

NodePtr GetParentElement(NodePtr child) {
  if (!child) {
    return nullptr;
  }
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

[[maybe_unused]] NodePtr FindChildElement(NodePtr parent,
                                          long role,
                                          int skipcount = 0) {
  NodePtr element = nullptr;
  if (parent) {
    int i = 0;
    TraversalAccessible(parent,
                        [&element, &role, &i, &skipcount](NodePtr child) {
                          // DebugLog(L"当前 {}, {}", i, skipcount);
                          if (GetAccessibleRole(child) == role) {
                            if (i == skipcount) {
                              element = child;
                            }
                            ++i;
                          }
                          return element != nullptr;
                        });
  }
  return element;
}

}  // namespace

NodePtr GetChromeWidgetWin(HWND hwnd) {
  NodePtr pacc_main_window = nullptr;
  wchar_t name[MAX_PATH];
  if (GetClassName(hwnd, name, MAX_PATH) &&
      wcsstr(name, L"Chrome_WidgetWin_") == name) {
    if (S_OK == AccessibleObjectFromWindow(hwnd, OBJID_WINDOW,
                                           IID_PPV_ARGS(&pacc_main_window))) {
      return pacc_main_window;
    }
  }
  DebugLog(L"GetChromeWidgetWin failed");
  return nullptr;
}

NodePtr GetTopContainerView(HWND hwnd) {
  NodePtr top_container_view = nullptr;
  NodePtr page_tab_list = FindPageTabList(GetChromeWidgetWin(hwnd));
  if (page_tab_list) {
    top_container_view = GetParentElement(page_tab_list);
  }
  if (!top_container_view) {
    DebugLog(L"GetTopContainerView failed");
  }
  return top_container_view;
}

// Gets the current number of tabs.
int GetTabCount(NodePtr top) {
  NodePtr page_tab_list = FindElementWithRole(top, ROLE_SYSTEM_PAGETABLIST);
  if (!page_tab_list) {
    return 0;
  }
  NodePtr page_tab = FindElementWithRole(page_tab_list, ROLE_SYSTEM_PAGETAB);
  if (!page_tab) {
    return 0;
  }
  NodePtr page_tab_pane = GetParentElement(page_tab);
  if (!page_tab_pane) {
    return 0;
  }
  std::vector<NodePtr> children;
  TraversalAccessible(page_tab_pane, [&children](NodePtr child) {
    children.push_back(child);
    return false;
  });

  int tab_count = 0;
  for (const auto& child : children) {
    auto role = GetAccessibleRole(child);
    auto state = GetAccessibleState(child);
    if (role == ROLE_SYSTEM_PAGETAB ||
        // Grouped and collapsed tabs are counted as one tab.
        (role == ROLE_SYSTEM_PAGETABLIST && (state & STATE_SYSTEM_COLLAPSED))) {
      ++tab_count;
    }
  }
  return tab_count;
}

// Whether the mouse is on a tab
bool IsOnOneTab(NodePtr top, const POINT& pt) {
  NodePtr page_tab_list = FindElementWithRole(top, ROLE_SYSTEM_PAGETABLIST);
  if (!page_tab_list) {
    return false;
  }
  NodePtr page_tab = FindElementWithRole(page_tab_list, ROLE_SYSTEM_PAGETAB);
  if (!page_tab) {
    return false;
  }
  NodePtr page_tab_pane = GetParentElement(page_tab);
  if (!page_tab_pane) {
    return false;
  }

  bool flag = false;
  TraversalAccessible(page_tab_pane, [&flag, &pt](NodePtr child) {
    if (GetAccessibleRole(child) != ROLE_SYSTEM_PAGETAB) {
      return false;
    }
    GetAccessibleSize(child, [&flag, &pt](const RECT& rect) {
      if (PtInRect(&rect, pt)) {
        flag = true;
      }
    });
    return flag;
  });
  return flag;
}

bool IsOnlyOneTab(NodePtr top) {
  if (!config.IsKeepLastTab()) {
    return false;
  }
  auto tab_count = GetTabCount(top);
  return tab_count <= 1;
}

// Whether the mouse is on the tab bar
bool IsOnTheTabBar(NodePtr top, const POINT& pt) {
  bool flag = false;
  NodePtr page_tab_list = FindElementWithRole(top, ROLE_SYSTEM_PAGETABLIST);
  if (!page_tab_list) {
    return false;
  }
  GetAccessibleSize(page_tab_list, [&flag, &pt](const RECT& rect) {
    if (PtInRect(&rect, pt)) {
      flag = true;
    }
  });
  return flag;
}

// Determine whether it is a new tab page from the name of the current tab page.
bool IsNameNewTab(NodePtr top) {
  bool flag = false;
  std::unique_ptr<wchar_t, decltype(&free)> new_tab_name(nullptr, free);
  NodePtr page_tab_list = FindElementWithRole(top, ROLE_SYSTEM_PAGETABLIST);
  if (!page_tab_list) {
    return false;
  }
  TraversalAccessible(page_tab_list, [&new_tab_name](NodePtr child) {
    if (GetAccessibleRole(child) == ROLE_SYSTEM_PUSHBUTTON) {
      GetAccessibleName(child, [&new_tab_name](BSTR bstr) {
        new_tab_name.reset(
            _wcsdup(bstr));  // Save the name obtained from the new tab button.
      });
    }
    return false;
  });
  NodePtr page_tab = FindElementWithRole(page_tab_list, ROLE_SYSTEM_PAGETAB);
  if (!page_tab) {
    return false;
  }
  NodePtr page_tab_pane = GetParentElement(page_tab);
  if (!page_tab_pane) {
    return false;
  }

  std::vector<std::wstring> disable_tab_names =
      StringSplit(config.GetDisableTabName(), L',', L"\"");
  TraversalAccessible(
      page_tab_pane, [&flag, &new_tab_name, &disable_tab_names](NodePtr child) {
        if (GetAccessibleState(child) & STATE_SYSTEM_SELECTED) {
          GetAccessibleName(
              child, [&flag, &new_tab_name, &disable_tab_names](BSTR bstr) {
                std::wstring_view bstr_view(bstr);
                std::wstring_view new_tab_view(new_tab_name.get());
                flag = (bstr_view.find(new_tab_view) != std::wstring::npos);
                for (const auto& tab_name : disable_tab_names) {
                  if (bstr_view.find(tab_name) != std::wstring::npos) {
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

// Determine whether it is a new tab page from the document value of the tab
// page.
bool IsDocNewTab() {
  const auto cr_command_line = config.GetCommandLine();
  if (cr_command_line.find(L"--force-renderer-accessibility") ==
      std::wstring::npos) {
    return false;
  }

  bool flag = false;
  HWND hwnd = FindWindowEx(GetForegroundWindow(), nullptr,
                           L"Chrome_RenderWidgetHostHWND", nullptr);
  NodePtr pacc_main_window = nullptr;
  if (S_OK != AccessibleObjectFromWindow(hwnd, OBJID_WINDOW,
                                         IID_PPV_ARGS(&pacc_main_window))) {
    return false;
  }

  NodePtr document =
      FindElementWithRole(pacc_main_window, ROLE_SYSTEM_DOCUMENT);
  if (document) {
    // The `accValue` of document needs to be obtained by adding the startup
    // parameter `--force-renderer-accessibility=basic`. However, this
    // parameter will slightly affect the performance of the browser when
    // loading pages with a large number of elements. Therefore, it is not
    // enabled by default. If users need to use this feature, they may add the
    // parameter manually.
    GetAccessibleValue(document, [&flag](BSTR bstr) {
      std::wstring_view bstr_view(bstr);
      flag = bstr_view.find(L"://newtab") != std::wstring_view::npos ||
             bstr_view.find(L"://new-tab-page") != std::wstring_view::npos;
    });
  }
  return flag;
}

bool IsOnNewTab(NodePtr top) {
  if (!config.IsNewTabDisable()) {
    return false;
  }
  return IsNameNewTab(top) || IsDocNewTab();
}

// Whether the mouse is on a bookmark.
bool IsOnBookmark(HWND hwnd, const POINT& pt) {
  bool flag = false;
  std::function<bool(NodePtr)> LambdaEnumChild =
      [&pt, &flag, &LambdaEnumChild](NodePtr child) -> bool {
    auto role = GetAccessibleRole(child);
    if (role == ROLE_SYSTEM_PUSHBUTTON || role == ROLE_SYSTEM_MENUITEM) {
      bool is_in_rect = false;
      GetAccessibleSize(child, [&is_in_rect, &pt](const RECT& rect) {
        if (PtInRect(&rect, pt)) {
          is_in_rect = true;
        }
      });
      if (is_in_rect) {
        GetAccessibleDescription(child, [&flag](BSTR bstr) {
          std::wstring_view bstr_view(bstr);
          flag = (bstr_view.find_first_of(L".:") != std::wstring_view::npos) &&
                 (bstr_view.substr(0, 11) != L"javascript:");
        });
        if (flag) {
          return true;  // Stop traversing if found.
        }
      }
    }
    // traverse the child nodes.
    TraversalAccessible(child, LambdaEnumChild);
    return flag;
  };
  // Start traversing.
  TraversalAccessible(GetChromeWidgetWin(hwnd), LambdaEnumChild);
  return flag;
}

// Expanded drop-down list in the address bar
bool IsOnExpandedList(HWND hwnd, const POINT& pt) {
  bool flag = false;
  std::function<bool(NodePtr)> LambdaEnumChild =
      [&pt, &flag, &LambdaEnumChild](NodePtr child) -> bool {
    if (GetAccessibleRole(child) == ROLE_SYSTEM_LIST &&
        (GetAccessibleState(child) & STATE_SYSTEM_EXPANDED)) {
      GetAccessibleSize(child, [&flag, &pt](const RECT& rect) {
        if (PtInRect(&rect, pt)) {
          flag = true;
        }
      });
      if (flag) {
        return true;
      }
    }
    TraversalAccessible(child, LambdaEnumChild);
    return flag;
  };
  TraversalAccessible(GetChromeWidgetWin(hwnd), LambdaEnumChild);
  return flag;
}

bool IsOmniboxFocus(NodePtr top) {
  if (!top) {
    return false;
  }

  bool is_focused = false;
  std::function<bool(NodePtr)> find_focused =
      [&is_focused, &find_focused](NodePtr node) -> bool {
    if (GetAccessibleRole(node) == ROLE_SYSTEM_TEXT &&
        (GetAccessibleState(node) & STATE_SYSTEM_FOCUSED)) {
      is_focused = true;
      return true;
    }
    TraversalAccessible(node, find_focused, false);
    return is_focused;
  };

  std::function<bool(NodePtr)> find_toolbar = [&](NodePtr node) -> bool {
    if (GetAccessibleRole(node) == ROLE_SYSTEM_TOOLBAR) {
      find_focused(node);
      if (is_focused) {
        return true;
      }
      return false;
    }
    TraversalAccessible(node, find_toolbar, false);

    return is_focused;
  };
  TraversalAccessible(top, find_toolbar, false);
  return is_focused;
}

// Whether the mouse is on the close button of a tab.
// Should be used together with `IsOnOneTab` to search the close button.
bool IsOnCloseButton(NodePtr top, const POINT& pt) {
  bool flag = false;
  TraversalAccessible(
      top,
      [&pt, &flag](NodePtr child) {
        if (GetAccessibleRole(child) == ROLE_SYSTEM_PUSHBUTTON) {
          GetAccessibleSize(child, [&pt, &flag](const RECT& rect) {
            if (PtInRect(&rect, pt)) {
              flag = true;
            }
          });
        }
        return flag;
      },
      true);  // raw_traversal
  return flag;
}

bool IsOnFindBarPane(const POINT& pt) {
  NodePtr root = nullptr;
  if ((S_OK != AccessibleObjectFromWindow(GetFocus(), OBJID_CLIENT,
                                          IID_PPV_ARGS(&root))) ||
      !root) {
    return false;
  }

  NodePtr text_element = nullptr;
  // Nov 6, 2025 - Chrome 142.0.7444.135
  //   root
  //   └─ PANE
  //      └─ PANE
  //         └─ PANE
  //            └─ PANE
  //               └─ TEXT (focused, the input field of find-in-page bar)
  std::function<bool(NodePtr)> find_focused =
      [&text_element, &find_focused](NodePtr node) -> bool {
    if (GetAccessibleRole(node) == ROLE_SYSTEM_TEXT &&
        (GetAccessibleState(node) & STATE_SYSTEM_FOCUSED)) {
      text_element = node;
      return true;
    }
    TraversalAccessible(node, find_focused, false);
    return text_element != nullptr;
  };
  TraversalAccessible(root, find_focused, false);
  if (!text_element) {
    return false;
  }

  auto parent = GetParentElement(text_element);
  // Assume there is only one level of PANE parent structure, which covers the
  // whole find-in-page bar.
  if (!parent || (GetAccessibleRole(parent) != ROLE_SYSTEM_PANE)) {
    return false;
  }

  bool flag = false;
  GetAccessibleSize(parent, [&flag, &pt](const RECT& rect) {
    if (PtInRect(&rect, pt)) {
      flag = true;
    }
  });
  return flag;
}