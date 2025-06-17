#ifndef CHROME_PLUS_SRC_IACCESSIBLE_H_
#define CHROME_PLUS_SRC_IACCESSIBLE_H_

#include <oleacc.h>
#include <wrl/client.h>

using NodePtr = Microsoft::WRL::ComPtr<IAccessible>;

NodePtr GetChromeWidgetWin(HWND hwnd);
NodePtr GetTopContainerView(HWND hwnd);
int GetTabCount(NodePtr top);
bool IsOnOneTab(NodePtr top, POINT pt);
bool IsOnlyOneTab(NodePtr top);
bool IsOnTheTabBar(NodePtr top, POINT pt);
bool IsNameNewTab(NodePtr top);
bool IsDocNewTab();
bool IsOnNewTab(NodePtr top);
bool IsOnBookmark(HWND hwnd, POINT pt);
bool IsOmniboxFocus(NodePtr top);
bool IsOnDialog(HWND hwnd, POINT pt);
bool IsOnCloseButton(NodePtr top, POINT pt);
bool IsOnPane(HWND hwnd, POINT pt);

#endif  // CHROME_PLUS_SRC_IACCESSIBLE_H_
