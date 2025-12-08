#pragma once

#include "../MinDef.h"

EXTERN_C_START

/* phnt */

typedef USHORT RTL_ATOM, *PRTL_ATOM;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAddAtom(
    _In_reads_bytes_opt_(Length) PCWSTR AtomName,
    _In_ ULONG Length,
    _Out_opt_ PRTL_ATOM Atom);

#if (NTDDI_VERSION >= NTDDI_WIN8)

#define ATOM_FLAG_GLOBAL 0x2

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAddAtomEx(
    _In_reads_bytes_opt_(Length) PCWSTR AtomName,
    _In_ ULONG Length,
    _Out_opt_ PRTL_ATOM Atom,
    _In_ ULONG Flags);

#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtFindAtom(
    _In_reads_bytes_opt_(Length) PCWSTR AtomName,
    _In_ ULONG Length,
    _Out_opt_ PRTL_ATOM Atom);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteAtom(
    _In_ RTL_ATOM Atom);

typedef enum _ATOM_INFORMATION_CLASS
{
    AtomBasicInformation,
    AtomTableInformation
} ATOM_INFORMATION_CLASS;

typedef struct _ATOM_BASIC_INFORMATION
{
    USHORT UsageCount;
    USHORT Flags;
    USHORT NameLength;
    _Field_size_bytes_(NameLength) WCHAR Name[ANYSIZE_ARRAY];
} ATOM_BASIC_INFORMATION, *PATOM_BASIC_INFORMATION;

typedef struct _ATOM_TABLE_INFORMATION
{
    ULONG NumberOfAtoms;
    _Field_size_(NumberOfAtoms) RTL_ATOM Atoms[ANYSIZE_ARRAY];
} ATOM_TABLE_INFORMATION, *PATOM_TABLE_INFORMATION;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationAtom(
    _In_ RTL_ATOM Atom,
    _In_ ATOM_INFORMATION_CLASS AtomInformationClass,
    _Out_writes_bytes_(AtomInformationLength) PVOID AtomInformation,
    _In_ ULONG AtomInformationLength,
    _Out_opt_ PULONG ReturnLength);

EXTERN_C_END
