#pragma once

#include "MinDef.h"

EXTERN_C_START

#pragma region NLS

/* ntnls.h */

#define MAXIMUM_LEADBYTES   12
/*
 * This structure is the data from the raw codepage files.
 * Note that we set the "Codepage" field last, so any threads accessing this pointers in this structure
 * should check to see if that is CP_UTF8 (65001) first. If so, they should not use the pointers.
 * MemoryBarrier might be warranted before checking CodePage to protect out-of-order reads of the pointers.
 * 
 * See also: https://learn.microsoft.com/en-us/previous-versions/mt791523(v=vs.85)
 */
typedef struct _CPTABLEINFO
{
    USHORT CodePage;                    // code page number (For UTF-8 the rest of the structure is unused)
    USHORT MaximumCharacterSize;        // max length (bytes) of a char
    USHORT DefaultChar;                 // default character (MB)
    USHORT UniDefaultChar;              // default character (Unicode)
    USHORT TransDefaultChar;            // translation of default char (Unicode)
    USHORT TransUniDefaultChar;         // translation of Unic default char (MB)
    USHORT DBCSCodePage;                // Non 0 for DBCS code pages
    UCHAR  LeadByte[MAXIMUM_LEADBYTES]; // lead byte ranges
    PUSHORT MultiByteTable;             // pointer to MB->Unicode translation table
    PVOID   WideCharTable;              // pointer to WC (Unicode->CodePage) translation table
    PUSHORT DBCSRanges;                 // pointer to DBCS ranges (UNUSED, DO NOT SET)
    PUSHORT DBCSOffsets;                // pointer to DBCS offsets
} CPTABLEINFO, *PCPTABLEINFO;

/* See also: https://learn.microsoft.com/en-us/previous-versions/mt791531(v=vs.85) */
typedef struct _NLSTABLEINFO
{
    CPTABLEINFO OemTableInfo;   // OEM table
    CPTABLEINFO AnsiTableInfo;  // ANSI table
    PUSHORT UpperCaseTable;     // 844 format upcase table
    PUSHORT LowerCaseTable;     // 844 format lower case table
} NLSTABLEINFO, *PNLSTABLEINFO;

typedef struct _RTL_NLS_STATE
{
    CPTABLEINFO DefaultAcpTableInfo;
    CPTABLEINFO DefaultOemTableInfo;
    PUSHORT ActiveCodePageData;
    PUSHORT OemCodePageData;
    PUSHORT LeadByteInfo;
    PUSHORT OemLeadByteInfo;
    PUSHORT CaseMappingData;
    PUSHORT UnicodeUpcaseTable844;
    PUSHORT UnicodeLowercaseTable844;
} RTL_NLS_STATE, *PRTL_NLS_STATE;

/* phnt */

/* Data exports (ntdll.lib/ntdllp.lib) */
#if !defined(_KERNEL_MODE)
NTSYSAPI USHORT NlsAnsiCodePage;
NTSYSAPI BOOLEAN NlsMbCodePageTag;
NTSYSAPI BOOLEAN NlsMbOemCodePageTag;
#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtInitializeNlsFiles(
    _Out_ PVOID *BaseAddress,
    _Out_ PLCID DefaultLocaleId,
    _Out_ PLARGE_INTEGER DefaultCasingTableSize,
    _Out_opt_ PULONG CurrentNLSVersion);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtGetNlsSectionPtr(
    _In_ ULONG SectionType,
    _In_ ULONG SectionData,
    _In_ PVOID ContextData,
    _Out_ PVOID *SectionPointer,
    _Out_ PULONG SectionSize);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtMapCMFModule(
    _In_ ULONG What,
    _In_ ULONG Index,
    _Out_opt_ PULONG CacheIndexOut,
    _Out_opt_ PULONG CacheFlagsOut,
    _Out_opt_ PULONG ViewSizeOut,
    _Out_opt_ PVOID *BaseAddress);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtGetMUIRegistryInfo(
    _In_ ULONG Flags,
    _Inout_ PULONG DataSize,
    _Out_ PVOID Data);

#pragma endregion

#pragma region Locale & MUI

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryDefaultLocale(
    _In_ BOOLEAN UserProfile,
    _Out_ PLCID DefaultLocaleId);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetDefaultLocale(
    _In_ BOOLEAN UserProfile,
    _In_ LCID DefaultLocaleId);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInstallUILanguage(
    _Out_ LANGID* InstallUILanguageId);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtFlushInstallUILanguage(
    _In_ LANGID InstallUILanguage,
    _In_ ULONG SetComittedFlag);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryDefaultUILanguage(
    _Out_ LANGID* DefaultUILanguageId);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetDefaultUILanguage(
    _In_ LANGID DefaultUILanguageId);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtIsUILanguageComitted(VOID);

#pragma endregion phnt

EXTERN_C_END
