#pragma once

#include "../../NDK.h"

EXTERN_C_START

__inline
ULONG64
NTAPI
_Inline_RtlGetEnabledExtendedFeatures(
    _In_ ULONG64 FeatureMask)
{
    return SharedUserData->XState.EnabledVolatileFeatures & FeatureMask;
}

EXTERN_C_END
