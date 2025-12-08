#pragma once

#include "../MinDef.h"
#include "Basic.h"
#include "JobInfo.h"
#include "PsInfo.h"

EXTERN_C_START

/* phnt */

#if !defined(_KERNEL_MODE)
#ifndef PROCESS_SET_PORT
#define PROCESS_SET_PORT 0x0800
#endif
#endif

#if !defined(_KERNEL_MODE)
#ifndef THREAD_ALERT
#define THREAD_ALERT 0x0004
#endif
#endif

#ifndef RTL_FLS_MAXIMUM_AVAILABLE
#define RTL_FLS_MAXIMUM_AVAILABLE 128
#endif

#ifndef TLS_EXPANSION_SLOTS
#define TLS_EXPANSION_SLOTS 1024
#endif

#if defined(_ARM64_)

#define CONTEXT_ARM   0x00200000L

#define CONTEXT_ARM_CONTROL (CONTEXT_ARM | 0x1L)
#define CONTEXT_ARM_INTEGER (CONTEXT_ARM | 0x2L)
#define CONTEXT_ARM_FLOATING_POINT  (CONTEXT_ARM | 0x4L)
#define CONTEXT_ARM_DEBUG_REGISTERS (CONTEXT_ARM | 0x8L)
#define CONTEXT_ARM_FULL (CONTEXT_ARM_CONTROL | CONTEXT_ARM_INTEGER | CONTEXT_ARM_FLOATING_POINT)
#define CONTEXT_ARM_ALL (CONTEXT_ARM_CONTROL | CONTEXT_ARM_INTEGER | CONTEXT_ARM_FLOATING_POINT | CONTEXT_ARM_DEBUG_REGISTERS)

#define ARM_MAX_BREAKPOINTS     8
#define ARM_MAX_WATCHPOINTS     1

typedef struct _ARM_NT_NEON128 {
    ULONGLONG Low;
    LONGLONG High;
} ARM_NT_NEON128, *PARM_NT_NEON128;

typedef struct DECLSPEC_ALIGN(8) DECLSPEC_NOINITALL _ARM_NT_CONTEXT {

    //
    // Control flags.
    //

    DWORD ContextFlags;

    //
    // Integer registers
    //

    DWORD R0;
    DWORD R1;
    DWORD R2;
    DWORD R3;
    DWORD R4;
    DWORD R5;
    DWORD R6;
    DWORD R7;
    DWORD R8;
    DWORD R9;
    DWORD R10;
    DWORD R11;
    DWORD R12;

    //
    // Control Registers
    //

    DWORD Sp;
    DWORD Lr;
    DWORD Pc;
    DWORD Cpsr;

    //
    // Floating Point/NEON Registers
    //

    DWORD Fpscr;
    DWORD Padding;
    union {
        ARM_NT_NEON128 Q[16];
        ULONGLONG D[32];
        DWORD S[32];
    } DUMMYUNIONNAME;

    //
    // Debug registers
    //

    DWORD Bvr[ARM_MAX_BREAKPOINTS];
    DWORD Bcr[ARM_MAX_BREAKPOINTS];
    DWORD Wvr[ARM_MAX_WATCHPOINTS];
    DWORD Wcr[ARM_MAX_WATCHPOINTS];

    DWORD Padding2[2];

} ARM_NT_CONTEXT, *PARM_NT_CONTEXT;

#endif

// private
typedef struct _THREAD_INDEX_INFORMATION
{
    ULONG Index;
    ULONG Sequence;
} THREAD_INDEX_INFORMATION, *PTHREAD_INDEX_INFORMATION;

// Processes

#if !defined(_KERNEL_MODE)

/**
 * Creates a new process.
 *
 * @param ProcessHandle A pointer to a handle that receives the process object handle.
 * @param DesiredAccess The access rights desired for the process object.
 * @param ObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new process.
 * @param ParentProcess A handle to the parent process.
 * @param InheritObjectTable If TRUE, the new process inherits the object table of the parent process.
 * @param SectionHandle Optional. A handle to a section object to be used for the new process.
 * @param DebugPort Optional. A handle to a debug port to be used for the new process.
 * @param TokenHandle Optional. A handle to an access token to be used for the new process.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateProcess(
    _Out_ PHANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE ParentProcess,
    _In_ BOOLEAN InheritObjectTable,
    _In_opt_ HANDLE SectionHandle,
    _In_opt_ HANDLE DebugPort,
    _In_opt_ HANDLE TokenHandle
    );

// begin_rev
#define PROCESS_CREATE_FLAGS_NONE 0x00000000
#define PROCESS_CREATE_FLAGS_BREAKAWAY 0x00000001                               // NtCreateProcessEx & NtCreateUserProcess
#define PROCESS_CREATE_FLAGS_NO_DEBUG_INHERIT 0x00000002                        // NtCreateProcessEx & NtCreateUserProcess
#define PROCESS_CREATE_FLAGS_INHERIT_HANDLES 0x00000004                         // NtCreateProcessEx & NtCreateUserProcess
#define PROCESS_CREATE_FLAGS_OVERRIDE_ADDRESS_SPACE 0x00000008                  // NtCreateProcessEx only
#define PROCESS_CREATE_FLAGS_LARGE_PAGES 0x00000010                             // NtCreateProcessEx only (requires SeLockMemoryPrivilege)
#define PROCESS_CREATE_FLAGS_LARGE_PAGE_SYSTEM_DLL 0x00000020                   // NtCreateProcessEx only (requires SeLockMemoryPrivilege)
#define PROCESS_CREATE_FLAGS_PROTECTED_PROCESS 0x00000040                       // NtCreateUserProcess only
#define PROCESS_CREATE_FLAGS_CREATE_SESSION 0x00000080                          // NtCreateProcessEx & NtCreateUserProcess (requires SeLoadDriverPrivilege)
#define PROCESS_CREATE_FLAGS_INHERIT_FROM_PARENT 0x00000100                     // NtCreateProcessEx & NtCreateUserProcess
#define PROCESS_CREATE_FLAGS_CREATE_SUSPENDED 0x00000200                        // NtCreateProcessEx & NtCreateUserProcess
#define PROCESS_CREATE_FLAGS_FORCE_BREAKAWAY 0x00000400                         // NtCreateProcessEx & NtCreateUserProcess (requires SeTcbPrivilege)
#define PROCESS_CREATE_FLAGS_MINIMAL_PROCESS 0x00000800                         // NtCreateProcessEx only
#define PROCESS_CREATE_FLAGS_RELEASE_SECTION 0x00001000                         // NtCreateProcessEx & NtCreateUserProcess
#define PROCESS_CREATE_FLAGS_CLONE_MINIMAL 0x00002000                           // NtCreateProcessEx only
#define PROCESS_CREATE_FLAGS_CLONE_MINIMAL_REDUCED_COMMIT 0x00004000
#define PROCESS_CREATE_FLAGS_AUXILIARY_PROCESS 0x00008000                       // NtCreateProcessEx & NtCreateUserProcess (requires SeTcbPrivilege)
#define PROCESS_CREATE_FLAGS_CREATE_STORE 0x00020000                            // NtCreateProcessEx & NtCreateUserProcess
#define PROCESS_CREATE_FLAGS_USE_PROTECTED_ENVIRONMENT 0x00040000               // NtCreateProcessEx & NtCreateUserProcess
#define PROCESS_CREATE_FLAGS_IMAGE_EXPANSION_MITIGATION_DISABLE 0x00080000
#define PROCESS_CREATE_FLAGS_PARTITION_CREATE_SLAB_IDENTITY 0x00400000          // NtCreateProcessEx & NtCreateUserProcess (requires SeLockMemoryPrivilege)
// end_rev

/**
 * Creates a new process with extended options.
 *
 * @param ProcessHandle A pointer to a handle that receives the process object handle.
 * @param DesiredAccess The access rights desired for the process object.
 * @param ObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new process.
 * @param ParentProcess A handle to the parent process.
 * @param Flags Flags that control the creation of the process. These flags are defined as PROCESS_CREATE_FLAGS_*.
 * @param SectionHandle Optional. A handle to a section object to be used for the new process.
 * @param DebugPort Optional. A handle to a debug port to be used for the new process.
 * @param TokenHandle Optional. A handle to an access token to be used for the new process.
 * @param Reserved Reserved for future use. Must be zero.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateProcessEx(
    _Out_ PHANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE ParentProcess,
    _In_ ULONG Flags, // PROCESS_CREATE_FLAGS_*
    _In_opt_ HANDLE SectionHandle,
    _In_opt_ HANDLE DebugPort,
    _In_opt_ HANDLE TokenHandle,
    _Reserved_ ULONG Reserved // JobMemberLevel
    );

/**
 * Opens an existing process object.
 *
 * @param ProcessHandle A pointer to a handle that receives the process object handle.
 * @param DesiredAccess The access rights desired for the process object.
 * @param ObjectAttributes A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new process.
 * @param ClientId Optional. A pointer to a CLIENT_ID structure that specifies the client ID of the process to be opened.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-ntopenprocess
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenProcess(
    _Out_ PHANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PCLIENT_ID ClientId
    );

/**
 * Terminates the specified process.
 *
 * @param ProcessHandle Optional. A handle to the process to be terminated. If this parameter is NULL, the calling process is terminated.
 * @param ExitStatus The exit status to be used by the process and the process's termination status.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-zwterminateprocess
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtTerminateProcess(
    _In_opt_ HANDLE ProcessHandle,
    _In_ NTSTATUS ExitStatus
    );

/**
 * Suspends the specified process.
 *
 * @param ProcessHandle A handle to the process to be suspended.
 * @return NTSTATUS Successful or errant status.
 * @remarks Use NtCreateProcessStateChange instead.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSuspendProcess(
    _In_ HANDLE ProcessHandle
    );

/**
 * Resumes the specified process.
 *
 * @param ProcessHandle A handle to the process to be resumed.
 * @return NTSTATUS Successful or errant status.
 * @remarks Use NtCreateProcessStateChange instead.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtResumeProcess(
    _In_ HANDLE ProcessHandle
    );

/**
 * Retrieves information about the specified process.
 *
 * @param ProcessHandle A handle to the process.
 * @param ProcessInformationClass The type of process information to be retrieved.
 * @param ProcessInformation A pointer to a buffer that receives the process information.
 * @param ProcessInformationLength The size of the buffer pointed to by the ProcessInformation parameter.
 * @param ReturnLength An optional pointer to a variable that receives the size of the data returned.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationProcess(
    _In_ HANDLE ProcessHandle,
    _In_ PROCESSINFOCLASS ProcessInformationClass,
    _Out_writes_bytes_(ProcessInformationLength) PVOID ProcessInformation,
    _In_ ULONG ProcessInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

/**
 * The PROCESS_GET_NEXT_FLAGS_PREVIOUS_PROCESS flag retrieves the previous process in the system.
 *
 * When calling NtGetNextProcess, this flag can be specified in the Flags parameter to indicate
 * that the function should return the previous process in the system enumeration order,
 * rather than the next process. This can be useful for iterating through processes in reverse order.
 */
#ifndef PROCESS_GET_NEXT_FLAGS_PREVIOUS_PROCESS
#define PROCESS_GET_NEXT_FLAGS_PREVIOUS_PROCESS 0x00000001
#endif

#if (NTDDI_VERSION >= NTDDI_WS03)

/**
 * Retrieves a handle to the next process in the system.
 *
 * @param ProcessHandle An optional handle to a process. If this parameter is NULL, the function retrieves the first process in the system.
 * @param DesiredAccess The access rights desired for the new process handle.
 * @param HandleAttributes The attributes for the new process handle.
 * @param Flags Flags that modify the behavior of the function. This can be a combination of the following flags:
 * - \ref PROCESS_GET_NEXT_FLAGS_PREVIOUS_PROCESS (0x00000001): Retrieve the previous process in the system.
 * @param NewProcessHandle A pointer to a variable that receives the handle to the next process.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtGetNextProcess(
    _In_opt_ HANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ ULONG HandleAttributes,
    _In_ ULONG Flags,
    _Out_ PHANDLE NewProcessHandle
    );
#endif

#if (NTDDI_VERSION >= NTDDI_WS03)
/**
 * Retrieves a handle to the next thread in the system.
 *
 * @param ProcessHandle A handle to the process for enumeration of threads.
 * @param ThreadHandle An optional handle to a thread. If this parameter is NULL, the function retrieves the first thread in the process.
 * @param DesiredAccess The access rights desired for the new thread handle.
 * @param HandleAttributes The attributes for the new thread handle.
 * @param Flags Flags that modify the behavior of the function. Unused and should be zero.
 * @param NewThreadHandle A pointer to a variable that receives the handle to the next thread.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtGetNextThread(
    _In_ HANDLE ProcessHandle,
    _In_opt_ HANDLE ThreadHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ ULONG HandleAttributes,
    _In_opt_ _Reserved_ ULONG Flags,
    _Out_ PHANDLE NewThreadHandle
    );
#endif

/**
 * Sets information for the specified process.
 *
 * @param ProcessHandle A handle to the process.
 * @param ProcessInformationClass The type of process information to be set.
 * @param ProcessInformation A pointer to a buffer that contains the process information.
 * @param ProcessInformationLength The size of the buffer pointed to by the ProcessInformation parameter.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationProcess(
    _In_ HANDLE ProcessHandle,
    _In_ PROCESSINFOCLASS ProcessInformationClass,
    _In_reads_bytes_(ProcessInformationLength) PVOID ProcessInformation,
    _In_ ULONG ProcessInformationLength
    );

#endif

#define STATECHANGE_SET_ATTRIBUTES 0x0001

typedef enum _PROCESS_STATE_CHANGE_TYPE
{
    ProcessStateChangeSuspend,
    ProcessStateChangeResume,
    ProcessStateChangeMax,
} PROCESS_STATE_CHANGE_TYPE, *PPROCESS_STATE_CHANGE_TYPE;

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)

/**
 * Creates a state change handle for changing the suspension state of a process.
 *
 * @param ProcessStateChangeHandle A pointer to a variable that receives the handle.
 * @param DesiredAccess The access rights desired for the handle.
 * @param ObjectAttributes Optional attributes for the handle.
 * @param ProcessHandle A handle to the process.
 * @param Reserved Reserved for future use.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateProcessStateChange(
    _Out_ PHANDLE ProcessStateChangeHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE ProcessHandle,
    _In_opt_ _Reserved_ ULONG Reserved
    );

/**
 * Changes the suspension state of a process.
 *
 * @param ProcessStateChangeHandle A handle to the process state change object.
 * @param ProcessHandle A handle to the process.
 * @param StateChangeType The type of state change.
 * @param ExtendedInformation Optional extended information.
 * @param ExtendedInformationLength The length of the extended information.
 * @param Reserved Reserved for future use.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtChangeProcessState(
    _In_ HANDLE ProcessStateChangeHandle,
    _In_ HANDLE ProcessHandle,
    _In_ PROCESS_STATE_CHANGE_TYPE StateChangeType,
    _In_opt_ _Reserved_ PVOID ExtendedInformation,
    _In_opt_ _Reserved_ SIZE_T ExtendedInformationLength,
    _In_opt_ _Reserved_ ULONG Reserved
    );

#endif

typedef enum _THREAD_STATE_CHANGE_TYPE
{
    ThreadStateChangeSuspend,
    ThreadStateChangeResume,
    ThreadStateChangeMax,
} THREAD_STATE_CHANGE_TYPE, *PTHREAD_STATE_CHANGE_TYPE;

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)

/**
 * Creates a state change handle for changing the suspension state of a thread.
 *
 * @param ThreadStateChangeHandle A pointer to a variable that receives the handle.
 * @param DesiredAccess The access rights desired for the handle.
 * @param ObjectAttributes Optional attributes for the handle.
 * @param ThreadHandle A handle to the thread.
 * @param Reserved Reserved for future use.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateThreadStateChange(
    _Out_ PHANDLE ThreadStateChangeHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE ThreadHandle,
    _In_opt_ _Reserved_ ULONG Reserved
    );

/**
 * Changes the suspension state of a thread.
 *
 * @param ThreadStateChangeHandle A handle to the thread state change object.
 * @param ThreadHandle A handle to the thread.
 * @param StateChangeType The type of state change.
 * @param ExtendedInformation Optional extended information.
 * @param ExtendedInformationLength The length of the extended information.
 * @param Reserved Reserved for future use.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtChangeThreadState(
    _In_ HANDLE ThreadStateChangeHandle,
    _In_ HANDLE ThreadHandle,
    _In_ THREAD_STATE_CHANGE_TYPE StateChangeType,
    _In_opt_ PVOID ExtendedInformation,
    _In_opt_ SIZE_T ExtendedInformationLength,
    _In_opt_ ULONG Reserved
    );

#endif

// Threads

#if !defined(_KERNEL_MODE)

/**
 * Creates a new thread in the specified process.
 *
 * @param ThreadHandle A pointer to a handle that receives the thread object handle.
 * @param DesiredAccess The access rights desired for the thread object.
 * @param ObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new thread.
 * @param ProcessHandle A handle to the process in which the thread is to be created.
 * @param ClientId A pointer to a CLIENT_ID structure that receives the client ID of the new thread.
 * @param ThreadContext A pointer to a CONTEXT structure that specifies the initial context of the new thread.
 * @param InitialTeb A pointer to an INITIAL_TEB structure that specifies the initial stack limits of the new thread.
 * @param CreateSuspended If TRUE, the thread is created in a suspended state.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateThread(
    _Out_ PHANDLE ThreadHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE ProcessHandle,
    _Out_ PCLIENT_ID ClientId,
    _In_ PCONTEXT ThreadContext,
    _In_ PINITIAL_TEB InitialTeb,
    _In_ BOOLEAN CreateSuspended
    );

/**
 * Opens an existing thread object.
 *
 * @param ThreadHandle A pointer to a handle that receives the thread object handle.
 * @param DesiredAccess The access rights desired for the thread object.
 * @param ObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new thread.
 * @param ClientId Optional. A pointer to a CLIENT_ID structure that specifies the client ID of the thread to be opened.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenThread(
    _Out_ PHANDLE ThreadHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PCLIENT_ID ClientId
    );

/**
 * Terminates the specified thread.
 *
 * @param ThreadHandle Optional. A handle to the thread to be terminated. If this parameter is NULL, the calling thread is terminated.
 * @param ExitStatus The exit status to be used by the thread and the thread's termination status.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtTerminateThread(
    _In_opt_ HANDLE ThreadHandle,
    _In_ NTSTATUS ExitStatus
    );

/**
 * Suspends the specified thread.
 *
 * @param ThreadHandle A handle to the thread to be suspended.
 * @param PreviousSuspendCount Optional. A pointer to a variable that receives the thread's previous suspend count.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSuspendThread(
    _In_ HANDLE ThreadHandle,
    _Out_opt_ PULONG PreviousSuspendCount
    );

/**
 * Resumes the specified thread.
 *
 * @param ThreadHandle A handle to the thread to be resumed.
 * @param PreviousSuspendCount Optional. A pointer to a variable that receives the thread's previous suspend count.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtResumeThread(
    _In_ HANDLE ThreadHandle,
    _Out_opt_ PULONG PreviousSuspendCount
    );

/**
 * Retrieves the number of the current processor.
 *
 * \return ULONG The number of the current processor.
 * \sa https://learn.microsoft.com/en-us/windows/win32/procthread/ntgetcurrentprocessornumber
 */
NTSYSCALLAPI
ULONG
NTAPI
NtGetCurrentProcessorNumber(
    VOID
    );

/**
 * Retrieves the number of the current processor.
 *
 * \param ProcessorNumber An optional pointer to a PROCESSOR_NUMBER structure that receives the processor number.
 * \return ULONG The number of the current processor.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-kegetcurrentprocessornumberex
 */
NTSYSCALLAPI
ULONG
NTAPI
NtGetCurrentProcessorNumberEx(
    _Out_opt_ PPROCESSOR_NUMBER ProcessorNumber
    );

/**
 * Retrieves the context of the specified thread.
 *
 * @param ThreadHandle A handle to the thread.
 * @param ThreadContext A pointer to a CONTEXT structure that receives the thread context.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtGetContextThread(
    _In_ HANDLE ThreadHandle,
    _Inout_ PCONTEXT ThreadContext
    );

/**
 * Sets the context of the specified thread.
 *
 * @param ThreadHandle A handle to the thread.
 * @param ThreadContext A pointer to a CONTEXT structure that specifies the thread context.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetContextThread(
    _In_ HANDLE ThreadHandle,
    _In_ PCONTEXT ThreadContext
    );

/**
 * Retrieves information about the specified thread.
 *
 * @param ThreadHandle A handle to the thread.
 * @param ThreadInformationClass The type of thread information to be retrieved.
 * @param ThreadInformation A pointer to a buffer that receives the thread information.
 * @param ThreadInformationLength The size of the buffer pointed to by the ThreadInformation parameter.
 * @param ReturnLength An optional pointer to a variable that receives the size of the data returned.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationThread(
    _In_ HANDLE ThreadHandle,
    _In_ THREADINFOCLASS ThreadInformationClass,
    _Out_writes_bytes_(ThreadInformationLength) PVOID ThreadInformation,
    _In_ ULONG ThreadInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

/**
 * Sets information for the specified thread.
 *
 * @param ThreadHandle A handle to the thread.
 * @param ThreadInformationClass The type of thread information to be set.
 * @param ThreadInformation A pointer to a buffer that contains the thread information.
 * @param ThreadInformationLength The size of the buffer pointed to by the ThreadInformation parameter.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationThread(
    _In_ HANDLE ThreadHandle,
    _In_ THREADINFOCLASS ThreadInformationClass,
    _In_reads_bytes_(ThreadInformationLength) PVOID ThreadInformation,
    _In_ ULONG ThreadInformationLength
    );

/**
 * The NtAlertThread routine alerts the specified thread.
 *
 * \param[in] ThreadHandle A handle to the thread to be alerted.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlertThread(
    _In_ HANDLE ThreadHandle
    );

/**
 * The NtAlertResumeThread routine resumes a specified thread that was previously suspended and alerts the thread.
 *
 * \param[in] ThreadHandle A handle to the thread to be resumed and alerted.
 * \param[out, optional] PreviousSuspendCount An optional pointer to a variable that receives the thread's previous suspend count.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlertResumeThread(
    _In_ HANDLE ThreadHandle,
    _Out_opt_ PULONG PreviousSuspendCount
    );

/**
 * The NtTestAlert routine indicates whether the current thread has an alert pending
 * and executes asynchronous procedure calls (APCs) queued to the current thread.
 *
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtTestAlert(
    VOID
    );

#if (NTDDI_VERSION >= NTDDI_WIN8)
// rev
/**
 * The NtAlertThreadByThreadId routine sends an alert to the specified thread.
 *
 * \param ThreadId The thread ID of the thread to be alerted.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlertThreadByThreadId(
    _In_ HANDLE ThreadId
    );
#endif

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)

/**
 * The NtAlertThreadByThreadIdEx routine sends an alert to the specified thread by its thread ID, with an optional lock.
 *
 * \param ThreadId The thread ID of the thread to be alerted.
 * \param Lock An optional pointer to an SRW lock to be used during the alert.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlertThreadByThreadIdEx(
    _In_ HANDLE ThreadId,
    _In_opt_ PRTL_SRWLOCK Lock
    );

/**
 * The NtAlertMultipleThreadByThreadId routine sends an alert to multiple threads by their thread IDs.
 *
 * \param MultipleThreadId A pointer to an array of thread IDs to be alerted.
 * \param Count The number of thread IDs in the array.
 * \param Boost A pointer to a boost value to be applied to the threads.
 * \param BoostCount The number of boost values in the array.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlertMultipleThreadByThreadId(
    _In_ PHANDLE MultipleThreadId,
    _In_ ULONG Count,
    _In_ PVOID Boost,
    _In_ ULONG BoostCount
    );

#endif

#if (NTDDI_VERSION >= NTDDI_WIN8)
// rev
/**
 * The NtAlertThreadByThreadIdEx routine sWaits for an alert to be delivered to the specified thread.
 *
 * \param Address The address to wait for an alert on.
 * \param Timeout The timeout value for waiting, or NULL for no timeout.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitForAlertByThreadId(
    _In_opt_ PVOID Address,
    _In_opt_ PLARGE_INTEGER Timeout
    );
#endif

/**
 * Impersonates a client thread.
 *
 * @param ServerThreadHandle A handle to the server thread.
 * @param ClientThreadHandle A handle to the client thread.
 * @param SecurityQos A pointer to a SECURITY_QUALITY_OF_SERVICE structure that specifies the impersonation level and context tracking mode.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtImpersonateThread(
    _In_ HANDLE ServerThreadHandle,
    _In_ HANDLE ClientThreadHandle,
    _In_ PSECURITY_QUALITY_OF_SERVICE SecurityQos
    );

/**
 * Registers a thread termination port.
 *
 * @param PortHandle A handle to the port to be registered.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtRegisterThreadTerminatePort(
    _In_ HANDLE PortHandle
    );

/**
 * Sets LDT (Local Descriptor Table) entries.
 *
 * @param Selector0 The first selector.
 * @param Entry0Low The low part of the first entry.
 * @param Entry0Hi The high part of the first entry.
 * @param Selector1 The second selector.
 * @param Entry1Low The low part of the second entry.
 * @param Entry1Hi The high part of the second entry.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetLdtEntries(
    _In_ ULONG Selector0,
    _In_ ULONG Entry0Low,
    _In_ ULONG Entry0Hi,
    _In_ ULONG Selector1,
    _In_ ULONG Entry1Low,
    _In_ ULONG Entry1Hi
    );

/**
 * Dispatches the Asynchronous Procedure Call (APC) from the NtQueueApc* functions to the specified routine.
 * 
 * @param ApcRoutine A pointer to the APC routine to be executed.
 * @param Parameter Optional. A pointer to a parameter to be passed to the APC routine.
 * @param ActxContext Optional. A handle to an activation context.
 */
NTSYSAPI
VOID
NTAPI
RtlDispatchAPC(
    _In_ PAPCFUNC ApcRoutine,
    _In_opt_ PVOID Parameter,
    _In_opt_ HANDLE ActxContext
    );

/**
 * A pointer to a function that serves as an APC routine.
 * 
 * @param ApcArgument1 Optional. A pointer to the first argument to be passed to the APC routine.
 * @param ApcArgument2 Optional. A pointer to the second argument to be passed to the APC routine.
 * @param ApcArgument3 Optional. A pointer to the third argument to be passed to the APC routine.
 */
typedef VOID (NTAPI* PPS_APC_ROUTINE)(
    _In_opt_ PVOID ApcArgument1,
    _In_opt_ PVOID ApcArgument2,
    _In_opt_ PVOID ApcArgument3
    );

/**
 * Encodes an APC routine pointer for use in a WOW64 environment.
 * 
 * @param ApcRoutine The APC routine pointer to be encoded.
 * @return PVOID The encoded APC routine pointer.
 */
#define Wow64EncodeApcRoutine(ApcRoutine) \
    ((PVOID)((0 - ((LONG_PTR)(ApcRoutine))) << 2))

/**
 * Decodes an APC routine pointer that was encoded for use in a WOW64 environment.
 * 
 * @param ApcRoutine The encoded APC routine pointer to be decoded.
 * @return PVOID The decoded APC routine pointer.
 */
#define Wow64DecodeApcRoutine(ApcRoutine) \
    ((PVOID)(0 - (((LONG_PTR)(ApcRoutine)) >> 2)))

/**
 * Queues an APC (Asynchronous Procedure Call) to a thread.
 *
 * @param ThreadHandle Handle to the thread to which the APC is to be queued.
 * @param ApcRoutine A pointer to the RtlDispatchAPC function or custom APC routine to be executed.
 * @param ApcArgument1 Optional first argument to be passed to the APC routine.
 * @param ApcArgument2 Optional second argument to be passed to the APC routine.
 * @param ApcArgument3 Optional third argument to be passed to the APC routine.
 * @return NTSTATUS Successful or errant status.
 * @remarks The APC will be executed in the context of the specified thread when the thread enters an alertable wait state or when any
 * process calls the NtTestAlert, NtAlertThread, NtAlertResumeThread or NtAlertThreadByThreadId functions.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueueApcThread(
    _In_ HANDLE ThreadHandle,
    _In_ PPS_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcArgument1,
    _In_opt_ PVOID ApcArgument2,
    _In_opt_ PVOID ApcArgument3
    );

/**
 * A special handle value used to queue a user APC (Asynchronous Procedure Call).
 */
#define QUEUE_USER_APC_SPECIAL_USER_APC ((HANDLE)0x1)

/**
 * Queues an APC (Asynchronous Procedure Call) to a thread.
 *
 * @param ThreadHandle Handle to the thread to which the APC is to be queued.
 * @param ReserveHandle Optional handle to a reserve object. This can be QUEUE_USER_APC_SPECIAL_USER_APC or a handle returned by NtAllocateReserveObject.
 * @param ApcRoutine A pointer to the RtlDispatchAPC function or custom APC routine to be executed.
 * @param ApcArgument1 Optional first argument to be passed to the APC routine.
 * @param ApcArgument2 Optional second argument to be passed to the APC routine.
 * @param ApcArgument3 Optional third argument to be passed to the APC routine.
 * @return NTSTATUS Successful or errant status.
 * @remarks The APC will be executed in the context of the specified thread after the thread enters an alertable wait state or immediately
 * when QUEUE_USER_APC_SPECIAL_USER_APC is used or NtTestAlert, NtAlertThread, NtAlertResumeThread or NtAlertThreadByThreadId are called.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueueApcThreadEx(
    _In_ HANDLE ThreadHandle,
    _In_opt_ HANDLE ReserveHandle, // NtAllocateReserveObject // SPECIAL_USER_APC
    _In_ PPS_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcArgument1,
    _In_opt_ PVOID ApcArgument2,
    _In_opt_ PVOID ApcArgument3
    );

/**
 * The APC_CALLBACK_DATA_CONTEXT structure is used to pass information to the APC callback routine.
 */
typedef struct _APC_CALLBACK_DATA_CONTEXT
{
    ULONG_PTR Parameter;
    PCONTEXT ContextRecord;
    ULONG_PTR Reserved0;
    ULONG_PTR Reserved1;
} APC_CALLBACK_DATA_CONTEXT, *PAPC_CALLBACK_DATA_CONTEXT;

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)
/**
 * Queues an Asynchronous Procedure Call (APC) to a specified thread.
 *
 * @param ThreadHandle A handle to the thread to which the APC is to be queued.
 * @param ReserveHandle An optional handle to a reserve object. This can be obtained using NtAllocateReserveObject.
 * @param ApcFlags Flags that control the behavior of the APC. These flags are defined in QUEUE_USER_APC_FLAGS.
 * @param ApcRoutine A pointer to the RtlDispatchAPC function or custom APC routine to be executed.
 * @param ApcArgument1 An optional argument to be passed to the APC routine.
 * @param ApcArgument2 An optional argument to be passed to the APC routine.
 * @param ApcArgument3 An optional argument to be passed to the APC routine.
 * @return NTSTATUS Successful or errant status.
 * @remarks The APC will be executed in the context of the specified thread when the thread enters an alertable wait state or immediately
 * when QUEUE_USER_APC_SPECIAL_USER_APC is used or any process calls the NtTestAlert, NtAlertThread,
 * NtAlertResumeThread or NtAlertThreadByThreadId functions.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueueApcThreadEx2(
    _In_ HANDLE ThreadHandle,
    _In_opt_ HANDLE ReserveHandle, // NtAllocateReserveObject
    _In_ ULONG ApcFlags, // QUEUE_USER_APC_FLAGS
    _In_ PPS_APC_ROUTINE ApcRoutine, // RtlDispatchAPC
    _In_opt_ PVOID ApcArgument1,
    _In_opt_ PVOID ApcArgument2,
    _In_opt_ PVOID ApcArgument3
    );
#endif

#endif

// User processes and threads

#if !defined(_KERNEL_MODE)

// Attributes (Win32 CreateProcess)

// PROC_THREAD_ATTRIBUTE_NUM (dmex)
#define _USE_FULL_PROC_THREAD_ATTRIBUTE
typedef enum _PROC_THREAD_ATTRIBUTE_NUM
{
    ProcThreadAttributeParentProcess = 0,                   // in HANDLE
    ProcThreadAttributeExtendedFlags = 1,                   // in ULONG (EXTENDED_PROCESS_CREATION_FLAG_*)
    ProcThreadAttributeHandleList = 2,                      // in HANDLE[]
    ProcThreadAttributeGroupAffinity = 3,                   // in GROUP_AFFINITY // since WIN7
    ProcThreadAttributePreferredNode = 4,                   // in USHORT
    ProcThreadAttributeIdealProcessor = 5,                  // in PROCESSOR_NUMBER
    ProcThreadAttributeUmsThread = 6,                       // in UMS_CREATE_THREAD_ATTRIBUTES
    ProcThreadAttributeMitigationPolicy = 7,                // in ULONG, ULONG64, or ULONG64[2]
    ProcThreadAttributePackageFullName = 8,                 // in WCHAR[] // since WIN8
    ProcThreadAttributeSecurityCapabilities = 9,            // in SECURITY_CAPABILITIES
    ProcThreadAttributeConsoleReference = 10,               // BaseGetConsoleReference (kernelbase.dll)
    ProcThreadAttributeProtectionLevel = 11,                // in ULONG (PROTECTION_LEVEL_*) // since WINBLUE
    ProcThreadAttributeOsMaxVersionTested = 12,             // in MAXVERSIONTESTED_INFO // since THRESHOLD // (from exe.manifest)
    ProcThreadAttributeJobList = 13,                        // in HANDLE[]
    ProcThreadAttributeChildProcessPolicy = 14,             // in ULONG (PROCESS_CREATION_CHILD_PROCESS_*) // since THRESHOLD2
    ProcThreadAttributeAllApplicationPackagesPolicy = 15,   // in ULONG (PROCESS_CREATION_ALL_APPLICATION_PACKAGES_*) // since REDSTONE
    ProcThreadAttributeWin32kFilter = 16,                   // in WIN32K_SYSCALL_FILTER
    ProcThreadAttributeSafeOpenPromptOriginClaim = 17,      // in SE_SAFE_OPEN_PROMPT_RESULTS
    ProcThreadAttributeDesktopAppPolicy = 18,               // in ULONG (PROCESS_CREATION_DESKTOP_APP_*) // since RS2
    ProcThreadAttributeBnoIsolation = 19,                   // in PROC_THREAD_BNOISOLATION_ATTRIBUTE
    ProcThreadAttributePseudoConsole = 22,                  // in HANDLE (HPCON) // since RS5
    ProcThreadAttributeIsolationManifest = 23,              // in ISOLATION_MANIFEST_PROPERTIES // rev (diversenok) // since 19H2+
    ProcThreadAttributeMitigationAuditPolicy = 24,          // in ULONG, ULONG64, or ULONG64[2] // since 21H1
    ProcThreadAttributeMachineType = 25,                    // in USHORT // since 21H2
    ProcThreadAttributeComponentFilter = 26,                // in ULONG
    ProcThreadAttributeEnableOptionalXStateFeatures = 27,   // in ULONG64 // since WIN11
    ProcThreadAttributeCreateStore = 28,                    // ULONG // rev (diversenok)
    ProcThreadAttributeTrustedApp = 29,
    ProcThreadAttributeSveVectorLength = 30,
    ProcThreadAttributeSmeVectorLength = 31                 // since 25H2
} PROC_THREAD_ATTRIBUTE_NUM;

#ifndef PROC_THREAD_ATTRIBUTE_EXTENDED_FLAGS
#define PROC_THREAD_ATTRIBUTE_EXTENDED_FLAGS \
    ProcThreadAttributeValue(ProcThreadAttributeExtendedFlags, FALSE, TRUE, TRUE)
#endif
#ifndef PROC_THREAD_ATTRIBUTE_PACKAGE_FULL_NAME
#define PROC_THREAD_ATTRIBUTE_PACKAGE_FULL_NAME \
    ProcThreadAttributeValue(ProcThreadAttributePackageFullName, FALSE, TRUE, FALSE)
#endif
#ifndef PROC_THREAD_ATTRIBUTE_CONSOLE_REFERENCE
#define PROC_THREAD_ATTRIBUTE_CONSOLE_REFERENCE \
    ProcThreadAttributeValue(ProcThreadAttributeConsoleReference, FALSE, TRUE, FALSE)
#endif
#ifndef PROC_THREAD_ATTRIBUTE_OSMAXVERSIONTESTED
#define PROC_THREAD_ATTRIBUTE_OSMAXVERSIONTESTED \
    ProcThreadAttributeValue(ProcThreadAttributeOsMaxVersionTested, FALSE, TRUE, FALSE)
#endif
#ifndef PROC_THREAD_ATTRIBUTE_SAFE_OPEN_PROMPT_ORIGIN_CLAIM
#define PROC_THREAD_ATTRIBUTE_SAFE_OPEN_PROMPT_ORIGIN_CLAIM \
    ProcThreadAttributeValue(ProcThreadAttributeSafeOpenPromptOriginClaim, FALSE, TRUE, FALSE)
#endif
#ifndef PROC_THREAD_ATTRIBUTE_BNO_ISOLATION
#define PROC_THREAD_ATTRIBUTE_BNO_ISOLATION \
    ProcThreadAttributeValue(ProcThreadAttributeBnoIsolation, FALSE, TRUE, FALSE)
#endif
#ifndef PROC_THREAD_ATTRIBUTE_ISOLATION_MANIFEST
#define PROC_THREAD_ATTRIBUTE_ISOLATION_MANIFEST \
    ProcThreadAttributeValue(ProcThreadAttributeIsolationManifest, FALSE, TRUE, FALSE)
#endif
#ifndef PROC_THREAD_ATTRIBUTE_CREATE_STORE
#define PROC_THREAD_ATTRIBUTE_CREATE_STORE \
    ProcThreadAttributeValue(ProcThreadAttributeCreateStore, FALSE, TRUE, FALSE)
#endif

// private
typedef struct _PROC_THREAD_ATTRIBUTE
{
    ULONG_PTR Attribute;
    SIZE_T Size;
    ULONG_PTR Value;
} PROC_THREAD_ATTRIBUTE, *PPROC_THREAD_ATTRIBUTE;

/**
 * The PROC_THREAD_ATTRIBUTE_LIST structure contains the list of attributes for process and thread creation.
 */
typedef struct _PROC_THREAD_ATTRIBUTE_LIST
{
    ULONG PresentFlags;             // A bitmask of flags that indicate the attributes for process and thread creation.
    ULONG AttributeCount;           // The number of attributes in the list.
    ULONG LastAttribute;            // The index of the last attribute in the list.
    ULONG SpareUlong0;              // Reserved for future use.
    PPROC_THREAD_ATTRIBUTE ExtendedFlagsAttribute; // A pointer to the extended flags attribute.
    _Field_size_(AttributeCount) PROC_THREAD_ATTRIBUTE Attributes[1]; // An array of attributes.
} PROC_THREAD_ATTRIBUTE_LIST, *PPROC_THREAD_ATTRIBUTE_LIST;

// private
#define EXTENDED_PROCESS_CREATION_FLAG_ELEVATION_HANDLED 0x00000001
#define EXTENDED_PROCESS_CREATION_FLAG_FORCELUA 0x00000002
#define EXTENDED_PROCESS_CREATION_FLAG_FORCE_BREAKAWAY 0x00000004 // requires SeTcbPrivilege // since WINBLUE

#define PROTECTION_LEVEL_WINTCB_LIGHT       0x00000000
#define PROTECTION_LEVEL_WINDOWS            0x00000001
#define PROTECTION_LEVEL_WINDOWS_LIGHT      0x00000002
#define PROTECTION_LEVEL_ANTIMALWARE_LIGHT  0x00000003
#define PROTECTION_LEVEL_LSA_LIGHT          0x00000004
#define PROTECTION_LEVEL_WINTCB             0x00000005
#define PROTECTION_LEVEL_CODEGEN_LIGHT      0x00000006
#define PROTECTION_LEVEL_AUTHENTICODE       0x00000007
#define PROTECTION_LEVEL_PPL_APP            0x00000008

#define PROTECTION_LEVEL_SAME               0xFFFFFFFF
#define PROTECTION_LEVEL_NONE               0xFFFFFFFE

// private
typedef enum _SE_SAFE_OPEN_PROMPT_EXPERIENCE_RESULTS
{
    SeSafeOpenExperienceNone = 0x00,
    SeSafeOpenExperienceCalled = 0x01,
    SeSafeOpenExperienceAppRepCalled = 0x02,
    SeSafeOpenExperiencePromptDisplayed = 0x04,
    SeSafeOpenExperienceUAC = 0x08,
    SeSafeOpenExperienceUninstaller = 0x10,
    SeSafeOpenExperienceIgnoreUnknownOrBad = 0x20,
    SeSafeOpenExperienceDefenderTrustedInstaller = 0x40,
    SeSafeOpenExperienceMOTWPresent = 0x80,
    SeSafeOpenExperienceElevatedNoPropagation = 0x100
} SE_SAFE_OPEN_PROMPT_EXPERIENCE_RESULTS;

// private
typedef struct _SE_SAFE_OPEN_PROMPT_RESULTS
{
    SE_SAFE_OPEN_PROMPT_EXPERIENCE_RESULTS Results;
    WCHAR Path[MAX_PATH];
} SE_SAFE_OPEN_PROMPT_RESULTS, *PSE_SAFE_OPEN_PROMPT_RESULTS;

typedef struct _PROC_THREAD_BNOISOLATION_ATTRIBUTE
{
    BOOL IsolationEnabled;
    WCHAR IsolationPrefix[0x88];
} PROC_THREAD_BNOISOLATION_ATTRIBUTE, *PPROC_THREAD_BNOISOLATION_ATTRIBUTE;

// private
typedef struct _ISOLATION_MANIFEST_PROPERTIES
{
    UNICODE_STRING InstancePath;
    UNICODE_STRING FriendlyName;
    UNICODE_STRING Description;
    ULONG_PTR Level;
} ISOLATION_MANIFEST_PROPERTIES, *PISOLATION_MANIFEST_PROPERTIES;

// Attributes (Native)

// private
typedef enum _PS_ATTRIBUTE_NUM
{
    PsAttributeParentProcess, // in HANDLE
    PsAttributeDebugObject, // in HANDLE
    PsAttributeToken, // in HANDLE
    PsAttributeClientId, // out PCLIENT_ID
    PsAttributeTebAddress, // out PTEB *
    PsAttributeImageName, // in PWSTR
    PsAttributeImageInfo, // out PSECTION_IMAGE_INFORMATION
    PsAttributeMemoryReserve, // in PPS_MEMORY_RESERVE
    PsAttributePriorityClass, // in UCHAR
    PsAttributeErrorMode, // in ULONG
    PsAttributeStdHandleInfo, // 10, in PPS_STD_HANDLE_INFO
    PsAttributeHandleList, // in HANDLE[]
    PsAttributeGroupAffinity, // in PGROUP_AFFINITY
    PsAttributePreferredNode, // in PUSHORT
    PsAttributeIdealProcessor, // in PPROCESSOR_NUMBER
    PsAttributeUmsThread, // ? in PUMS_CREATE_THREAD_ATTRIBUTES
    PsAttributeMitigationOptions, // in PPS_MITIGATION_OPTIONS_MAP (PROCESS_CREATION_MITIGATION_POLICY_*) // since WIN8
    PsAttributeProtectionLevel, // in PS_PROTECTION // since WINBLUE
    PsAttributeSecureProcess, // in PPS_TRUSTLET_CREATE_ATTRIBUTES, since THRESHOLD
    PsAttributeJobList, // in HANDLE[]
    PsAttributeChildProcessPolicy, // 20, in PULONG (PROCESS_CREATION_CHILD_PROCESS_*) // since THRESHOLD2
    PsAttributeAllApplicationPackagesPolicy, // in PULONG (PROCESS_CREATION_ALL_APPLICATION_PACKAGES_*) // since REDSTONE
    PsAttributeWin32kFilter, // in PWIN32K_SYSCALL_FILTER
    PsAttributeSafeOpenPromptOriginClaim, // in SE_SAFE_OPEN_PROMPT_RESULTS
    PsAttributeBnoIsolation, // in PPS_BNO_ISOLATION_PARAMETERS // since REDSTONE2
    PsAttributeDesktopAppPolicy, // in PULONG (PROCESS_CREATION_DESKTOP_APP_*)
    PsAttributeChpe, // in BOOLEAN // since REDSTONE3
    PsAttributeMitigationAuditOptions, // in PPS_MITIGATION_AUDIT_OPTIONS_MAP (PROCESS_CREATION_MITIGATION_AUDIT_POLICY_*) // since 21H1
    PsAttributeMachineType, // in USHORT // since 21H2
    PsAttributeComponentFilter,
    PsAttributeEnableOptionalXStateFeatures, // since WIN11
    PsAttributeSupportedMachines, // since 24H2
    PsAttributeSveVectorLength, // PPS_PROCESS_CREATION_SVE_VECTOR_LENGTH
    PsAttributeMax
} PS_ATTRIBUTE_NUM;

// private
#define PS_ATTRIBUTE_NUMBER_MASK 0x0000ffff
#define PS_ATTRIBUTE_THREAD 0x00010000 // may be used with thread creation
#define PS_ATTRIBUTE_INPUT 0x00020000 // input only
#define PS_ATTRIBUTE_ADDITIVE 0x00040000 // "accumulated" e.g. bitmasks, counters, etc.

// begin_rev

#define PsAttributeValue(Number, Thread, Input, Additive) \
    (((Number) & PS_ATTRIBUTE_NUMBER_MASK) | \
    ((Thread) ? PS_ATTRIBUTE_THREAD : 0) | \
    ((Input) ? PS_ATTRIBUTE_INPUT : 0) | \
    ((Additive) ? PS_ATTRIBUTE_ADDITIVE : 0))

#define PS_ATTRIBUTE_PARENT_PROCESS \
    PsAttributeValue(PsAttributeParentProcess, FALSE, TRUE, TRUE)
#define PS_ATTRIBUTE_DEBUG_OBJECT \
    PsAttributeValue(PsAttributeDebugObject, FALSE, TRUE, TRUE)
#define PS_ATTRIBUTE_TOKEN \
    PsAttributeValue(PsAttributeToken, FALSE, TRUE, TRUE)
#define PS_ATTRIBUTE_CLIENT_ID \
    PsAttributeValue(PsAttributeClientId, TRUE, FALSE, FALSE)
#define PS_ATTRIBUTE_TEB_ADDRESS \
    PsAttributeValue(PsAttributeTebAddress, TRUE, FALSE, FALSE)
#define PS_ATTRIBUTE_IMAGE_NAME \
    PsAttributeValue(PsAttributeImageName, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_IMAGE_INFO \
    PsAttributeValue(PsAttributeImageInfo, FALSE, FALSE, FALSE)
#define PS_ATTRIBUTE_MEMORY_RESERVE \
    PsAttributeValue(PsAttributeMemoryReserve, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_PRIORITY_CLASS \
    PsAttributeValue(PsAttributePriorityClass, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_ERROR_MODE \
    PsAttributeValue(PsAttributeErrorMode, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_STD_HANDLE_INFO \
    PsAttributeValue(PsAttributeStdHandleInfo, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_HANDLE_LIST \
    PsAttributeValue(PsAttributeHandleList, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_GROUP_AFFINITY \
    PsAttributeValue(PsAttributeGroupAffinity, TRUE, TRUE, FALSE)
#define PS_ATTRIBUTE_PREFERRED_NODE \
    PsAttributeValue(PsAttributePreferredNode, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_IDEAL_PROCESSOR \
    PsAttributeValue(PsAttributeIdealProcessor, TRUE, TRUE, FALSE)
#define PS_ATTRIBUTE_UMS_THREAD \
    PsAttributeValue(PsAttributeUmsThread, TRUE, TRUE, FALSE)
#define PS_ATTRIBUTE_MITIGATION_OPTIONS \
    PsAttributeValue(PsAttributeMitigationOptions, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_PROTECTION_LEVEL \
    PsAttributeValue(PsAttributeProtectionLevel, FALSE, TRUE, TRUE)
#define PS_ATTRIBUTE_SECURE_PROCESS \
    PsAttributeValue(PsAttributeSecureProcess, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_JOB_LIST \
    PsAttributeValue(PsAttributeJobList, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_CHILD_PROCESS_POLICY \
    PsAttributeValue(PsAttributeChildProcessPolicy, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_ALL_APPLICATION_PACKAGES_POLICY \
    PsAttributeValue(PsAttributeAllApplicationPackagesPolicy, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_WIN32K_FILTER \
    PsAttributeValue(PsAttributeWin32kFilter, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_SAFE_OPEN_PROMPT_ORIGIN_CLAIM \
    PsAttributeValue(PsAttributeSafeOpenPromptOriginClaim, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_BNO_ISOLATION \
    PsAttributeValue(PsAttributeBnoIsolation, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_DESKTOP_APP_POLICY \
    PsAttributeValue(PsAttributeDesktopAppPolicy, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_CHPE \
    PsAttributeValue(PsAttributeChpe, FALSE, TRUE, TRUE)
#define PS_ATTRIBUTE_MITIGATION_AUDIT_OPTIONS \
    PsAttributeValue(PsAttributeMitigationAuditOptions, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_MACHINE_TYPE \
    PsAttributeValue(PsAttributeMachineType, FALSE, TRUE, TRUE)
#define PS_ATTRIBUTE_COMPONENT_FILTER \
    PsAttributeValue(PsAttributeComponentFilter, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_ENABLE_OPTIONAL_XSTATE_FEATURES \
    PsAttributeValue(PsAttributeEnableOptionalXStateFeatures, TRUE, TRUE, FALSE)

// end_rev

// begin_private

typedef struct _PS_ATTRIBUTE
{
    ULONG_PTR Attribute;
    SIZE_T Size;
    union
    {
        ULONG_PTR Value;
        PVOID ValuePtr;
    };
    PSIZE_T ReturnLength;
} PS_ATTRIBUTE, *PPS_ATTRIBUTE;

typedef struct _PS_ATTRIBUTE_LIST
{
    SIZE_T TotalLength;
    PS_ATTRIBUTE Attributes[1];
} PS_ATTRIBUTE_LIST, *PPS_ATTRIBUTE_LIST;

typedef struct _PS_MEMORY_RESERVE
{
    PVOID ReserveAddress;
    SIZE_T ReserveSize;
} PS_MEMORY_RESERVE, *PPS_MEMORY_RESERVE;

typedef enum _PS_STD_HANDLE_STATE
{
    PsNeverDuplicate,
    PsRequestDuplicate, // duplicate standard handles specified by PseudoHandleMask, and only if StdHandleSubsystemType matches the image subsystem
    PsAlwaysDuplicate, // always duplicate standard handles
    PsMaxStdHandleStates
} PS_STD_HANDLE_STATE;

// begin_rev
#define PS_STD_INPUT_HANDLE 0x1
#define PS_STD_OUTPUT_HANDLE 0x2
#define PS_STD_ERROR_HANDLE 0x4
// end_rev

typedef struct _PS_STD_HANDLE_INFO
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG StdHandleState : 2; // PS_STD_HANDLE_STATE
            ULONG PseudoHandleMask : 3; // PS_STD_*
        };
    };
    ULONG StdHandleSubsystemType;
} PS_STD_HANDLE_INFO, *PPS_STD_HANDLE_INFO;

typedef union _PS_TRUSTLET_ATTRIBUTE_ACCESSRIGHTS
{
    UCHAR Trustlet : 1;
    UCHAR Ntos : 1;
    UCHAR WriteHandle : 1;
    UCHAR ReadHandle : 1;
    UCHAR Reserved : 4;
    UCHAR AccessRights;
} PS_TRUSTLET_ATTRIBUTE_ACCESSRIGHTS, *PPS_TRUSTLET_ATTRIBUTE_ACCESSRIGHTS;

typedef struct _PS_TRUSTLET_ATTRIBUTE_TYPE
{
    union
    {
        struct
        {
            UCHAR Version;
            UCHAR DataCount;
            UCHAR SemanticType;
            PS_TRUSTLET_ATTRIBUTE_ACCESSRIGHTS AccessRights;
        };
        ULONG AttributeType;
    };
} PS_TRUSTLET_ATTRIBUTE_TYPE, *PPS_TRUSTLET_ATTRIBUTE_TYPE;

typedef struct _PS_TRUSTLET_ATTRIBUTE_HEADER
{
    PS_TRUSTLET_ATTRIBUTE_TYPE AttributeType;
    ULONG InstanceNumber : 8;
    ULONG Reserved : 24;
} PS_TRUSTLET_ATTRIBUTE_HEADER, *PPS_TRUSTLET_ATTRIBUTE_HEADER;

typedef struct _PS_TRUSTLET_ATTRIBUTE_DATA
{
    PS_TRUSTLET_ATTRIBUTE_HEADER Header;
    ULONGLONG Data[1];
} PS_TRUSTLET_ATTRIBUTE_DATA, *PPS_TRUSTLET_ATTRIBUTE_DATA;

typedef struct _PS_TRUSTLET_CREATE_ATTRIBUTES
{
    ULONGLONG TrustletIdentity;
    PS_TRUSTLET_ATTRIBUTE_DATA Attributes[1];
} PS_TRUSTLET_CREATE_ATTRIBUTES, *PPS_TRUSTLET_CREATE_ATTRIBUTES;

// private
typedef struct _PS_BNO_ISOLATION_PARAMETERS
{
    UNICODE_STRING IsolationPrefix;
    ULONG HandleCount;
    PVOID *Handles;
    BOOLEAN IsolationEnabled;
} PS_BNO_ISOLATION_PARAMETERS, *PPS_BNO_ISOLATION_PARAMETERS;

// private
typedef union _PS_PROCESS_CREATION_SVE_VECTOR_LENGTH
{
    ULONG VectorLength : 24;
    ULONG FlagsReserved : 8;
} PS_PROCESS_CREATION_SVE_VECTOR_LENGTH, *PPS_PROCESS_CREATION_SVE_VECTOR_LENGTH;

// private
typedef enum _PS_MITIGATION_OPTION
{
    PS_MITIGATION_OPTION_NX,
    PS_MITIGATION_OPTION_SEHOP,
    PS_MITIGATION_OPTION_FORCE_RELOCATE_IMAGES,
    PS_MITIGATION_OPTION_HEAP_TERMINATE,
    PS_MITIGATION_OPTION_BOTTOM_UP_ASLR,
    PS_MITIGATION_OPTION_HIGH_ENTROPY_ASLR,
    PS_MITIGATION_OPTION_STRICT_HANDLE_CHECKS,
    PS_MITIGATION_OPTION_WIN32K_SYSTEM_CALL_DISABLE,
    PS_MITIGATION_OPTION_EXTENSION_POINT_DISABLE,
    PS_MITIGATION_OPTION_PROHIBIT_DYNAMIC_CODE,
    PS_MITIGATION_OPTION_CONTROL_FLOW_GUARD,
    PS_MITIGATION_OPTION_BLOCK_NON_MICROSOFT_BINARIES,
    PS_MITIGATION_OPTION_FONT_DISABLE,
    PS_MITIGATION_OPTION_IMAGE_LOAD_NO_REMOTE,
    PS_MITIGATION_OPTION_IMAGE_LOAD_NO_LOW_LABEL,
    PS_MITIGATION_OPTION_IMAGE_LOAD_PREFER_SYSTEM32,
    PS_MITIGATION_OPTION_RETURN_FLOW_GUARD,
    PS_MITIGATION_OPTION_LOADER_INTEGRITY_CONTINUITY,
    PS_MITIGATION_OPTION_STRICT_CONTROL_FLOW_GUARD,
    PS_MITIGATION_OPTION_RESTRICT_SET_THREAD_CONTEXT,
    PS_MITIGATION_OPTION_ROP_STACKPIVOT, // since REDSTONE3
    PS_MITIGATION_OPTION_ROP_CALLER_CHECK,
    PS_MITIGATION_OPTION_ROP_SIMEXEC,
    PS_MITIGATION_OPTION_EXPORT_ADDRESS_FILTER,
    PS_MITIGATION_OPTION_EXPORT_ADDRESS_FILTER_PLUS,
    PS_MITIGATION_OPTION_RESTRICT_CHILD_PROCESS_CREATION,
    PS_MITIGATION_OPTION_IMPORT_ADDRESS_FILTER,
    PS_MITIGATION_OPTION_MODULE_TAMPERING_PROTECTION,
    PS_MITIGATION_OPTION_RESTRICT_INDIRECT_BRANCH_PREDICTION,
    PS_MITIGATION_OPTION_SPECULATIVE_STORE_BYPASS_DISABLE, // since REDSTONE5
    PS_MITIGATION_OPTION_ALLOW_DOWNGRADE_DYNAMIC_CODE_POLICY,
    PS_MITIGATION_OPTION_CET_USER_SHADOW_STACKS,
    PS_MITIGATION_OPTION_USER_CET_SET_CONTEXT_IP_VALIDATION, // since 21H1
    PS_MITIGATION_OPTION_BLOCK_NON_CET_BINARIES,
    PS_MITIGATION_OPTION_CET_DYNAMIC_APIS_OUT_OF_PROC_ONLY,
    PS_MITIGATION_OPTION_REDIRECTION_TRUST, // since 22H1
    PS_MITIGATION_OPTION_RESTRICT_CORE_SHARING,
    PS_MITIGATION_OPTION_FSCTL_SYSTEM_CALL_DISABLE, // since 24H2
} PS_MITIGATION_OPTION;

// windows-internals-book:"Chapter 5"
typedef enum _PS_CREATE_STATE
{
    PsCreateInitialState,
    PsCreateFailOnFileOpen,
    PsCreateFailOnSectionCreate,
    PsCreateFailExeFormat,
    PsCreateFailMachineMismatch,
    PsCreateFailExeName, // Debugger specified
    PsCreateSuccess,
    PsCreateMaximumStates
} PS_CREATE_STATE;

typedef struct _PS_CREATE_INFO
{
    SIZE_T Size;
    PS_CREATE_STATE State;
    union
    {
        // PsCreateInitialState
        struct
        {
            union
            {
                ULONG InitFlags;
                struct
                {
                    UCHAR WriteOutputOnExit : 1;
                    UCHAR DetectManifest : 1;
                    UCHAR IFEOSkipDebugger : 1;
                    UCHAR IFEODoNotPropagateKeyState : 1;
                    UCHAR SpareBits1 : 4;
                    UCHAR SpareBits2 : 8;
                    USHORT ProhibitedImageCharacteristics : 16;
                };
            };
            ACCESS_MASK AdditionalFileAccess;
        } InitState;

        // PsCreateFailOnSectionCreate
        struct
        {
            HANDLE FileHandle;
        } FailSection;

        // PsCreateFailExeFormat
        struct
        {
            USHORT DllCharacteristics;
        } ExeFormat;

        // PsCreateFailExeName
        struct
        {
            HANDLE IFEOKey;
        } ExeName;

        // PsCreateSuccess
        struct
        {
            union
            {
                ULONG OutputFlags;
                struct
                {
                    UCHAR ProtectedProcess : 1;
                    UCHAR AddressSpaceOverride : 1;
                    UCHAR DevOverrideEnabled : 1; // from Image File Execution Options
                    UCHAR ManifestDetected : 1;
                    UCHAR ProtectedProcessLight : 1;
                    UCHAR SpareBits1 : 3;
                    UCHAR SpareBits2 : 8;
                    USHORT SpareBits3 : 16;
                };
            };
            HANDLE FileHandle;
            HANDLE SectionHandle;
            ULONGLONG UserProcessParametersNative;
            ULONG UserProcessParametersWow64;
            ULONG CurrentParameterFlags;
            ULONGLONG PebAddressNative;
            ULONG PebAddressWow64;
            ULONGLONG ManifestAddress;
            ULONG ManifestSize;
        } SuccessState;
    };
} PS_CREATE_INFO, *PPS_CREATE_INFO;

// end_private

/**
 * Creates a new process and primary thread.
 *
 * @param ProcessHandle A pointer to a handle that receives the process object handle.
 * @param ThreadHandle A pointer to a handle that receives the thread object handle.
 * @param ProcessDesiredAccess The access rights desired for the process object.
 * @param ThreadDesiredAccess The access rights desired for the thread object.
 * @param ProcessObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new process.
 * @param ThreadObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new thread.
 * @param ProcessFlags Flags that control the creation of the process. These flags are defined as PROCESS_CREATE_FLAGS_*.
 * @param ThreadFlags Flags that control the creation of the thread. These flags are defined as THREAD_CREATE_FLAGS_*.
 * @param ProcessParameters Optional. A pointer to a RTL_USER_PROCESS_PARAMETERS structure that specifies the parameters for the new process.
 * @param CreateInfo A pointer to a PS_CREATE_INFO structure that specifies additional information for the process creation.
 * @param AttributeList Optional. A pointer to a list of attributes for the process and thread.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateUserProcess(
    _Out_ PHANDLE ProcessHandle,
    _Out_ PHANDLE ThreadHandle,
    _In_ ACCESS_MASK ProcessDesiredAccess,
    _In_ ACCESS_MASK ThreadDesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ProcessObjectAttributes,
    _In_opt_ PCOBJECT_ATTRIBUTES ThreadObjectAttributes,
    _In_ ULONG ProcessFlags, // PROCESS_CREATE_FLAGS_*
    _In_ ULONG ThreadFlags, // THREAD_CREATE_FLAGS_*
    _In_opt_ PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
    _Inout_ PPS_CREATE_INFO CreateInfo,
    _In_opt_ PPS_ATTRIBUTE_LIST AttributeList
    );

// begin_rev
#define THREAD_CREATE_FLAGS_NONE 0x00000000
#define THREAD_CREATE_FLAGS_CREATE_SUSPENDED 0x00000001 // NtCreateUserProcess & NtCreateThreadEx
#define THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH 0x00000002 // NtCreateThreadEx only
#define THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER 0x00000004 // NtCreateThreadEx only
#define THREAD_CREATE_FLAGS_LOADER_WORKER 0x00000010 // NtCreateThreadEx only // since THRESHOLD
#define THREAD_CREATE_FLAGS_SKIP_LOADER_INIT 0x00000020 // NtCreateThreadEx only // since REDSTONE2
#define THREAD_CREATE_FLAGS_BYPASS_PROCESS_FREEZE 0x00000040 // NtCreateThreadEx only // since 19H1
// end_rev

/**
 * Creates a new thread in the specified process.
 *
 * @param ThreadHandle A pointer to a handle that receives the thread object handle.
 * @param DesiredAccess The access rights desired for the thread object.
 * @param ObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new thread.
 * @param ProcessHandle A handle to the process in which the thread is to be created.
 * @param StartRoutine A pointer to the application-defined function to be executed by the thread.
 * \param Argument Optional. A pointer to a variable that is passed to the thread.
 * @param CreateFlags Flags that control the creation of the thread. These flags are defined as THREAD_CREATE_FLAGS_*.
 * @param ZeroBits The number of zero bits in the starting address of the thread's stack.
 * @param StackSize The initial size of the thread's stack, in bytes.
 * @param MaximumStackSize The maximum size of the thread's stack, in bytes.
 * @param AttributeList Optional. A pointer to a list of attributes for the thread.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateThreadEx(
    _Out_ PHANDLE ThreadHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE ProcessHandle,
    _In_ PUSER_THREAD_START_ROUTINE StartRoutine,
    _In_opt_ PVOID Argument,
    _In_ ULONG CreateFlags, // THREAD_CREATE_FLAGS_*
    _In_ SIZE_T ZeroBits,
    _In_ SIZE_T StackSize,
    _In_ SIZE_T MaximumStackSize,
    _In_opt_ PPS_ATTRIBUTE_LIST AttributeList
    );

#endif

// Job objects

#if !defined(_KERNEL_MODE)

/**
 * Creates or opens a job object.
 *
 * @param JobHandle A handle to the job object.
 * @param DesiredAccess The access rights desired for the thread object.
 * @param ObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new thread.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateJobObject(
    _Out_ PHANDLE JobHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes
    );

/**
 * Opens an existing job object.
 *
 * @param JobHandle A handle to the job object.
 * @param DesiredAccess The access rights desired for the thread object.
 * @param ObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new thread.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenJobObject(
    _Out_ PHANDLE JobHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes
    );

/**
 * Assigns a process to an existing job object.
 *
 * @param JobHandle A handle to the job object to which the process will be associated. The handle must have the JOB_OBJECT_ASSIGN_PROCESS access right.
 * @param ProcessHandle A handle to the process to associate with the job object. The handle must have the PROCESS_SET_QUOTA and PROCESS_TERMINATE access rights.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAssignProcessToJobObject(
    _In_ HANDLE JobHandle,
    _In_ HANDLE ProcessHandle
    );

/**
 * Terminates all processes associated with the job object. If the job is nested, all processes currently associated with the job and all child jobs in the hierarchy are terminated.
 *
 * @param JobHandle A handle to the job whose processes will be terminated. The handle must have the JOB_OBJECT_TERMINATE access right.
 * @param ExitStatus The exit status to be used by all processes and threads in the job object. 
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtTerminateJobObject(
    _In_ HANDLE JobHandle,
    _In_ NTSTATUS ExitStatus
    );

/**
 * Checks if a process is associated with a job object.
 *
 * @param ProcessHandle A handle to the process to be checked.
 * @param JobHandle An optional handle to the job object. If this parameter is NULL, the function checks if the process is associated with any job object.
 * @return NTSTATUS Successful or errant status.
 * @remarks This function can be used to determine if a process is running within a job object, which can be useful for managing process resources and constraints.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtIsProcessInJob(
    _In_ HANDLE ProcessHandle,
    _In_opt_ HANDLE JobHandle
    );

/**
 * Retrieves information about a job object.
 *
 * @param JobHandle An optional handle to the job object. If this parameter is NULL, the function retrieves information about the job object associated with the calling process.
 * @param JobObjectInformationClass The type of job object information to be retrieved.
 * @param JobObjectInformation A pointer to a buffer that receives the job object information.
 * @param JobObjectInformationLength The size of the buffer pointed to by the JobObjectInformation parameter.
 * @param ReturnLength An optional pointer to a variable that receives the size of the data returned.
 * @return NTSTATUS Successful or errant status.
 * @remarks This function can be used to query various types of information about a job object, such as accounting information, limit information, and process ID list.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationJobObject(
    _In_opt_ HANDLE JobHandle,
    _In_ JOBOBJECTINFOCLASS JobObjectInformationClass,
    _Out_writes_bytes_(JobObjectInformationLength) PVOID JobObjectInformation,
    _In_ ULONG JobObjectInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

/**
 * Sets information for a job object.
 *
 * @param JobHandle A handle to the job object.
 * @param JobObjectInformationClass The type of job object information to be set.
 * @param JobObjectInformation A pointer to a buffer that contains the job object information.
 * @param JobObjectInformationLength The size of the buffer pointed to by the JobObjectInformation parameter.
 * @return NTSTATUS Successful or errant status.
 * @remarks This function can be used to set various types of information for a job object, such as limit information, UI restrictions, and security limit information.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationJobObject(
    _In_ HANDLE JobHandle,
    _In_ JOBOBJECTINFOCLASS JobObjectInformationClass,
    _In_reads_bytes_(JobObjectInformationLength) PVOID JobObjectInformation,
    _In_ ULONG JobObjectInformationLength
    );

/**
 * Creates a set of job objects.
 *
 * @param NumJob The number of job objects in the set.
 * @param UserJobSet A pointer to an array of JOB_SET_ARRAY structures that specify the job objects in the set.
 * @param Flags Reserved for future use. Must be zero.
 * @return NTSTATUS Successful or errant status.
 * @remarks This function can be used to create a set of job objects, which can be useful for managing groups of related processes.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateJobSet(
    _In_ ULONG NumJob,
    _In_reads_(NumJob) PJOB_SET_ARRAY UserJobSet,
    _In_ ULONG Flags
    );

#if (NTDDI_VERSION >= NTDDI_WIN10)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtRevertContainerImpersonation(
    VOID
    );
#endif

#endif

// Reserve objects

#if !defined(_KERNEL_MODE)

// private
typedef enum _MEMORY_RESERVE_TYPE
{
    MemoryReserveUserApc,
    MemoryReserveIoCompletion,
    MemoryReserveTypeMax
} MEMORY_RESERVE_TYPE;

/**
 * Allocates a memory reserve object.
 *
 * @param MemoryReserveHandle Pointer to a variable that receives the memory reserve object handle.
 * @param ObjectAttributes Pointer to an object attributes structure.
 * @param Type The type of memory reserve.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAllocateReserveObject(
    _Out_ PHANDLE MemoryReserveHandle,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_ MEMORY_RESERVE_TYPE Type
    );

// Process snapshotting

// Capture/creation flags.
typedef enum _PSSNT_CAPTURE_FLAGS
{
    PSSNT_CAPTURE_NONE                                = 0x00000000,
    PSSNT_CAPTURE_VA_CLONE                            = 0x00000001,
    PSSNT_CAPTURE_RESERVED_00000002                   = 0x00000002,
    PSSNT_CAPTURE_HANDLES                             = 0x00000004,
    PSSNT_CAPTURE_HANDLE_NAME_INFORMATION             = 0x00000008,
    PSSNT_CAPTURE_HANDLE_BASIC_INFORMATION            = 0x00000010,
    PSSNT_CAPTURE_HANDLE_TYPE_SPECIFIC_INFORMATION    = 0x00000020,
    PSSNT_CAPTURE_HANDLE_TRACE                        = 0x00000040,
    PSSNT_CAPTURE_THREADS                             = 0x00000080,
    PSSNT_CAPTURE_THREAD_CONTEXT                      = 0x00000100,
    PSSNT_CAPTURE_THREAD_CONTEXT_EXTENDED             = 0x00000200,
    PSSNT_CAPTURE_RESERVED_00000400                   = 0x00000400,
    PSSNT_CAPTURE_VA_SPACE                            = 0x00000800,
    PSSNT_CAPTURE_VA_SPACE_SECTION_INFORMATION        = 0x00001000,
    PSSNT_CAPTURE_IPT_TRACE                           = 0x00002000,
    PSSNT_CAPTURE_RESERVED_00004000                   = 0x00004000,

    PSSNT_CREATE_BREAKAWAY_OPTIONAL                   = 0x04000000,
    PSSNT_CREATE_BREAKAWAY                            = 0x08000000,
    PSSNT_CREATE_FORCE_BREAKAWAY                      = 0x10000000,
    PSSNT_CREATE_USE_VM_ALLOCATIONS                   = 0x20000000,
    PSSNT_CREATE_MEASURE_PERFORMANCE                  = 0x40000000,
    PSSNT_CREATE_RELEASE_SECTION                      = 0x80000000
} PSSNT_CAPTURE_FLAGS;
DEFINE_ENUM_FLAG_OPERATORS(PSSNT_CAPTURE_FLAGS);

typedef enum _PSSNT_DUPLICATE_FLAGS
{
    PSSNT_DUPLICATE_NONE         = 0x00,
    PSSNT_DUPLICATE_CLOSE_SOURCE = 0x01
} PSSNT_DUPLICATE_FLAGS;
DEFINE_ENUM_FLAG_OPERATORS(PSSNT_DUPLICATE_FLAGS);

typedef enum _PSSNT_QUERY_INFORMATION_CLASS
{
    PSSNT_QUERY_PROCESS_INFORMATION = 0, // PSS_PROCESS_INFORMATION
    PSSNT_QUERY_VA_CLONE_INFORMATION = 1, // PSS_VA_CLONE_INFORMATION
    PSSNT_QUERY_AUXILIARY_PAGES_INFORMATION = 2, // PSS_AUXILIARY_PAGES_INFORMATION
    PSSNT_QUERY_VA_SPACE_INFORMATION = 3, // PSS_VA_SPACE_INFORMATION
    PSSNT_QUERY_HANDLE_INFORMATION = 4, // PSS_HANDLE_INFORMATION
    PSSNT_QUERY_THREAD_INFORMATION = 5, // PSS_THREAD_INFORMATION
    PSSNT_QUERY_HANDLE_TRACE_INFORMATION = 6, // PSS_HANDLE_TRACE_INFORMATION
    PSSNT_QUERY_PERFORMANCE_COUNTERS = 7 // PSS_PERFORMANCE_COUNTERS
} PSSNT_QUERY_INFORMATION_CLASS;

#define PSSNT_SIGNATURE_PSSD 'PSSD' // 0x50535344

#if (NTDDI_VERSION >= NTDDI_WINBLUE)

// rev
/**
 * The PssNtCaptureSnapshot routine captures a snapshot of the specified process.
 *
 * \param SnapshotHandle Pointer to a variable that receives the snapshot handle.
 * \param ProcessHandle Handle to the process.
 * \param CaptureFlags Flags indicating what to capture.
 * \param ThreadContextFlags Optional flags for capturing thread context.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
PssNtCaptureSnapshot(
    _Out_ PHANDLE SnapshotHandle,
    _In_ HANDLE ProcessHandle,
    _In_ PSSNT_CAPTURE_FLAGS CaptureFlags,
    _In_opt_ ULONG ThreadContextFlags
    );

// rev
/**
 * The PssNtDuplicateSnapshot routine duplicates a process snapshot from one process to another.
 *
 * \param SourceProcessHandle Handle to the source process.
 * \param SnapshotHandle Handle to the snapshot to duplicate.
 * \param TargetProcessHandle Handle to the target process.
 * \param TargetSnapshotHandle Pointer to a variable that receives the duplicated snapshot handle.
 * \param Flags Optional flags for duplication.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
PssNtDuplicateSnapshot(
    _In_ HANDLE SourceProcessHandle,
    _In_ HANDLE SnapshotHandle,
    _In_ HANDLE TargetProcessHandle,
    _Out_ PHANDLE TargetSnapshotHandle,
    _In_opt_ PSSNT_DUPLICATE_FLAGS Flags
    );

// rev
/**
 * The PssNtFreeSnapshot routine frees a snapshot.
 *
 * \param SnapshotHandle Handle to the snapshot to free.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
PssNtFreeSnapshot(
    _In_ HANDLE SnapshotHandle
    );

// rev
/**
 * The PssNtFreeRemoteSnapshot routine frees a remote process snapshot.
 *
 * \param ProcessHandle A handle to the process that contains the snapshot. The handle must have PROCESS_VM_READ, PROCESS_VM_OPERATION, and PROCESS_DUP_HANDLE rights.
 * \param SnapshotHandle Handle to the snapshot to free.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
PssNtFreeRemoteSnapshot(
    _In_ HANDLE ProcessHandle,
    _In_ HANDLE SnapshotHandle
    );

// rev
/**
 * The PssNtQuerySnapshot routine queries information from the specified snapshot.
 *
 * \param SnapshotHandle Handle to the snapshot.
 * \param InformationClass The information class to query.
 * \param Buffer Pointer to a buffer that receives the queried information.
 * \param BufferLength Length of the buffer.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
PssNtQuerySnapshot(
    _In_ HANDLE SnapshotHandle,
    _In_ PSSNT_QUERY_INFORMATION_CLASS InformationClass,
    _Out_writes_bytes_(BufferLength) PVOID Buffer,
    _In_ ULONG BufferLength
    );

// rev
typedef struct _PSSNT_WALK_MARKER_INFO
{
    ULONG Signature; // Win32: 0x4D575350 // 'PSSM'
    HANDLE SectionHandle;
} PSSNT_WALK_MARKER_INFO, *PPSSNT_WALK_MARKER_INFO;
    
// rev
NTSYSAPI
NTSTATUS
NTAPI
PssNtWalkSnapshot(
    _In_ HANDLE SnapshotHandle,
    _In_ ULONG InformationClass,
    _In_ HANDLE WalkMarkerHandle,
    _Out_writes_bytes_(BufferLength) PVOID Buffer,
    _In_ ULONG BufferLength
    );

// rev
NTSYSAPI
NTSTATUS
NTAPI
PssNtFreeWalkMarker(
    _Inout_ PHANDLE WalkMarkerHandle
    );

// rev
NTSYSAPI
NTSTATUS
NTAPI
PssNtValidateDescriptor(
    _In_ HANDLE SnapshotHandle,
    _In_opt_ PVOID ExceptionAddress
    );

#endif /* (NTDDI_VERSION >= NTDDI_WINBLUE) */

// rev
/**
 * Flag indicating the type of bulk information to query.
 */
#define MEMORY_BULK_INFORMATION_FLAG_BASIC 0x00000001

// rev
/**
 * The NTPSS_MEMORY_BULK_INFORMATION structure is used to query basic memory information in bulk for a process.
 */
typedef struct _NTPSS_MEMORY_BULK_INFORMATION
{
    ULONG QueryFlags;
    ULONG NumberOfEntries;
    PVOID NextValidAddress;
} NTPSS_MEMORY_BULK_INFORMATION, *PNTPSS_MEMORY_BULK_INFORMATION;

#if (NTDDI_VERSION >= NTDDI_WIN10_MN)
// rev
/**
 * Captures virtual address space bulk information for a process.
 *
 * @param ProcessHandle Handle to the process.
 * @param BaseAddress Optional base address to start the capture.
 * @param BulkInformation Pointer to the memory bulk information structure.
 * @param BulkInformationLength Length of the memory bulk information structure.
 * @param ReturnLength Optional pointer to a variable that receives the length of the captured information.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtPssCaptureVaSpaceBulk(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID BaseAddress,
    _In_ PNTPSS_MEMORY_BULK_INFORMATION BulkInformation,
    _In_ SIZE_T BulkInformationLength,
    _Out_opt_ PSIZE_T ReturnLength
    );
#endif

#endif

EXTERN_C_END
