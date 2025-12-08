#pragma once

#include "../../../NDK.h"

EXTERN_C_START

__inline
VOID
NTAPI
_Inline_RtlCopyLuid(
    _Out_ PLUID DestinationLuid,
    _In_ PLUID SourceLuid)
{
    *DestinationLuid = *SourceLuid;
}

__inline
VOID
NTAPI
_Inline_RtlCopyLuidAndAttributesArray(
    _In_ ULONG Count,
    _In_ PLUID_AND_ATTRIBUTES Src,
    _In_ PLUID_AND_ATTRIBUTES Dest)
{
    memcpy(Dest, Src, Count * sizeof(LUID_AND_ATTRIBUTES));
}

EXTERN_C_END
