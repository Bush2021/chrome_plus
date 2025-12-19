#ifndef CHROME_PLUS_SRC_HOTKEY_H_
#define CHROME_PLUS_SRC_HOTKEY_H_

#include <windows.h>

void GetHotkey();

UINT ParseTranslateKey();
UINT ParseSwitchToPrevKey();
UINT ParseSwitchToNextKey();

#endif  // CHROME_PLUS_SRC_HOTKEY_H_
