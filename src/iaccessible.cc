#include "iaccessible.h"

#include <windows.h>

#include <oleacc.h>

#include <algorithm>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include "config.h"
#include "utils.h"

namespace {

template <typename Function>
void GetAccessibleName(const NodePtr& node, Function f) {
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
void GetAccessibleDescription(const NodePtr& node, Function f) {
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
void GetAccessibleValue(const NodePtr& node, Function f) {
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
void GetAccessibleSize(const NodePtr& node, Function f) {
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

long GetAccessibleRole(const NodePtr& node) {
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

long GetAccessibleState(const NodePtr& node) {
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
void TraversalAccessible(const NodePtr& node,
                         Function f,
                         bool raw_traversal = false) {
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

NodePtr GetParentElement(const NodePtr& child) {
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

bool IsBrowserUIContainer(long role) {
  // We should keep these restrictions to avoid unbounded DFS, which may
  // traverse into unexpected subtree (e.g. web content) instead of staying
  // within the browser UI tree. We might get buggy controls or false
  // positives (#56, #191).
  return role == ROLE_SYSTEM_PANE || role == ROLE_SYSTEM_TOOLBAR;
}
// Recursively find an element with the specified role, with or without pruning.
// To find a element when specifing more than one condition (e.g. name
// description, state, etc), use lambda to recurse manually.
NodePtr FindElementWithRole(const NodePtr& node,
                            long target_role,
                            auto predicate) {
  if (!node) {
    return nullptr;
  }
  NodePtr element = nullptr;
  TraversalAccessible(node, [&](const NodePtr& child) {
    const auto child_role = GetAccessibleRole(child);
    if (child_role == target_role) {
      element = child;
      return true;
    } else if (predicate(child_role)) {
      element = FindElementWithRole(child, target_role, predicate);
      if (element) {
        return true;
      }
    }
    return false;
  });
  return element;
}

NodePtr FindElementWithRole(const NodePtr& node, long target_role) {
  return FindElementWithRole(node, target_role, IsBrowserUIContainer);
}

NodePtr FindPageTabPane(const NodePtr& node) {
  if (!node) {
    return nullptr;
  }
  NodePtr page_tab_list = FindElementWithRole(node, ROLE_SYSTEM_PAGETABLIST);
  if (!page_tab_list) {
    return nullptr;
  }
  NodePtr page_tab = FindElementWithRole(page_tab_list, ROLE_SYSTEM_PAGETAB);
  if (!page_tab) {
    return nullptr;
  }
  return GetParentElement(page_tab);
}

[[maybe_unused]] NodePtr FindChildElement(const NodePtr& parent,
                                          long role,
                                          int skipcount = 0) {
  NodePtr element = nullptr;
  if (parent) {
    int i = 0;
    TraversalAccessible(
        parent, [&element, &role, &i, &skipcount](const NodePtr& child) {
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

// This is the name of the new tab button since the name may vary with different
// language versions of Chrome. Should be same as the name of the default new
// tab page.
std::optional<std::wstring> GetStdNameFromNewTabButton(
    const NodePtr& page_tab_list) {
  static std::optional<std::wstring> cached_std_name;
  if (cached_std_name.has_value()) {
    return cached_std_name;
  }

  if (!page_tab_list) {
    return std::nullopt;
  }

  std::wstring std_name;
  TraversalAccessible(page_tab_list, [&std_name](const NodePtr& child) {
    if (!std_name.empty()) {
      return false;
    }
    if (GetAccessibleRole(child) != ROLE_SYSTEM_PUSHBUTTON) {
      return false;
    }
    GetAccessibleName(child, [&std_name](BSTR bstr) {
      // TODO: figure out why sometimes (143.0.7499.41) the name is empty even
      // we do find the push button (#191).
      if (!bstr) {
        return;
      }
      std_name.assign(bstr);
    });
    return !std_name.empty();
  });

  if (!std_name.empty()) {
    cached_std_name = std_name;
  }
  return cached_std_name;
}

// Determine whether it is a new tab page from the name of the current tab page.
bool IsNameNewTab(const NodePtr& top) {
  if (!top) {
    return false;
  }

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

  bool is_new_tab = false;
  const auto names_from_config =
      StringSplit(config.GetDisableTabName(), L',', L"\"");
  const auto std_name = GetStdNameFromNewTabButton(page_tab_list);
  TraversalAccessible(
      page_tab_pane,
      [&is_new_tab, &std_name, &names_from_config](const NodePtr& child) {
        if ((GetAccessibleState(child) & STATE_SYSTEM_SELECTED) == 0) {
          return false;
        }
        GetAccessibleName(
            child, [&is_new_tab, &std_name, &names_from_config](BSTR bstr) {
              if (!bstr || is_new_tab) {
                return;
              }

              std::wstring_view selected_tab_name(bstr);
              if (std_name.has_value() &&
                  selected_tab_name.find(std_name.value()) !=
                      std::wstring_view::npos) {
                is_new_tab = true;
                return;
              }

              for (const auto& name_from_config : names_from_config) {
                if (name_from_config.empty()) {
                  continue;
                }
                if (selected_tab_name.find(name_from_config) !=
                    std::wstring_view::npos) {
                  is_new_tab = true;
                  break;
                }
              }
            });
        return is_new_tab;
      });
  return is_new_tab;
}

// Determine whether it is a new tab page from the document value of the tab
// page.
bool IsDocNewTab() {
  static const bool has_a11y_flag =
      config.GetCommandLine().find(L"--force-renderer-accessibility") !=
      std::wstring::npos;

  if (!has_a11y_flag) {
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
      FindElementWithRole(pacc_main_window, ROLE_SYSTEM_DOCUMENT,
                          [](long role) { return role == ROLE_SYSTEM_PANE; });
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

}  // namespace

NodePtr GetChromeWidgetWin(HWND hwnd) {
  NodePtr pacc_main_window = nullptr;
  wchar_t name[MAX_PATH];
  if (!GetClassName(hwnd, name, MAX_PATH)) {
    DebugLog(L"GetChromeWidgetWin failed: GetClassName failed, error {}",
             GetLastError());
    return nullptr;
  }
  if (wcsstr(name, L"Chrome_WidgetWin_") != name) {
    DebugLog(L"GetChromeWidgetWin failed: class name mismatch, got '{}'", name);
    return nullptr;
  }
  if (S_OK == AccessibleObjectFromWindow(hwnd, OBJID_WINDOW,
                                         IID_PPV_ARGS(&pacc_main_window))) {
    return pacc_main_window;
  }
  return nullptr;
}

NodePtr GetTopContainerView(HWND hwnd) {
  NodePtr top_container_view = nullptr;
  NodePtr page_tab_list =
      FindElementWithRole(GetChromeWidgetWin(hwnd), ROLE_SYSTEM_PAGETABLIST);
  if (page_tab_list) {
    top_container_view = GetParentElement(page_tab_list);
  }
  if (!top_container_view) {
    DebugLog(L"GetTopContainerView failed");
  }
  return top_container_view;
}

// Gets the current number of tabs.
int GetTabCount(const NodePtr& top) {
  NodePtr page_tab_pane = FindPageTabPane(top);
  if (!page_tab_pane) {
    return 0;
  }
  std::vector<NodePtr> children;
  TraversalAccessible(page_tab_pane, [&children](const NodePtr& child) {
    children.push_back(child);
    return false;
  });

  return static_cast<int>(
      std::ranges::count_if(children, [](const auto& child) {
        auto role = GetAccessibleRole(child);
        return role == ROLE_SYSTEM_PAGETAB ||
               (role == ROLE_SYSTEM_PAGETABLIST &&
                (GetAccessibleState(child) & STATE_SYSTEM_COLLAPSED));
      }));
}

// Whether the mouse is on a tab
bool IsOnOneTab(const NodePtr& top, POINT pt) {
  NodePtr page_tab_pane = FindPageTabPane(top);
  if (!page_tab_pane) {
    return false;
  }

  bool flag = false;
  TraversalAccessible(page_tab_pane, [&flag, &pt](const NodePtr& child) {
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

bool IsOnlyOneTab(const NodePtr& top) {
  if (!config.IsKeepLastTab()) {
    return false;
  }
  auto tab_count = GetTabCount(top);
  return tab_count <= 1;
}

// Whether the mouse is on the tab bar
bool IsOnTheTabBar(const NodePtr& top, POINT pt) {
  bool flag = false;
  NodePtr page_tab_list = FindElementWithRole(top, ROLE_SYSTEM_PAGETABLIST);
  if (!page_tab_list) {
    return false;
  }
  GetAccessibleSize(page_tab_list, [&flag, &pt](RECT rect) {
    if (PtInRect(&rect, pt)) {
      flag = true;
    }
  });
  return flag;
}

bool IsOnNewTab(const NodePtr& top) {
  if (!config.IsNewTabDisable()) {
    return false;
  }
  return IsNameNewTab(top) || IsDocNewTab();
}

// Whether the mouse is on a bookmark.
bool IsOnBookmark(HWND hwnd, POINT pt) {
  bool flag = false;
  auto find_bookmark = [&pt, &flag](this auto&& self,
                                    const NodePtr& child) -> bool {
    auto role = GetAccessibleRole(child);
    if (role == ROLE_SYSTEM_PUSHBUTTON || role == ROLE_SYSTEM_MENUITEM) {
      bool is_in_rect = false;
      GetAccessibleSize(child, [&is_in_rect, &pt](RECT rect) {
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
    TraversalAccessible(child, self);
    return flag;
  };
  // Start traversing.
  TraversalAccessible(GetChromeWidgetWin(hwnd), find_bookmark);
  return flag;
}

// Expanded drop-down list in the address bar
bool IsOnExpandedList(HWND hwnd, POINT pt) {
  bool flag = false;
  auto find_drop_down_list = [&pt, &flag](this auto&& self,
                                          const NodePtr& child) -> bool {
    if (GetAccessibleRole(child) == ROLE_SYSTEM_LIST &&
        (GetAccessibleState(child) & STATE_SYSTEM_EXPANDED)) {
      GetAccessibleSize(child, [&flag, &pt](RECT rect) {
        if (PtInRect(&rect, pt)) {
          flag = true;
        }
      });
      if (flag) {
        return true;
      }
    }
    TraversalAccessible(child, self);
    return flag;
  };
  TraversalAccessible(GetChromeWidgetWin(hwnd), find_drop_down_list);
  return flag;
}

bool IsOmniboxFocus(const NodePtr& top) {
  if (!top) {
    return false;
  }

  bool is_focused = false;
  auto find_focused = [&is_focused](this auto&& self,
                                    const NodePtr& node) -> bool {
    if (GetAccessibleRole(node) == ROLE_SYSTEM_TEXT &&
        (GetAccessibleState(node) & STATE_SYSTEM_FOCUSED)) {
      is_focused = true;
      return true;
    }
    TraversalAccessible(node, self, false);
    return is_focused;
  };

  auto find_toolbar = [&](this auto&& self, const NodePtr& node) -> bool {
    if (GetAccessibleRole(node) == ROLE_SYSTEM_TOOLBAR) {
      find_focused(node);
      if (is_focused) {
        return true;
      }
      return false;
    }
    TraversalAccessible(node, self, false);

    return is_focused;
  };
  TraversalAccessible(top, find_toolbar, false);
  return is_focused;
}

// Whether the mouse is on the close button of a tab.
// Should be used together with `IsOnOneTab` to search the close button.
bool IsOnCloseButton(const NodePtr& top, POINT pt) {
  if (!top) {
    return false;
  }

  bool found = false;
  auto find_hit_button = [&](this auto&& self, const NodePtr& node) -> bool {
    if (GetAccessibleRole(node) == ROLE_SYSTEM_PUSHBUTTON) {
      GetAccessibleSize(node, [&](RECT rect) {
        if (PtInRect(&rect, pt)) {
          found = true;
        }
      });
      if (found) {
        return true;
      }
    }
    TraversalAccessible(node, self);
    return found;
  };
  TraversalAccessible(top, find_hit_button, false);
  return found;
}

bool IsOnFindBarPane(POINT pt) {
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
  auto find_focused = [&text_element](this auto&& self,
                                      const NodePtr& node) -> bool {
    if (GetAccessibleRole(node) == ROLE_SYSTEM_TEXT &&
        (GetAccessibleState(node) & STATE_SYSTEM_FOCUSED)) {
      text_element = node;
      return true;
    }
    TraversalAccessible(node, self, false);
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
  GetAccessibleSize(parent, [&flag, &pt](RECT rect) {
    if (PtInRect(&rect, pt)) {
      flag = true;
    }
  });
  return flag;
}