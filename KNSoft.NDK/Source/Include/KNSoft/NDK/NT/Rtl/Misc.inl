#pragma once

#include "../../NDK.h"

EXTERN_C_START

__inline
LOGICAL
NTAPI
_Inline_RtlQueryPerformanceFrequency(
    _Out_ PLARGE_INTEGER PerformanceFrequency)
{
    if (SharedUserData->NtMajorVersion > 6 ||
        SharedUserData->NtMajorVersion == 6 && SharedUserData->NtMinorVersion >= 2)
    {
        PerformanceFrequency->QuadPart = SharedUserData->QpcFrequency;
    } else
    {
        LARGE_INTEGER PerformanceCounter;
        NtQueryPerformanceCounter(&PerformanceCounter, PerformanceFrequency);
    }
    return TRUE;
}

__inline
ULONG
NTAPI
_Inline_RtlGetCurrentServiceSessionId(VOID)
{
    PSILO_USER_SHARED_DATA SharedData = NtCurrentPeb()->SharedData;
    return SharedData == NULL ? 0 : SharedData->ServiceSessionId;
}

__inline
ULONG
NTAPI
_Inline_RtlGetActiveConsoleId(VOID)
{
    return _Inline_RtlGetCurrentServiceSessionId() == 0 ?
        SharedUserData->ActiveConsoleId :
        NtCurrentPeb()->SharedData->ActiveConsoleId;
}

#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)
__inline
LONGLONG
NTAPI
_Inline_RtlGetConsoleSessionForegroundProcessId(VOID)
{
    return _Inline_RtlGetCurrentServiceSessionId() == 0 ?
        SharedUserData->ConsoleSessionForegroundProcessId :
        NtCurrentPeb()->SharedData->ConsoleSessionForegroundProcessId;
}
#endif

EXTERN_C_END
