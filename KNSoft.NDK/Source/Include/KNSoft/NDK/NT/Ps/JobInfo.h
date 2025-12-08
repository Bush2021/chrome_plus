#pragma once

#include "../MinDef.h"
#include "../Ex/SysInfo.h"

EXTERN_C_START

/* phnt */

typedef enum _JOBOBJECTINFOCLASS
{
    JobObjectBasicAccountingInformation = 1,            // q: JOBOBJECT_BASIC_ACCOUNTING_INFORMATION
    JobObjectBasicLimitInformation = 2,                 // qs: JOBOBJECT_BASIC_LIMIT_INFORMATION
    JobObjectBasicProcessIdList = 3,                    // q: JOBOBJECT_BASIC_PROCESS_ID_LIST
    JobObjectBasicUIRestrictions = 4,                   // qs: JOBOBJECT_BASIC_UI_RESTRICTIONS
    JobObjectSecurityLimitInformation = 5,              // qs: JOBOBJECT_SECURITY_LIMIT_INFORMATION
    JobObjectEndOfJobTimeInformation = 6,               // qs: JOBOBJECT_END_OF_JOB_TIME_INFORMATION
    JobObjectAssociateCompletionPortInformation = 7,    // s: JOBOBJECT_ASSOCIATE_COMPLETION_PORT
    JobObjectBasicAndIoAccountingInformation = 8,       // q: JOBOBJECT_BASIC_AND_IO_ACCOUNTING_INFORMATION
    JobObjectExtendedLimitInformation = 9,              // qs: JOBOBJECT_EXTENDED_LIMIT_INFORMATION[V2]
    JobObjectJobSetInformation = 10,                    // q: JOBOBJECT_JOBSET_INFORMATION
    JobObjectGroupInformation = 11,                     // q: USHORT
    JobObjectNotificationLimitInformation = 12,         // q: JOBOBJECT_NOTIFICATION_LIMIT_INFORMATION
    JobObjectLimitViolationInformation = 13,            // q: JOBOBJECT_LIMIT_VIOLATION_INFORMATION
    JobObjectGroupInformationEx = 14,                   // qs: GROUP_AFFINITY (ARRAY)
    JobObjectCpuRateControlInformation = 15,            // qs: JOBOBJECT_CPU_RATE_CONTROL_INFORMATION
    JobObjectCompletionFilter = 16,                     // qs: ULONG
    JobObjectCompletionCounter = 17,                    // qs: ULONG
    JobObjectFreezeInformation = 18,                    // qs: JOBOBJECT_FREEZE_INFORMATION
    JobObjectExtendedAccountingInformation = 19,        // qs: JOBOBJECT_EXTENDED_ACCOUNTING_INFORMATION
    JobObjectWakeInformation = 20,                      // qs: JOBOBJECT_WAKE_INFORMATION
    JobObjectBackgroundInformation = 21,                // s: BOOLEAN
    JobObjectSchedulingRankBiasInformation = 22,
    JobObjectTimerVirtualizationInformation = 23,
    JobObjectCycleTimeNotification = 24,
    JobObjectClearEvent = 25,
    JobObjectInterferenceInformation = 26,              // q: JOBOBJECT_INTERFERENCE_INFORMATION
    JobObjectClearPeakJobMemoryUsed = 27,
    JobObjectMemoryUsageInformation = 28,               // q: JOBOBJECT_MEMORY_USAGE_INFORMATION, // JOBOBJECT_MEMORY_USAGE_INFORMATION_V2
    JobObjectSharedCommit = 29,
    JobObjectContainerId = 30,                          // q: JOBOBJECT_CONTAINER_IDENTIFIER_V2
    JobObjectIoRateControlInformation = 31,             // qs: JOBOBJECT_IO_RATE_CONTROL_INFORMATION_NATIVE, JOBOBJECT_IO_RATE_CONTROL_INFORMATION_NATIVE_V2, JOBOBJECT_IO_RATE_CONTROL_INFORMATION_NATIVE_V3
    JobObjectNetRateControlInformation = 32,            // qs: JOBOBJECT_NET_RATE_CONTROL_INFORMATION
    JobObjectNotificationLimitInformation2 = 33,        // qs: JOBOBJECT_NOTIFICATION_LIMIT_INFORMATION_2
    JobObjectLimitViolationInformation2 = 34,           // qs: JOBOBJECT_LIMIT_VIOLATION_INFORMATION_2
    JobObjectCreateSilo = 35,                           // s: NULL
    JobObjectSiloBasicInformation = 36,                 // q: SILOOBJECT_BASIC_INFORMATION
    JobObjectSiloRootDirectory = 37,                    // q: SILOOBJECT_ROOT_DIRECTORY
    JobObjectServerSiloBasicInformation = 38,           // q: SERVERSILO_BASIC_INFORMATION
    JobObjectServerSiloUserSharedData = 39,             // q: SILO_USER_SHARED_DATA, // NtQueryInformationJobObject(NULL, 39, Buffer, sizeof(SILO_USER_SHARED_DATA), 0);
    JobObjectServerSiloInitialize = 40,                 // qs: SERVERSILO_INIT_INFORMATION
    JobObjectServerSiloRunningState = 41,
    JobObjectIoAttribution = 42,                        // q: JOBOBJECT_IO_ATTRIBUTION_INFORMATION
    JobObjectMemoryPartitionInformation = 43,
    JobObjectContainerTelemetryId = 44,                 // s: GUID // NtSetInformationJobObject(_In_ PGUID, 44, _In_ PGUID, sizeof(GUID)); // daxexec
    JobObjectSiloSystemRoot = 45,                       // s: UNICODE_STRING
    JobObjectEnergyTrackingState = 46,                  // q: JOBOBJECT_ENERGY_TRACKING_STATE
    JobObjectThreadImpersonationInformation = 47,       // q; s: BOOLEAN
    JobObjectIoPriorityLimit = 48,                      // qs: JOBOBJECT_IO_PRIORITY_LIMIT
    JobObjectPagePriorityLimit = 49,                    // qs: JOBOBJECT_PAGE_PRIORITY_LIMIT
    JobObjectServerSiloDiagnosticInformation = 50,      // q: SERVERSILO_DIAGNOSTIC_INFORMATION, // since 24H2
    JobObjectNetworkAccountingInformation = 51,         // q: JOBOBJECT_NETWORK_ACCOUNTING_INFORMATION
    JobObjectCpuPartition = 52,                         // since 25H2
    MaxJobObjectInfoClass = 53
} JOBOBJECTINFOCLASS, *PJOBOBJECTINFOCLASS;

#define JOB_OBJECT_LIMIT_SILO_READY 0x00400000

/**
 * The JOBOBJECT_EXTENDED_LIMIT_INFORMATION_V2 structure contains basic and extended limit information for a job object.
 *
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-jobobject_extended_limit_information
 */
typedef struct _JOBOBJECT_EXTENDED_LIMIT_INFORMATION_V2
{
    JOBOBJECT_BASIC_LIMIT_INFORMATION BasicLimitInformation;
    IO_COUNTERS IoInfo;
    SIZE_T ProcessMemoryLimit;
    SIZE_T JobMemoryLimit;
    SIZE_T PeakProcessMemoryUsed;
    SIZE_T PeakJobMemoryUsed;
    SIZE_T JobTotalMemoryLimit;
} JOBOBJECT_EXTENDED_LIMIT_INFORMATION_V2, *PJOBOBJECT_EXTENDED_LIMIT_INFORMATION_V2;

typedef struct _JOBOBJECT_EXTENDED_ACCOUNTING_INFORMATION
{
    JOBOBJECT_BASIC_ACCOUNTING_INFORMATION BasicInfo;
    IO_COUNTERS IoInfo;
    PROCESS_DISK_COUNTERS DiskIoInfo;
    ULONG64 ContextSwitches;
    LARGE_INTEGER TotalCycleTime;
    ULONG64 ReadyTime;
    PROCESS_ENERGY_VALUES EnergyValues;
} JOBOBJECT_EXTENDED_ACCOUNTING_INFORMATION, *PJOBOBJECT_EXTENDED_ACCOUNTING_INFORMATION;

// private
typedef enum _PS_WAKE_REASON
{
    PsWakeReasonUser = 0,
    PsWakeReasonExecutionRequired = 1,
    PsWakeReasonKernel = 2,
    PsWakeReasonInstrumentation = 3,
    PsWakeReasonPreserveProcess = 4,
    PsWakeReasonActivityReference = 5,
    PsWakeReasonWorkOnBehalf = 6,
    PsMaxWakeReasons = 7,
} PS_WAKE_REASON, *PPS_WAKE_REASON;

typedef struct _JOBOBJECT_WAKE_INFORMATION
{
    HANDLE NotificationChannel;
    ULONG64 WakeCounters[PsMaxWakeReasons];
} JOBOBJECT_WAKE_INFORMATION, *PJOBOBJECT_WAKE_INFORMATION;

typedef struct _JOBOBJECT_WAKE_INFORMATION_V1
{
    HANDLE NotificationChannel;
    ULONG64 WakeCounters[4];
} JOBOBJECT_WAKE_INFORMATION_V1, *PJOBOBJECT_WAKE_INFORMATION_V1;

typedef struct _JOBOBJECT_INTERFERENCE_INFORMATION
{
    ULONG64 Count;
} JOBOBJECT_INTERFERENCE_INFORMATION, *PJOBOBJECT_INTERFERENCE_INFORMATION;

typedef struct _JOBOBJECT_WAKE_FILTER
{
    ULONG HighEdgeFilter;
    ULONG LowEdgeFilter;
} JOBOBJECT_WAKE_FILTER, *PJOBOBJECT_WAKE_FILTER;

typedef struct _JOBOBJECT_FREEZE_INFORMATION
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG FreezeOperation : 1;
            ULONG FilterOperation : 1;
            ULONG SwapOperation : 1;
            ULONG Reserved : 29;
        };
    };
    BOOLEAN Freeze;
    BOOLEAN Swap;
    UCHAR Reserved0[2];
    JOBOBJECT_WAKE_FILTER WakeFilter;
} JOBOBJECT_FREEZE_INFORMATION, *PJOBOBJECT_FREEZE_INFORMATION;

typedef struct _JOBOBJECT_CONTAINER_IDENTIFIER_V2
{
    GUID ContainerId;
    GUID ContainerTelemetryId;
    ULONG JobId;
} JOBOBJECT_CONTAINER_IDENTIFIER_V2, *PJOBOBJECT_CONTAINER_IDENTIFIER_V2;

typedef struct _JOBOBJECT_MEMORY_USAGE_INFORMATION
{
    ULONG64 JobMemory;
    ULONG64 PeakJobMemoryUsed;
} JOBOBJECT_MEMORY_USAGE_INFORMATION, *PJOBOBJECT_MEMORY_USAGE_INFORMATION;

typedef struct _JOBOBJECT_MEMORY_USAGE_INFORMATION_V2
{
    JOBOBJECT_MEMORY_USAGE_INFORMATION BasicInfo;
    ULONG64 JobSharedMemory;
    ULONG64 Reserved[2];
} JOBOBJECT_MEMORY_USAGE_INFORMATION_V2, *PJOBOBJECT_MEMORY_USAGE_INFORMATION_V2;

typedef struct _SILO_USER_SHARED_DATA
{
    ULONG ServiceSessionId;
    ULONG ActiveConsoleId;
    LONGLONG ConsoleSessionForegroundProcessId;
    NT_PRODUCT_TYPE NtProductType;
    ULONG SuiteMask;
    ULONG SharedUserSessionId; // since RS2
    BOOLEAN IsMultiSessionSku;
    BOOLEAN IsStateSeparationEnabled;
    WCHAR NtSystemRoot[260];
    USHORT UserModeGlobalLogger[16];
    ULONG TimeZoneId; // since 21H2
    LONG TimeZoneBiasStamp;
    KSYSTEM_TIME TimeZoneBias;
    LARGE_INTEGER TimeZoneBiasEffectiveStart;
    LARGE_INTEGER TimeZoneBiasEffectiveEnd;
} SILO_USER_SHARED_DATA, *PSILO_USER_SHARED_DATA;

#define SILO_OBJECT_ROOT_DIRECTORY_SHADOW_ROOT 0x00000001
#define SILO_OBJECT_ROOT_DIRECTORY_INITIALIZE 0x00000002
#define SILO_OBJECT_ROOT_DIRECTORY_SHADOW_DOS_DEVICES 0x00000004

typedef struct _SILOOBJECT_ROOT_DIRECTORY
{
    union
    {
        ULONG ControlFlags; // SILO_OBJECT_ROOT_DIRECTORY_*
        UNICODE_STRING Path;
    };
} SILOOBJECT_ROOT_DIRECTORY, *PSILOOBJECT_ROOT_DIRECTORY;

typedef struct _SERVERSILO_INIT_INFORMATION
{
    HANDLE DeleteEvent;
    BOOLEAN IsDownlevelContainer;
} SERVERSILO_INIT_INFORMATION, *PSERVERSILO_INIT_INFORMATION;

typedef struct _JOBOBJECT_ENERGY_TRACKING_STATE
{
    ULONG64 Value;
    ULONG UpdateMask;
    ULONG DesiredState;
} JOBOBJECT_ENERGY_TRACKING_STATE, *PJOBOBJECT_ENERGY_TRACKING_STATE;

typedef enum _JOBOBJECT_IO_PRIORITY_LIMIT_FLAGS
{
    JOBOBJECT_IO_PRIORITY_LIMIT_ENABLE = 0x1,
    JOBOBJECT_IO_PRIORITY_LIMIT_VALID_FLAGS = 0x1,
} JOBOBJECT_IO_PRIORITY_LIMIT_FLAGS;

typedef struct _JOBOBJECT_IO_PRIORITY_LIMIT
{
    JOBOBJECT_IO_PRIORITY_LIMIT_FLAGS Flags;
    ULONG Priority;
} JOBOBJECT_IO_PRIORITY_LIMIT, *PJOBOBJECT_IO_PRIORITY_LIMIT;

typedef enum _JOBOBJECT_PAGE_PRIORITY_LIMIT_FLAGS
{
    JOBOBJECT_PAGE_PRIORITY_LIMIT_ENABLE = 0x1,
    JOBOBJECT_PAGE_PRIORITY_LIMIT_VALID_FLAGS = 0x1,
} JOBOBJECT_PAGE_PRIORITY_LIMIT_FLAGS;

typedef struct _JOBOBJECT_PAGE_PRIORITY_LIMIT
{
    JOBOBJECT_PAGE_PRIORITY_LIMIT_FLAGS Flags;
    ULONG Priority;
} JOBOBJECT_PAGE_PRIORITY_LIMIT, *PJOBOBJECT_PAGE_PRIORITY_LIMIT;

#if !defined(NTDDI_WIN11_GE) || (NTDDI_VERSION < NTDDI_WIN11_GE)

typedef struct _SERVERSILO_DIAGNOSTIC_INFORMATION
{
    NTSTATUS ExitStatus;
    WCHAR CriticalProcessName[15];
} SERVERSILO_DIAGNOSTIC_INFORMATION, *PSERVERSILO_DIAGNOSTIC_INFORMATION;

typedef struct _JOBOBJECT_NETWORK_ACCOUNTING_INFORMATION
{
    ULONG64 DataBytesIn;
    ULONG64 DataBytesOut;
} JOBOBJECT_NETWORK_ACCOUNTING_INFORMATION, *PJOBOBJECT_NETWORK_ACCOUNTING_INFORMATION;

#endif

EXTERN_C_END
