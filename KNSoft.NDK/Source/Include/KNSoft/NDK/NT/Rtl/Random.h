#pragma once

#include "../MinDef.h"

EXTERN_C_START

/* 
 * By KNSoft.NDK.
 * Note: According to Microsoft Learning, return value of RtlRandom/RtlRandomEx in the range [0..MAXLONG-1].
 * But SAL in Windows SDK is [0..MAXLONG].
 */

_Ret_range_(0, MAXLONG)
NTSYSAPI
ULONG
NTAPI
RtlUniform(
    _Inout_ PULONG Seed);

_Ret_range_(0, MAXLONG - 1)
NTSYSAPI
ULONG
NTAPI
RtlRandom(
    _Inout_ PULONG Seed);

_Ret_range_(0, MAXLONG - 1)
NTSYSAPI
ULONG
NTAPI
RtlRandomEx(
    _Inout_ PULONG Seed);

EXTERN_C_END
