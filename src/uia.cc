#include "uia.h"

#include <uiautomation.h>
#include <wrl/client.h>

#include <algorithm>
#include <initializer_list>
#include <optional>
#include <string_view>
#include <vector>

#include "com_initializer.h"
#include "utils.h"

namespace {

using Microsoft::WRL::ComPtr;

class ScopedVariant {
 public:
  ScopedVariant() { VariantInit(&value_); }
  ~ScopedVariant() { VariantClear(&value_); }
  ScopedVariant(const ScopedVariant&) = delete;
  ScopedVariant& operator=(const ScopedVariant&) = delete;
  ScopedVariant(ScopedVariant&&) = delete;
  ScopedVariant& operator=(ScopedVariant&&) = delete;

  VARIANT* Ptr() { return &value_; }
  VARIANT& Ref() { return value_; }

 private:
  VARIANT value_;
};

struct CachedClassConditions {
  ComPtr<IUIAutomationCondition> horizontal_tab_strip_region_view;
  ComPtr<IUIAutomationCondition> tab_strip_drag_context;
  ComPtr<IUIAutomationCondition> tab_container_impl;
  ComPtr<IUIAutomationCondition> vertical_unpinned_tab_container_view;
  ComPtr<IUIAutomationCondition> scroll_view;
  ComPtr<IUIAutomationCondition> tab;
  ComPtr<IUIAutomationCondition> vertical_tab_view;
  ComPtr<IUIAutomationCondition> tab_close_button;
  ComPtr<IUIAutomationCondition> bookmark_button;
  ComPtr<IUIAutomationCondition> menu_item_view;
  ComPtr<IUIAutomationCondition> omnibox_view_views;
  ComPtr<IUIAutomationCondition> omnibox_result_view;
  ComPtr<IUIAutomationCondition> tab_strip_control_button;
  ComPtr<IUIAutomationCondition> frame_grab_handle;
};

// TODO: Evaluate `IUIAutomationCacheRequest` and `BuildCache` for tab
// enumeration if tab scans become a measurable hot path.
struct UiaSession {
  ComInitializer com_initializer;
  bool init_attempted = false;
  bool init_succeeded = false;
  ComPtr<IUIAutomation> automation;
  ComPtr<IUIAutomationTreeWalker> control_view_walker;
  ComPtr<IUIAutomationTreeWalker> raw_view_walker;
  CachedClassConditions class_conditions;
};

UiaSession& GetThreadLocalUiaSession() {
  thread_local UiaSession session;
  return session;
}

bool CreateClassCondition(const ComPtr<IUIAutomation>& automation,
                          std::wstring_view class_name,
                          ComPtr<IUIAutomationCondition>* condition) {
  if (!automation || class_name.empty() || !condition) {
    return false;
  }

  ScopedVariant value;
  value.Ref().vt = VT_BSTR;
  value.Ref().bstrVal = SysAllocStringLen(class_name.data(),
                                          static_cast<UINT>(class_name.size()));
  if (!value.Ref().bstrVal) {
    return false;
  }

  return SUCCEEDED(
      automation->CreatePropertyCondition(UIA_ClassNamePropertyId, value.Ref(),
                                          condition->ReleaseAndGetAddressOf()));
}

bool InitializeClassConditions(UiaSession* session) {
  if (!session || !session->automation) {
    return false;
  }

  auto& conditions = session->class_conditions;
  return CreateClassCondition(session->automation,
                              L"HorizontalTabStripRegionView",
                              &conditions.horizontal_tab_strip_region_view) &&
         CreateClassCondition(session->automation,
                              L"TabStrip::TabDragContextImpl",
                              &conditions.tab_strip_drag_context) &&
         CreateClassCondition(session->automation, L"TabContainerImpl",
                              &conditions.tab_container_impl) &&
         CreateClassCondition(
             session->automation, L"VerticalUnpinnedTabContainerView",
             &conditions.vertical_unpinned_tab_container_view) &&
         CreateClassCondition(session->automation, L"ScrollView",
                              &conditions.scroll_view) &&
         CreateClassCondition(session->automation, L"Tab", &conditions.tab) &&
         CreateClassCondition(session->automation, L"VerticalTabView",
                              &conditions.vertical_tab_view) &&
         CreateClassCondition(session->automation, L"TabCloseButton",
                              &conditions.tab_close_button) &&
         CreateClassCondition(session->automation, L"BookmarkButton",
                              &conditions.bookmark_button) &&
         CreateClassCondition(session->automation, L"MenuItemView",
                              &conditions.menu_item_view) &&
         CreateClassCondition(session->automation, L"OmniboxViewViews",
                              &conditions.omnibox_view_views) &&
         CreateClassCondition(session->automation, L"OmniboxResultView",
                              &conditions.omnibox_result_view) &&
         CreateClassCondition(session->automation, L"TabStripControlButton",
                              &conditions.tab_strip_control_button) &&
         CreateClassCondition(session->automation, L"FrameGrabHandle",
                              &conditions.frame_grab_handle);
}

UiaSession* GetUiaSession() {
  auto& session = GetThreadLocalUiaSession();
  if (session.init_attempted) {
    return session.init_succeeded ? &session : nullptr;
  }
  session.init_attempted = true;

  if (!session.com_initializer.IsInitialized()) {
    DebugLog(L"UIA: COM initialization failed");
    return nullptr;
  }

  if (FAILED(CoCreateInstance(
          CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
          IID_PPV_ARGS(session.automation.ReleaseAndGetAddressOf())))) {
    DebugLog(L"UIA: CoCreateInstance(CUIAutomation) failed");
    return nullptr;
  }

  if (FAILED(session.automation->get_ControlViewWalker(
          &session.control_view_walker)) ||
      !session.control_view_walker) {
    DebugLog(L"UIA: get_ControlViewWalker failed");
    return nullptr;
  }

  if (FAILED(session.automation->get_RawViewWalker(&session.raw_view_walker)) ||
      !session.raw_view_walker) {
    DebugLog(L"UIA: get_RawViewWalker failed");
    return nullptr;
  }

  if (!InitializeClassConditions(&session)) {
    DebugLog(L"UIA: failed to cache class conditions");
    return nullptr;
  }

  session.init_succeeded = true;
  return &session;
}

class ScopedBstr {
 public:
  ScopedBstr() = default;
  ~ScopedBstr() { SysFreeString(bstr_); }
  ScopedBstr(const ScopedBstr&) = delete;
  ScopedBstr& operator=(const ScopedBstr&) = delete;

  BSTR* Receive() { return &bstr_; }
  BSTR Get() const { return bstr_; }
  UINT Length() const { return SysStringLen(bstr_); }
  explicit operator bool() const { return bstr_ != nullptr; }

 private:
  BSTR bstr_ = nullptr;
};

bool BstrEqualsStringView(BSTR bstr, std::wstring_view expected) {
  return bstr != nullptr &&
         std::wstring_view(bstr, SysStringLen(bstr)) == expected;
}

ComPtr<IUIAutomationElement> GetFocusedElement(const UiaSession& session) {
  ComPtr<IUIAutomationElement> focused;
  if (FAILED(session.automation->GetFocusedElement(
          focused.ReleaseAndGetAddressOf())) ||
      !focused) {
    return nullptr;
  }
  return focused;
}

ComPtr<IUIAutomationElement> GetElementFromWindow(const UiaSession& session,
                                                  HWND hwnd) {
  if (!hwnd) {
    return nullptr;
  }

  ComPtr<IUIAutomationElement> element;
  if (FAILED(session.automation->ElementFromHandle(
          hwnd, element.ReleaseAndGetAddressOf())) ||
      !element) {
    return nullptr;
  }
  return element;
}

ComPtr<IUIAutomationElement> GetElementAtPoint(const UiaSession& session,
                                               POINT pt) {
  ComPtr<IUIAutomationElement> element;
  if (FAILED(session.automation->ElementFromPoint(
          pt, element.ReleaseAndGetAddressOf())) ||
      !element) {
    return nullptr;
  }
  return element;
}

std::optional<std::wstring> GetStringProperty(
    const ComPtr<IUIAutomationElement>& element,
    PROPERTYID property_id) {
  if (!element) {
    return std::nullopt;
  }

  ScopedVariant property;
  if (FAILED(element->GetCurrentPropertyValue(property_id, property.Ptr())) ||
      property.Ref().vt != VT_BSTR || !property.Ref().bstrVal) {
    return std::nullopt;
  }
  return std::wstring(property.Ref().bstrVal);
}

bool HasClassName(const ComPtr<IUIAutomationElement>& element,
                  std::wstring_view expected_class_name) {
  if (!element) {
    return false;
  }

  ScopedBstr class_name;
  if (FAILED(element->get_CurrentClassName(class_name.Receive())) ||
      !class_name) {
    return false;
  }

  return BstrEqualsStringView(class_name.Get(), expected_class_name);
}

bool HasAnyClassName(
    const ComPtr<IUIAutomationElement>& element,
    std::initializer_list<std::wstring_view> expected_class_names) {
  if (!element) {
    return false;
  }

  ScopedBstr class_name;
  if (FAILED(element->get_CurrentClassName(class_name.Receive())) ||
      !class_name) {
    return false;
  }

  const std::wstring_view class_name_view(class_name.Get(),
                                          class_name.Length());
  return std::ranges::contains(expected_class_names, class_name_view);
}

template <typename Visitor>
bool TraverseDescendantsRaw(const UiaSession& session,
                            const ComPtr<IUIAutomationElement>& root,
                            Visitor visitor) {
  if (!root || !session.raw_view_walker) {
    return false;
  }

  std::vector<ComPtr<IUIAutomationElement>> stack;
  auto push_children = [&](const ComPtr<IUIAutomationElement>& parent) {
    ComPtr<IUIAutomationElement> child;
    if (FAILED(session.raw_view_walker->GetFirstChildElement(
            parent.Get(), child.ReleaseAndGetAddressOf()))) {
      return;
    }
    while (child) {
      stack.emplace_back(child);
      ComPtr<IUIAutomationElement> next;
      if (FAILED(session.raw_view_walker->GetNextSiblingElement(
              child.Get(), next.ReleaseAndGetAddressOf()))) {
        break;
      }
      child = std::move(next);
    }
  };

  // Seed the stack with root's direct children, not root itself, so the
  // visitor is only called on actual descendants.
  push_children(root);

  while (!stack.empty()) {
    auto node = std::move(stack.back());
    stack.pop_back();
    if (visitor(node)) {
      return true;
    }

    push_children(node);
  }

  return false;
}

ComPtr<IUIAutomationElement> FindFirstDescendantByClass(
    const ComPtr<IUIAutomationElement>& root,
    const ComPtr<IUIAutomationCondition>& class_condition) {
  if (!root || !class_condition) {
    return nullptr;
  }

  ComPtr<IUIAutomationElement> hit;
  if (FAILED(root->FindFirst(TreeScope_Subtree, class_condition.Get(),
                             hit.ReleaseAndGetAddressOf()))) {
    return nullptr;
  }
  return hit;
}

ComPtr<IUIAutomationElement> FindFirstDescendantByClassRaw(
    const UiaSession& session,
    const ComPtr<IUIAutomationElement>& root,
    std::wstring_view class_name) {
  ComPtr<IUIAutomationElement> hit;
  TraverseDescendantsRaw(session, root, [&](const auto& node) {
    if (HasClassName(node, class_name)) {
      hit = node;
      return true;
    }
    return false;
  });
  return hit;
}

std::optional<int> CountDescendantsByClassRaw(
    const UiaSession& session,
    const ComPtr<IUIAutomationElement>& root,
    std::wstring_view class_name) {
  if (!root || !session.raw_view_walker) {
    return std::nullopt;
  }

  int count = 0;
  TraverseDescendantsRaw(session, root, [&](const auto& node) {
    if (HasClassName(node, class_name)) {
      ++count;
    }
    return false;
  });
  return count;
}

ComPtr<IUIAutomationElement> FindAncestorByClassImpl(
    const UiaSession& session,
    const ComPtr<IUIAutomationElement>& element,
    std::wstring_view class_name,
    bool include_self) {
  if (!element || !session.control_view_walker) {
    return nullptr;
  }

  ComPtr<IUIAutomationElement> current = element;
  if (include_self && HasClassName(current, class_name)) {
    return current;
  }

  while (true) {
    ComPtr<IUIAutomationElement> parent;
    if (FAILED(session.control_view_walker->GetParentElement(
            current.Get(), parent.ReleaseAndGetAddressOf())) ||
        !parent) {
      return nullptr;
    }
    if (HasClassName(parent, class_name)) {
      return parent;
    }
    current = std::move(parent);
  }
}

ComPtr<IUIAutomationElement> FindAncestorByClass(
    const UiaSession& session,
    const ComPtr<IUIAutomationElement>& element,
    std::wstring_view class_name) {
  return FindAncestorByClassImpl(session, element, class_name, false);
}

ComPtr<IUIAutomationElement> FindAncestorOrSelfByClass(
    const UiaSession& session,
    const ComPtr<IUIAutomationElement>& element,
    std::wstring_view class_name) {
  return FindAncestorByClassImpl(session, element, class_name, true);
}

ComPtr<IUIAutomationElement> FindSiblingByClass(
    const UiaSession& session,
    const ComPtr<IUIAutomationElement>& element,
    std::wstring_view class_name) {
  if (!element || !session.control_view_walker) {
    return nullptr;
  }

  for (const bool forward : {true, false}) {
    ComPtr<IUIAutomationElement> current;
    HRESULT hr = forward
                     ? session.control_view_walker->GetNextSiblingElement(
                           element.Get(), current.ReleaseAndGetAddressOf())
                     : session.control_view_walker->GetPreviousSiblingElement(
                           element.Get(), current.ReleaseAndGetAddressOf());
    while (SUCCEEDED(hr) && current) {
      if (HasClassName(current, class_name)) {
        return current;
      }

      ComPtr<IUIAutomationElement> next;
      hr = forward ? session.control_view_walker->GetNextSiblingElement(
                         current.Get(), next.ReleaseAndGetAddressOf())
                   : session.control_view_walker->GetPreviousSiblingElement(
                         current.Get(), next.ReleaseAndGetAddressOf());
      current = std::move(next);
    }
  }

  return nullptr;
}

enum class TabContainerKind {
  kHorizontal,
  kVertical,
};

struct TabContainer {
  ComPtr<IUIAutomationElement> element;
  TabContainerKind kind = TabContainerKind::kHorizontal;
};

TreeScope GetTabElementScope(TabContainerKind kind) {
  // Horizontal tabs are direct children of the trusted container. Vertical
  // tabs are usually nested below a `ScrollView` wrapper, so subtree search is
  // required once the vertical container is known to be credible.
  return kind == TabContainerKind::kHorizontal ? TreeScope_Children
                                               : TreeScope_Subtree;
}

std::wstring_view GetTabElementClassName(TabContainerKind kind) {
  return kind == TabContainerKind::kHorizontal ? L"Tab" : L"VerticalTabView";
}

const ComPtr<IUIAutomationCondition>& GetTabElementCondition(
    const UiaSession& session,
    TabContainerKind kind) {
  return kind == TabContainerKind::kHorizontal
             ? session.class_conditions.tab
             : session.class_conditions.vertical_tab_view;
}

bool IsWindowFullScreen(HWND hwnd) {
  RECT window_rect;
  if (!GetWindowRect(hwnd, &window_rect)) {
    return false;
  }

  const HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
  if (!monitor) {
    return false;
  }

  MONITORINFO monitor_info = {sizeof(monitor_info)};
  if (!GetMonitorInfoW(monitor, &monitor_info)) {
    return false;
  }

  return window_rect.left == monitor_info.rcMonitor.left &&
         window_rect.top == monitor_info.rcMonitor.top &&
         window_rect.right == monitor_info.rcMonitor.right &&
         window_rect.bottom == monitor_info.rcMonitor.bottom;
}

std::optional<TabContainer> FindHorizontalTabContainerForWindow(
    const UiaSession& session,
    const ComPtr<IUIAutomationElement>& window_element) {
  if (!window_element) {
    return std::nullopt;
  }

  // Trust horizontal tab containers only when they are anchored to visible tab
  // strip UI. Popup windows may still expose internal tab-like nodes in raw
  // UIA, but users do not see a real tab strip there.
  if (const auto tab_strip = FindFirstDescendantByClass(
          window_element, session.class_conditions.tab_strip_drag_context)) {
    if (const auto container =
            FindSiblingByClass(session, tab_strip, L"TabContainerImpl")) {
      return TabContainer{container, TabContainerKind::kHorizontal};
    }
  }

  if (const auto tab_strip_region = FindFirstDescendantByClass(
          window_element,
          session.class_conditions.horizontal_tab_strip_region_view)) {
    if (const auto container = FindFirstDescendantByClass(
            tab_strip_region, session.class_conditions.tab_container_impl)) {
      return TabContainer{container, TabContainerKind::kHorizontal};
    }
  }
  return std::nullopt;
}

std::optional<TabContainer> FindVerticalTabContainerForWindow(
    const UiaSession& session,
    const ComPtr<IUIAutomationElement>& window_element) {
  if (!window_element) {
    return std::nullopt;
  }

  if (const auto container = FindFirstDescendantByClass(
          window_element,
          session.class_conditions.vertical_unpinned_tab_container_view)) {
    return TabContainer{container, TabContainerKind::kVertical};
  }

  return std::nullopt;
}

std::optional<TabContainer> FindFullscreenTabContainerFallback(
    const UiaSession& session,
    const ComPtr<IUIAutomationElement>& window_element,
    HWND hwnd) {
  if (!window_element || !IsWindowFullScreen(hwnd)) {
    return std::nullopt;
  }

  // In fullscreen Chrome hides the visible tab strip from control view, so the
  // raw-view fallback is limited to this mode to avoid misclassifying popups as
  // tabbed browser windows.
  if (const auto container = FindFirstDescendantByClassRaw(
          session, window_element, L"TabContainerImpl")) {
    return TabContainer{container, TabContainerKind::kHorizontal};
  }

  if (const auto container = FindFirstDescendantByClassRaw(
          session, window_element, L"VerticalUnpinnedTabContainerView")) {
    return TabContainer{container, TabContainerKind::kVertical};
  }

  return std::nullopt;
}

std::optional<TabContainer> FindTabContainerForWindow(const UiaSession& session,
                                                      HWND hwnd) {
  const auto window_element = GetElementFromWindow(session, hwnd);
  if (!window_element) {
    return std::nullopt;
  }

  if (const auto horizontal =
          FindHorizontalTabContainerForWindow(session, window_element)) {
    return horizontal;
  }

  if (const auto vertical =
          FindVerticalTabContainerForWindow(session, window_element)) {
    return vertical;
  }
  return FindFullscreenTabContainerFallback(session, window_element, hwnd);
}

std::optional<TabContainer> FindVerticalTabContainerAtPoint(
    const UiaSession& session,
    const ComPtr<IUIAutomationElement>& pointed) {
  if (!pointed) {
    return std::nullopt;
  }

  if (const auto container = FindAncestorOrSelfByClass(
          session, pointed, L"VerticalUnpinnedTabContainerView")) {
    return TabContainer{container, TabContainerKind::kVertical};
  }

  // Vertical tabs can be wrapped in an intermediate ScrollView. Restrict the
  // extra subtree search to that wrapper instead of treating all ScrollView
  // hits as tab UI.
  if (const auto scroll_view =
          FindAncestorOrSelfByClass(session, pointed, L"ScrollView")) {
    if (const auto container = FindFirstDescendantByClass(
            scroll_view,
            session.class_conditions.vertical_unpinned_tab_container_view)) {
      return TabContainer{container, TabContainerKind::kVertical};
    }
  }

  return std::nullopt;
}

std::optional<TabContainer> FindHorizontalTabContainerAtPoint(
    const UiaSession& session,
    const ComPtr<IUIAutomationElement>& pointed) {
  if (!pointed) {
    return std::nullopt;
  }

  if (const auto container =
          FindAncestorOrSelfByClass(session, pointed, L"TabContainerImpl")) {
    return TabContainer{container, TabContainerKind::kHorizontal};
  }

  if (const auto tab_strip = FindAncestorOrSelfByClass(
          session, pointed, L"TabStrip::TabDragContextImpl")) {
    if (const auto container =
            FindSiblingByClass(session, tab_strip, L"TabContainerImpl")) {
      return TabContainer{container, TabContainerKind::kHorizontal};
    }
  }

  return std::nullopt;
}

std::optional<TabContainer> FindTabContainerAtPoint(const UiaSession& session,
                                                    POINT pt) {
  const auto pointed = GetElementAtPoint(session, pt);
  if (!pointed) {
    return std::nullopt;
  }

  if (const auto vertical = FindVerticalTabContainerAtPoint(session, pointed)) {
    return vertical;
  }

  return FindHorizontalTabContainerAtPoint(session, pointed);
}

ComPtr<IUIAutomationElementArray> FindTabElements(
    const UiaSession& session,
    const TabContainer& tab_container) {
  if (!tab_container.element) {
    return nullptr;
  }

  ComPtr<IUIAutomationElementArray> tab_elements;
  if (FAILED(tab_container.element->FindAll(
          GetTabElementScope(tab_container.kind),
          GetTabElementCondition(session, tab_container.kind).Get(),
          tab_elements.ReleaseAndGetAddressOf())) ||
      !tab_elements) {
    return nullptr;
  }

  return tab_elements;
}

ComPtr<IUIAutomationElement> FindTabElementAtPoint(
    const ComPtr<IUIAutomationElementArray>& tab_elements,
    POINT pt) {
  if (!tab_elements) {
    return nullptr;
  }

  int count = 0;
  if (FAILED(tab_elements->get_Length(&count))) {
    return nullptr;
  }

  for (int i = 0; i < count; ++i) {
    ComPtr<IUIAutomationElement> tab_element;
    if (FAILED(tab_elements->GetElement(
            i, tab_element.ReleaseAndGetAddressOf())) ||
        !tab_element) {
      continue;
    }

    RECT rect;
    if (FAILED(tab_element->get_CurrentBoundingRectangle(&rect))) {
      continue;
    }
    if (PtInRect(&rect, pt)) {
      return tab_element;
    }
  }

  return nullptr;
}

bool IsOnTabCloseButton(const UiaSession& session,
                        const ComPtr<IUIAutomationElement>& tab_element,
                        POINT pt) {
  if (!tab_element) {
    return false;
  }

  const auto close_button = FindFirstDescendantByClass(
      tab_element, session.class_conditions.tab_close_button);
  if (!close_button) {
    return false;
  }

  RECT close_button_rect;
  if (FAILED(close_button->get_CurrentBoundingRectangle(&close_button_rect))) {
    return false;
  }
  return PtInRect(&close_button_rect, pt) != FALSE;
}

ComPtr<IUIAutomationElement> FindSelectedTabElement(
    const UiaSession& session,
    const TabContainer& tab_container) {
  const auto tab_elements = FindTabElements(session, tab_container);
  if (!tab_elements) {
    return nullptr;
  }

  int length = 0;
  if (FAILED(tab_elements->get_Length(&length))) {
    return nullptr;
  }

  for (int i = 0; i < length; ++i) {
    ComPtr<IUIAutomationElement> tab;
    if (FAILED(tab_elements->GetElement(i, tab.ReleaseAndGetAddressOf())) ||
        !tab) {
      continue;
    }

    ScopedVariant is_selected;
    if (FAILED(tab->GetCurrentPropertyValue(
            UIA_SelectionItemIsSelectedPropertyId, is_selected.Ptr()))) {
      continue;
    }

    if (is_selected.Ref().vt == VT_BOOL &&
        is_selected.Ref().boolVal == VARIANT_TRUE) {
      return tab;
    }
  }
  return nullptr;
}

std::optional<std::wstring> GetNewTabButtonName(
    const UiaSession& session,
    const ComPtr<IUIAutomationElement>& window_element) {
  static std::optional<std::wstring> cached_name;
  if (cached_name.has_value()) {
    return cached_name;
  }

  const auto button = FindFirstDescendantByClass(
      window_element, session.class_conditions.tab_strip_control_button);
  if (!button) {
    return std::nullopt;
  }

  auto name = GetStringProperty(button, UIA_NamePropertyId);
  if (name && !name->empty()) {
    cached_name = name;
  }
  return cached_name;
}

}  // namespace

std::optional<TabHitResult> FindTabHitResult(POINT pt,
                                             bool need_count,
                                             bool need_close_button) {
  const UiaSession* session = GetUiaSession();
  if (!session) {
    return std::nullopt;
  }

  const auto tab_container = FindTabContainerAtPoint(*session, pt);
  if (!tab_container) {
    return std::nullopt;
  }

  const auto tab_elements = FindTabElements(*session, *tab_container);
  if (!tab_elements) {
    return std::nullopt;
  }

  const auto tab_element = FindTabElementAtPoint(tab_elements, pt);
  if (!tab_element) {
    return std::nullopt;
  }

  int tab_count = 0;
  if (need_count) {
    const auto raw_count =
        CountDescendantsByClassRaw(*session, tab_container->element,
                                   GetTabElementClassName(tab_container->kind));
    if (!raw_count) {
      return std::nullopt;
    }
    tab_count = *raw_count;
  }

  TabHitResult hit_result;
  hit_result.tab = tab_element;
  hit_result.tab_count = need_count ? tab_count : 0;
  hit_result.on_close_button =
      need_close_button && IsOnTabCloseButton(*session, tab_element, pt);
  return hit_result;
}

std::optional<int> FindTabCount(HWND hwnd) {
  const UiaSession* session = GetUiaSession();
  if (!session) {
    return std::nullopt;
  }

  const auto tab_container = FindTabContainerForWindow(*session, hwnd);
  if (!tab_container) {
    return std::nullopt;
  }

  // Raw view is required: tabs inside a collapsed tab group are hidden from
  // control view but still present in the raw tree, and they must be counted so
  // `keep_tab` does not mistake the last visible tab for the last tab overall.
  // Scoping the raw traversal to a credible tab container keeps it cheap.
  return CountDescendantsByClassRaw(
      *session, tab_container->element,
      GetTabElementClassName(tab_container->kind));
}

bool IsOnTabBar(POINT pt) {
  const UiaSession* session = GetUiaSession();
  if (!session) {
    return false;
  }

  const auto pointed = GetElementAtPoint(*session, pt);
  if (!pointed) {
    return false;
  }

  if (HasAnyClassName(
          pointed, {L"HorizontalTabStripRegionView",
                    L"TabStrip::TabDragContextImpl", L"TabStripControlButton",
                    L"FrameGrabHandle", L"VerticalTabStripBottomContainer"})) {
    return true;
  }

  if (FindAncestorByClass(*session, pointed, L"HorizontalTabStripRegionView") !=
      nullptr) {
    return true;
  }

  return FindVerticalTabContainerAtPoint(*session, pointed).has_value();
}

bool IsOnBookmark(POINT pt) {
  const UiaSession* session = GetUiaSession();
  if (!session) {
    return false;
  }

  const auto pointed = GetElementAtPoint(*session, pt);
  if (!pointed) {
    return false;
  }

  if (!HasAnyClassName(pointed, {L"BookmarkButton", L"MenuItemView"})) {
    return false;
  }

  const auto full_description =
      GetStringProperty(pointed, UIA_FullDescriptionPropertyId);
  if (!full_description) {
    return false;
  }

  const std::wstring_view full_description_view(*full_description);
  const bool is_blocked_scheme =
      full_description_view.starts_with(L"javascript:");
  const bool looks_like_url = full_description_view.contains(L':') ||
                              full_description_view.contains(L'.');
  return !is_blocked_scheme && looks_like_url;
}

bool IsOmniboxFocused() {
  const UiaSession* session = GetUiaSession();
  if (!session) {
    return false;
  }

  const auto focused = GetFocusedElement(*session);
  if (!focused) {
    return false;
  }

  return HasAnyClassName(focused, {L"OmniboxViewViews", L"OmniboxResultView"});
}

bool IsOnNewTab(HWND hwnd, const std::vector<std::wstring>& extra_tab_names) {
  const UiaSession* session = GetUiaSession();
  if (!session) {
    return false;
  }

  const auto tab_container = FindTabContainerForWindow(*session, hwnd);
  if (!tab_container) {
    return false;
  }

  const auto selected_tab = FindSelectedTabElement(*session, *tab_container);
  if (!selected_tab) {
    return false;
  }

  const auto selected_name =
      GetStringProperty(selected_tab, UIA_NamePropertyId);
  if (!selected_name) {
    return false;
  }

  const auto window_element = GetElementFromWindow(*session, hwnd);
  const auto std_name = GetNewTabButtonName(*session, window_element);
  if (std_name && selected_name->contains(*std_name)) {
    return true;
  }

  for (size_t i = 0; i < extra_tab_names.size(); ++i) {
    if (!extra_tab_names[i].empty() &&
        selected_name->contains(extra_tab_names[i])) {
      return true;
    }
  }

  return false;
}
