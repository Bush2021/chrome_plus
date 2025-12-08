#pragma once

#include "../../NDK.h"

EXTERN_C_START

__inline
NTSTATUS
NTAPI
_Inline_RtlWow64GetThreadContext(
    _In_ HANDLE ThreadHandle,
    _Inout_ PWOW64_CONTEXT ThreadContext)
{
    return NtQueryInformationThread(ThreadHandle, ThreadWow64Context, ThreadContext, sizeof(WOW64_CONTEXT), NULL);
}

__inline
NTSTATUS
NTAPI
_Inline_RtlWow64SetThreadContext(
    _In_ HANDLE ThreadHandle,
    _In_ PWOW64_CONTEXT ThreadContext)
{
    return NtSetInformationThread(ThreadHandle, ThreadWow64Context, ThreadContext, sizeof(WOW64_CONTEXT));
}

EXTERN_C_END
