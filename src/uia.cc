#include "uia.h"

#include <UIAutomation.h>
#include <oleauto.h>
#include <wrl/client.h>
#include <cwchar>
#include "utils.h"

namespace {
static Microsoft::WRL::ComPtr<IUIAutomationElement> UiaElementFromPoint(
    const POINT& pt) {
  Microsoft::WRL::ComPtr<IUIAutomation> automation;
  HRESULT hr =
      CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                       IID_PPV_ARGS(&automation));
  if (FAILED(hr) || !automation) {
    //DebugLog(L"UIA: CoCreateInstance failed: 0x{:08X}", hr);
    return nullptr;
  }
  Microsoft::WRL::ComPtr<IUIAutomationElement> element;
  hr = automation->ElementFromPoint(pt, &element);
  if (FAILED(hr) || !element) {
    //DebugLog(L"UIA: ElementFromPoint failed: 0x{:08X}", hr);
    return nullptr;
  }
  //DebugLog(L"UIA: ElementFromPoint succeeded");
  return element;
}

static bool UiaIsElementClass(IUIAutomationElement* element,
                              const wchar_t* expect) {
  if (!element || !expect) {
    //DebugLog(L"UIA: UiaIsElementClass: null element or expect");
    return false;
  }
  BSTR bstr = nullptr;
  HRESULT hr = element->get_CurrentClassName(&bstr);
  if (FAILED(hr) || !bstr) {
    //DebugLog(L"UIA: get_CurrentClassName failed: 0x{:08X}", hr);
    return false;
  }
  //DebugLog(L"UIA: get_CurrentClassName: {}", std::wstring(bstr));
  bool ok = (_wcsicmp(bstr, expect) == 0);
  SysFreeString(bstr);
  return ok;
}

}  // namespace

bool UiaIsOnOneTab(POINT pt) {
  //DebugLog(L"UIA: UiaIsOnOneTab called, pt=({}, {})", pt.x, pt.y);
  HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  bool need_uninit = SUCCEEDED(hr);

  auto cleanup = [&]() {
    if (need_uninit)
      CoUninitialize();
  };

  auto element = UiaElementFromPoint(pt);
  if (!element) {
    //DebugLog(L"UIA: UiaElementFromPoint returned nullptr");
    cleanup();
    return false;
  }
  if (!UiaIsElementClass(element.Get(), L"TabStrip::TabDragContextImpl")) {
    //DebugLog(L"UIA: Element is not TabStrip::TabDragContextImpl");
    cleanup();
    return false;
  }
  //DebugLog(L"UIA: Element is TabStrip::TabDragContextImpl");
  cleanup();
  return true;
}
