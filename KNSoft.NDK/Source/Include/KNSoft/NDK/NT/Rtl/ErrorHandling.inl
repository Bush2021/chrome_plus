#pragma once

#include "../../NDK.h"

EXTERN_C_START

_When_(Status < 0, _Out_range_(>, 0))
_When_(Status >= 0, _Out_range_(==, 0))
__inline
ULONG
NTAPI
_Inline_RtlNtStatusToDosError(
    _In_ NTSTATUS Status)
{
    NtWriteTeb(LastStatusValue, Status);
    return RtlNtStatusToDosErrorNoTeb(Status);
}

__inline
NTSTATUS
NTAPI
_Inline_RtlGetLastNtStatus(VOID)
{
    return NtReadTeb(LastStatusValue);
}

__inline
_Check_return_
_Post_equals_last_error_
ULONG
NTAPI
_Inline_RtlGetLastWin32Error(VOID)
{
    return NtReadTeb(LastErrorValue);
}

__inline
VOID
NTAPI
_Inline_RtlSetLastWin32Error(
    _In_ ULONG Win32Error)
{
    NtWriteTeb(LastErrorValue, Win32Error);
}

__inline
VOID
NTAPI
_Inline_RtlRestoreLastWin32Error(
    _In_ ULONG Win32Error)
{
    NtWriteTeb(LastErrorValue, Win32Error);
}

__inline
VOID
NTAPI
_Inline_RtlSetLastWin32ErrorAndNtStatusFromNtStatus(
    _In_ NTSTATUS Status)
{
    _Inline_RtlSetLastWin32Error(_Inline_RtlNtStatusToDosError(Status));
}

EXTERN_C_END
