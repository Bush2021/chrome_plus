#pragma once

#include "../../MinDef.h"

EXTERN_C_START

/* wdm.h */

#define RtlEqualLuid(L1, L2) (((L1)->LowPart == (L2)->LowPart) && ((L1)->HighPart  == (L2)->HighPart))
#define RtlIsZeroLuid(L1) ((BOOLEAN) (((L1)->LowPart | (L1)->HighPart) == 0))

/* ntddk.h */

FORCEINLINE
LUID
NTAPI_INLINE
RtlConvertLongToLuid(
    _In_ LONG Long)
{
    LUID TempLuid;
    LARGE_INTEGER TempLi;

    TempLi.QuadPart = Long;
    TempLuid.LowPart = TempLi.u.LowPart;
    TempLuid.HighPart = TempLi.u.HighPart;
    return(TempLuid);
}

FORCEINLINE
LUID
NTAPI_INLINE
RtlConvertUlongToLuid(
    _In_ ULONG Ulong)
{
    LUID TempLuid;

    TempLuid.LowPart = Ulong;
    TempLuid.HighPart = 0;
    return(TempLuid);
}

FORCEINLINE
LONGLONG
NTAPI_INLINE
RtlConvertLuidToLonglong(
    _In_ LUID Luid)
{
    LONGLONG TempLl;

    TempLl = Luid.LowPart;
    TempLl += ((LONGLONG)(Luid.HighPart) << 32);

    return(TempLl);
}

/* ntifs.h */

NTSYSAPI
VOID
NTAPI
RtlCopyLuid(
    _Out_ PLUID DestinationLuid,
    _In_ PLUID SourceLuid);

/* phnt */

NTSYSAPI
VOID
NTAPI
RtlCopyLuidAndAttributesArray(
    _In_ ULONG Count,
    _In_ PLUID_AND_ATTRIBUTES Src,
    _In_ PLUID_AND_ATTRIBUTES Dest);

EXTERN_C_END
