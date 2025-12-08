#pragma once

#include "../../MinDef.h"
#include "../../Ob/Misc.h"

EXTERN_C_START

/* phnt */

#define BOUNDARY_DESCRIPTOR_ADD_APPCONTAINER_SID 0x0001

_Ret_maybenull_
_Success_(return != NULL)
NTSYSAPI
POBJECT_BOUNDARY_DESCRIPTOR
NTAPI
RtlCreateBoundaryDescriptor(
    _In_ PUNICODE_STRING Name,
    _In_ ULONG Flags);

NTSYSAPI
VOID
NTAPI
RtlDeleteBoundaryDescriptor(
    _In_ _Post_invalid_ POBJECT_BOUNDARY_DESCRIPTOR BoundaryDescriptor);

NTSYSAPI
NTSTATUS
NTAPI
RtlAddSIDToBoundaryDescriptor(
    _Inout_ POBJECT_BOUNDARY_DESCRIPTOR *BoundaryDescriptor,
    _In_ PSID RequiredSid);

NTSYSAPI
NTSTATUS
NTAPI
RtlAddIntegrityLabelToBoundaryDescriptor(
    _Inout_ POBJECT_BOUNDARY_DESCRIPTOR *BoundaryDescriptor,
    _In_ PSID IntegrityLabel);

EXTERN_C_END
