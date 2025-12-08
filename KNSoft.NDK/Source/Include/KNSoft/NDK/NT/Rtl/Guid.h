#pragma once

#include "../MinDef.h"

EXTERN_C_START

/* wdm.h */

#ifndef DEFINE_GUIDEX
    #define DEFINE_GUIDEX(name) EXTERN_C const CDECL GUID name
#endif // !defined(DEFINE_GUIDEX)

#ifndef STATICGUIDOF
    #define STATICGUIDOF(guid) STATIC_##guid
#endif // !defined(STATICGUIDOF)

#ifndef __IID_ALIGNED__
    #define __IID_ALIGNED__
    #ifdef __cplusplus
        inline int IsEqualGUIDAligned(REFGUID guid1, REFGUID guid2)
        {
            return ((*(PLONGLONG)(&guid1) == *(PLONGLONG)(&guid2)) && (*((PLONGLONG)(&guid1) + 1) == *((PLONGLONG)(&guid2) + 1)));
        }
    #else // !__cplusplus
        #define IsEqualGUIDAligned(guid1, guid2) \
            ((*(PLONGLONG)(guid1) == *(PLONGLONG)(guid2)) && (*((PLONGLONG)(guid1) + 1) == *((PLONGLONG)(guid2) + 1)))
    #endif // !__cplusplus
#endif // !__IID_ALIGNED__

//
// Length in characters of the GUID represented as string not including the
// null terminator.
//

#define RTL_GUID_STRING_SIZE 38

/**
 * The RtlStringFromGUID routine converts a given GUID from binary format into a Unicode string.
 *
 * \param[in] Guid Specifies the binary-format GUID to convert.
 * \param[out] GuidString Pointer to a caller-supplied variable in which a pointer to the converted GUID string is returned and must free by calling RtlFreeUnicodeString.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlstringfromguid
 */
_IRQL_requires_max_(PASSIVE_LEVEL)
_Must_inspect_result_
NTSYSAPI
NTSTATUS
NTAPI
RtlStringFromGUID(
    _In_ PGUID Guid,
    _Out_ _At_(GuidString->Buffer, __drv_allocatesMem(Mem)) PUNICODE_STRING GuidString);

_IRQL_requires_max_(PASSIVE_LEVEL)
_Must_inspect_result_
NTSYSAPI
NTSTATUS
NTAPI
RtlGUIDFromString(
    _In_ PCUNICODE_STRING GuidString,
    _Out_ GUID* Guid);

/* phnt */

#if (NTDDI_VERSION >= NTDDI_WINBLUE)
// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlStringFromGUIDEx(
    _In_ PGUID Guid,
    _When_(AllocateGuidString, _Out_ _At_(GuidString->Buffer, __drv_allocatesMem(Mem))) _When_(!AllocateGuidString, _Inout_) PUNICODE_STRING GuidString,
    _In_ BOOLEAN AllocateGuidString);
#endif

EXTERN_C_END
