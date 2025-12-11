#ifndef CHROME_PLUS_SRC_IACCESSIBLE_H_
#define CHROME_PLUS_SRC_IACCESSIBLE_H_

#include <oleacc.h>
#include <wrl/client.h>

#include <compare>

using NodePtr = Microsoft::WRL::ComPtr<IAccessible>;

struct ElementFingerprint {
  long role = 0;
  RECT rect = {0, 0, 0, 0};

  bool operator==(const ElementFingerprint& other) const {
    return role == other.role && rect.left == other.rect.left &&
           rect.top == other.rect.top && rect.right == other.rect.right &&
           rect.bottom == other.rect.bottom;
  }
};

NodePtr GetChromeWidgetWin(HWND hwnd);
NodePtr GetTopContainerView(HWND hwnd);
ElementFingerprint GetElementFingerprint(const NodePtr& node);
int GetTabCount(const NodePtr& top);
bool IsOnOneTab(const NodePtr& top, POINT pt);
bool IsOnOneTab(const NodePtr& top, POINT pt, NodePtr* tab_out);
bool IsOnlyOneTab(const NodePtr& top);
bool IsOnTheTabBar(const NodePtr& top, POINT pt);
bool IsOnNewTab(const NodePtr& top);
bool IsOnBookmark(HWND hwnd, POINT pt);
bool IsOnExpandedList(HWND hwnd, POINT pt);
bool IsOmniboxFocus(const NodePtr& top);
bool IsOnCloseButton(const NodePtr& top, POINT pt);
bool IsOnFindBarPane(POINT pt);

#endif  // CHROME_PLUS_SRC_IACCESSIBLE_H_
