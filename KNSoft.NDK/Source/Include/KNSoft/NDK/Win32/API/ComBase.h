#pragma once

#include "../../NT/MinDef.h"

#include <WinBase.h>

EXTERN_C_START

// private
typedef enum _MTA_HOST_USAGE_FLAGS
{
    MTA_HOST_USAGE_NONE = 0x0,
    MTA_HOST_USAGE_MTAINITIALIZED = 0x1,
    MTA_HOST_USAGE_ACTIVATORINITIALIZED = 0x2,
    MTA_HOST_USAGE_UNLOADCALLED = 0x4,
} MTA_HOST_USAGE_FLAGS, *PMTA_HOST_USAGE_FLAGS;

// private
typedef struct _MTA_USAGE_GLOBALS
{
    _Reserved_ ULONG StackCapture;
    PULONG MTAInits; // A pointer to the total number of MTA inits
    PULONG MTAIncInits; // A pointer to the number of MTA inits from CoIncrementMTAUsage
    PULONG MTAWaiters; // A pointer to the number of callers waiting inside CoWaitMTACompletion
    PULONG MTAIncrementorSize; // A pointer to the size of the cookie returned by CoIncrementMTAUsage
    ULONG CompletionTimeOut; // A timeout for CoWaitMTACompletion in milliseconds
    _Reserved_ PLIST_ENTRY ListEntryHeadMTAUsageIncrementor;
    _Reserved_ PULONG MTAIncrementorCompleted;
    _Reserved_ PVOID* MTAUsageCompletedIncrementorHead;
    PMTA_HOST_USAGE_FLAGS MTAHostUsageFlags; // A pointer to the MTA usage flags // since THRESHOLD
} MTA_USAGE_GLOBALS, *PMTA_USAGE_GLOBALS;

#if (NTDDI_VERSION >= NTDDI_WIN8)
// private // combase.dll, ordinal 70
_Success_(return != 0)
_Must_inspect_result_
WINBASEAPI
PMTA_USAGE_GLOBALS
WINAPI
CoGetMTAUsageInfo(VOID);
#endif

EXTERN_C_END
