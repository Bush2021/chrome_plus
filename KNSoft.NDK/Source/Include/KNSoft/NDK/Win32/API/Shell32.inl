#pragma once

#include "../../NDK.h"
#include "../../NT/NT.inl"

#ifndef ARGPARSE_ALLOC_FUNCNAME

#define TChar WCHAR
#include "../../Package/ArgParse.inl"
#undef TChar

#define TChar CHAR
#include "../../Package/ArgParse.inl"
#undef TChar

#endif

EXTERN_C_START

/* The original declaration in Windows SDK has no _Success_ notation */
#pragma warning(disable: 6101)

__inline
LPWSTR*
STDAPICALLTYPE
_Inline_CommandLineToArgvW(
    _In_ LPCWSTR lpCmdLine,
    _Out_ int* pNumArgs)
{
    NTSTATUS Status;
    ULONG ArgC;
    WCHAR** ArgV;

    Status = ARGPARSE_ALLOC_FUNCNAME(WCHAR)(lpCmdLine, &ArgC, &ArgV);
    if (!NT_SUCCESS(Status))
    {
        _Inline_RtlSetLastWin32Error(_Inline_RtlNtStatusToDosError(Status));
        return NULL;
    }

    *pNumArgs = ArgC;
    return ArgV;
}

__inline
LPSTR*
STDAPICALLTYPE
_Inline_CommandLineToArgvA(
    _In_ LPCSTR lpCmdLine,
    _Out_ int* pNumArgs)
{
    NTSTATUS Status;
    ULONG ArgC;
    CHAR** ArgV;

    Status = ARGPARSE_ALLOC_FUNCNAME(CHAR)(lpCmdLine, &ArgC, &ArgV);
    if (!NT_SUCCESS(Status))
    {
        _Inline_RtlSetLastWin32Error(_Inline_RtlNtStatusToDosError(Status));
        return NULL;
    }

    *pNumArgs = ArgC;
    return ArgV;
}

#pragma warning(default: 6101)

EXTERN_C_END
