#pragma once

#include "../MinDef.h"
#include "../Ke/Ke.h"
#include "JobInfo.h"

EXTERN_C_START

/* phnt */

typedef struct _TEB *PTEB;
typedef struct _PEB *PPEB;

/* NtQueryInformationProcess / NtSetInformationProcess types */
typedef enum _PROCESSINFOCLASS
{
    ProcessBasicInformation,                        // q: PROCESS_BASIC_INFORMATION, PROCESS_EXTENDED_BASIC_INFORMATION
    ProcessQuotaLimits,                             // qs: QUOTA_LIMITS, QUOTA_LIMITS_EX
    ProcessIoCounters,                              // q: IO_COUNTERS
    ProcessVmCounters,                              // q: VM_COUNTERS, VM_COUNTERS_EX, VM_COUNTERS_EX2
    ProcessTimes,                                   // q: KERNEL_USER_TIMES
    ProcessBasePriority,                            // s: KPRIORITY
    ProcessRaisePriority,                           // s: ULONG
    ProcessDebugPort,                               // q: HANDLE
    ProcessExceptionPort,                           // s: PROCESS_EXCEPTION_PORT (requires SeTcbPrivilege)
    ProcessAccessToken,                             // s: PROCESS_ACCESS_TOKEN
    ProcessLdtInformation,                          // qs: PROCESS_LDT_INFORMATION // 10
    ProcessLdtSize,                                 // s: PROCESS_LDT_SIZE
    ProcessDefaultHardErrorMode,                    // qs: ULONG
    ProcessIoPortHandlers,                          // s: PROCESS_IO_PORT_HANDLER_INFORMATION // (kernel-mode only)
    ProcessPooledUsageAndLimits,                    // q: POOLED_USAGE_AND_LIMITS
    ProcessWorkingSetWatch,                         // q: PROCESS_WS_WATCH_INFORMATION[]; s: void
    ProcessUserModeIOPL,                            // qs: ULONG (requires SeTcbPrivilege)
    ProcessEnableAlignmentFaultFixup,               // s: BOOLEAN
    ProcessPriorityClass,                           // qs: PROCESS_PRIORITY_CLASS
    ProcessWx86Information,                         // qs: ULONG (requires SeTcbPrivilege) (VdmAllowed)
    ProcessHandleCount,                             // q: ULONG, PROCESS_HANDLE_INFORMATION // 20
    ProcessAffinityMask,                            // qs: KAFFINITY, qs: GROUP_AFFINITY
    ProcessPriorityBoost,                           // qs: ULONG
    ProcessDeviceMap,                               // qs: PROCESS_DEVICEMAP_INFORMATION, PROCESS_DEVICEMAP_INFORMATION_EX
    ProcessSessionInformation,                      // q: PROCESS_SESSION_INFORMATION
    ProcessForegroundInformation,                   // s: PROCESS_FOREGROUND_BACKGROUND
    ProcessWow64Information,                        // q: ULONG_PTR
    ProcessImageFileName,                           // q: UNICODE_STRING
    ProcessLUIDDeviceMapsEnabled,                   // q: ULONG
    ProcessBreakOnTermination,                      // qs: ULONG
    ProcessDebugObjectHandle,                       // q: HANDLE // 30
    ProcessDebugFlags,                              // qs: ULONG
    ProcessHandleTracing,                           // q: PROCESS_HANDLE_TRACING_QUERY; s: PROCESS_HANDLE_TRACING_ENABLE[_EX] or void to disable
    ProcessIoPriority,                              // qs: IO_PRIORITY_HINT
    ProcessExecuteFlags,                            // qs: ULONG (MEM_EXECUTE_OPTION_*)
    ProcessTlsInformation,                          // qs: PROCESS_TLS_INFORMATION // ProcessResourceManagement
    ProcessCookie,                                  // q: ULONG
    ProcessImageInformation,                        // q: SECTION_IMAGE_INFORMATION
    ProcessCycleTime,                               // q: PROCESS_CYCLE_TIME_INFORMATION // since VISTA
    ProcessPagePriority,                            // qs: PAGE_PRIORITY_INFORMATION
    ProcessInstrumentationCallback,                 // s: PVOID or PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION // 40
    ProcessThreadStackAllocation,                   // s: PROCESS_STACK_ALLOCATION_INFORMATION, PROCESS_STACK_ALLOCATION_INFORMATION_EX
    ProcessWorkingSetWatchEx,                       // q: PROCESS_WS_WATCH_INFORMATION_EX[]; s: void
    ProcessImageFileNameWin32,                      // q: UNICODE_STRING
    ProcessImageFileMapping,                        // q: HANDLE (input)
    ProcessAffinityUpdateMode,                      // qs: PROCESS_AFFINITY_UPDATE_MODE
    ProcessMemoryAllocationMode,                    // qs: PROCESS_MEMORY_ALLOCATION_MODE
    ProcessGroupInformation,                        // q: USHORT[]
    ProcessTokenVirtualizationEnabled,              // s: ULONG
    ProcessConsoleHostProcess,                      // qs: ULONG_PTR // ProcessOwnerInformation
    ProcessWindowInformation,                       // q: PROCESS_WINDOW_INFORMATION // 50
    ProcessHandleInformation,                       // q: PROCESS_HANDLE_SNAPSHOT_INFORMATION // since WIN8
    ProcessMitigationPolicy,                        // s: PROCESS_MITIGATION_POLICY_INFORMATION
    ProcessDynamicFunctionTableInformation,         // s: PROCESS_DYNAMIC_FUNCTION_TABLE_INFORMATION
    ProcessHandleCheckingMode,                      // qs: ULONG; s: 0 disables, otherwise enables
    ProcessKeepAliveCount,                          // q: PROCESS_KEEPALIVE_COUNT_INFORMATION
    ProcessRevokeFileHandles,                       // s: PROCESS_REVOKE_FILE_HANDLES_INFORMATION
    ProcessWorkingSetControl,                       // s: PROCESS_WORKING_SET_CONTROL
    ProcessHandleTable,                             // q: ULONG[] // since WINBLUE
    ProcessCheckStackExtentsMode,                   // qs: ULONG // KPROCESS->CheckStackExtents (CFG)
    ProcessCommandLineInformation,                  // q: UNICODE_STRING // 60
    ProcessProtectionInformation,                   // q: PS_PROTECTION
    ProcessMemoryExhaustion,                        // s: PROCESS_MEMORY_EXHAUSTION_INFO // since THRESHOLD
    ProcessFaultInformation,                        // s: PROCESS_FAULT_INFORMATION
    ProcessTelemetryIdInformation,                  // q: PROCESS_TELEMETRY_ID_INFORMATION
    ProcessCommitReleaseInformation,                // qs: PROCESS_COMMIT_RELEASE_INFORMATION
    ProcessDefaultCpuSetsInformation,               // qs: SYSTEM_CPU_SET_INFORMATION[5] // ProcessReserved1Information
    ProcessAllowedCpuSetsInformation,               // qs: SYSTEM_CPU_SET_INFORMATION[5] // ProcessReserved2Information
    ProcessSubsystemProcess,                        // s: void // EPROCESS->SubsystemProcess
    ProcessJobMemoryInformation,                    // q: PROCESS_JOB_MEMORY_INFO
    ProcessInPrivate,                               // q: BOOLEAN; s: void // ETW // since THRESHOLD2 // 70
    ProcessRaiseUMExceptionOnInvalidHandleClose,    // qs: ULONG; s: 0 disables, otherwise enables
    ProcessIumChallengeResponse,
    ProcessChildProcessInformation,                 // q: PROCESS_CHILD_PROCESS_INFORMATION
    ProcessHighGraphicsPriorityInformation,         // q: BOOLEAN; s: BOOLEAN (requires SeTcbPrivilege)
    ProcessSubsystemInformation,                    // q: SUBSYSTEM_INFORMATION_TYPE // since REDSTONE2
    ProcessEnergyValues,                            // q: PROCESS_ENERGY_VALUES, PROCESS_EXTENDED_ENERGY_VALUES, PROCESS_EXTENDED_ENERGY_VALUES_V1
    ProcessPowerThrottlingState,                    // qs: POWER_THROTTLING_PROCESS_STATE
    ProcessActivityThrottlePolicy,                  // q: PROCESS_ACTIVITY_THROTTLE_POLICY // ProcessReserved3Information
    ProcessWin32kSyscallFilterInformation,          // q: WIN32K_SYSCALL_FILTER
    ProcessDisableSystemAllowedCpuSets,             // s: BOOLEAN // 80
    ProcessWakeInformation,                         // q: PROCESS_WAKE_INFORMATION // (kernel-mode only)
    ProcessEnergyTrackingState,                     // qs: PROCESS_ENERGY_TRACKING_STATE
    ProcessManageWritesToExecutableMemory,          // s: MANAGE_WRITES_TO_EXECUTABLE_MEMORY // since REDSTONE3
    ProcessCaptureTrustletLiveDump,                 // q: ULONG
    ProcessTelemetryCoverage,                       // q: TELEMETRY_COVERAGE_HEADER; s: TELEMETRY_COVERAGE_POINT
    ProcessEnclaveInformation,
    ProcessEnableReadWriteVmLogging,                // qs: PROCESS_READWRITEVM_LOGGING_INFORMATION
    ProcessUptimeInformation,                       // q: PROCESS_UPTIME_INFORMATION
    ProcessImageSection,                            // q: HANDLE
    ProcessDebugAuthInformation,                    // s: CiTool.exe --device-id // PplDebugAuthorization // since RS4 // 90
    ProcessSystemResourceManagement,                // s: PROCESS_SYSTEM_RESOURCE_MANAGEMENT
    ProcessSequenceNumber,                          // q: ULONGLONG
    ProcessLoaderDetour,                            // qs: Obsolete // since RS5
    ProcessSecurityDomainInformation,               // q: PROCESS_SECURITY_DOMAIN_INFORMATION
    ProcessCombineSecurityDomainsInformation,       // s: PROCESS_COMBINE_SECURITY_DOMAINS_INFORMATION
    ProcessEnableLogging,                           // qs: PROCESS_LOGGING_INFORMATION
    ProcessLeapSecondInformation,                   // qs: PROCESS_LEAP_SECOND_INFORMATION
    ProcessFiberShadowStackAllocation,              // s: PROCESS_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION // since 19H1
    ProcessFreeFiberShadowStackAllocation,          // s: PROCESS_FREE_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION
    ProcessAltSystemCallInformation,                // s: PROCESS_SYSCALL_PROVIDER_INFORMATION // since 20H1 // 100
    ProcessDynamicEHContinuationTargets,            // s: PROCESS_DYNAMIC_EH_CONTINUATION_TARGETS_INFORMATION
    ProcessDynamicEnforcedCetCompatibleRanges,      // s: PROCESS_DYNAMIC_ENFORCED_ADDRESS_RANGE_INFORMATION // since 20H2
    ProcessCreateStateChange,                       // s: Obsolete // since WIN11
    ProcessApplyStateChange,                        // s: Obsolete
    ProcessEnableOptionalXStateFeatures,            // s: ULONG64 // EnableProcessOptionalXStateFeatures
    ProcessAltPrefetchParam,                        // qs: OVERRIDE_PREFETCH_PARAMETER // App Launch Prefetch (ALPF) // since 22H1
    ProcessAssignCpuPartitions,                     // s: HANDLE
    ProcessPriorityClassEx,                         // s: PROCESS_PRIORITY_CLASS_EX
    ProcessMembershipInformation,                   // q: PROCESS_MEMBERSHIP_INFORMATION
    ProcessEffectiveIoPriority,                     // q: IO_PRIORITY_HINT // 110
    ProcessEffectivePagePriority,                   // q: ULONG
    ProcessSchedulerSharedData,                     // q: SCHEDULER_SHARED_DATA_SLOT_INFORMATION // since 24H2
    ProcessSlistRollbackInformation,
    ProcessNetworkIoCounters,                       // q: PROCESS_NETWORK_COUNTERS
    ProcessFindFirstThreadByTebValue,               // q: PROCESS_TEB_VALUE_INFORMATION // NtCurrentProcess
    ProcessEnclaveAddressSpaceRestriction,          // qs: // since 25H2
    ProcessAvailableCpus,                           // q: PROCESS_AVAILABLE_CPUS_INFORMATION
    MaxProcessInfoClass
} PROCESSINFOCLASS;

/* NtQueryInformationThread/NtSetInformationThread type */
typedef enum _THREADINFOCLASS
{
    ThreadBasicInformation,                         // q: THREAD_BASIC_INFORMATION
    ThreadTimes,                                    // q: KERNEL_USER_TIMES
    ThreadPriority,                                 // s: KPRIORITY (requires SeIncreaseBasePriorityPrivilege)
    ThreadBasePriority,                             // s: KPRIORITY
    ThreadAffinityMask,                             // s: KAFFINITY
    ThreadImpersonationToken,                       // s: HANDLE
    ThreadDescriptorTableEntry,                     // q: DESCRIPTOR_TABLE_ENTRY (or WOW64_DESCRIPTOR_TABLE_ENTRY)
    ThreadEnableAlignmentFaultFixup,                // s: BOOLEAN
    ThreadEventPair,                                // q: Obsolete
    ThreadQuerySetWin32StartAddress,                // qs: PVOID (requires THREAD_SET_LIMITED_INFORMATION)
    ThreadZeroTlsCell,                              // s: ULONG // TlsIndex // 10
    ThreadPerformanceCount,                         // q: LARGE_INTEGER
    ThreadAmILastThread,                            // q: ULONG
    ThreadIdealProcessor,                           // s: ULONG
    ThreadPriorityBoost,                            // qs: ULONG
    ThreadSetTlsArrayAddress,                       // s: ULONG_PTR
    ThreadIsIoPending,                              // q: ULONG
    ThreadHideFromDebugger,                         // q: BOOLEAN; s: void
    ThreadBreakOnTermination,                       // qs: ULONG
    ThreadSwitchLegacyState,                        // s: void // NtCurrentThread // NPX/FPU
    ThreadIsTerminated,                             // q: ULONG // 20
    ThreadLastSystemCall,                           // q: THREAD_LAST_SYSCALL_INFORMATION
    ThreadIoPriority,                               // qs: IO_PRIORITY_HINT (requires SeIncreaseBasePriorityPrivilege)
    ThreadCycleTime,                                // q: THREAD_CYCLE_TIME_INFORMATION (requires THREAD_QUERY_LIMITED_INFORMATION)
    ThreadPagePriority,                             // qs: PAGE_PRIORITY_INFORMATION
    ThreadActualBasePriority,                       // s: LONG (requires SeIncreaseBasePriorityPrivilege)
    ThreadTebInformation,                           // q: THREAD_TEB_INFORMATION (requires THREAD_GET_CONTEXT + THREAD_SET_CONTEXT)
    ThreadCSwitchMon,                               // q: Obsolete
    ThreadCSwitchPmu,                               // q: Obsolete
    ThreadWow64Context,                             // qs: WOW64_CONTEXT, ARM_NT_CONTEXT since 20H1
    ThreadGroupInformation,                         // qs: GROUP_AFFINITY // 30
    ThreadUmsInformation,                           // q: THREAD_UMS_INFORMATION // Obsolete
    ThreadCounterProfiling,                         // q: BOOLEAN; s: THREAD_PROFILING_INFORMATION?
    ThreadIdealProcessorEx,                         // qs: PROCESSOR_NUMBER; s: previous PROCESSOR_NUMBER on return
    ThreadCpuAccountingInformation,                 // q: BOOLEAN; s: HANDLE (NtOpenSession) // NtCurrentThread // since WIN8
    ThreadSuspendCount,                             // q: ULONG // since WINBLUE
    ThreadHeterogeneousCpuPolicy,                   // q: KHETERO_CPU_POLICY // since THRESHOLD
    ThreadContainerId,                              // q: GUID
    ThreadNameInformation,                          // qs: THREAD_NAME_INFORMATION (requires THREAD_SET_LIMITED_INFORMATION)
    ThreadSelectedCpuSets,                          // q: ULONG[]
    ThreadSystemThreadInformation,                  // q: SYSTEM_THREAD_INFORMATION // 40
    ThreadActualGroupAffinity,                      // q: GROUP_AFFINITY // since THRESHOLD2
    ThreadDynamicCodePolicyInfo,                    // q: ULONG; s: ULONG (NtCurrentThread)
    ThreadExplicitCaseSensitivity,                  // qs: ULONG; s: 0 disables, otherwise enables // (requires SeDebugPrivilege and PsProtectedSignerAntimalware)
    ThreadWorkOnBehalfTicket,                       // q: ALPC_WORK_ON_BEHALF_TICKET // RTL_WORK_ON_BEHALF_TICKET_EX // NtCurrentThread
    ThreadSubsystemInformation,                     // q: SUBSYSTEM_INFORMATION_TYPE // since REDSTONE2
    ThreadDbgkWerReportActive,                      // s: ULONG; s: 0 disables, otherwise enables
    ThreadAttachContainer,                          // s: HANDLE (job object) // NtCurrentThread
    ThreadManageWritesToExecutableMemory,           // s: MANAGE_WRITES_TO_EXECUTABLE_MEMORY // since REDSTONE3
    ThreadPowerThrottlingState,                     // qs: POWER_THROTTLING_THREAD_STATE // since REDSTONE3 (set), WIN11 22H2 (query)
    ThreadWorkloadClass,                            // q: THREAD_WORKLOAD_CLASS // since REDSTONE5 // 50
    ThreadCreateStateChange,                        // q: // since WIN11
    ThreadApplyStateChange,
    ThreadStrongerBadHandleChecks,                  // s: ULONG // NtCurrentThread // since 22H1
    ThreadEffectiveIoPriority,                      // q: IO_PRIORITY_HINT
    ThreadEffectivePagePriority,                    // q: ULONG
    ThreadUpdateLockOwnership,                      // THREAD_LOCK_OWNERSHIP // since 24H2
    ThreadSchedulerSharedDataSlot,                  // SCHEDULER_SHARED_DATA_SLOT_INFORMATION
    ThreadTebInformationAtomic,                     // q: THREAD_TEB_INFORMATION (requires THREAD_GET_CONTEXT + THREAD_QUERY_INFORMATION)
    ThreadIndexInformation,                         // THREAD_INDEX_INFORMATION
    MaxThreadInfoClass
} THREADINFOCLASS;

#if !defined(_KERNEL_MODE)
// Use with both ProcessPagePriority and ThreadPagePriority
typedef struct _PAGE_PRIORITY_INFORMATION
{
    ULONG PagePriority;
} PAGE_PRIORITY_INFORMATION, *PPAGE_PRIORITY_INFORMATION;

// Process information structures

/**
 * The PROCESS_BASIC_INFORMATION structure contains basic information about a process.
 * 
 * @remarks https://learn.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-ntqueryinformationprocess#process_basic_information
 */
typedef struct _PROCESS_BASIC_INFORMATION
{
    NTSTATUS ExitStatus;                    // The exit status of the process. (GetExitCodeProcess)
    PPEB PebBaseAddress;                    // A pointer to the process environment block (PEB) of the process.
    KAFFINITY AffinityMask;                 // The affinity mask of the process. (GetProcessAffinityMask) (deprecated)
    KPRIORITY BasePriority;                 // The base priority of the process. (GetPriorityClass)
    HANDLE UniqueProcessId;                 // The unique identifier of the process. (GetProcessId)
    HANDLE InheritedFromUniqueProcessId;    // The unique identifier of the parent process.
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

/**
 * The PROCESS_EXTENDED_BASIC_INFORMATION structure contains extended basic information about a process.
 */
typedef struct _PROCESS_EXTENDED_BASIC_INFORMATION {
    _In_ SIZE_T Size;
    union
    {
        PROCESS_BASIC_INFORMATION BasicInfo;
        struct
        {
            NTSTATUS ExitStatus;                    // The exit status of the process. (GetExitCodeProcess)
            PPEB PebBaseAddress;                    // A pointer to the process environment block (PEB) of the process.
            KAFFINITY AffinityMask;                 // The affinity mask of the process. (GetProcessAffinityMask) (deprecated)
            KPRIORITY BasePriority;                 // The base priority of the process. (GetPriorityClass)
            HANDLE UniqueProcessId;                 // The unique identifier of the process. (GetProcessId)
            HANDLE InheritedFromUniqueProcessId;    // The unique identifier of the parent process.
        };
    };
    union
    {
        struct {
            ULONG IsProtectedProcess : 1;
            ULONG IsWow64Process : 1;
            ULONG IsProcessDeleting : 1;
            ULONG IsCrossSessionCreate : 1;
            ULONG IsFrozen : 1;
            ULONG IsBackground : 1; // WIN://BGKD
            ULONG IsStronglyNamed : 1; // WIN://SYSAPPID
            ULONG IsSecureProcess : 1;
            ULONG IsSubsystemProcess : 1;
            ULONG IsTrustedApp : 1;
            ULONG SpareBits : 22;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_EXTENDED_BASIC_INFORMATION, *PPROCESS_EXTENDED_BASIC_INFORMATION;

/**
 * The VM_COUNTERS structure contains various memory usage statistics for a process.
 *
 * @remarks https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-process_memory_counters
 */
typedef struct _VM_COUNTERS
{
    SIZE_T PeakVirtualSize;             // The peak virtual address space size of this process, in bytes.
    SIZE_T VirtualSize;                 // The virtual address space size of this process, in bytes.
    ULONG PageFaultCount;               // The number of page faults.
    SIZE_T PeakWorkingSetSize;          // The peak working set size, in bytes.
    SIZE_T WorkingSetSize;              // The current working set size, in bytes
    SIZE_T QuotaPeakPagedPoolUsage;     // The peak paged pool usage, in bytes.
    SIZE_T QuotaPagedPoolUsage;         // The current paged pool usage, in bytes.
    SIZE_T QuotaPeakNonPagedPoolUsage;  // The peak non-paged pool usage, in bytes.
    SIZE_T QuotaNonPagedPoolUsage;      // The current non-paged pool usage, in bytes.
    SIZE_T PagefileUsage;               // The Commit Charge value in bytes for this process. Commit Charge is the total amount of private memory that the memory manager has committed for a running process.
    SIZE_T PeakPagefileUsage;           // The peak value in bytes of the Commit Charge during the lifetime of this process.
} VM_COUNTERS, *PVM_COUNTERS;

/**
 * The VM_COUNTERS_EX structure extends VM_COUNTERS to include private memory usage.
 *
 * @remarks https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-process_memory_counters_ex2
 */
typedef struct _VM_COUNTERS_EX
{
    SIZE_T PeakVirtualSize;             // The peak virtual address space size of this process, in bytes.
    SIZE_T VirtualSize;                 // The virtual address space size of this process, in bytes.
    ULONG PageFaultCount;               // The number of page faults.
    SIZE_T PeakWorkingSetSize;          // The peak working set size, in bytes.
    SIZE_T WorkingSetSize;              // The current working set size, in bytes
    SIZE_T QuotaPeakPagedPoolUsage;     // The peak paged pool usage, in bytes.
    SIZE_T QuotaPagedPoolUsage;         // The current paged pool usage, in bytes.
    SIZE_T QuotaPeakNonPagedPoolUsage;  // The peak non-paged pool usage, in bytes.
    SIZE_T QuotaNonPagedPoolUsage;      // The current non-paged pool usage, in bytes.
    SIZE_T PagefileUsage;               // The Commit Charge value in bytes for this process. Commit Charge is the total amount of private memory that the memory manager has committed for a running process.
    SIZE_T PeakPagefileUsage;           // The peak value in bytes of the Commit Charge during the lifetime of this process.
    SIZE_T PrivateUsage;                // Same as PagefileUsage. The Commit Charge value in bytes for this process. Commit Charge is the total amount of private memory that the memory manager has committed for a running process.
} VM_COUNTERS_EX, *PVM_COUNTERS_EX;

/**
 * The VM_COUNTERS_EX2 structure extends VM_COUNTERS_EX to include private working set size and shared commit usage.
 *
 * @remarks https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-process_memory_counters_ex2
 */
typedef struct _VM_COUNTERS_EX2
{
    union
    {
        VM_COUNTERS_EX CountersEx;
        struct
        {
            SIZE_T PeakVirtualSize;             // The peak virtual address space size of this process, in bytes.
            SIZE_T VirtualSize;                 // The virtual address space size of this process, in bytes.
            ULONG PageFaultCount;               // The number of page faults.
            SIZE_T PeakWorkingSetSize;          // The peak working set size, in bytes.
            SIZE_T WorkingSetSize;              // The current working set size, in bytes
            SIZE_T QuotaPeakPagedPoolUsage;     // The peak paged pool usage, in bytes.
            SIZE_T QuotaPagedPoolUsage;         // The current paged pool usage, in bytes.
            SIZE_T QuotaPeakNonPagedPoolUsage;  // The peak non-paged pool usage, in bytes.
            SIZE_T QuotaNonPagedPoolUsage;      // The current non-paged pool usage, in bytes.
            SIZE_T PagefileUsage;               // The Commit Charge value in bytes for this process. Commit Charge is the total amount of private memory that the memory manager has committed for a running process.
            SIZE_T PeakPagefileUsage;           // The peak value in bytes of the Commit Charge during the lifetime of this process.
            SIZE_T PrivateUsage;                // Same as PagefileUsage. The Commit Charge value in bytes for this process. Commit Charge is the total amount of private memory that the memory manager has committed for a running process.
        };
    };
    SIZE_T PrivateWorkingSetSize;               // The current private working set size, in bytes.
    SIZE_T SharedCommitUsage;                   // The current shared commit usage, in bytes.
} VM_COUNTERS_EX2, *PVM_COUNTERS_EX2;

/**
 * The KERNEL_USER_TIMES structure contains timing information for a process or thread.
 * 
 * @remarks https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getthreadtimes
 */
typedef struct _KERNEL_USER_TIMES
{
    LARGE_INTEGER CreateTime;        // The creation time of the process or thread.
    LARGE_INTEGER ExitTime;          // The exit time of the process or thread.
    LARGE_INTEGER KernelTime;        // The amount of time the process has executed in kernel mode.
    LARGE_INTEGER UserTime;          // The amount of time the process has executed in user mode.
} KERNEL_USER_TIMES, *PKERNEL_USER_TIMES;

/**
 * The POOLED_USAGE_AND_LIMITS structure contains information about the usage and limits of paged and non-paged pool memory.
 */
typedef struct _POOLED_USAGE_AND_LIMITS
{
    SIZE_T PeakPagedPoolUsage;       // The peak paged pool usage.
    SIZE_T PagedPoolUsage;           // The current paged pool usage.
    SIZE_T PagedPoolLimit;           // The limit on paged pool usage.
    SIZE_T PeakNonPagedPoolUsage;    // The peak non-paged pool usage.
    SIZE_T NonPagedPoolUsage;        // The current non-paged pool usage.
    SIZE_T NonPagedPoolLimit;        // The limit on non-paged pool usage.
    SIZE_T PeakPagefileUsage;        // The peak pagefile usage.
    SIZE_T PagefileUsage;            // The current pagefile usage.
    SIZE_T PagefileLimit;            // The limit on pagefile usage.
} POOLED_USAGE_AND_LIMITS, *PPOOLED_USAGE_AND_LIMITS;

#define PROCESS_EXCEPTION_PORT_ALL_STATE_BITS 0x00000003
#define PROCESS_EXCEPTION_PORT_ALL_STATE_FLAGS ((ULONG_PTR)((1UL << PROCESS_EXCEPTION_PORT_ALL_STATE_BITS) - 1))

/**
 * The PROCESS_EXCEPTION_PORT structure is used to manage exception ports for a process.
 */
typedef struct _PROCESS_EXCEPTION_PORT
{
    //
    // Handle to the exception port. No particular access required.
    //
    _In_ HANDLE ExceptionPortHandle;

    //
    // Miscellaneous state flags to be cached along with the exception
    // port in the kernel.
    //
    _Inout_ ULONG StateFlags;

} PROCESS_EXCEPTION_PORT, *PPROCESS_EXCEPTION_PORT;

/**
 * The PROCESS_ACCESS_TOKEN structure is used to manage the security context of a process or thread.
 * 
 * A process's access token can only be changed if the process has no threads or a single thread that has not yet begun execution.
 */
typedef struct _PROCESS_ACCESS_TOKEN
{
    //
    // Handle to Primary token to assign to the process.
    // TOKEN_ASSIGN_PRIMARY access to this token is needed.
    //
    HANDLE Token;
    
    //
    // Handle to the initial thread of the process.
    // THREAD_QUERY_INFORMATION access to this thread is needed.
    //
    // N.B. This field is unused.
    //
    HANDLE Thread;

} PROCESS_ACCESS_TOKEN, *PPROCESS_ACCESS_TOKEN;

/**
 * The PROCESS_LDT_INFORMATION structure is used to manage Local Descriptor Table (LDT) entries for a process.
 */
typedef struct _PROCESS_LDT_INFORMATION
{
    ULONG Start;
    ULONG Length;
    LDT_ENTRY LdtEntries[1];
} PROCESS_LDT_INFORMATION, *PPROCESS_LDT_INFORMATION;

/**
 * The PROCESS_LDT_SIZE structure is used to specify the size of the Local Descriptor Table (LDT) for a process.
 */
typedef struct _PROCESS_LDT_SIZE
{
    ULONG Length;
} PROCESS_LDT_SIZE, *PPROCESS_LDT_SIZE;

/**
 * The PROCESS_WS_WATCH_INFORMATION structure is used to store information about working set watch events for a process.
 *
 * @remarks https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-psapi_ws_watch_information
 */
typedef struct _PROCESS_WS_WATCH_INFORMATION
{
    PVOID FaultingPc; // A pointer to the instruction that caused the page fault.
    PVOID FaultingVa; // A pointer to the page that was added to the working set.
} PROCESS_WS_WATCH_INFORMATION, *PPROCESS_WS_WATCH_INFORMATION;

#endif

/**
 * The PROCESS_WS_WATCH_INFORMATION_EX structure contains extended information about a page added to a process working set.
 *
 * @remarks https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-psapi_ws_watch_information_ex
 */
typedef struct _PROCESS_WS_WATCH_INFORMATION_EX
{
    union
    {
        PROCESS_WS_WATCH_INFORMATION BasicInfo;
        struct
        {
            PVOID FaultingPc;   // The address of the instruction that caused the page fault.
            PVOID FaultingVa;   // The virtual address that caused the page fault.
        };
    };
    HANDLE FaultingThreadId;    // The identifier of the thread that caused the page fault.
    ULONG_PTR Flags;            // This member is reserved for future use.
} PROCESS_WS_WATCH_INFORMATION_EX, *PPROCESS_WS_WATCH_INFORMATION_EX;

#define PROCESS_PRIORITY_CLASS_UNKNOWN 0
#define PROCESS_PRIORITY_CLASS_IDLE 1
#define PROCESS_PRIORITY_CLASS_NORMAL 2
#define PROCESS_PRIORITY_CLASS_HIGH 3
#define PROCESS_PRIORITY_CLASS_REALTIME 4
#define PROCESS_PRIORITY_CLASS_BELOW_NORMAL 5
#define PROCESS_PRIORITY_CLASS_ABOVE_NORMAL 6

/**
 * The PROCESS_PRIORITY_CLASS structure is used to manage the priority class of a process.
 */
typedef struct _PROCESS_PRIORITY_CLASS
{
    BOOLEAN Foreground;
    UCHAR PriorityClass;
} PROCESS_PRIORITY_CLASS, *PPROCESS_PRIORITY_CLASS;

/**
 * The PROCESS_PRIORITY_CLASS_EX structure extends PROCESS_PRIORITY_CLASS to include validity flags.
 */
typedef struct _PROCESS_PRIORITY_CLASS_EX
{
    union
    {
        struct
        {
            USHORT ForegroundValid : 1;
            USHORT PriorityClassValid : 1;
        };
        USHORT AllFlags;
    };
    UCHAR PriorityClass;
    BOOLEAN Foreground;
} PROCESS_PRIORITY_CLASS_EX, *PPROCESS_PRIORITY_CLASS_EX;

/**
 * The PROCESS_FOREGROUND_BACKGROUND structure is used to manage the priority class of a process, specifically whether it runs in the foreground or background.
 */
typedef struct _PROCESS_FOREGROUND_BACKGROUND
{
    BOOLEAN Foreground;
} PROCESS_FOREGROUND_BACKGROUND, *PPROCESS_FOREGROUND_BACKGROUND;

#if !defined(_KERNEL_MODE)

/**
 * The PROCESS_DEVICEMAP_INFORMATION structure contains information about a process's device map.
 */
typedef struct _PROCESS_DEVICEMAP_INFORMATION
{
    union
    {
        struct
        {
            HANDLE DirectoryHandle; // A handle to a directory object that can be set as the new device map for the process. This handle must have DIRECTORY_TRAVERSE access.
        } Set;
        struct
        {
            ULONG DriveMap;         // A bitmask that indicates which drive letters are currently in use in the process's device map.
            UCHAR DriveType[32];    // A value that indicates the type of each drive (e.g., local disk, network drive, etc.). // DRIVE_* WinBase.h
        } Query;
    };
} PROCESS_DEVICEMAP_INFORMATION, *PPROCESS_DEVICEMAP_INFORMATION;

/**
 * The PROCESS_LUID_DOSDEVICES_ONLY flag limits the device map to only devices from the current process or logon session.
 */
#define PROCESS_LUID_DOSDEVICES_ONLY 0x00000001

/**
 * The _PROCESS_DEVICEMAP_INFORMATION_EX structure contains information about a process's device map.
 */
typedef struct _PROCESS_DEVICEMAP_INFORMATION_EX
{
    union
    {
        struct
        {
            HANDLE DirectoryHandle; // A handle to a directory object that can be set as the new device map for the process. This handle must have DIRECTORY_TRAVERSE access.
        } Set;
        struct
        {
            ULONG DriveMap;         // A bitmask that indicates which drive letters are currently in use in the process's device map.
            UCHAR DriveType[32];    // A value that indicates the type of each drive (e.g., local disk, network drive, etc.). // DRIVE_* WinBase.h
        } Query;
    };
    ULONG Flags; // PROCESS_LUID_DOSDEVICES_ONLY
} PROCESS_DEVICEMAP_INFORMATION_EX, *PPROCESS_DEVICEMAP_INFORMATION_EX;

/**
 * The PROCESS_SESSION_INFORMATION structure is used to store information about the session ID of a process.
 */
typedef struct _PROCESS_SESSION_INFORMATION
{
    ULONG SessionId;
} PROCESS_SESSION_INFORMATION, *PPROCESS_SESSION_INFORMATION;

#define PROCESS_HANDLE_EXCEPTIONS_ENABLED 0x00000001
#define PROCESS_HANDLE_RAISE_EXCEPTION_ON_INVALID_HANDLE_CLOSE_DISABLED 0x00000000
#define PROCESS_HANDLE_RAISE_EXCEPTION_ON_INVALID_HANDLE_CLOSE_ENABLED 0x00000001

/**
 * The PROCESS_HANDLE_TRACING_ENABLE structure is used to enable handle tracing for a process.
 */
typedef struct _PROCESS_HANDLE_TRACING_ENABLE
{
    ULONG Flags;        // Flags that control handle tracing.
} PROCESS_HANDLE_TRACING_ENABLE, *PPROCESS_HANDLE_TRACING_ENABLE;

/**
 * The PROCESS_HANDLE_TRACING_MAX_SLOTS macro specifies the maximum number of slots.
 */
#define PROCESS_HANDLE_TRACING_MAX_SLOTS 0x20000

/**
 * The PROCESS_HANDLE_TRACING_ENABLE_EX structure extends PROCESS_HANDLE_TRACING_ENABLE to include the total number of slots.
 */
typedef struct _PROCESS_HANDLE_TRACING_ENABLE_EX
{
    ULONG Flags;        // Flags that control handle tracing.
    ULONG TotalSlots;   // Total number of handle tracing slots.
} PROCESS_HANDLE_TRACING_ENABLE_EX, *PPROCESS_HANDLE_TRACING_ENABLE_EX;

#define PROCESS_HANDLE_TRACE_TYPE_OPEN 1
#define PROCESS_HANDLE_TRACE_TYPE_CLOSE 2
#define PROCESS_HANDLE_TRACE_TYPE_BADREF 3

/**
 * The PROCESS_HANDLE_TRACING_ENTRY structure contains information about the handle operation associated with the event.
 */
typedef struct _PROCESS_HANDLE_TRACING_ENTRY
{
    HANDLE Handle;          // The handle associated with the event.
    CLIENT_ID ClientId;     // The process and thread associated with the event.
    ULONG Type;             // The type of handle operation associated with the event.
    PVOID Stacks[16];
} PROCESS_HANDLE_TRACING_ENTRY, *PPROCESS_HANDLE_TRACING_ENTRY;

/**
 * The PROCESS_HANDLE_TRACING_QUERY structure is used to query all handle events or a specific handle event for a process.
 */
typedef struct _PROCESS_HANDLE_TRACING_QUERY
{
    _In_opt_ HANDLE Handle;
    _Out_ ULONG TotalTraces;
    _Out_ _Field_size_(TotalTraces) PROCESS_HANDLE_TRACING_ENTRY HandleTrace[1];
} PROCESS_HANDLE_TRACING_QUERY, *PPROCESS_HANDLE_TRACING_QUERY;

#endif

/**
 * The THREAD_TLS_INFORMATION structure contains information about the Thread Local Storage (TLS) data for a thread.
 */
typedef struct _THREAD_TLS_INFORMATION
{
    ULONG Flags;         // Flags that provide additional information about the TLS data.
    PVOID NewTlsData;    // Pointer to the new TLS data.
    PVOID OldTlsData;    // Pointer to the old TLS data.
    HANDLE ThreadId;     // Handle to the thread associated with the TLS data.
} THREAD_TLS_INFORMATION, *PTHREAD_TLS_INFORMATION;

/**
 * The PROCESS_TLS_INFORMATION_TYPE enumeration defines the types of TLS operations that can be performed on a process.
 */
typedef enum _PROCESS_TLS_INFORMATION_TYPE
{
    ProcessTlsReplaceIndex,     // Replace the TLS index.
    ProcessTlsReplaceVector,    // Replace the TLS vector.
    MaxProcessTlsOperation      // Maximum value for the enumeration.
} PROCESS_TLS_INFORMATION_TYPE, *PPROCESS_TLS_INFORMATION_TYPE;

/**
 * The PROCESS_TLS_INFORMATION structure contains information about the TLS operations for a process.
 */
typedef struct _PROCESS_TLS_INFORMATION
{
    ULONG Flags;                // Flags that provide additional information about the TLS operation.
    ULONG OperationType;        // The type of TLS operation to be performed.
    ULONG ThreadDataCount;      // The number of THREAD_TLS_INFORMATION structures in the ThreadData array.
    ULONG TlsIndex;             // The TLS index to be replaced.
    ULONG PreviousCount;        // The previous count of TLS data.
    _Field_size_(ThreadDataCount) THREAD_TLS_INFORMATION ThreadData[1]; // Array of THREAD_TLS_INFORMATION structures.
} PROCESS_TLS_INFORMATION, *PPROCESS_TLS_INFORMATION;

/**
 * The PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION structure contains information about the instrumentation callback for a process.
 */
typedef struct _PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION
{
    ULONG Version;  // The version of the instrumentation callback information.
    ULONG Reserved; // Reserved for future use.
    PVOID Callback; // Pointer to the callback function.
} PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION, *PPROCESS_INSTRUMENTATION_CALLBACK_INFORMATION;

/**
 * The PROCESS_STACK_ALLOCATION_INFORMATION structure contains information about the stack allocation for a process.
 */
typedef struct _PROCESS_STACK_ALLOCATION_INFORMATION
{
    SIZE_T ReserveSize; // The size of the stack to be reserved.
    SIZE_T ZeroBits;    // The number of zero bits in the stack base address.
    PVOID StackBase;    // Pointer to the base of the stack.
} PROCESS_STACK_ALLOCATION_INFORMATION, *PPROCESS_STACK_ALLOCATION_INFORMATION;

/**
 * The PROCESS_STACK_ALLOCATION_INFORMATION_EX structure extends PROCESS_STACK_ALLOCATION_INFORMATION to include additional fields.
 */
typedef struct _PROCESS_STACK_ALLOCATION_INFORMATION_EX
{
    ULONG PreferredNode; // The preferred NUMA node for the stack allocation.
    ULONG Reserved0;     // Reserved for future use.
    ULONG Reserved1;     // Reserved for future use.
    ULONG Reserved2;     // Reserved for future use.
    PROCESS_STACK_ALLOCATION_INFORMATION AllocInfo; // The stack allocation information.
} PROCESS_STACK_ALLOCATION_INFORMATION_EX, *PPROCESS_STACK_ALLOCATION_INFORMATION_EX;

/**
 * The PROCESS_AFFINITY_UPDATE_MODE union is used to specify the affinity update mode for a process.
 */
typedef struct _PROCESS_AFFINITY_UPDATE_MODE
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG EnableAutoUpdate : 1; // Indicates whether auto-update of affinity is enabled.
            ULONG Permanent : 1;        // Indicates whether the affinity update is permanent.
            ULONG Reserved : 30;        // Reserved for future use.
        };
    };
} PROCESS_AFFINITY_UPDATE_MODE, *PPROCESS_AFFINITY_UPDATE_MODE;

/**
 * The PROCESS_MEMORY_ALLOCATION_MODE union is used to specify the memory allocation mode for a process.
 */
typedef struct _PROCESS_MEMORY_ALLOCATION_MODE
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG TopDown : 1;      // Indicates whether memory allocation should be top-down.
            ULONG Reserved : 31;    // Reserved for future use.
        };
    };
} PROCESS_MEMORY_ALLOCATION_MODE, *PPROCESS_MEMORY_ALLOCATION_MODE;

/**
 * The PROCESS_HANDLE_INFORMATION structure contains information about the handles of a process.
 */
typedef struct _PROCESS_HANDLE_INFORMATION
{
    ULONG HandleCount;              // The number of handles in the process.
    ULONG HandleCountHighWatermark; // The highest number of handles that the process has had.
} PROCESS_HANDLE_INFORMATION, *PPROCESS_HANDLE_INFORMATION;

/**
 * The PROCESS_CYCLE_TIME_INFORMATION structure contains information about the cycle time of a process.
 */
typedef struct _PROCESS_CYCLE_TIME_INFORMATION
{
    ULONGLONG AccumulatedCycles; // The total number of cycles accumulated by the process.
    ULONGLONG CurrentCycleCount; // The current cycle count of the process.
} PROCESS_CYCLE_TIME_INFORMATION, *PPROCESS_CYCLE_TIME_INFORMATION;

/**
 * The PROCESS_WINDOW_INFORMATION structure contains information about the windows of a process.
 */
typedef struct _PROCESS_WINDOW_INFORMATION
{
    ULONG WindowFlags;          // Flags that provide information about the window.
    USHORT WindowTitleLength;   // The length of the window title.
    _Field_size_bytes_(WindowTitleLength) WCHAR WindowTitle[1]; // The title of the window.
} PROCESS_WINDOW_INFORMATION, *PPROCESS_WINDOW_INFORMATION;

/**
 * The PROCESS_HANDLE_TABLE_ENTRY_INFO structure contains information about a handle table entry of a process.
 */
typedef struct _PROCESS_HANDLE_TABLE_ENTRY_INFO
{
    HANDLE HandleValue;         // The value of the handle.
    SIZE_T HandleCount;         // The number of references to the handle.
    SIZE_T PointerCount;        // The number of pointers to the handle.
    ACCESS_MASK GrantedAccess;  // The access rights granted to the handle.
    ULONG ObjectTypeIndex;      // The index of the object type.
    ULONG HandleAttributes;     // The attributes of the handle.
    ULONG Reserved;             // Reserved for future use.
} PROCESS_HANDLE_TABLE_ENTRY_INFO, *PPROCESS_HANDLE_TABLE_ENTRY_INFO;

/**
 * The PROCESS_HANDLE_SNAPSHOT_INFORMATION structure contains information about the handle snapshot of a process.
 */
typedef struct _PROCESS_HANDLE_SNAPSHOT_INFORMATION
{
    ULONG_PTR NumberOfHandles;
    ULONG_PTR Reserved;
    _Field_size_(NumberOfHandles) PROCESS_HANDLE_TABLE_ENTRY_INFO Handles[1];
} PROCESS_HANDLE_SNAPSHOT_INFORMATION, *PPROCESS_HANDLE_SNAPSHOT_INFORMATION;

#if !defined(_KERNEL_MODE)

#if !defined(NTDDI_WIN10_FE) || (NTDDI_VERSION < NTDDI_WIN10_FE)
typedef struct _PROCESS_MITIGATION_REDIRECTION_TRUST_POLICY
{
    union {
        ULONG Flags;
        struct {
            ULONG EnforceRedirectionTrust : 1;
            ULONG AuditRedirectionTrust : 1;
            ULONG ReservedFlags : 30;
        };
    };
} PROCESS_MITIGATION_REDIRECTION_TRUST_POLICY, *PPROCESS_MITIGATION_REDIRECTION_TRUST_POLICY;
#endif

#if !defined(NTDDI_WIN10_NI) || (NTDDI_VERSION < NTDDI_WIN10_NI)
typedef struct _PROCESS_MITIGATION_USER_POINTER_AUTH_POLICY {
    union {
        ULONG Flags;
        struct {
            ULONG EnablePointerAuthUserIp : 1;
            ULONG ReservedFlags : 31;
        };
    };
} PROCESS_MITIGATION_USER_POINTER_AUTH_POLICY, *PPROCESS_MITIGATION_USER_POINTER_AUTH_POLICY;

typedef struct _PROCESS_MITIGATION_SEHOP_POLICY {
    union {
        ULONG Flags;
        struct {
            ULONG EnableSehop : 1;
            ULONG ReservedFlags : 31;
        };
    };
} PROCESS_MITIGATION_SEHOP_POLICY, *PPROCESS_MITIGATION_SEHOP_POLICY;
#endif

typedef struct _PROCESS_MITIGATION_ACTIVATION_CONTEXT_TRUST_POLICY2
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG AssemblyManifestRedirectionTrust : 1;
            ULONG ReservedFlags : 31;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_ACTIVATION_CONTEXT_TRUST_POLICY2, *PPROCESS_MITIGATION_ACTIVATION_CONTEXT_TRUST_POLICY2;

// enum PROCESS_MITIGATION_POLICY
#define PROCESS_MITIGATION_POLICY ULONG
#define ProcessDEPPolicy 0                      // The data execution prevention (DEP) policy of the process.
#define ProcessASLRPolicy 1                     // The Address Space Layout Randomization (ASLR) policy of the process.
#define ProcessDynamicCodePolicy 2              // Disables the ability to generate dynamic code or modify existing executable code.
#define ProcessStrictHandleCheckPolicy 3        // Enables the ability to generate a fatal error if the process manipulates an invalid handle.
#define ProcessSystemCallDisablePolicy 4        // Disables the ability of the process to use NTUser/GDI functions at the lowest layer.
#define ProcessMitigationOptionsMask 5          // Returns the mask of valid bits for all the mitigation options on the system.
#define ProcessExtensionPointDisablePolicy 6    // Disables the ability of the process to load legacy extension point DLLs.
#define ProcessControlFlowGuardPolicy 7
#define ProcessSignaturePolicy 8                // Disables the ability of the process to load images not signed by Microsoft, the Windows Store and the Windows Hardware Quality Labs (WHQL).
#define ProcessFontDisablePolicy 9              // Disables the ability of the process to load non-system fonts.
#define ProcessImageLoadPolicy 10               // Disables the ability of the process to load images from some locations, such a remote devices or files that have the low mandatory label.
#define ProcessSystemCallFilterPolicy 11
#define ProcessPayloadRestrictionPolicy 12
#define ProcessChildProcessPolicy 13            // Disables the ability to create child processes.
#define ProcessSideChannelIsolationPolicy 14
#define ProcessUserShadowStackPolicy 15         // since 20H1
#define ProcessRedirectionTrustPolicy 16
#define ProcessUserPointerAuthPolicy 17
#define ProcessSEHOPPolicy 18
#define ProcessActivationContextTrustPolicy 19
#define MaxProcessMitigationPolicy 20

/**
 * The PROCESS_MITIGATION_POLICY_INFORMATION structure represents the different process mitigation policies.
 *
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/winnt/ne-winnt-process_mitigation_policy
 */
typedef struct _PROCESS_MITIGATION_POLICY_INFORMATION
{
    PROCESS_MITIGATION_POLICY Policy;
    union
    {
        PROCESS_MITIGATION_ASLR_POLICY ASLRPolicy;
        PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY StrictHandleCheckPolicy;
        PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY SystemCallDisablePolicy;
        PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY ExtensionPointDisablePolicy;
        PROCESS_MITIGATION_DYNAMIC_CODE_POLICY DynamicCodePolicy;
        PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY ControlFlowGuardPolicy;
        PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY SignaturePolicy;
        PROCESS_MITIGATION_FONT_DISABLE_POLICY FontDisablePolicy;
        PROCESS_MITIGATION_IMAGE_LOAD_POLICY ImageLoadPolicy;
        PROCESS_MITIGATION_SYSTEM_CALL_FILTER_POLICY SystemCallFilterPolicy;
        PROCESS_MITIGATION_PAYLOAD_RESTRICTION_POLICY PayloadRestrictionPolicy;
        PROCESS_MITIGATION_CHILD_PROCESS_POLICY ChildProcessPolicy;
        PROCESS_MITIGATION_SIDE_CHANNEL_ISOLATION_POLICY SideChannelIsolationPolicy;
        PROCESS_MITIGATION_USER_SHADOW_STACK_POLICY UserShadowStackPolicy;
        PROCESS_MITIGATION_REDIRECTION_TRUST_POLICY RedirectionTrustPolicy;
        PROCESS_MITIGATION_USER_POINTER_AUTH_POLICY UserPointerAuthPolicy;
        PROCESS_MITIGATION_SEHOP_POLICY SEHOPPolicy;
        PROCESS_MITIGATION_ACTIVATION_CONTEXT_TRUST_POLICY2 ActivationContextTrustPolicy;
    };
} PROCESS_MITIGATION_POLICY_INFORMATION, *PPROCESS_MITIGATION_POLICY_INFORMATION;

// private
typedef struct _DYNAMIC_FUNCTION_TABLE DYNAMIC_FUNCTION_TABLE, *PDYNAMIC_FUNCTION_TABLE;

/**
 * The PROCESS_DYNAMIC_FUNCTION_TABLE_INFORMATION structure is used to manage dynamic function tables for a process.
 */
typedef struct _PROCESS_DYNAMIC_FUNCTION_TABLE_INFORMATION
{
    PDYNAMIC_FUNCTION_TABLE DynamicFunctionTable; // Pointer to the dynamic function table.
    BOOLEAN Remove; // Indicates whether to remove (TRUE) or add (FALSE) the dynamic function table.
} PROCESS_DYNAMIC_FUNCTION_TABLE_INFORMATION, *PPROCESS_DYNAMIC_FUNCTION_TABLE_INFORMATION;

/**
 * The PROCESS_KEEPALIVE_COUNT_INFORMATION structure contains information about the wake and no-wake counts for a process.
 */
typedef struct _PROCESS_KEEPALIVE_COUNT_INFORMATION
{
    ULONG WakeCount;    // The number of times the process has been awakened.
    ULONG NoWakeCount;  // The number of times the process was not awakened when expected.
} PROCESS_KEEPALIVE_COUNT_INFORMATION, *PPROCESS_KEEPALIVE_COUNT_INFORMATION;

/**
 * The PROCESS_REVOKE_FILE_HANDLES_INFORMATION structure revokes handles to the specified device from a process.
 */
typedef struct _PROCESS_REVOKE_FILE_HANDLES_INFORMATION
{
    UNICODE_STRING TargetDevicePath;
} PROCESS_REVOKE_FILE_HANDLES_INFORMATION, *PPROCESS_REVOKE_FILE_HANDLES_INFORMATION;

// rev

#define PROCESS_WORKING_SET_CONTROL_VERSION 3

/**
 * The PROCESS_WORKING_SET_OPERATION enumeration defines the operation to perform on a process's working set.
 */
typedef enum _PROCESS_WORKING_SET_OPERATION
{
    ProcessWorkingSetSwap,              // Swap the working set of a process to disk. // (requires SeDebugPrivilege)
    ProcessWorkingSetEmpty,             // Remove all pages from the working set of a process.
    ProcessWorkingSetEmptyPrivatePages, // Remove private pages from the working set of a process.
    ProcessWorkingSetOperationMax
} PROCESS_WORKING_SET_OPERATION;

/**
 * The PROCESS_WORKING_SET_FLAG_EMPTY_PRIVATE_PAGES flag indicates that the operation should target private pages in the working set.
 * Private pages are those that are not shared with other processes.
 */
#define PROCESS_WORKING_SET_FLAG_EMPTY_PRIVATE_PAGES 0x01
/**
 * The PROCESS_WORKING_SET_FLAG_EMPTY_SHARED_PAGES flag indicates that the operation should target shared pages in the working set.
 * Shared pages are those that are shared between multiple processes.
 */
#define PROCESS_WORKING_SET_FLAG_EMPTY_SHARED_PAGES  0x02
/**
 * The PROCESS_WORKING_SET_FLAG_EMPTY_PAGES flag indicates that the operation should target pages in the working set.
 */
#define PROCESS_WORKING_SET_FLAG_EMPTY_PAGES         0x04
/**
 * The PROCESS_WORKING_SET_FLAG_COMPRESS flag indicates that the operation should compress the pages before they are removed from the working set.
 * Compression is typically used in conjunction with other flags to specify that the pages should be compressed as part of the operation.
 */
#define PROCESS_WORKING_SET_FLAG_COMPRESS            0x08
/**
 * The PROCESS_WORKING_SET_FLAG_STORE flag indicates that the operation should store the compressed pages.
 * This is useful when the compressed data might be needed later, allowing for efficient retrieval and decompression when required.
 * This flag is typically used in conjunction with the PROCESS_WORKING_SET_FLAG_COMPRESS flag to specify that the compressed pages should be stored.
 */
#define PROCESS_WORKING_SET_FLAG_STORE               0x10

/**
 * The PROCESS_WORKING_SET_CONTROL structure is used to control the working set of a process.
 */
typedef struct _PROCESS_WORKING_SET_CONTROL
{
    ULONG Version;
    PROCESS_WORKING_SET_OPERATION Operation;
    ULONG Flags;
} PROCESS_WORKING_SET_CONTROL, *PPROCESS_WORKING_SET_CONTROL;

/**
 * The PS_PROTECTED_TYPE enumeration defines the types of protection that can be applied to a process.
 */
typedef enum _PS_PROTECTED_TYPE
{
    PsProtectedTypeNone,            // No protection.
    PsProtectedTypeProtectedLight,  // Light protection.
    PsProtectedTypeProtected,       // Full protection.
    PsProtectedTypeMax
} PS_PROTECTED_TYPE;

/**
 * The PS_PROTECTED_SIGNER enumeration defines the types of signers that can be associated with a protected process.
 */
typedef enum _PS_PROTECTED_SIGNER
{
    PsProtectedSignerNone,          // No signer.
    PsProtectedSignerAuthenticode,  // Authenticode signer.
    PsProtectedSignerCodeGen,       // Code generation signer.
    PsProtectedSignerAntimalware,   // Antimalware signer.
    PsProtectedSignerLsa,           // Local Security Authority signer.
    PsProtectedSignerWindows,       // Windows signer.
    PsProtectedSignerWinTcb,        // Windows Trusted Computing Base signer.
    PsProtectedSignerWinSystem,     // Windows system signer.
    PsProtectedSignerApp,           // Application signer.
    PsProtectedSignerMax
} PS_PROTECTED_SIGNER;

#define PS_PROTECTED_SIGNER_MASK 0xFF
#define PS_PROTECTED_AUDIT_MASK 0x08
#define PS_PROTECTED_TYPE_MASK 0x07

// ProtectionLevel.Level = PsProtectedValue(PsProtectedSignerCodeGen, FALSE, PsProtectedTypeProtectedLight)
#define PsProtectedValue(PsSigner, PsAudit, PsType) ( \
    (((PsSigner) & PS_PROTECTED_SIGNER_MASK) << 4) | \
    (((PsAudit) & PS_PROTECTED_AUDIT_MASK) << 3) | \
    (((PsType) & PS_PROTECTED_TYPE_MASK)) \
    )

// InitializePsProtection(&ProtectionLevel, PsProtectedSignerCodeGen, FALSE, PsProtectedTypeProtectedLight)
#define InitializePsProtection(PsProtectionLevel, PsSigner, PsAudit, PsType) { \
    (PsProtectionLevel)->Signer = (PsSigner); \
    (PsProtectionLevel)->Audit = (PsAudit); \
    (PsProtectionLevel)->Type = (PsType); \

/**
 * The PROCESS_FAULT_INFORMATION structure contains information about process faults.
 */
typedef struct _PROCESS_FAULT_INFORMATION
{
    ULONG FaultFlags;       // Flags that provide additional information about the fault.
    ULONG AdditionalInfo;   // Additional information about the fault.
} PROCESS_FAULT_INFORMATION, *PPROCESS_FAULT_INFORMATION;

/**
 * The PROCESS_TELEMETRY_ID_INFORMATION structure contains telemetry information about a process.
 */
typedef struct _PROCESS_TELEMETRY_ID_INFORMATION
{
    ULONG HeaderSize;                       // The size of the structure, in bytes.
    ULONG ProcessId;                        // The ID of the process.
    ULONG64 ProcessStartKey;                // The start key of the process.
    ULONG64 CreateTime;                     // The creation time of the process.
    ULONG64 CreateInterruptTime;            // The interrupt time at creation.
    ULONG64 CreateUnbiasedInterruptTime;    // The unbiased interrupt time at creation.
    ULONG64 ProcessSequenceNumber;          // The monotonic sequence number of the process.
    ULONG64 SessionCreateTime;              // The session creation time.
    ULONG SessionId;                        // The ID of the session.
    ULONG BootId;                           // The boot ID.
    ULONG ImageChecksum;                    // The checksum of the process image.
    ULONG ImageTimeDateStamp;               // The timestamp of the process image.
    ULONG UserSidOffset;                    // The offset to the user SID.
    ULONG ImagePathOffset;                  // The offset to the image path.
    ULONG PackageNameOffset;                // The offset to the package name.
    ULONG RelativeAppNameOffset;            // The offset to the relative application name.
    ULONG CommandLineOffset;                // The offset to the command line.
} PROCESS_TELEMETRY_ID_INFORMATION, *PPROCESS_TELEMETRY_ID_INFORMATION;

/**
 * The PROCESS_COMMIT_RELEASE_INFORMATION structure contains information about the commit and release of memory for a process.
 */
typedef struct _PROCESS_COMMIT_RELEASE_INFORMATION
{
    ULONG Version;
    struct
    {
        ULONG Eligible : 1;
        ULONG ReleaseRepurposedMemResetCommit : 1;
        ULONG ForceReleaseMemResetCommit : 1; 
        ULONG Spare : 29;
    };
    SIZE_T CommitDebt;
    SIZE_T CommittedMemResetSize;
    SIZE_T RepurposedMemResetSize;
} PROCESS_COMMIT_RELEASE_INFORMATION, *PPROCESS_COMMIT_RELEASE_INFORMATION;

/**
 * The PROCESS_JOB_MEMORY_INFO structure represents app memory usage at a single point in time.
 *
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-app_memory_information
 */
typedef struct _PROCESS_JOB_MEMORY_INFO
{
    ULONG64 SharedCommitUsage;        // The current shared commit usage, in bytes.
    ULONG64 PrivateCommitUsage;       // The current private commit usage, in bytes.
    ULONG64 PeakPrivateCommitUsage;   // The peak private commit usage, in bytes.
    ULONG64 PrivateCommitLimit;       // The private commit limit, in bytes.
    ULONG64 TotalCommitLimit;         // The total commit limit, in bytes.
} PROCESS_JOB_MEMORY_INFO, *PPROCESS_JOB_MEMORY_INFO;

/**
 * The PROCESS_CHILD_PROCESS_INFORMATION structure contains information about child process policies.
 */
typedef struct _PROCESS_CHILD_PROCESS_INFORMATION
{
    BOOLEAN ProhibitChildProcesses;         // Child processes are prohibited.
    BOOLEAN AlwaysAllowSecureChildProcess;  // Secure child processes are always allowed.
    BOOLEAN AuditProhibitChildProcesses;    // Child processes are audited.
} PROCESS_CHILD_PROCESS_INFORMATION, *PPROCESS_CHILD_PROCESS_INFORMATION;

#define POWER_THROTTLING_PROCESS_CURRENT_VERSION 1
#define POWER_THROTTLING_PROCESS_EXECUTION_SPEED 0x1
#define POWER_THROTTLING_PROCESS_DELAYTIMERS 0x2
#define POWER_THROTTLING_PROCESS_IGNORE_TIMER_RESOLUTION 0x4 // since WIN11
#define POWER_THROTTLING_PROCESS_VALID_FLAGS \
    ((POWER_THROTTLING_PROCESS_EXECUTION_SPEED | POWER_THROTTLING_PROCESS_DELAYTIMERS | POWER_THROTTLING_PROCESS_IGNORE_TIMER_RESOLUTION))

/**
 * The POWER_THROTTLING_PROCESS_STATE structure is used to manage the power throttling state of a process.
 */
typedef struct _POWER_THROTTLING_PROCESS_STATE
{
    ULONG Version;       // The version of the structure.
    ULONG ControlMask;   // A mask that specifies the control settings for power throttling.
    ULONG StateMask;     // A mask that specifies the current state of power throttling.
} POWER_THROTTLING_PROCESS_STATE, *PPOWER_THROTTLING_PROCESS_STATE;

// rev (tyranid)
#define WIN32K_SYSCALL_FILTER_STATE_ENABLE 0x1
#define WIN32K_SYSCALL_FILTER_STATE_AUDIT 0x2

/**
 * The WIN32K_SYSCALL_FILTER structure is used to specify filtering options for Win32k system calls.
 */
typedef struct _WIN32K_SYSCALL_FILTER
{
    ULONG FilterState; // The state of the Win32k syscall filter (e.g., enable, audit).
    ULONG FilterSet;   // The set of Win32k syscalls to be filtered.
} WIN32K_SYSCALL_FILTER, *PWIN32K_SYSCALL_FILTER;

typedef struct _PROCESS_WAKE_INFORMATION
{
    ULONG64 NotificationChannel;
    ULONG WakeCounters[PsMaxWakeReasons];
    JOBOBJECT_WAKE_FILTER WakeFilter;
} PROCESS_WAKE_INFORMATION, *PPROCESS_WAKE_INFORMATION;

typedef struct _PROCESS_ENERGY_TRACKING_STATE
{
    ULONG StateUpdateMask;
    ULONG StateDesiredValue;
    ULONG StateSequence;
    ULONG UpdateTag : 1;
    WCHAR Tag[64];
} PROCESS_ENERGY_TRACKING_STATE, *PPROCESS_ENERGY_TRACKING_STATE;

/**
 * The MANAGE_WRITES_TO_EXECUTABLE_MEMORY structure controls write permissions to executable memory
 * for processes and threads, and provides a signal for kernel write events.
 */
typedef struct _MANAGE_WRITES_TO_EXECUTABLE_MEMORY
{
    ULONG Version : 8;                         // The version of the structure.
    ULONG ProcessEnableWriteExceptions : 1;    // Enables write exceptions for the process.
    ULONG ThreadAllowWrites : 1;               // Allows the thread to write to executable memory.
    ULONG Spare : 22;                          // Reserved for future use.
    HANDLE KernelWriteToExecutableSignal;      // Pointer to kernel signal for write-to-executable events (19H1+).
} MANAGE_WRITES_TO_EXECUTABLE_MEMORY, *PMANAGE_WRITES_TO_EXECUTABLE_MEMORY;

#define POWER_THROTTLING_THREAD_CURRENT_VERSION 1
#define POWER_THROTTLING_THREAD_EXECUTION_SPEED 0x1
#define POWER_THROTTLING_THREAD_VALID_FLAGS (POWER_THROTTLING_THREAD_EXECUTION_SPEED)

/**
 * The POWER_THROTTLING_THREAD_STATE structure contains the throttling policies of a thread subject to power management.
 *
 * \remarks https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_power_throttling_thread_state
 */
typedef struct _POWER_THROTTLING_THREAD_STATE
{
    ULONG Version;          // The version of this structure. Set to THREAD_POWER_THROTTLING_CURRENT_VERSION.
    ULONG ControlMask;      // Flags that enable the caller to take control of the power throttling mechanism.
    ULONG StateMask;        // Flags that manage the power throttling mechanism on/off state.
} POWER_THROTTLING_THREAD_STATE, *PPOWER_THROTTLING_THREAD_STATE;

#define PROCESS_READWRITEVM_LOGGING_ENABLE_READVM 1
#define PROCESS_READWRITEVM_LOGGING_ENABLE_WRITEVM 2
#define PROCESS_READWRITEVM_LOGGING_ENABLE_READVM_V 1UL
#define PROCESS_READWRITEVM_LOGGING_ENABLE_WRITEVM_V 2UL

/**
 * The PROCESS_READWRITEVM_LOGGING_INFORMATION structure provides flags to enable logging
 * of read and write operations to a process's virtual memory.
 */
typedef struct _PROCESS_READWRITEVM_LOGGING_INFORMATION
{
    union
    {
        UCHAR Flags;
        struct
        {
            UCHAR EnableReadVmLogging : 1;  // Enable logging of read operations to virtual memory.
            UCHAR EnableWriteVmLogging : 1; // Enable logging of write operations to virtual memory.
            UCHAR Unused : 6;
        };
    };
} PROCESS_READWRITEVM_LOGGING_INFORMATION, *PPROCESS_READWRITEVM_LOGGING_INFORMATION;

/**
 * The PROCESS_UPTIME_INFORMATION structure contains information about the uptime of a process and diagnostic information.
 */
typedef struct _PROCESS_UPTIME_INFORMATION
{
    ULONGLONG QueryInterruptTime;      // The interrupt time when the query was made.
    ULONGLONG QueryUnbiasedTime;       // The unbiased time when the query was made.
    ULONGLONG EndInterruptTime;        // The interrupt time when the process ended.
    ULONGLONG TimeSinceCreation;       // The total time elapsed since the process was created.
    ULONGLONG Uptime;                  // The total uptime of the process.
    ULONGLONG SuspendedTime;           // The total time the process was in a suspended state.
    struct
    {
        ULONG HangCount : 4;           // The number of times the process was detected as hanging.
        ULONG GhostCount : 4;          // The number of times the process was detected as a ghost process.
        ULONG Crashed : 1;             // Indicates whether the process has crashed (1 if true, 0 otherwise).
        ULONG Terminated : 1;          // Indicates whether the process has been terminated (1 if true, 0 otherwise).
    };
} PROCESS_UPTIME_INFORMATION, *PPROCESS_UPTIME_INFORMATION;

/**
 * The PROCESS_SYSTEM_RESOURCE_MANAGEMENT union is used to specify system resource management flags for a process.
 */
typedef struct _PROCESS_SYSTEM_RESOURCE_MANAGEMENT
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG Foreground : 1; // Indicates if the process is a foreground process (1 = foreground, 0 = background).
            ULONG Reserved : 31;
        };
    };
} PROCESS_SYSTEM_RESOURCE_MANAGEMENT, *PPROCESS_SYSTEM_RESOURCE_MANAGEMENT;

/**
 * The PROCESS_SECURITY_DOMAIN_INFORMATION structure contains the security domain identifier for a process.
 *
 * This structure is used to query or set the security domain of a process, which can be used for isolation
 * and security boundary purposes in Windows. The SecurityDomain field is a 64-bit value that uniquely
 * identifies the security domain associated with the process.
 */
typedef struct _PROCESS_SECURITY_DOMAIN_INFORMATION
{
    ULONGLONG SecurityDomain; // The unique identifier of the process's security domain.
} PROCESS_SECURITY_DOMAIN_INFORMATION, *PPROCESS_SECURITY_DOMAIN_INFORMATION;

typedef struct _PROCESS_COMBINE_SECURITY_DOMAINS_INFORMATION
{
    HANDLE ProcessHandle;
} PROCESS_COMBINE_SECURITY_DOMAINS_INFORMATION, *PPROCESS_COMBINE_SECURITY_DOMAINS_INFORMATION;

/**
 * The PROCESS_LOGGING_INFORMATION structure provides flags to enable or disable logging
 * for specific process and thread events, such as virtual memory access, suspend/resume,
 * execution protection, and impersonation.
 */
typedef struct _PROCESS_LOGGING_INFORMATION
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG EnableReadVmLogging : 1;                  // Enables logging of read operations to process virtual memory.
            ULONG EnableWriteVmLogging : 1;                 // Enables logging of write operations to process virtual memory.
            ULONG EnableProcessSuspendResumeLogging : 1;    // Enables logging of process suspend and resume events.
            ULONG EnableThreadSuspendResumeLogging : 1;     // Enables logging of thread suspend and resume events.
            ULONG EnableLocalExecProtectVmLogging : 1;      // Enables logging of local execution protection for virtual memory.
            ULONG EnableRemoteExecProtectVmLogging : 1;     // Enables logging of remote execution protection for virtual memory.
            ULONG EnableImpersonationLogging : 1;           // Enables logging of impersonation events.
            ULONG Reserved : 25;
        };
    };
} PROCESS_LOGGING_INFORMATION, *PPROCESS_LOGGING_INFORMATION;

/**
 * This value changes the seconds field during a positive leap second adjustment by the system.
 * If enabled, then the seconds field returns any positive leap second (For example: 23:59:59 -> 23:59:60 -> 00:00:00).
 * If not enabled, then the 59th second preceding a positive leap second will be shown for 2 seconds with the milliseconds
 * value ticking twice as slow. (For example: 23:59:59 -> 23:59:59.500 -> 00:00:00, which takes 2 seconds in wall clock time).
 * Note: Leap second adjustments are disabled by default for each process, this flag also does not persist if the process is restarted.
 */
#define PROCESS_LEAP_SECOND_FLAG_ENABLE_SIXTY_SECOND 0x1
#define PROCESS_LEAP_SECOND_VALID_FLAGS (PROCESS_LEAP_SECOND_INFO_FLAG_ENABLE_SIXTY_SECOND)

/**
 * The PROCESS_LEAP_SECOND_INFORMATION structure contains information about leap second adjustments for a process.
 *
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-process_leap_second_info
 */
typedef struct _PROCESS_LEAP_SECOND_INFORMATION
{
    ULONG Flags;
    ULONG Reserved;
} PROCESS_LEAP_SECOND_INFORMATION, *PPROCESS_LEAP_SECOND_INFORMATION;

typedef struct _PROCESS_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION
{
    ULONGLONG ReserveSize;
    ULONGLONG CommitSize;
    ULONG PreferredNode;
    ULONG Reserved;
    PVOID Ssp;
} PROCESS_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION, *PPROCESS_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION;

typedef struct _PROCESS_FREE_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION
{
    PVOID Ssp;
} PROCESS_FREE_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION, *PPROCESS_FREE_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION;

/**
 * The PROCESS_SYSCALL_PROVIDER_INFORMATION structure contains information about a system call provider
 * and level for system call filtering or instrumentation.
 */
typedef struct _PROCESS_SYSCALL_PROVIDER_INFORMATION
{
    GUID ProviderId; // The unique identifier of the system call provider.
    UCHAR Level;     // The level or mode of the provider.
} PROCESS_SYSCALL_PROVIDER_INFORMATION, *PPROCESS_SYSCALL_PROVIDER_INFORMATION;

/**
 * Contains dynamic enforced address ranges used by various features related to user-mode Hardware-enforced Stack Protection (HSP).
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-process_dynamic_enforced_address_range
 */
//typedef struct _PROCESS_DYNAMIC_ENFORCED_ADDRESS_RANGE
//{
//    ULONG_PTR BaseAddress;
//    SIZE_T Size;
//    ULONG Flags;
//} PROCESS_DYNAMIC_ENFORCED_ADDRESS_RANGE, *PPROCESS_DYNAMIC_ENFORCED_ADDRESS_RANGE;
//
//typedef struct _PROCESS_DYNAMIC_ENFORCED_ADDRESS_RANGES_INFORMATION
//{
//    USHORT NumberOfRanges;
//    USHORT Reserved;
//    ULONG Reserved2;
//    PPROCESS_DYNAMIC_ENFORCED_ADDRESS_RANGE Ranges;
//} PROCESS_DYNAMIC_ENFORCED_ADDRESS_RANGES_INFORMATION, *PPROCESS_DYNAMIC_ENFORCED_ADDRESS_RANGES_INFORMATION;

/**
 * The PROCESS_MEMBERSHIP_INFORMATION structure contains the Silo identifier of the process.
 *
 * \remarks https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-process_membership_information
 */
typedef struct _PROCESS_MEMBERSHIP_INFORMATION
{
    ULONG ServerSiloId;
} PROCESS_MEMBERSHIP_INFORMATION, *PPROCESS_MEMBERSHIP_INFORMATION;

#if !defined(NTDDI_WIN11_GE) || (NTDDI_VERSION < NTDDI_WIN11_GE)
typedef struct _PROCESS_NETWORK_COUNTERS
{
    ULONG64 BytesIn;
    ULONG64 BytesOut;
} PROCESS_NETWORK_COUNTERS, *PPROCESS_NETWORK_COUNTERS;
#endif

typedef struct _PROCESS_TEB_VALUE_INFORMATION
{
    ULONG ThreadId;
    ULONG TebOffset;
    ULONG_PTR Value;
} PROCESS_TEB_VALUE_INFORMATION, *PPROCESS_TEB_VALUE_INFORMATION;

// rev
typedef struct _PROCESS_AVAILABLE_CPUS_INFORMATION
{
    ULONG64 ObservedSequenceNumber;
    ULONG64 SequenceNumber;
    ULONG AvailableCpusCount;
    PKAFFINITY_EX Affinity;
} PROCESS_AVAILABLE_CPUS_INFORMATION, *PPROCESS_AVAILABLE_CPUS_INFORMATION;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryPortInformationProcess(
    VOID
    );

#endif /* !defined(_KERNEL_MODE) */

// Thread information structures

/**
 * The THREAD_BASIC_INFORMATION structure contains basic information about the thread.
 */
typedef struct _THREAD_BASIC_INFORMATION
{
    NTSTATUS ExitStatus;        // The exit status of the thread or STATUS_PENDING when the thread has not terminated. (GetExitCodeThread)
    PTEB TebBaseAddress;        // The base address of the memory region containing the TEB structure. (NtCurrentTeb)
    CLIENT_ID ClientId;         // The process and thread identifier of the thread.
    KAFFINITY AffinityMask;     // The affinity mask of the thread. (deprecated) (SetThreadAffinityMask)
    KPRIORITY Priority;         // The current priority of the thread. (GetThreadPriority)
    KPRIORITY BasePriority;     // The current base priority of the thread determined by the thread priority and process priority class.
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

/**
 * The THREAD_LAST_SYSCALL_INFORMATION structure contains information about the last system call made by a thread.
 */
typedef struct _THREAD_LAST_SYSCALL_INFORMATION
{
    PVOID FirstArgument;        // Pointer to the first argument of the last system call.
    USHORT SystemCallNumber;    // The system call number of the last system call made by the thread.
    ULONG64 WaitTime;           // The time spent waiting for the system call to complete, in milliseconds.
} THREAD_LAST_SYSCALL_INFORMATION, *PTHREAD_LAST_SYSCALL_INFORMATION;

/**
 * The THREAD_CYCLE_TIME_INFORMATION structure contains information about the cycle time of a thread.
 */
typedef struct _THREAD_CYCLE_TIME_INFORMATION
{
    ULONG64 AccumulatedCycles;        // The total number of cycles accumulated by the thread.
    ULONG64 CurrentCycleCount;        // The current cycle count of the thread.
} THREAD_CYCLE_TIME_INFORMATION, *PTHREAD_CYCLE_TIME_INFORMATION;

// RtlAbPostRelease / ReleaseAllUserModeAutoBoostLockHandles 
typedef struct _THREAD_LOCK_OWNERSHIP
{
    ULONG SrwLock[1];
} THREAD_LOCK_OWNERSHIP, *PTHREAD_LOCK_OWNERSHIP;

typedef enum _SCHEDULER_SHARED_DATA_SLOT_ACTION
{
    SchedulerSharedSlotAssign,
    SchedulerSharedSlotFree,
    SchedulerSharedSlotQuery
} SCHEDULER_SHARED_DATA_SLOT_ACTION;

typedef struct _SCHEDULER_SHARED_DATA_SLOT_INFORMATION
{
    SCHEDULER_SHARED_DATA_SLOT_ACTION Action;
    PVOID SchedulerSharedDataHandle;
    PVOID Slot;
} SCHEDULER_SHARED_DATA_SLOT_INFORMATION, *PSCHEDULER_SHARED_DATA_SLOT_INFORMATION;

typedef struct _THREAD_TEB_INFORMATION
{
    _Inout_bytecount_(BytesToRead) PVOID TebInformation; // Buffer to write data into.
    _In_ ULONG TebOffset;                                // Offset in TEB to begin reading from.
    _In_ ULONG BytesToRead;                              // Number of bytes to read.
} THREAD_TEB_INFORMATION, *PTHREAD_TEB_INFORMATION;

/**
 * The COUNTER_READING structure is used to store individual counter data from a hardware counter.
 *
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-hardware_counter_data
 */
typedef struct _COUNTER_READING
{   
    HARDWARE_COUNTER_TYPE Type;     // Specifies the type of hardware counter data collected.  
    ULONG Index;                    // An identifier for the specific counter.   
    ULONG64 Start;                  // The initial value of the counter when measurement started.   
    ULONG64 Total;                  // The accumulated value of the counter over the measurement period.
} COUNTER_READING, *PCOUNTER_READING;

#ifndef THREAD_PERFORMANCE_DATA_VERSION
#define THREAD_PERFORMANCE_DATA_VERSION 1
#endif

/**
 * The THREAD_PERFORMANCE_DATA structure aggregates various performance metrics for a thread.
 * 
 * @remarks https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-performance_data
 */
typedef struct _THREAD_PERFORMANCE_DATA
{
    USHORT Size;                                    // The size of the structure.
    USHORT Version;                                 // The version of the structure. Must be set to \ref THREAD_PERFORMANCE_DATA_VERSION.
    PROCESSOR_NUMBER ProcessorNumber;               // The processor number that identifies where the thread is running.
    ULONG ContextSwitches;                          // The number of context switches that occurred from the time profiling was enabled.
    ULONG HwCountersCount;                          // The number of array elements in the HwCounters array that contain hardware counter data.
    ULONG64 UpdateCount;                            // The number of times that the read operation read the data to ensure a consistent snapshot of the data.
    ULONG64 WaitReasonBitMap;                       // A bitmask of \ref KWAIT_REASON that identifies the reasons for the context switches that occurred since the last time the data was read.
    ULONG64 HardwareCounters;                       // A bitmask of hardware counters used to collect counter data.
    COUNTER_READING CycleTime;                      // The cycle time of the thread (excludes the time spent interrupted) from the time profiling was enabled.
    COUNTER_READING HwCounters[MAX_HW_COUNTERS];    // The \ref COUNTER_READING structure that contains hardware counter data.
} THREAD_PERFORMANCE_DATA, *PTHREAD_PERFORMANCE_DATA;

#ifndef THREAD_PROFILING_FLAG_DISPATCH
#define THREAD_PROFILING_FLAG_DISPATCH 0x00000001
#endif

#ifndef THREAD_PROFILING_FLAG_HARDWARE_COUNTERS
#define THREAD_PROFILING_FLAG_HARDWARE_COUNTERS 0x00000002
#endif

/**
 * The THREAD_PROFILING_INFORMATION structure contains profiling information and references to performance data.
 *
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-readthreadprofilingdata
 */
typedef struct _THREAD_PROFILING_INFORMATION
{
    // To receive hardware performance counter data, set this parameter to a bitmask that identifies the hardware counters to collect.
    // You can specify up to 16 performance counters. Each bit relates directly to the zero-based hardware counter index for the hardware
    // performance counters that you configured. Set to zero if you are not collecting hardware counter data.
    // If you set a bit for a hardware counter that has not been configured, the counter value that is read for that counter is zero.
    ULONG64 HardwareCounters;
    // To receive thread profiling data such as context switch count, set this parameter to \ref THREAD_PROFILING_FLAG_DISPATCH.
    ULONG Flags;
    // Enable or disable thread profiling on the specified thread.
    ULONG Enable;
    // The PERFORMANCE_DATA structure that contains thread profiling and hardware counter data.
    PTHREAD_PERFORMANCE_DATA PerformanceData;
} THREAD_PROFILING_INFORMATION, *PTHREAD_PROFILING_INFORMATION;

typedef struct _RTL_UMS_CONTEXT
{
    SINGLE_LIST_ENTRY Link;
    CONTEXT Context;
    PVOID Teb;
    PVOID UserContext;
    volatile ULONG ScheduledThread : 1;
    volatile ULONG Suspended : 1;
    volatile ULONG VolatileContext : 1;
    volatile ULONG Terminated : 1;
    volatile ULONG DebugActive : 1;
    volatile ULONG RunningOnSelfThread : 1;
    volatile ULONG DenyRunningOnSelfThread : 1;
    volatile LONG Flags;
    volatile ULONG64 KernelUpdateLock : 2;
    volatile ULONG64 PrimaryClientID : 62;
    volatile ULONG64 ContextLock;
    struct _RTL_UMS_CONTEXT* PrimaryUmsContext;
    ULONG SwitchCount;
    ULONG KernelYieldCount;
    ULONG MixedYieldCount;
    ULONG YieldCount;
} RTL_UMS_CONTEXT, *PRTL_UMS_CONTEXT;

typedef enum _THREAD_UMS_INFORMATION_COMMAND
{
    UmsInformationCommandInvalid,
    UmsInformationCommandAttach,
    UmsInformationCommandDetach,
    UmsInformationCommandQuery
} THREAD_UMS_INFORMATION_COMMAND;

typedef struct _RTL_UMS_COMPLETION_LIST
{
    PSINGLE_LIST_ENTRY ThreadListHead;
    PVOID CompletionEvent;
    ULONG CompletionFlags;
    SINGLE_LIST_ENTRY InternalListHead;
} RTL_UMS_COMPLETION_LIST, *PRTL_UMS_COMPLETION_LIST;

typedef struct _THREAD_UMS_INFORMATION
{
    THREAD_UMS_INFORMATION_COMMAND Command;
    PRTL_UMS_COMPLETION_LIST CompletionList;
    PRTL_UMS_CONTEXT UmsContext;
    union
    {
        ULONG Flags;
        struct
        {
            ULONG IsUmsSchedulerThread : 1;
            ULONG IsUmsWorkerThread : 1;
            ULONG SpareBits : 30;
        };
    };
} THREAD_UMS_INFORMATION, *PTHREAD_UMS_INFORMATION;

/**
 * The THREAD_NAME_INFORMATION structure assigns a description to a thread.
 *
 * @remarks The handle must have THREAD_SET_LIMITED_INFORMATION access.
 * @remarks https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreaddescription
 */
typedef struct _THREAD_NAME_INFORMATION
{
    // An Unicode string that specifies the description of the thread.
    UNICODE_STRING ThreadName;
} THREAD_NAME_INFORMATION, *PTHREAD_NAME_INFORMATION;

typedef struct _ALPC_WORK_ON_BEHALF_TICKET
{
    ULONG ThreadId;
    ULONG ThreadCreationTimeLow;
} ALPC_WORK_ON_BEHALF_TICKET, *PALPC_WORK_ON_BEHALF_TICKET;

typedef struct _RTL_WORK_ON_BEHALF_TICKET_EX
{
    ALPC_WORK_ON_BEHALF_TICKET Ticket;
    union
    {
        ULONG Flags;
        struct
        {
            ULONG CurrentThread : 1;
            ULONG Reserved1 : 31;
        };
    };
    ULONG Reserved2;
} RTL_WORK_ON_BEHALF_TICKET_EX, *PRTL_WORK_ON_BEHALF_TICKET_EX;

#if !defined(_KERNEL_MODE)

typedef enum _SUBSYSTEM_INFORMATION_TYPE
{
    SubsystemInformationTypeWin32,
    SubsystemInformationTypeWSL,
    MaxSubsystemInformationType
} SUBSYSTEM_INFORMATION_TYPE;
#endif

typedef enum _THREAD_WORKLOAD_CLASS
{
    ThreadWorkloadClassDefault,
    ThreadWorkloadClassGraphics,
    MaxThreadWorkloadClass
} THREAD_WORKLOAD_CLASS;

EXTERN_C_END
