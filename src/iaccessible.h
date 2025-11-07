#ifndef CHROME_PLUS_SRC_IACCESSIBLE_H_
#define CHROME_PLUS_SRC_IACCESSIBLE_H_

#include <oleacc.h>
#include <wrl/client.h>

using NodePtr = Microsoft::WRL::ComPtr<IAccessible>;

NodePtr GetChromeWidgetWin(HWND hwnd);
NodePtr GetTopContainerView(HWND hwnd);
int GetTabCount(NodePtr top);
bool IsOnOneTab(NodePtr top, const POINT& pt);
bool IsOnlyOneTab(NodePtr top);
bool IsOnTheTabBar(NodePtr top, const POINT& pt);
bool IsNameNewTab(NodePtr top);
bool IsDocNewTab();
bool IsOnNewTab(NodePtr top);
bool IsOnBookmark(HWND hwnd, const POINT& pt);
bool IsOnExpandedList(HWND hwnd, const POINT& pt);
bool IsOmniboxFocus(NodePtr top);
bool IsOnCloseButton(NodePtr top, const POINT& pt);
bool IsOnFindBarPane(const POINT& pt);

#endif  // CHROME_PLUS_SRC_IACCESSIBLE_H_
