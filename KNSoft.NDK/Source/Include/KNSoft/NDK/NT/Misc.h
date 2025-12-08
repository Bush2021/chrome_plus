#pragma once

#include "MinDef.h"

EXTERN_C_START

/* fltKernel.h */

#define FLT_PORT_CONNECT 0x0001
#define FLT_PORT_ALL_ACCESS (FLT_PORT_CONNECT | STANDARD_RIGHTS_ALL)

#if (NTDDI_VERSION >= NTDDI_WIN10_MN)
// rev
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDirectGraphicsCall(
    _In_ ULONG InputBufferLength,
    _In_reads_bytes_opt_(InputBufferLength) PVOID InputBuffer,
    _In_ ULONG OutputBufferLength,
    _Out_writes_bytes_opt_(OutputBufferLength) PVOID OutputBuffer,
    _Out_ PULONG ReturnLength);
#endif

EXTERN_C_END
