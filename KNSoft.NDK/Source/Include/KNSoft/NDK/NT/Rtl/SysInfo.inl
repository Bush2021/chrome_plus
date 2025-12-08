#pragma once

#include "../../NDK.h"

EXTERN_C_START

__inline
VOID
NTAPI
_Inline_RtlGetNtVersionNumbers(
    _Out_opt_ PULONG NtMajorVersion,
    _Out_opt_ PULONG NtMinorVersion,
    _Out_opt_ PULONG NtBuildNumber)
{
    PPEB Peb = NtCurrentPeb();

    if (NtMajorVersion != NULL)
    {
        *NtMajorVersion = Peb->OSMajorVersion;
    }
    if (NtMinorVersion != NULL)
    {
        *NtMinorVersion = Peb->OSMinorVersion;
    }
    if (NtBuildNumber != NULL)
    {
        *NtBuildNumber = Peb->OSBuildNumber | 0xF0000000; // What's this? However, only low 16-bit should be used.
    }
}

__inline
ULONG
NTAPI
_Inline_RtlGetNtGlobalFlags(VOID)
{
    return NtCurrentPeb()->NtGlobalFlag;
}

__inline
BOOLEAN
NTAPI
_Inline_RtlIsProcessorFeaturePresent(
    _In_ ULONG ProcessorFeature)
{
    return ProcessorFeature < PROCESSOR_FEATURE_MAX ?
        SharedUserData->ProcessorFeatures[ProcessorFeature] :
        FALSE;
}

EXTERN_C_END
