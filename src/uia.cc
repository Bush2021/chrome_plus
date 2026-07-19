#include "uia.h"

#include <uiautomation.h>
#include <wrl/client.h>

#include <algorithm>
#include <array>
#include <initializer_list>
#include <optional>
#include <string_view>
#include <utility>
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
  ComPtr<IUIAutomationCondition> tab_strip_drag_context;
  ComPtr<IUIAutomationCondition> tab_container_impl;
  ComPtr<IUIAutomationCondition> vertical_unpinned_tab_container_view;
  ComPtr<IUIAutomationCondition> tab;
  ComPtr<IUIAutomationCondition> vertical_tab_view;
  ComPtr<IUIAutomationCondition> tab_close_button;
  ComPtr<IUIAutomationCondition> bookmark_button;
  ComPtr<IUIAutomationCondition> menu_item_view;
  ComPtr<IUIAutomationCondition> tab_strip_control_button;
};

enum class TabContainerKind {
  kHorizontal,
  kVertical,
};

struct TabContainer {
  ComPtr<IUIAutomationElement> element;
  TabContainerKind kind = TabContainerKind::kHorizontal;
};

// Tab UI resolved for one top-level window, kept until validation fails or
// the window/fullscreen state changes. UIA elements are live references, so
// property reads on cached elements (bounding rectangles in particular) track
// resizes without re-resolving; staleness is detected by rectangle checks in
// `GetValidatedTabUi`, not by failing reads.
struct TabUiCache {
  HWND window = nullptr;
  bool fullscreen = false;
  // Windows without tab UI (popups, app windows) would re-run the resolve on
  // every click; remember the failure and retry at most once a second.
  ULONGLONG retry_after_ticks = 0;
  // Null when the container came from the fullscreen raw-view fallback (the
  // control view hides the tab strip region there).
  ComPtr<IUIAutomationElement> region;
  TabContainer container;
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
  TabUiCache tab_ui_cache;
};

UiaSession& GetThreadLocalUiaSession() {
  // UI Automation proxies can block while being released from Chrome's UI
  // thread during process teardown. Keep the apartment-bound session isolated
  // per thread, but intentionally give it process lifetime so no COM cleanup
  // runs from the injected DLL's TLS teardown path. Windows reclaims the
  // allocation and COM references when the process exits.
  thread_local UiaSession* session = new UiaSession;
  return *session;
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
                              L"TabStrip::TabDragContextImpl",
                              &conditions.tab_strip_drag_context) &&
         CreateClassCondition(session->automation, L"TabContainerImpl",
                              &conditions.tab_container_impl) &&
         CreateClassCondition(
             session->automation, L"VerticalUnpinnedTabContainerView",
             &conditions.vertical_unpinned_tab_container_view) &&
         CreateClassCondition(session->automation, L"Tab", &conditions.tab) &&
         CreateClassCondition(session->automation, L"VerticalTabView",
                              &conditions.vertical_tab_view) &&
         CreateClassCondition(session->automation, L"TabCloseButton",
                              &conditions.tab_close_button) &&
         CreateClassCondition(session->automation, L"BookmarkButton",
                              &conditions.bookmark_button) &&
         CreateClassCondition(session->automation, L"MenuItemView",
                              &conditions.menu_item_view) &&
         CreateClassCondition(session->automation, L"TabStripControlButton",
                              &conditions.tab_strip_control_button);
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

bool IsValidBookmark(const ComPtr<IUIAutomationElement>& element) {
  if (!HasAnyClassName(element, {L"BookmarkButton", L"MenuItemView"})) {
    return false;
  }
  const auto full_description =
      GetStringProperty(element, UIA_FullDescriptionPropertyId);
  if (!full_description) {
    return false;
  }
  const std::wstring_view view(*full_description);
  return !view.starts_with(L"javascript:") &&
         (view.contains(L':') || view.contains(L'.'));
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

// True when the element roots a hosted-HWND subtree (its native window
// handle is set). Views elements are virtual children of the top-level
// window and report no handle of their own.
bool HasNativeWindowHandle(const ComPtr<IUIAutomationElement>& element) {
  if (!element) {
    return false;
  }

  UIA_HWND native_window = nullptr;
  if (FAILED(element->get_CurrentNativeWindowHandle(&native_window))) {
    return false;
  }
  return native_window != nullptr;
}

// Breadth-first search for the first descendant whose class name is in
// `target_class_names`, restricted to the browser chrome.

// `FindFirst(TreeScope_Subtree)` from the window root is pre-order, and the
// web-content branch precedes the tab strip there (`BrowserView` children in
// order: `TopContainerView`, the content `View ▸ MultiContentsView ▸
// Document`, and only then `HorizontalTabStripRegionView`. Every such search
// walked the entire renderer accessibility tree, and the first touch of web
// content makes Chromium turn on `AXMode::kWebContents` for the rest of the
// session, after which the renderer maintains a full accessibility tree for
// every page (ui/accessibility/platform/ax_platform.h,
// `OnPropertiesUsedInWebContent`). That combination was the DOM-heavy-page
// click/resize/scroll lag of issue #270.

// BFS reaches the tab strip region at its shallow depth after a couple dozen
// class reads without entering sibling subtrees, and web content stays
// structurally unreachable: the walk never descends into an element hosting
// its own HWND (`Intermediate D3D Window`, the WebContents host
// `Chrome_WidgetWin_1`, `Chrome_RenderWidgetHostHWND`) nor into the known
// content-branch views, and `max_visited`/depth budgets bound the walk even
// if a future Chrome reshuffles the tree.
ComPtr<IUIAutomationElement> FindShallowDescendantByClasses(
    IUIAutomationTreeWalker* walker,
    const ComPtr<IUIAutomationElement>& anchor,
    std::initializer_list<std::wstring_view> target_class_names,
    int max_visited) {
  if (!walker || !anchor) {
    return nullptr;
  }

  constexpr int kMaxDepth = 12;

  struct QueuedElement {
    ComPtr<IUIAutomationElement> element;
    int depth;
  };
  std::vector<QueuedElement> queue;
  size_t next_index = 0;

  auto enqueue_children = [&](const ComPtr<IUIAutomationElement>& parent,
                              int depth) {
    ComPtr<IUIAutomationElement> child;
    if (FAILED(walker->GetFirstChildElement(parent.Get(),
                                            child.ReleaseAndGetAddressOf()))) {
      return;
    }
    while (child) {
      queue.push_back({child, depth});
      ComPtr<IUIAutomationElement> sibling;
      if (FAILED(walker->GetNextSiblingElement(
              child.Get(), sibling.ReleaseAndGetAddressOf()))) {
        break;
      }
      child = std::move(sibling);
    }
  };

  enqueue_children(anchor, 1);

  int visited = 0;
  while (next_index < queue.size()) {
    // Moved (not referenced) out of the queue: enqueue_children() below can
    // reallocate `queue`, which would invalidate a reference into it.
    const QueuedElement current = std::move(queue[next_index]);
    ++next_index;
    if (++visited > max_visited) {
      DebugLog(L"UIA: chrome-only BFS exhausted its element budget");
      return nullptr;
    }

    ScopedBstr class_name;
    if (FAILED(current.element->get_CurrentClassName(class_name.Receive()))) {
      continue;
    }
    const std::wstring_view class_name_view =
        class_name ? std::wstring_view(class_name.Get(), class_name.Length())
                   : std::wstring_view();
    if (std::ranges::contains(target_class_names, class_name_view)) {
      return current.element;
    }

    if (current.depth >= kMaxDepth) {
      continue;
    }
    if (std::ranges::contains(
            std::initializer_list<std::wstring_view>{
                L"MultiContentsView", L"WebView", L"ContentsWebView"},
            class_name_view) ||
        HasNativeWindowHandle(current.element)) {
      continue;
    }
    enqueue_children(current.element, current.depth + 1);
  }

  return nullptr;
}

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

// Resolves the tab container inside an already validated tab strip region.
// The region subtree is content-free, so scoped `FindFirst` is safe here. Trust
// is anchored on the region itself: popup windows expose no
// `HorizontalTabStripRegionView`/`VerticalTabStripRegionView`, so tab-like
// nodes elsewhere can never be misclassified as a tab strip.
std::optional<TabContainer> FindTabContainerInRegion(
    const UiaSession& session,
    const ComPtr<IUIAutomationElement>& region,
    bool vertical) {
  if (vertical) {
    if (const auto container = FindFirstDescendantByClass(
            region,
            session.class_conditions.vertical_unpinned_tab_container_view)) {
      return TabContainer{container, TabContainerKind::kVertical};
    }
    return std::nullopt;
  }

  if (const auto tab_strip = FindFirstDescendantByClass(
          region, session.class_conditions.tab_strip_drag_context)) {
    if (const auto container =
            FindSiblingByClass(session, tab_strip, L"TabContainerImpl")) {
      return TabContainer{container, TabContainerKind::kHorizontal};
    }
  }

  if (const auto container = FindFirstDescendantByClass(
          region, session.class_conditions.tab_container_impl)) {
    return TabContainer{container, TabContainerKind::kHorizontal};
  }
  return std::nullopt;
}

TabUiCache* ResolveTabUi(UiaSession* session, HWND hwnd) {
  TabUiCache& cache = session->tab_ui_cache;
  const bool fullscreen = IsWindowFullScreen(hwnd);
  if (cache.window == hwnd && cache.fullscreen == fullscreen) {
    if (cache.container.element) {
      return &cache;
    }
    if (GetTickCount64() < cache.retry_after_ticks) {
      return nullptr;
    }
  }

  cache = TabUiCache();
  cache.window = hwnd;
  cache.fullscreen = fullscreen;
  cache.retry_after_ticks = GetTickCount64() + 1000;

  const auto window_element = GetElementFromWindow(*session, hwnd);
  if (!window_element) {
    return nullptr;
  }

  if (const auto region = FindShallowDescendantByClasses(
          session->control_view_walker.Get(), window_element,
          {L"HorizontalTabStripRegionView", L"VerticalTabStripRegionView"},
          /*max_visited=*/256)) {
    const bool vertical = HasClassName(region, L"VerticalTabStripRegionView");
    if (auto container = FindTabContainerInRegion(*session, region, vertical)) {
      cache.region = region;
      cache.container = std::move(*container);
      return &cache;
    }
    return nullptr;
  }

  if (fullscreen) {
    // In fullscreen Chrome hides the tab strip from the control view, so fall
    // back to the raw view -- still the chrome-only walk, with a larger
    // budget because the raw view exposes more nodes per level. Limiting the
    // fallback to fullscreen keeps popups from being misclassified as tabbed
    // browser windows.
    if (const auto container = FindShallowDescendantByClasses(
            session->raw_view_walker.Get(), window_element,
            {L"TabContainerImpl", L"VerticalUnpinnedTabContainerView"},
            /*max_visited=*/512)) {
      const bool vertical =
          HasClassName(container, L"VerticalUnpinnedTabContainerView");
      cache.container =
          TabContainer{container, vertical ? TabContainerKind::kVertical
                                           : TabContainerKind::kHorizontal};
      return &cache;
    }
  }

  return nullptr;
}

// Returns the cached tab UI for `hwnd`, retrying once with a fresh resolve
// when validation fails. `gate_rect` receives the live rectangle of the tab
// strip region (or of the container in the fullscreen fallback) for point
// gating.
//
// A failing read is not a usable staleness signal here: toggling between
// horizontal and vertical tabs keeps both region views alive as `BrowserView`
// children and only destroys/recreates the inner tab strip
// (`BrowserView::OnVerticalTabStripModeChanged` in
// chrome/browser/ui/views/frame/browser_view.cc), and Chromium's UIA provider
// answers rectangle reads on the hidden region and on the destroyed container
// with S_OK and an empty rectangle rather than an error. Outside fullscreen a
// live tab strip always has a non-empty rectangle, so an empty region or
// container rectangle marks a cache entry orphaned by a layout toggle.
TabUiCache* GetValidatedTabUi(UiaSession* session, HWND hwnd, RECT* gate_rect) {
  for (int attempt = 0; attempt < 2; ++attempt) {
    TabUiCache* ui = ResolveTabUi(session, hwnd);
    if (!ui) {
      return nullptr;
    }

    const ComPtr<IUIAutomationElement>& gate =
        ui->region ? ui->region : ui->container.element;
    if (SUCCEEDED(gate->get_CurrentBoundingRectangle(gate_rect))) {
      if (!ui->region) {
        // Fullscreen raw-view fallback: the auto-hidden strip may
        // legitimately report an empty rectangle, so a successful read stays
        // the only gate.
        return ui;
      }
      RECT container_rect;
      if (!IsRectEmpty(gate_rect) &&
          SUCCEEDED(ui->container.element->get_CurrentBoundingRectangle(
              &container_rect)) &&
          !IsRectEmpty(&container_rect)) {
        return ui;
      }
    }
    session->tab_ui_cache = TabUiCache();
  }
  return nullptr;
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

std::optional<TabHitResult> BuildTabHitResult(const UiaSession& session,
                                              const TabContainer& tab_container,
                                              POINT pt,
                                              bool need_count,
                                              bool need_close_button) {
  const auto tab_elements = FindTabElements(session, tab_container);
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
        CountDescendantsByClassRaw(session, tab_container.element,
                                   GetTabElementClassName(tab_container.kind));
    if (!raw_count) {
      return std::nullopt;
    }
    tab_count = *raw_count;
  }

  TabHitResult hit_result;
  hit_result.tab = tab_element;
  hit_result.tab_count = need_count ? tab_count : 0;
  hit_result.on_close_button =
      need_close_button && IsOnTabCloseButton(session, tab_element, pt);
  return hit_result;
}

// `region` is the tab strip region: the New Tab `TabStripControlButton` sits
// beside the tab container inside it, and anchoring there keeps the search
// out of the content branch (a window-root search would cross it, issue
// #270). A null region (fullscreen raw fallback) skips straight to the
// user-configured names in the caller.
std::optional<std::wstring> GetNewTabButtonName(
    const UiaSession& session,
    const ComPtr<IUIAutomationElement>& region) {
  static std::optional<std::wstring> cached_name;
  if (cached_name.has_value()) {
    return cached_name;
  }

  const auto button = FindFirstDescendantByClass(
      region, session.class_conditions.tab_strip_control_button);
  if (!button) {
    return std::nullopt;
  }

  auto name = GetStringProperty(button, UIA_NamePropertyId);
  if (name && !name->empty()) {
    cached_name = name;
  }
  return cached_name;
}

ComPtr<IUIAutomationElement> FindBookmarkInAnchor(
    const ComPtr<IUIAutomationElement>& anchor,
    const ComPtr<IUIAutomationCondition>& item_condition,
    POINT pt) {
  ComPtr<IUIAutomationElementArray> elements;
  if (FAILED(anchor->FindAll(TreeScope_Subtree, item_condition.Get(),
                             elements.ReleaseAndGetAddressOf())) ||
      !elements) {
    return nullptr;
  }
  int length = 0;
  if (FAILED(elements->get_Length(&length))) {
    return nullptr;
  }
  for (int i = 0; i < length; ++i) {
    ComPtr<IUIAutomationElement> element;
    if (FAILED(elements->GetElement(i, element.ReleaseAndGetAddressOf())) ||
        !element) {
      continue;
    }
    RECT rect;
    if (FAILED(element->get_CurrentBoundingRectangle(&rect))) {
      continue;
    }
    if (PtInRect(&rect, pt) && IsValidBookmark(element)) {
      return element;
    }
  }
  return nullptr;
}

// True when `window` hosts web content. WebContents on Windows always carries
// a `Chrome_RenderWidgetHostHWND` child HWND, kept by Chromium for
// screen-reader and legacy-trackpad-driver compat
// (content/browser/renderer_host/legacy_render_widget_host_win.h);
// views-only windows such as bookmark folder menus have no child HWNDs at
// all. Should Chromium ever drop the legacy HWND, this check fails open and
// merely restores the pre-gate behavior.
bool WindowHostsWebContent(HWND window) {
  bool found = false;
  EnumChildWindows(
      window,
      [](HWND child, LPARAM param) -> BOOL {
        std::array<wchar_t, 64> class_name{};
        if (GetClassNameW(child, class_name.data(),
                          static_cast<int>(class_name.size())) &&
            std::wstring_view(class_name.data()) ==
                L"Chrome_RenderWidgetHostHWND") {
          *reinterpret_cast<bool*>(param) = true;
          return FALSE;
        }
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&found));
  return found;
}

// Resolve a bookmark under `pt` without `ElementFromPoint`, mirroring the tab
// hit-testing approach (see the comment block above `FindTabHitResult`).
// Every scan stays out of web content: the anchored subtrees have no content
// branch, and the discovery walk is the chrome-only BFS.
ComPtr<IUIAutomationElement>
FindBookmarkCoveringPoint(const UiaSession& session, HWND window, POINT pt) {
  const auto window_element = GetElementFromWindow(session, window);
  if (!window_element) {
    return nullptr;
  }

  // Main browser window: anchor the scan to `TopContainerView`, the content-
  // free sibling of the page branch. `BookmarkButton` only ever lives in the
  // bookmark bar beneath it, and the omnibox results popup is mirrored under a
  // different `BrowserRootView` branch (outside `TopContainerView`), so a
  // covered `BookmarkButton` is unambiguous -- no narrowing to
  // `BookmarkBarView` and no former #238 z-order workaround needed. The BFS
  // (not a root-scoped `FindFirst`) matters on windows that lack
  // `TopContainerView`, e.g. undocked DevTools: pre-order `FindFirst` walks
  // the entire renderer tree before failing and switches web-contents
  // accessibility on, the #270 failure mode.
  if (const auto top_container = FindShallowDescendantByClasses(
          session.control_view_walker.Get(), window_element,
          {L"TopContainerView"}, /*max_visited=*/256)) {
    return FindBookmarkInAnchor(top_container,
                                session.class_conditions.bookmark_button, pt);
  }

  // `FindBarHost` is a separate widget positioned from
  // `BrowserView::find_bar_host_view()` (chrome/browser/ui/views/
  // find_bar_host.cc and frame/browser_view.h). While it is visible, UIA can
  // expose only `FindBarView` from the root HWND. Point lookup still reaches
  // uncovered browser chrome, but keep it gated on a Views HWND so a page
  // click never crosses `Chrome_RenderWidgetHostHWND`.
  const HWND point_window = WindowFromPoint(pt);
  if (point_window && IsChromeWindow(point_window)) {
    ComPtr<IUIAutomationElement> pointed;
    if (SUCCEEDED(session.automation->ElementFromPoint(
            pt, pointed.ReleaseAndGetAddressOf())) &&
        IsValidBookmark(pointed)) {
      return pointed;
    }
  }

  // Bookmark folder menu: its own top-level popup window, all views, so the
  // window root is a safe anchor for the subtree `FindAll`. Windows that host
  // web content without `TopContainerView` (undocked DevTools again) must be
  // screened out first, or that `FindAll` crosses the renderer tree. Items
  // are `MenuItemView` (separators share the class but `IsValidBookmark`
  // rejects them).
  if (WindowHostsWebContent(window)) {
    return nullptr;
  }
  return FindBookmarkInAnchor(window_element,
                              session.class_conditions.menu_item_view, pt);
}

}  // namespace

// Resolve tabs through the root window's UIA tree instead of via
// `IUIAutomation::ElementFromPoint`. UIA's documented contract for point
// lookup is mouse-input-equivalence: the provider returned must
// "correspond to the element that would receive mouse input at the
// specified point" (`IRawElementProviderFragmentRoot::ElementProviderFrom-
// Point` on Microsoft Learn). UIAutomationCore therefore routes through
// Win32 hit-testing — `WindowFromPoint`/`ChildWindowFromPointEx` first,
// then the resulting HWND's fragment-root provider — so a child HWND with
// its own provider unconditionally shadows its parent for any point inside
// the child's rect.

// Chromium's `LegacyRenderWidgetHostHWND` (class
// `Chrome_RenderWidgetHostHWND`, see content/browser/renderer_host/
// legacy_render_widget_host_win.h) is a child HWND "the same size as the
// content area" kept for screen-reader and legacy-trackpad-driver compat.
// It covers the tab strip y-range and stays parented under
// `Chrome_WidgetWin_1` even when DevTools is undocked, so
// `ElementFromPoint` over a tab returns a node from its fragment tree
// instead of the `Tab`. Chromium would have to implement
// `IRawElementProviderHwndOverride` upstream to fix this in UIA itself.

// Querying the tab container from the root window via `ElementFromHandle`
// sidesteps HWND routing; the per-tab `PtInRect` inside `BuildTabHitResult`
// still rejects clicks that miss every tab.
std::optional<TabHitResult> FindTabHitResult(POINT pt,
                                             bool need_count,
                                             bool need_close_button) {
  UiaSession* session = GetUiaSession();
  if (!session) {
    return std::nullopt;
  }

  const HWND hwnd = WindowFromPoint(pt);
  const HWND root = hwnd ? GetAncestor(hwnd, GA_ROOT) : nullptr;
  if (!root || !IsChromeWindow(root)) {
    return std::nullopt;
  }

  RECT region_rect;
  TabUiCache* ui = GetValidatedTabUi(session, root, &region_rect);
  if (!ui) {
    return std::nullopt;
  }

  // Points outside the tab strip region -- page clicks are the overwhelming
  // majority -- return before any tree search.
  if (!PtInRect(&region_rect, pt)) {
    return std::nullopt;
  }

  return BuildTabHitResult(*session, ui->container, pt, need_count,
                           need_close_button);
}

bool SelectTab(const TabHitResult& hit_result) {
  if (!hit_result.tab) {
    return false;
  }

  ComPtr<IUnknown> pattern;
  HRESULT hr = hit_result.tab->GetCurrentPattern(
      UIA_SelectionItemPatternId, pattern.ReleaseAndGetAddressOf());
  if (FAILED(hr) || !pattern) {
    DebugLog(L"UIA: tab selection item pattern unavailable");
    return false;
  }

  ComPtr<IUIAutomationSelectionItemPattern> selection_item;
  hr = pattern->QueryInterface(
      IID_PPV_ARGS(selection_item.ReleaseAndGetAddressOf()));
  if (FAILED(hr) || !selection_item) {
    DebugLog(L"UIA: tab selection item QueryInterface failed");
    return false;
  }

  BOOL selected = FALSE;
  if (SUCCEEDED(selection_item->get_CurrentIsSelected(&selected)) && selected) {
    return true;
  }

  hr = selection_item->Select();
  if (FAILED(hr)) {
    DebugLog(L"UIA: tab Select failed");
    return false;
  }
  return true;
}

std::optional<int> FindTabCount(HWND hwnd) {
  UiaSession* session = GetUiaSession();
  if (!session) {
    return std::nullopt;
  }

  // The rectangle is unused here; the validated resolve proves the cached
  // container is still alive so the raw count below cannot silently return 0
  // over a dead element.
  RECT region_rect;
  TabUiCache* ui = GetValidatedTabUi(session, hwnd, &region_rect);
  if (!ui) {
    return std::nullopt;
  }

  // Raw view is required: tabs inside a collapsed tab group are hidden from
  // control view but still present in the raw tree, and they must be counted so
  // `keep_tab` does not mistake the last visible tab for the last tab overall.
  // Scoping the raw traversal to a credible tab container keeps it cheap.
  return CountDescendantsByClassRaw(*session, ui->container.element,
                                    GetTabElementClassName(ui->container.kind));
}

// The wheel path used `IUIAutomation::ElementFromPoint`, whose HWND routing
// descends into the renderer accessibility tree whenever the pointer is over
// the page (the same mechanism the bookmark path dropped for issue #270's
// sibling fix in `IsOnBookmark`). With `wheel_tab` on -- the default -- every
// scroll tick over web content paid that walk and switched web-contents
// accessibility on. A rectangle test against the cached tab strip region
// covers the same UI (tabs, new-tab button, grab handle, vertical strip) with
// no per-tick tree access at all.
bool IsOnTabBar(POINT pt) {
  UiaSession* session = GetUiaSession();
  if (!session) {
    return false;
  }

  const HWND hwnd = WindowFromPoint(pt);
  const HWND root = hwnd ? GetAncestor(hwnd, GA_ROOT) : nullptr;
  if (!root || !IsChromeWindow(root)) {
    return false;
  }

  RECT region_rect;
  if (!GetValidatedTabUi(session, root, &region_rect)) {
    return false;
  }

  return PtInRect(&region_rect, pt) != FALSE;
}

bool IsOnBookmark(POINT pt) {
  const UiaSession* session = GetUiaSession();
  if (!session) {
    return false;
  }

  // Climb to the top-level window with `GA_ROOT` before gating and anchoring is
  // needed. On secondary windows/monitors `WindowFromPoint` over a bookmark can
  // return a child window -- the content area's `Chrome_RenderWidgetHostHWND`,
  // or a child whose class is `Chrome_WidgetWin_1` as well. Gating on the bare
  // handle then either rejects the click outright (render-widget host) or
  // anchors the search to the content subtree and misses the bookmark bar on
  // the top-level tree -- the #238 regression from dropping `GA_ROOT`. Web
  // content is still screened out downstream since the search anchors
  // `TopContainerView`, so a real page click lands in no `BookmarkButton` rect.
  const HWND hwnd = WindowFromPoint(pt);
  const HWND root = hwnd ? GetAncestor(hwnd, GA_ROOT) : nullptr;
  if (!root || !IsChromeWindow(root)) {
    return false;
  }

  return FindBookmarkCoveringPoint(*session, root, pt) != nullptr;
}

bool IsOmniboxFocused() {
  // When focus sits in web content the focused HWND is the renderer's
  // `Chrome_RenderWidgetHostHWND` child, and resolving UIA focus there reads
  // renderer nodes -- enough for Chromium to enable web-contents accessibility.
  // The omnibox is a views control on the top-level window, so a Win32 class
  // check screens the typing-in-page case out before any UIA call.
  const HWND focus = GetFocus();
  std::array<wchar_t, 64> focus_class{};
  if (!focus ||
      !GetClassNameW(focus, focus_class.data(),
                     static_cast<int>(focus_class.size())) ||
      std::wstring_view(focus_class.data()) == L"Chrome_RenderWidgetHostHWND") {
    return false;
  }

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
  UiaSession* session = GetUiaSession();
  if (!session) {
    return false;
  }

  RECT region_rect;
  TabUiCache* ui = GetValidatedTabUi(session, hwnd, &region_rect);
  if (!ui) {
    return false;
  }

  const auto selected_tab = FindSelectedTabElement(*session, ui->container);
  if (!selected_tab) {
    return false;
  }

  const auto selected_name =
      GetStringProperty(selected_tab, UIA_NamePropertyId);
  if (!selected_name) {
    return false;
  }

  const auto std_name = GetNewTabButtonName(*session, ui->region);
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
