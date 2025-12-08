#pragma once

#include "../../NT/MinDef.h"

EXTERN_C_START

/* See also: RtlGenRandom */
BOOLEAN
__stdcall
SystemFunction036(
    _Out_writes_bytes_(RandomBufferLength) PVOID RandomBuffer,
    _In_ ULONG RandomBufferLength
    );

/* See also: RtlEncryptMemory */
NTSTATUS
__stdcall
SystemFunction040(
    _Inout_updates_bytes_(MemorySize) PVOID Memory,
    _In_ ULONG MemorySize,
    _In_ ULONG OptionFlags
    );

/* See also: RtlDecryptMemory */
NTSTATUS
__stdcall
SystemFunction041(
    _Inout_updates_bytes_(MemorySize) PVOID Memory,
    _In_ ULONG MemorySize,
    _In_ ULONG OptionFlags
    );

EXTERN_C_END
