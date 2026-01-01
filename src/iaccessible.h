#ifndef CHROME_PLUS_SRC_IACCESSIBLE_H_
#define CHROME_PLUS_SRC_IACCESSIBLE_H_

#include <oleacc.h>
#include <wrl/client.h>

using NodePtr = Microsoft::WRL::ComPtr<IAccessible>;

struct TabInfo {
  NodePtr tab;
  int tab_count = 0;
};
struct BookmarkState {
  bool on_bookmark = false;
  bool on_expanded_list = false;
};

NodePtr GetChromeWidgetWin(HWND hwnd);
NodePtr GetTopContainerView(HWND hwnd);
int GetTabCount(const NodePtr& top);
bool IsOnTheTabBar(const NodePtr& top, POINT pt);
bool IsOnNewTab(const NodePtr& top);
TabInfo GetTabInfo(const NodePtr& top, POINT pt, bool need_count);
BookmarkState CheckBookmarkState(HWND hwnd, POINT pt);
bool IsOmniboxFocus(const NodePtr& top);
bool IsOnCloseButton(const NodePtr& top, POINT pt);
bool IsOnFindBarPane(POINT pt);

#endif  // CHROME_PLUS_SRC_IACCESSIBLE_H_
