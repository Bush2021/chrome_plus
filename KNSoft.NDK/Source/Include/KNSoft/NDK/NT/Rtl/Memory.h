#pragma once

#include "../MinDef.h"

EXTERN_C_START

#pragma region Non-Temporal

#define RtlCopyBytes RtlCopyMemory
#define RtlZeroBytes RtlZeroMemory
#define RtlFillBytes RtlFillMemory

#if defined(_M_AMD64) || defined(_M_ARM64)

NTSYSAPI
VOID
NTAPI
RtlCopyMemoryNonTemporal(
   _Out_writes_bytes_all_(Length) VOID UNALIGNED *Destination,
   _In_reads_bytes_(Length) CONST VOID UNALIGNED *Source,
   _In_ SIZE_T Length);

NTSYSAPI
VOID
NTAPI
RtlFillMemoryNonTemporal(
   _Out_writes_bytes_all_(Length) VOID UNALIGNED *Destination,
   _In_ SIZE_T Length,
   _In_ CONST UCHAR Value);

#else

#define RtlCopyMemoryNonTemporal RtlCopyMemory
#define RtlFillMemoryNonTemporal RtlFillMemory

#endif

#pragma endregion wdm.h

#pragma region Memory

_Must_inspect_result_
NTSYSAPI
SIZE_T
NTAPI
RtlCompareMemoryUlong(
    _In_reads_bytes_(Length) PVOID Source,
    _In_ SIZE_T Length,
    _In_ ULONG Pattern);

#if defined(_M_AMD64) && !defined(_M_ARM64EC)

FORCEINLINE
VOID
RtlFillMemoryUlong(
    _Out_writes_bytes_all_(Length) PVOID Destination,
    _In_ SIZE_T Length,
    _In_ ULONG Pattern)
{

    PULONG Address = (PULONG)Destination;

    //
    // If the number of DWORDs is not zero, then fill the specified buffer
    // with the specified pattern.
    //
    if ((Length /= 4) != 0)
    {

        //
        // If the destination is not quadword aligned (ignoring low bits),
        // then align the destination by storing one DWORD.
        //
        if (((ULONG64)Address & 4) != 0)
        {
            *Address = Pattern;
            if ((Length -= 1) == 0)
            {
                return;
            }

            Address += 1;
        }

        //
        // If the number of QWORDs is not zero, then fill the destination
        // buffer a QWORD at a time.
        //
        __stosq((PULONG64)(Address),
                Pattern | ((ULONG64)Pattern << 32),
                Length / 2);

        if ((Length & 1) != 0)
        {
            Address[Length - 1] = Pattern;
        }
    }

    return;
}

#define RtlFillMemoryUlonglong(Destination, Length, Pattern)                \
    __stosq((PULONG64)(Destination), Pattern, (Length) / 8)

#else

NTSYSAPI
VOID
NTAPI
RtlFillMemoryUlong(
    _Out_writes_bytes_all_(Length) PVOID Destination,
    _In_ SIZE_T Length,
    _In_ ULONG Pattern);

NTSYSAPI
VOID
NTAPI
RtlFillMemoryUlonglong(
   _Out_writes_bytes_all_(Length) PVOID Destination,
   _In_ SIZE_T Length,
   _In_ ULONGLONG Pattern);

#endif // defined(_M_AMD64) && !defined(_M_ARM64EC)

#pragma endregion ntifs.h

#pragma region Memory Cache

_Function_class_(RTL_SECURE_MEMORY_CACHE_CALLBACK)
typedef NTSTATUS(NTAPI RTL_SECURE_MEMORY_CACHE_CALLBACK)(
    _In_ PVOID Address,
    _In_ SIZE_T Length
    );
typedef RTL_SECURE_MEMORY_CACHE_CALLBACK *PRTL_SECURE_MEMORY_CACHE_CALLBACK;

NTSYSAPI
NTSTATUS
NTAPI
RtlRegisterSecureMemoryCacheCallback(
    _In_ PRTL_SECURE_MEMORY_CACHE_CALLBACK Callback
);

NTSYSAPI
NTSTATUS
NTAPI
RtlDeregisterSecureMemoryCacheCallback(
    _In_ PRTL_SECURE_MEMORY_CACHE_CALLBACK Callback
);

// ros
NTSYSAPI
BOOLEAN
NTAPI
RtlFlushSecureMemoryCache(
    _In_ PVOID MemoryCache,
    _In_opt_ SIZE_T MemoryLength
);

#pragma endregion phnt

#pragma region Memory Zones

typedef struct _RTL_MEMORY_ZONE_SEGMENT
{
    struct _RTL_MEMORY_ZONE_SEGMENT *NextSegment;
    SIZE_T Size;
    PVOID Next;
    PVOID Limit;
} RTL_MEMORY_ZONE_SEGMENT, *PRTL_MEMORY_ZONE_SEGMENT;

typedef struct _RTL_MEMORY_ZONE
{
    RTL_MEMORY_ZONE_SEGMENT Segment;
    RTL_SRWLOCK Lock;
    ULONG LockCount;
    PRTL_MEMORY_ZONE_SEGMENT FirstSegment;
} RTL_MEMORY_ZONE, *PRTL_MEMORY_ZONE;

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateMemoryZone(
    _Out_ PVOID *MemoryZone,
    _In_ SIZE_T InitialSize,
    _Reserved_ ULONG Flags
);

NTSYSAPI
NTSTATUS
NTAPI
RtlDestroyMemoryZone(
    _In_ _Post_invalid_ PVOID MemoryZone
);

NTSYSAPI
NTSTATUS
NTAPI
RtlAllocateMemoryZone(
    _In_ PVOID MemoryZone,
    _In_ SIZE_T BlockSize,
    _Out_ PVOID *Block
);

NTSYSAPI
NTSTATUS
NTAPI
RtlResetMemoryZone(
    _In_ PVOID MemoryZone
);

NTSYSAPI
NTSTATUS
NTAPI
RtlLockMemoryZone(
    _In_ PVOID MemoryZone
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUnlockMemoryZone(
    _In_ PVOID MemoryZone
);

#pragma endregion phnt

#pragma region Memory Block Lookaside List

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateMemoryBlockLookaside(
    _Out_ PVOID *MemoryBlockLookaside,
    _Reserved_ ULONG Flags,
    _In_ ULONG InitialSize,
    _In_ ULONG MinimumBlockSize,
    _In_ ULONG MaximumBlockSize
);

NTSYSAPI
NTSTATUS
NTAPI
RtlDestroyMemoryBlockLookaside(
    _In_ PVOID MemoryBlockLookaside
);

NTSYSAPI
NTSTATUS
NTAPI
RtlAllocateMemoryBlockLookaside(
    _In_ PVOID MemoryBlockLookaside,
    _In_ ULONG BlockSize,
    _Out_ PVOID *Block
);

NTSYSAPI
NTSTATUS
NTAPI
RtlFreeMemoryBlockLookaside(
    _In_ PVOID MemoryBlockLookaside,
    _In_ PVOID Block
);

NTSYSAPI
NTSTATUS
NTAPI
RtlExtendMemoryBlockLookaside(
    _In_ PVOID MemoryBlockLookaside,
    _In_ ULONG Increment
);

NTSYSAPI
NTSTATUS
NTAPI
RtlResetMemoryBlockLookaside(
    _In_ PVOID MemoryBlockLookaside
);

NTSYSAPI
NTSTATUS
NTAPI
RtlLockMemoryBlockLookaside(
    _In_ PVOID MemoryBlockLookaside
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUnlockMemoryBlockLookaside(
    _In_ PVOID MemoryBlockLookaside
);

#pragma endregion phnt

/* phnt */
#if (NTDDI_VERSION >= NTDDI_WIN10_VB)
NTSYSAPI
BOOLEAN
NTAPI
RtlIsZeroMemory(
    _In_ PVOID Buffer,
    _In_ SIZE_T Length);
#endif

NTSYSAPI
NTSTATUS
NTAPI
RtlCopyMappedMemory(
    _Out_writes_bytes_all_(Length) PVOID Destination,
    _In_reads_bytes_(Length) PVOID Source,
    _In_ SIZE_T Length);

EXTERN_C_END
