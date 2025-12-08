#pragma once

#include "../MinDef.h"

EXTERN_C_START

#pragma region Critical Section

/**
 * The RtlInitializeCriticalSection routine initializes a critical section object.
 *
 * \param CriticalSection A pointer to the critical section object.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-initializecriticalsection
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlInitializeCriticalSection(
    _Out_ PRTL_CRITICAL_SECTION CriticalSection
);

/**
 * The RtlInitializeCriticalSectionAndSpinCount routine initializes a critical section object and sets the spin count for the critical section.
 *
 * \param CriticalSection A pointer to the critical section object.
 * \param SpinCount The spin count for the critical section object. On single-processor systems, the spin count is ignored.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-initializecriticalsectionandspincount
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlInitializeCriticalSectionAndSpinCount(
    _Out_ PRTL_CRITICAL_SECTION CriticalSection,
    _In_ ULONG SpinCount
);

/**
 * The RtlInitializeCriticalSectionEx routine initializes a critical section object and sets the spin count for the critical section with flags.
 *
 * \param CriticalSection A pointer to the critical section object.
 * \param SpinCount The spin count for the critical section object. On single-processor systems, the spin count is ignored.
 * \param Flags This parameter can be 0 or the CRITICAL_SECTION_NO_DEBUG_INFO flag.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-initializecriticalsectionex
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlInitializeCriticalSectionEx(
    _Out_ PRTL_CRITICAL_SECTION CriticalSection,
    _In_ ULONG SpinCount,
    _In_ ULONG Flags
);

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteCriticalSection(
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection
);

_Acquires_exclusive_lock_(*CriticalSection)
NTSYSAPI
NTSTATUS
NTAPI
RtlEnterCriticalSection(
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection
);

_Releases_exclusive_lock_(*CriticalSection)
NTSYSAPI
NTSTATUS
NTAPI
RtlLeaveCriticalSection(
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection
);

_When_(return != 0, _Acquires_exclusive_lock_(*CriticalSection))
NTSYSAPI
LOGICAL
NTAPI
RtlTryEnterCriticalSection(
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection
);

NTSYSAPI
LOGICAL
NTAPI
RtlIsCriticalSectionLocked(
    _In_ PRTL_CRITICAL_SECTION CriticalSection
);

NTSYSAPI
LOGICAL
NTAPI
RtlIsCriticalSectionLockedByThread(
    _In_ PRTL_CRITICAL_SECTION CriticalSection
);

NTSYSAPI
ULONG
NTAPI
RtlGetCriticalSectionRecursionCount(
    _In_ PRTL_CRITICAL_SECTION CriticalSection
);

NTSYSAPI
ULONG
NTAPI
RtlSetCriticalSectionSpinCount(
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection,
    _In_ ULONG SpinCount
);

NTSYSAPI
HANDLE
NTAPI
RtlQueryCriticalSectionOwner(
    _In_ HANDLE EventHandle,
    _In_ BOOLEAN ExactMatchOwnerAddress
);

NTSYSAPI
VOID
NTAPI
RtlCheckForOrphanedCriticalSections(
    _In_ HANDLE ThreadHandle
);

/**
 * Enables the creation of early critical section events.
 *
 * This function allows the system to create critical section events early in the process
 * initialization. It is typically used to ensure that critical sections are properly
 * initialized and can be used safely during the early stages of process startup.
 * @remarks This function sets the FLG_CRITSEC_EVENT_CREATION flag in the PEB flags field.
 * @return A pointer to the Process Environment Block (PEB).
 */
NTSYSAPI
PPEB
NTAPI
RtlEnableEarlyCriticalSectionEventCreation(VOID);

#pragma region RTL_CRITICAL_SECTION[_DEBUG](64/32)

typedef struct _RTL_CRITICAL_SECTION64 RTL_CRITICAL_SECTION64, *PRTL_CRITICAL_SECTION64;
typedef struct _RTL_CRITICAL_SECTION32 RTL_CRITICAL_SECTION32, *PRTL_CRITICAL_SECTION32;

typedef struct _RTL_CRITICAL_SECTION_DEBUG64
{
    WORD Type;
    WORD CreatorBackTraceIndex;
    RTL_CRITICAL_SECTION64* POINTER_64 CriticalSection;
    LIST_ENTRY64 ProcessLocksList;
    DWORD EntryCount;
    DWORD ContentionCount;
    DWORD Flags;
    WORD CreatorBackTraceIndexHigh;
    WORD Identifier;
} RTL_CRITICAL_SECTION_DEBUG64, *PRTL_CRITICAL_SECTION_DEBUG64, RTL_RESOURCE_DEBUG64, *PRTL_RESOURCE_DEBUG64;

typedef struct _RTL_CRITICAL_SECTION_DEBUG32
{
    WORD Type;
    WORD CreatorBackTraceIndex;
    RTL_CRITICAL_SECTION32* POINTER_32 CriticalSection;
    LIST_ENTRY32 ProcessLocksList;
    DWORD EntryCount;
    DWORD ContentionCount;
    DWORD Flags;
    WORD CreatorBackTraceIndexHigh;
    WORD Identifier;
} RTL_CRITICAL_SECTION_DEBUG32, *PRTL_CRITICAL_SECTION_DEBUG32, RTL_RESOURCE_DEBUG32, *PRTL_RESOURCE_DEBUG32;

struct _RTL_CRITICAL_SECTION64
{
    RTL_CRITICAL_SECTION_DEBUG64* POINTER_64 DebugInfo;
    LONG LockCount;
    LONG RecursionCount;
    VOID* POINTER_64 OwningThread;
    VOID* POINTER_64 LockSemaphore;
    ULONGLONG SpinCount;
};

struct _RTL_CRITICAL_SECTION32
{
    RTL_CRITICAL_SECTION_DEBUG32* POINTER_32 DebugInfo;
    LONG LockCount;
    LONG RecursionCount;
    VOID* POINTER_32 OwningThread;
    VOID* POINTER_32 LockSemaphore;
    ULONG SpinCount;
};

#pragma endregion

#pragma endregion phnt & KNSoft.NDK

#pragma region Run Once

typedef
_Function_class_(RTL_RUN_ONCE_INIT_FN)
_IRQL_requires_same_
LOGICAL
NTAPI
RTL_RUN_ONCE_INIT_FN(
    _Inout_ PRTL_RUN_ONCE RunOnce,
    _Inout_opt_ PVOID Parameter,
    _Inout_opt_ PVOID *Context);
typedef RTL_RUN_ONCE_INIT_FN *PRTL_RUN_ONCE_INIT_FN;

NTSYSAPI
VOID
NTAPI
RtlRunOnceInitialize(
    _Out_ PRTL_RUN_ONCE RunOnce
);

_Maybe_raises_SEH_exception_
NTSYSAPI
NTSTATUS
NTAPI
RtlRunOnceExecuteOnce(
    _Inout_ PRTL_RUN_ONCE RunOnce,
    _In_ __callback PRTL_RUN_ONCE_INIT_FN InitFn,
    _Inout_opt_ PVOID Parameter,
    _Outptr_opt_result_maybenull_ PVOID* Context
);

_Must_inspect_result_
NTSYSAPI
NTSTATUS
NTAPI
RtlRunOnceBeginInitialize(
    _Inout_ PRTL_RUN_ONCE RunOnce,
    _In_ ULONG Flags,
    _Outptr_opt_result_maybenull_ PVOID* Context
);

NTSYSAPI
NTSTATUS
NTAPI
RtlRunOnceComplete(
    _Inout_ PRTL_RUN_ONCE RunOnce,
    _In_ ULONG Flags,
    _In_opt_ PVOID Context
);

#pragma endregion ntddk.h

#pragma region Resource

typedef struct _RTL_RESOURCE
{
    RTL_CRITICAL_SECTION CriticalSection;

    HANDLE SharedSemaphore;
    volatile ULONG NumberOfWaitingShared;
    HANDLE ExclusiveSemaphore;
    volatile ULONG NumberOfWaitingExclusive;

    volatile LONG NumberOfActive; // negative: exclusive acquire; zero: not acquired; positive: shared acquire(s)
    HANDLE ExclusiveOwnerThread;

    ULONG Flags; // RTL_RESOURCE_FLAG_*

    PRTL_RESOURCE_DEBUG DebugInfo;
} RTL_RESOURCE, *PRTL_RESOURCE;

#define RTL_RESOURCE_FLAG_LONG_TERM ((ULONG)0x00000001)

NTSYSAPI
VOID
NTAPI
RtlInitializeResource(
    _Out_ PRTL_RESOURCE Resource
);

NTSYSAPI
VOID
NTAPI
RtlDeleteResource(
    _Inout_ PRTL_RESOURCE Resource
);

NTSYSAPI
BOOLEAN
NTAPI
RtlAcquireResourceShared(
    _Inout_ PRTL_RESOURCE Resource,
    _In_ BOOLEAN Wait
);

NTSYSAPI
BOOLEAN
NTAPI
RtlAcquireResourceExclusive(
    _Inout_ PRTL_RESOURCE Resource,
    _In_ BOOLEAN Wait
);

NTSYSAPI
VOID
NTAPI
RtlReleaseResource(
    _Inout_ PRTL_RESOURCE Resource
);

NTSYSAPI
VOID
NTAPI
RtlConvertSharedToExclusive(
    _Inout_ PRTL_RESOURCE Resource
);

NTSYSAPI
VOID
NTAPI
RtlConvertExclusiveToShared(
    _Inout_ PRTL_RESOURCE Resource
);

NTSYSAPI
ULONG
NTAPI
RtlDumpResource(
    _Inout_ PRTL_RESOURCE Resource
);

#pragma endregion phnt

#pragma region SRW Lock

// winbase:InitializeSRWLock
/**
 * The RtlInitializeSRWLock routine initializes a slim reader/writer (SRW) lock.
 * 
 * \param SRWLock A pointer to the SRW lock.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-initializesrwlock
 */
NTSYSAPI
VOID
NTAPI
RtlInitializeSRWLock(
    _Out_ PRTL_SRWLOCK SRWLock
);

// winbase:AcquireSRWLockExclusive
_Acquires_exclusive_lock_(*SRWLock)
NTSYSAPI
VOID
NTAPI
RtlAcquireSRWLockExclusive(
    _Inout_ PRTL_SRWLOCK SRWLock
);

// winbase:AcquireSRWLockShared
_Acquires_shared_lock_(*SRWLock)
NTSYSAPI
VOID
NTAPI
RtlAcquireSRWLockShared(
    _Inout_ PRTL_SRWLOCK SRWLock
);

// winbase:ReleaseSRWLockExclusive
_Releases_exclusive_lock_(*SRWLock)
NTSYSAPI
VOID
NTAPI
RtlReleaseSRWLockExclusive(
    _Inout_ PRTL_SRWLOCK SRWLock
);

// winbase:ReleaseSRWLockShared
_Releases_shared_lock_(*SRWLock)
NTSYSAPI
VOID
NTAPI
RtlReleaseSRWLockShared(
    _Inout_ PRTL_SRWLOCK SRWLock
);

// winbase:TryAcquireSRWLockExclusive
_When_(return != 0, _Acquires_exclusive_lock_(*SRWLock))
NTSYSAPI
BOOLEAN
NTAPI
RtlTryAcquireSRWLockExclusive(
    _Inout_ PRTL_SRWLOCK SRWLock
);

// winbase:TryAcquireSRWLockShared
_When_(return != 0, _Acquires_shared_lock_(*SRWLock))
NTSYSAPI
BOOLEAN
NTAPI
RtlTryAcquireSRWLockShared(
    _Inout_ PRTL_SRWLOCK SRWLock
);

// rev
NTSYSAPI
VOID
NTAPI
RtlAcquireReleaseSRWLockExclusive(
    _Inout_ PRTL_SRWLOCK SRWLock
);

#if (NTDDI_VERSION >= NTDDI_WIN10)
// rev
NTSYSAPI
BOOLEAN
NTAPI
RtlConvertSRWLockExclusiveToShared(
    _Inout_ PRTL_SRWLOCK SRWLock
);
#endif

#pragma endregion phnt

#pragma region Condition Variable

// winbase:InitializeConditionVariable
NTSYSAPI
VOID
NTAPI
RtlInitializeConditionVariable(
    _Out_ PRTL_CONDITION_VARIABLE ConditionVariable
);

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlSleepConditionVariableCS(
    _Inout_ PRTL_CONDITION_VARIABLE ConditionVariable,
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection,
    _In_opt_ PLARGE_INTEGER Timeout
);

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlSleepConditionVariableSRW(
    _Inout_ PRTL_CONDITION_VARIABLE ConditionVariable,
    _Inout_ PRTL_SRWLOCK SRWLock,
    _In_opt_ PLARGE_INTEGER Timeout,
    _In_ ULONG Flags
);

// winbase:WakeConditionVariable
NTSYSAPI
VOID
NTAPI
RtlWakeConditionVariable(
    _Inout_ PRTL_CONDITION_VARIABLE ConditionVariable
);

// winbase:WakeAllConditionVariable
NTSYSAPI
VOID
NTAPI
RtlWakeAllConditionVariable(
    _Inout_ PRTL_CONDITION_VARIABLE ConditionVariable
);

#pragma endregion phnt

#pragma region Barrier

#define RTL_BARRIER_FLAGS_SPIN_ONLY     0x00000001 // never block on event - always spin
#define RTL_BARRIER_FLAGS_BLOCK_ONLY    0x00000002 // always block on event - never spin
#define RTL_BARRIER_FLAGS_NO_DELETE     0x00000004 // use if barrier will never be deleted

NTSYSAPI
NTSTATUS
NTAPI
RtlInitBarrier(
    _Out_ PRTL_BARRIER Barrier,
    _In_ ULONG TotalThreads,
    _In_ ULONG SpinCount
);

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteBarrier(
    _In_ PRTL_BARRIER Barrier
);

NTSYSAPI
BOOLEAN
NTAPI
RtlBarrier(
    _Inout_ PRTL_BARRIER Barrier,
    _In_ ULONG Flags
);

NTSYSAPI
BOOLEAN
NTAPI
RtlBarrierForDelete(
    _Inout_ PRTL_BARRIER Barrier,
    _In_ ULONG Flags
);

#pragma endregion phnt

#pragma region Wait on Address

#if (NTDDI_VERSION >= NTDDI_WIN8)

NTSYSAPI
NTSTATUS
NTAPI
RtlWaitOnAddress(
    _In_reads_bytes_(AddressSize) volatile VOID * Address,
    _In_reads_bytes_(AddressSize) PVOID CompareAddress,
    _In_ SIZE_T AddressSize,
    _In_opt_ PLARGE_INTEGER Timeout
);

NTSYSAPI
VOID
NTAPI
RtlWakeAddressAll(
    _In_ PVOID Address
);

NTSYSAPI
VOID
NTAPI
RtlWakeAddressAllNoFence(
    _In_ PVOID Address
);

NTSYSAPI
VOID
NTAPI
RtlWakeAddressSingle(
    _In_ PVOID Address
);

NTSYSAPI
VOID
NTAPI
RtlWakeAddressSingleNoFence(
    _In_ PVOID Address
);

#endif

#pragma endregion phnt

#pragma region RCU

//
// Read-Copy Update. 
//
// RCU synchronization allows concurrent access to shared data structures, 
// such as linked lists, trees, or hash tables, without using traditional locking methods 
// in scenarios where read operations are frequent and need to be fast.
// @remarks RCU synchronization is not for general-purpose synchronization.
// Teb->Rcu is used to store the RCU state.

NTSYSAPI
PVOID
NTAPI
RtlRcuAllocate(
    _In_ SIZE_T Size
);

NTSYSAPI
LOGICAL
NTAPI
RtlRcuFree(
    _In_ PULONG Rcu
);

NTSYSAPI
VOID
NTAPI
RtlRcuReadLock(
    _Inout_ PRTL_SRWLOCK SRWLock,
    _Out_ PULONG Rcu
);

NTSYSAPI
VOID
NTAPI
RtlRcuReadUnlock(
    _Inout_ PRTL_SRWLOCK SRWLock,
    _Inout_ PULONG * Rcu
);

NTSYSAPI
LONG
NTAPI
RtlRcuSynchronize(
    _Inout_ PRTL_SRWLOCK SRWLock
);

#pragma endregion phnt

EXTERN_C_END
