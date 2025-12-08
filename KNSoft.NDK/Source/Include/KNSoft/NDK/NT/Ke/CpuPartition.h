#pragma once

#include "../MinDef.h"

EXTERN_C_START

/* phnt */

#if (NTDDI_VERSION >= NTDDI_WIN11_GA)

// rev
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenCpuPartition(
    _Out_ PHANDLE CpuPartitionHandle, 
    _In_ ACCESS_MASK DesiredAccess, 
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes);

// rev
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateCpuPartition(
    _Out_ PHANDLE CpuPartitionHandle, 
    _In_ ACCESS_MASK DesiredAccess, 
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes);

// rev
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationCpuPartition(
    _In_ HANDLE CpuPartitionHandle, 
    _In_ ULONG CpuPartitionInformationClass, 
    _In_reads_bytes_(CpuPartitionInformationLength) PVOID CpuPartitionInformation, 
    _In_ ULONG CpuPartitionInformationLength, 
    _Reserved_ PVOID, 
    _Reserved_ ULONG, 
    _Reserved_ ULONG);

// rev
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationCpuPartition(
    _In_ HANDLE CpuPartitionHandle,
    _In_ ULONG CpuPartitionInformationClass,
    _Out_writes_bytes_opt_(CpuPartitionInformationLength) PVOID CpuPartitionInformation,
    _In_ ULONG CpuPartitionInformationLength,
    _Out_opt_ PULONG ReturnLength);

#endif

EXTERN_C_END
