#pragma once

#include "../../NDK.h"

EXTERN_C_START

__inline
VOID
NTAPI
_Inline_RtlPushFrame(
    _In_ PTEB_ACTIVE_FRAME Frame)
{
    Frame->Previous = NtReadTeb(ActiveFrame);
    NtWriteTeb(ActiveFrame, Frame);
}

__inline
VOID
NTAPI
_Inline_RtlPopFrame(
    _In_ PTEB_ACTIVE_FRAME Frame)
{
    NtWriteTeb(ActiveFrame, Frame->Previous);
}

__inline
PTEB_ACTIVE_FRAME
NTAPI
_Inline_RtlGetFrame(VOID)
{
    return NtReadTeb(ActiveFrame);
}

EXTERN_C_END
