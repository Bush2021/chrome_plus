#pragma once

#include "NT.h"

EXTERN_C_START

#if (NTDDI_VERSION >= NTDDI_WIN8)

NTSYSCALLAPI
NTSTATUS
NTAPI
ZwWaitForWorkViaWorkerFactory(
    _In_ HANDLE WorkerFactoryHandle,
    _Out_writes_to_(Count, *PacketsReturned) PFILE_IO_COMPLETION_INFORMATION MiniPackets,
    _In_ ULONG Count,
    _Out_ PULONG PacketsReturned,
    _In_ PWORKER_FACTORY_DEFERRED_WORK DeferredWork
    );

#else

NTSYSCALLAPI
NTSTATUS
NTAPI
ZwWaitForWorkViaWorkerFactory(
    _In_ HANDLE WorkerFactoryHandle,
    _Out_ PFILE_IO_COMPLETION_INFORMATION MiniPacket
    );

#endif

EXTERN_C_END
