#include "uia.h"

#include <string_view>

#include "utils.h"

namespace uia {

namespace {

// Lazy-initialized UIA automation instance.
// Returns nullptr if initialization fails.
Microsoft::WRL::ComPtr<IUIAutomation> GetAutomation() {
  static Microsoft::WRL::ComPtr<IUIAutomation> automation = []() {
    Microsoft::WRL::ComPtr<IUIAutomation> instance;
    HRESULT hr =
        CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                         IID_PPV_ARGS(&instance));
    if (FAILED(hr)) {
      DebugLog(L"[UIA] CoCreateInstance failed: 0x{:08X}", hr);
      return Microsoft::WRL::ComPtr<IUIAutomation>{nullptr};
    }
    DebugLog(L"[UIA] Automation initialized successfully");
    return instance;
  }();
  return automation;
}

// Get the ClassName property of a UIA element.
// Returns empty string if failed.
std::wstring GetClassName(IUIAutomationElement* element) {
  if (!element) {
    return {};
  }

  BSTR bstr = nullptr;
  if (SUCCEEDED(element->get_CurrentClassName(&bstr)) && bstr) {
    std::wstring result(bstr);
    SysFreeString(bstr);
    return result;
  }
  return {};
}

// Walk up the UIA tree to find an ancestor with the specified ClassName.
// Returns the element if found, nullptr otherwise.
Microsoft::WRL::ComPtr<IUIAutomationElement> FindAncestorByClassName(
    IUIAutomation* automation,
    IUIAutomationElement* element,
    std::wstring_view target_class,
    int max_depth = 10) {
  if (!automation || !element) {
    return nullptr;
  }

  Microsoft::WRL::ComPtr<IUIAutomationTreeWalker> walker;
  if (FAILED(automation->get_ControlViewWalker(&walker)) || !walker) {
    return nullptr;
  }

  Microsoft::WRL::ComPtr<IUIAutomationElement> current;
  if (FAILED(walker->GetParentElement(element, &current)) || !current) {
    return nullptr;
  }

  for (int depth = 0; depth < max_depth && current; ++depth) {
    std::wstring class_name = GetClassName(current.Get());
    if (class_name == target_class) {
      return current;
    }

    Microsoft::WRL::ComPtr<IUIAutomationElement> parent;
    if (FAILED(walker->GetParentElement(current.Get(), &parent)) || !parent) {
      break;
    }
    current = parent;
  }

  return nullptr;
}

}  // namespace

std::optional<bool> IsOnCloseButton(POINT pt) {
  auto automation = GetAutomation();
  if (!automation) {
    DebugLog(L"[UIA] IsOnCloseButton: Automation not available, fallback");
    return std::nullopt;
  }

  Microsoft::WRL::ComPtr<IUIAutomationElement> element;
  HRESULT hr = automation->ElementFromPoint(pt, &element);
  if (FAILED(hr) || !element) {
    DebugLog(L"[UIA] IsOnCloseButton: ElementFromPoint failed: 0x{:08X}", hr);
    return std::nullopt;
  }

  // Check if the element itself is a TabCloseButton.
  std::wstring class_name = GetClassName(element.Get());
  DebugLog(L"[UIA] IsOnCloseButton: Element ClassName = '{}'", class_name);

  if (class_name == L"TabCloseButton") {
    DebugLog(L"[UIA] IsOnCloseButton: Direct hit on TabCloseButton");
    return true;
  }

  // Check if any ancestor is a TabCloseButton (in case we hit a child element).
  auto ancestor =
      FindAncestorByClassName(automation.Get(), element.Get(), L"TabCloseButton", 3);
  if (ancestor) {
    DebugLog(L"[UIA] IsOnCloseButton: Found TabCloseButton ancestor");
    return true;
  }

  // Verify we're in a Chrome tab area by checking for Tab ancestor.
  // This helps distinguish from other buttons.
  auto tab_ancestor =
      FindAncestorByClassName(automation.Get(), element.Get(), L"TabStrip::TabDragContextImpl", 5);
  if (!tab_ancestor) {
    // Not in a tab area at all, definitely not a tab close button.
    DebugLog(L"[UIA] IsOnCloseButton: Not in Tab area");
    return false;
  }

  DebugLog(L"[UIA] IsOnCloseButton: In Tab area but not on close button");
  return false;
}

}  // namespace uia
