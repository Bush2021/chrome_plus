#pragma once

#include "../MinDef.h"

EXTERN_C_START

/* phnt */

typedef enum _RTL_BSD_ITEM_TYPE
{
    RtlBsdItemVersionNumber, // q; s: ULONG
    RtlBsdItemProductType, // q; s: NT_PRODUCT_TYPE (ULONG)
    RtlBsdItemAabEnabled, // q: s: BOOLEAN // AutoAdvancedBoot
    RtlBsdItemAabTimeout, // q: s: UCHAR // AdvancedBootMenuTimeout
    RtlBsdItemBootGood, // q: s: BOOLEAN // LastBootSucceeded
    RtlBsdItemBootShutdown, // q: s: BOOLEAN // LastBootShutdown
    RtlBsdSleepInProgress, // q: s: BOOLEAN // SleepInProgress
    RtlBsdPowerTransition, // q: s: RTL_BSD_DATA_POWER_TRANSITION
    RtlBsdItemBootAttemptCount, // q: s: UCHAR // BootAttemptCount
    RtlBsdItemBootCheckpoint, // q: s: UCHAR // LastBootCheckpoint
    RtlBsdItemBootId, // q; s: ULONG (USER_SHARED_DATA->BootId) // 10
    RtlBsdItemShutdownBootId, // q; s: ULONG
    RtlBsdItemReportedAbnormalShutdownBootId, // q; s: ULONG
    RtlBsdItemErrorInfo, // RTL_BSD_DATA_ERROR_INFO
    RtlBsdItemPowerButtonPressInfo, // RTL_BSD_POWER_BUTTON_PRESS_INFO
    RtlBsdItemChecksum, // q: s: UCHAR
    RtlBsdPowerTransitionExtension,
    RtlBsdItemFeatureConfigurationState, // q; s: ULONG
    RtlBsdItemRevocationListInfo, // 24H2
    RtlBsdItemMax
} RTL_BSD_ITEM_TYPE;

#define BOOT_STATUS_FIELD_MAX RtlBsdItemMax

typedef struct _RTL_BSD_DATA_POWER_TRANSITION
{
    LARGE_INTEGER PowerButtonTimestamp;
    struct
    {
        BOOLEAN SystemRunning : 1;
        BOOLEAN ConnectedStandbyInProgress : 1;
        BOOLEAN UserShutdownInProgress : 1;
        BOOLEAN SystemShutdownInProgress : 1;
        BOOLEAN SleepInProgress : 4;
    } Flags;
    UCHAR ConnectedStandbyScenarioInstanceId;
    UCHAR ConnectedStandbyEntryReason;
    UCHAR ConnectedStandbyExitReason;
    USHORT SystemSleepTransitionCount;
    LARGE_INTEGER LastReferenceTime;
    ULONG LastReferenceTimeChecksum;
    ULONG LastUpdateBootId;
} RTL_BSD_DATA_POWER_TRANSITION, *PRTL_BSD_DATA_POWER_TRANSITION;

typedef struct _RTL_BSD_DATA_ERROR_INFO
{
    ULONG BootId;
    ULONG RepeatCount;
    ULONG OtherErrorCount;
    ULONG Code;
    ULONG OtherErrorCount2;
} RTL_BSD_DATA_ERROR_INFO, *PRTL_BSD_DATA_ERROR_INFO;

typedef struct _RTL_BSD_POWER_BUTTON_PRESS_INFO
{
    LARGE_INTEGER LastPressTime;
    ULONG CumulativePressCount;
    USHORT LastPressBootId;
    UCHAR LastPowerWatchdogStage;
    struct
    {
        UCHAR WatchdogArmed : 1;
        UCHAR ShutdownInProgress : 1;
    } Flags;
    LARGE_INTEGER LastReleaseTime;
    ULONG CumulativeReleaseCount;
    USHORT LastReleaseBootId;
    USHORT ErrorCount;
    UCHAR CurrentConnectedStandbyPhase;
    ULONG TransitionLatestCheckpointId;
    ULONG TransitionLatestCheckpointType;
    ULONG TransitionLatestCheckpointSequenceNumber;
} RTL_BSD_POWER_BUTTON_PRESS_INFO, *PRTL_BSD_POWER_BUTTON_PRESS_INFO;

typedef struct _RTL_BSD_ITEM
{
    RTL_BSD_ITEM_TYPE Type;
    PVOID DataBuffer;
    ULONG DataLength;
} RTL_BSD_ITEM, *PRTL_BSD_ITEM;

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateBootStatusDataFile(VOID);

NTSYSAPI
NTSTATUS
NTAPI
RtlLockBootStatusData(
    _Out_ PHANDLE FileHandle);

NTSYSAPI
NTSTATUS
NTAPI
RtlUnlockBootStatusData(
    _In_ HANDLE FileHandle);

NTSYSAPI
NTSTATUS
NTAPI
RtlGetSetBootStatusData(
    _In_ HANDLE FileHandle,
    _In_ BOOLEAN Read,
    _In_ RTL_BSD_ITEM_TYPE DataClass,
    _In_ PVOID Buffer,
    _In_ ULONG BufferSize,
    _Out_opt_ PULONG ReturnLength);

#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)

NTSYSAPI
NTSTATUS
NTAPI
RtlCheckBootStatusIntegrity(
    _In_ HANDLE FileHandle,
    _Out_ PBOOLEAN Verified);

NTSYSAPI
NTSTATUS
NTAPI
RtlRestoreBootStatusDefaults(
    _In_ HANDLE FileHandle);

#endif

#if (NTDDI_VERSION >= NTDDI_WIN10_RS3)

NTSYSAPI
NTSTATUS
NTAPI
RtlRestoreSystemBootStatusDefaults(VOID);

NTSYSAPI
NTSTATUS
NTAPI
RtlGetSystemBootStatus(
    _In_ RTL_BSD_ITEM_TYPE BootStatusInformationClass,
    _Out_ PVOID DataBuffer,
    _In_ ULONG DataLength,
    _Out_opt_ PULONG ReturnLength);

NTSYSAPI
NTSTATUS
NTAPI
RtlSetSystemBootStatus(
    _In_ RTL_BSD_ITEM_TYPE BootStatusInformationClass,
    _In_ PVOID DataBuffer,
    _In_ ULONG DataLength,
    _Out_opt_ PULONG ReturnLength);

#endif

#if (NTDDI_VERSION >= NTDDI_WIN8)

NTSYSAPI
NTSTATUS
NTAPI
RtlCheckPortableOperatingSystem(
    _Out_ PBOOLEAN IsPortable);

NTSYSAPI
NTSTATUS
NTAPI
RtlSetPortableOperatingSystem(
    _In_ BOOLEAN IsPortable);

#endif

EXTERN_C_END
