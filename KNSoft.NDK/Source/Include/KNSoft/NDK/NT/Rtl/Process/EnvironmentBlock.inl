#pragma once

#include "../../../NDK.h"

EXTERN_C_START

__inline
PPEB
NTAPI
_Inline_RtlGetCurrentPeb(VOID)
{
    return NtCurrentPeb();
}

__inline
NTSTATUS
NTAPI
_Inline_RtlAcquirePebLock(VOID)
{
    return RtlEnterCriticalSection(NtCurrentPeb()->FastPebLock);
}

__inline
NTSTATUS
NTAPI
_Inline_RtlReleasePebLock(VOID)
{
    return RtlLeaveCriticalSection(NtCurrentPeb()->FastPebLock);
}

__inline
LOGICAL
NTAPI
_Inline_RtlTryAcquirePebLock(VOID)
{
    return RtlTryEnterCriticalSection(NtCurrentPeb()->FastPebLock);
}

EXTERN_C_END
