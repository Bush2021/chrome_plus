#pragma once

#include "../../MinDef.h"

EXTERN_C_START

/* phnt */

#if (NTDDI_VERSION >= NTDDI_WIN8)

NTSYSAPI
NTSTATUS
NTAPI
RtlIsUntrustedObject(
    _In_opt_ HANDLE Handle,
    _In_opt_ PVOID Object,
    _Out_ PBOOLEAN IsUntrustedObject);

NTSYSAPI
ULONG
NTAPI
RtlQueryValidationRunlevel(
    _In_opt_ PUNICODE_STRING ComponentName);

NTSYSAPI
NTSTATUS
NTAPI
RtlNewSecurityGrantedAccess(
    _In_ ACCESS_MASK DesiredAccess,
    _Out_ PPRIVILEGE_SET NewPrivileges,
    _Inout_ PULONG Length, 
    _In_opt_ HANDLE TokenHandle,
    _In_ PGENERIC_MAPPING GenericMapping,   
    _Out_ PACCESS_MASK RemainingDesiredAccess);

#endif

EXTERN_C_END
