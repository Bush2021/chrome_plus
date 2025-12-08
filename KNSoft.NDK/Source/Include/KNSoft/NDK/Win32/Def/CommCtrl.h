#pragma once

#include <CommCtrl.h>

#ifdef _USE_COMMCTL60
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#pragma region Patches C26454 warning

#undef NM_FIRST
#define NM_FIRST 0x100000000

#undef TCN_FIRST
#define TCN_FIRST 0xFFFFFDDA

#undef TVN_FIRST
#define TVN_FIRST 0xFFFFFE70

#undef LVN_FIRST
#define LVN_FIRST 0xFFFFFF9C

#pragma endregion
