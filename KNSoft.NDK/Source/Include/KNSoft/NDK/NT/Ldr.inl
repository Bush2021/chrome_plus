#pragma once

#include "../NDK.h"

EXTERN_C_START

__inline
NTSTATUS
NTAPI
_Inline_LdrGetDllHandle(
    _In_opt_ PCWSTR DllPath,
    _In_opt_ PULONG DllCharacteristics,
    _In_ PUNICODE_STRING DllName,
    _Out_ PVOID* DllHandle)
{
    return LdrGetDllHandleEx(LDR_GET_DLL_HANDLE_EX_UNCHANGED_REFCOUNT,
                             DllPath,
                             DllCharacteristics,
                             DllName,
                             DllHandle);
}

EXTERN_C_END
