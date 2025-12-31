#ifndef CHROME_PLUS_SRC_IACCESSIBLE_H_
#define CHROME_PLUS_SRC_IACCESSIBLE_H_

#include <oleacc.h>
#include <wrl/client.h>

using NodePtr = Microsoft::WRL::ComPtr<IAccessible>;

NodePtr GetChromeWidgetWin(HWND hwnd);
NodePtr GetTopContainerView(HWND hwnd);
int GetTabCount(const NodePtr& top);
NodePtr GetTabUnderMouse(const NodePtr& top, POINT pt);
bool IsOnOneTab(const NodePtr& top, POINT pt);
bool IsOnlyOneTab(const NodePtr& top);
bool IsOnTheTabBar(const NodePtr& top, POINT pt);
bool IsOnNewTab(const NodePtr& top);
bool IsOnBookmark(HWND hwnd, POINT pt);
bool IsOnExpandedList(HWND hwnd, POINT pt);
bool IsOmniboxFocus(const NodePtr& top);
bool IsOnCloseButton(const NodePtr& top, POINT pt);
bool IsOnFindBarPane(POINT pt);

#endif  // CHROME_PLUS_SRC_IACCESSIBLE_H_
