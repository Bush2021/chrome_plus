#pragma once

#include "../MinDef.h"
#include "../Ps/PsInfo.h"

EXTERN_C_START

/* Caution: Exported from Wow Ntdll.dll only!!! */

_Must_inspect_result_
__drv_allocatesMem(Mem)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtWow64AllocateVirtualMemory64(
    _In_ HANDLE ProcessHandle,
    _Inout_ _At_(*BaseAddress, _Readable_bytes_(*RegionSize) _Writable_bytes_(*RegionSize) _Post_readable_byte_size_(*RegionSize)) PULONGLONG BaseAddress,
    _In_ ULONGLONG ZeroBits,
    _Inout_ PULONGLONG RegionSize,
    _In_ ULONG AllocationType,
    _In_ ULONG Protect);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtWow64QueryInformationProcess64(
    _In_ HANDLE ProcessHandle,
    _In_ PROCESSINFOCLASS ProcessInformationClass,
    _Out_writes_bytes_to_(ProcessInformationLength, *ReturnLength) PVOID ProcessInformation,
    _In_ ULONG ProcessInformationLength,
    _Out_opt_ PULONG ReturnLength);

/**
 * Reads virtual memory of a 64-bit process from a 32-bit process.
 *
 * \param ProcessHandle A handle to the process whose memory is to be read.
 * \param BaseAddress A pointer to the base address in the specified process from which to read.
 * \param Buffer A pointer to a buffer that receives the contents from the address space of the specified process.
 * \param NumberOfBytesToRead The number of bytes to be read from the specified process.
 * \param NumberOfBytesRead A pointer to a variable that receives the number of bytes transferred into the specified buffer.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtWow64ReadVirtualMemory64(
    _In_ HANDLE ProcessHandle,
    _In_ ULONGLONG BaseAddress,
    _Out_writes_bytes_to_(NumberOfBytesToRead, *NumberOfBytesRead) PVOID Buffer,
    _In_ ULONGLONG NumberOfBytesToRead,
    _Out_opt_ PULONGLONG NumberOfBytesRead);

/**
 * Writes virtual memory to a 64-bit process from a 32-bit process.
 *
 * @param ProcessHandle A handle to the process whose memory is to be written.
 * @param BaseAddress A pointer to the base address in the specified process to which to write.
 * @param Buffer A pointer to the buffer that contains the data to be written to the address space of the specified process.
 * @param NumberOfBytesToWrite The number of bytes to be written to the specified process.
 * @param NumberOfBytesWritten A pointer to a variable that receives the number of bytes transferred into the specified buffer.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtWow64WriteVirtualMemory64(
    _In_ HANDLE ProcessHandle,
    _In_ ULONGLONG BaseAddress,
    _In_reads_bytes_(NumberOfBytesToWrite) PVOID Buffer,
    _In_ ULONGLONG NumberOfBytesToWrite,
    _Out_opt_ PULONGLONG NumberOfBytesWritten);

NTSYSCALLAPI
BOOLEAN
NTAPI
NtWow64IsProcessorFeaturePresent(
    _In_ ULONG ProcessorFeature);

// rev
NTSYSCALLAPI
NTSTATUS
NTAPI
NtWow64GetNativeSystemInformation(
    _In_ SYSTEM_INFORMATION_CLASS SystemInformationClass,
    _In_ PVOID NativeSystemInformation,
    _In_ ULONG InformationLength,
    _Out_opt_ PULONG ReturnLength);

EXTERN_C_END
