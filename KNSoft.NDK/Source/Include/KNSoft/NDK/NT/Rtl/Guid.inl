#pragma once

#include "../../NDK.h"

#include <stdio.h>

EXTERN_C_START

__inline
NTSTATUS
NTAPI
_Inline_RtlStringFromGUIDEx(
    _In_ PGUID Guid,
    _When_(AllocateGuidString, _Out_ _At_(GuidString->Buffer, __drv_allocatesMem(Mem))) _When_(!AllocateGuidString, _Inout_) PUNICODE_STRING GuidString,
    _In_ BOOLEAN AllocateGuidString)
{
    if (AllocateGuidString)
    {
        GuidString->MaximumLength = RTL_GUID_STRING_SIZE * sizeof(WCHAR);
        GuidString->Buffer = (PWCH)RtlAllocateHeap(RtlProcessHeap(), 0, RTL_GUID_STRING_SIZE * sizeof(WCHAR));
        if (GuidString->Buffer == NULL)
        {
            return STATUS_NO_MEMORY;
        }
    } else
    {
        if (GuidString->MaximumLength < RTL_GUID_STRING_SIZE * sizeof(WCHAR))
        {
            return STATUS_BUFFER_TOO_SMALL;
        }
    }

    GuidString->Length = RTL_GUID_STRING_SIZE * sizeof(WCHAR);

    /* False positive: GuidString->Buffer should be ready */
#pragma warning(disable: __WARNING_USING_UNINIT_VAR)
    swprintf_s(GuidString->Buffer,
               GuidString->MaximumLength / sizeof(WCHAR),
               L"{%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
               Guid->Data1,
               Guid->Data2,
               Guid->Data3,
               Guid->Data4[0],
               Guid->Data4[1],
               Guid->Data4[2],
               Guid->Data4[3],
               Guid->Data4[4],
               Guid->Data4[5],
               Guid->Data4[6],
               Guid->Data4[7]);
#pragma warning(default: __WARNING_USING_UNINIT_VAR)

    return STATUS_SUCCESS;
}

_IRQL_requires_max_(PASSIVE_LEVEL)
_Must_inspect_result_
__inline
NTSTATUS
NTAPI
_Inline_RtlStringFromGUID(
    _In_ PGUID Guid,
    _Out_ _At_(GuidString->Buffer, __drv_allocatesMem(Mem)) PUNICODE_STRING GuidString)
{
    return _Inline_RtlStringFromGUIDEx(Guid, GuidString, TRUE);
}

EXTERN_C_END
