#pragma once

#include "MinDef.h"

#include "Cm/Cm.h"
#include "Ex/Ex.h"
#include "Io/Io.h"
#include "Ke/Ke.h"
#include "Lpc/Lpc.h"
#include "Mm/Mm.h"
#include "Ob/Ob.h"
#include "Po/Po.h"
#include "Ps/Ps.h"
#include "Rtl/Rtl.h"

#include "Se/Se.h"
#include "Win32K/Win32K.h"
#include "Wow/Wow.h"
#include "AppHelp.h"
#include "Bcd.h"
#include "Dbg.h"
#include "Etw.h"
#include "Image.h"
#include "Ldr.h"
#include "Misc.h"
#include "Nls.h"
#include "Sam.h"
#include "Sxs.h"
#include "Tm.h"
#include "ZwApi.h"

#define _USE_FULL_PROC_THREAD_ATTRIBUTE
#include <WinBase.h>
#include <ntlsa.h>

#include "Rtl/ActCtx.h"

#ifndef _KNSOFT_NDK_NO_EXTENSION
#include "Extension/Misc.h"
#include "Extension/Runtime.h"
#include "Extension/Security.h"
#endif
