#pragma once

#include "../MinDef.h"

EXTERN_C_START

/* phnt */

NTSYSAPI
PIMAGE_NT_HEADERS
NTAPI
RtlImageNtHeader(
    _In_ PVOID BaseOfImage
);

/**
 * Flag to disable range checking in RtlImageNtHeaderEx.
 */
#define RTL_IMAGE_NT_HEADER_EX_FLAG_NO_RANGE_CHECK 0x00000001

NTSYSAPI
NTSTATUS
NTAPI
RtlImageNtHeaderEx(
    _In_ ULONG Flags,
    _In_ PVOID BaseOfImage,
    _In_ ULONG64 Size,
    _Out_ PIMAGE_NT_HEADERS *OutHeaders
);

NTSYSAPI
PVOID
NTAPI
RtlAddressInSectionTable(
    _In_ PIMAGE_NT_HEADERS NtHeaders,
    _In_ PVOID BaseOfImage,
    _In_ ULONG VirtualAddress
);

NTSYSAPI
PIMAGE_SECTION_HEADER
NTAPI
RtlSectionTableFromVirtualAddress(
    _In_ PIMAGE_NT_HEADERS NtHeaders,
    _In_ PVOID BaseOfImage,
    _In_ ULONG VirtualAddress
);

NTSYSAPI
PVOID
NTAPI
RtlImageDirectoryEntryToData(
    _In_ PVOID BaseOfImage,
    _In_ BOOLEAN MappedAsImage,
    _In_ USHORT DirectoryEntry,
    _Out_ PULONG Size
);

NTSYSAPI
PIMAGE_SECTION_HEADER
NTAPI
RtlImageRvaToSection(
    _In_ PIMAGE_NT_HEADERS NtHeaders,
    _In_ PVOID BaseOfImage,
    _In_ ULONG Rva
);

NTSYSAPI
PVOID
NTAPI
RtlImageRvaToVa(
    _In_ PIMAGE_NT_HEADERS NtHeaders,
    _In_ PVOID BaseOfImage,
    _In_ ULONG Rva,
    _Out_opt_ PIMAGE_SECTION_HEADER *LastRvaSection
);

#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)

// rev
NTSYSAPI
PVOID
NTAPI
RtlFindExportedRoutineByName(
    _In_ PVOID BaseOfImage,
    _In_z_ PCSTR RoutineName
);

// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlGuardCheckLongJumpTarget(
    _In_ PVOID PcValue,
    _In_ BOOL IsFastFail,
    _Out_ PBOOL IsLongJumpTarget
);

#endif

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)
NTSYSAPI
VOID
NTAPI
RtlValidateUserCallTarget(
    _In_ PVOID Address,
    _Out_ PULONG Flags
);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)
// rev
#define IMAGE_FILE_NATIVE_MACHINE_I386  0x1
#define IMAGE_FILE_NATIVE_MACHINE_AMD64 0x2
#define IMAGE_FILE_NATIVE_MACHINE_ARMNT 0x4
#define IMAGE_FILE_NATIVE_MACHINE_ARM64 0x8

// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlGetImageFileMachines(
    _In_ PCWSTR FileName,
    _Out_ PUSHORT FileMachines
    );
#endif

EXTERN_C_END
