/*
 * KNSoft.NDK (https://github.com/KNSoft/KNSoft.NDK)
 * Copyright (c) KNSoft.org (https://github.com/KNSoft). All rights reserved.
 * Licensed under the MIT license.
 */

#pragma once

#include "NT/NT.h"

/* Windows.h and winioctl.h */

#define _WINSOCKAPI_ // Use WinSock2.h
#include <Windows.h>

#include <initguid.h>
#include <winioctl.h>

#include "Win32/Def/WinBase.h"
#include "Win32/Def/WinUser.h"
#include "Win32/Def/CommCtrl.h"
#include "Win32/Def/UxTheme.h"
#include "NT/Afd.h"

/* APIs */

#include "Win32/API/Ntdll.h"
#include "Win32/API/Kernel32.h"
#include "Win32/API/KernelBase.h"
#include "Win32/API/SecHost.h"
#include "Win32/API/User32.h"
#include "Win32/API/UserMgrCli.h"
#include "Win32/API/WinSta.h"
#include "Win32/API/AdvAPI32.h"
#include "Win32/API/ComBase.h"

/* Additional headers */

#include <intrin.h>
#include <suppress.h>

/* Enable extensions */

#ifndef _KNSOFT_NDK_NO_EXTENSION
#include "NDK.Ext.h"
#endif

/* Sanity checks */
_STATIC_ASSERT(__alignof(LARGE_INTEGER) == 8);
_STATIC_ASSERT(__alignof(PROCESS_CYCLE_TIME_INFORMATION) == 8);
