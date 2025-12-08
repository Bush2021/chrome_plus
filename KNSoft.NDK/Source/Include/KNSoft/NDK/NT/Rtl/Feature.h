#pragma once

#include "../MinDef.h"

EXTERN_C_START

/* phnt */

// private
typedef ULONG RTL_FEATURE_ID;
typedef ULONGLONG RTL_FEATURE_CHANGE_STAMP, *PRTL_FEATURE_CHANGE_STAMP;
typedef UCHAR RTL_FEATURE_VARIANT;
typedef ULONG RTL_FEATURE_VARIANT_PAYLOAD;
typedef PVOID RTL_FEATURE_CONFIGURATION_CHANGE_REGISTRATION, *PRTL_FEATURE_CONFIGURATION_CHANGE_REGISTRATION;

// private
typedef struct _RTL_FEATURE_USAGE_REPORT
{
    ULONG FeatureId;
    USHORT ReportingKind;
    USHORT ReportingOptions;
} RTL_FEATURE_USAGE_REPORT, *PRTL_FEATURE_USAGE_REPORT;

// private
typedef enum _RTL_FEATURE_CONFIGURATION_TYPE
{
    RtlFeatureConfigurationBoot,
    RtlFeatureConfigurationRuntime,
    RtlFeatureConfigurationCount
} RTL_FEATURE_CONFIGURATION_TYPE;

// private
typedef struct _RTL_FEATURE_CONFIGURATION
{
    ULONG FeatureId;
    union
    {
        ULONG Flags;
        struct
        {
            ULONG Priority : 4;
            ULONG EnabledState : 2;
            ULONG IsWexpConfiguration : 1;
            ULONG HasSubscriptions : 1;
            ULONG Variant : 6;
            ULONG VariantPayloadKind : 2;
            ULONG Reserved : 16;
        };
    };
    ULONG VariantPayload;
} RTL_FEATURE_CONFIGURATION, *PRTL_FEATURE_CONFIGURATION;

// private
typedef struct _RTL_FEATURE_CONFIGURATION_INTERNAL
{
    ULONG FeatureId;
    union
    {
        struct
        {
            ULONG Priority : 4;
            ULONG EnabledState : 2;
            ULONG IsWexpConfiguration : 1;
            ULONG HasSubscriptions : 1;
            ULONG Variant : 6;
            ULONG VariantPayloadKind : 2;
            ULONG Reserved : 16;
        };
        ULONG Flags;
    };
    ULONG VariantPayload;
    union
    {
        struct
        {
            ULONG ChangeTimeUpgrade : 1;
            ULONG HasGroupBypass : 1;
            ULONG Reserved2 : 30;
        };
        ULONG Flags2;
    };
} RTL_FEATURE_CONFIGURATION_INTERNAL, *PRTL_FEATURE_CONFIGURATION_INTERNAL;

// private
typedef struct _SYSTEM_FEATURE_CONFIGURATION_SECTIONS_INFORMATION_ENTRY
{
    RTL_FEATURE_CHANGE_STAMP ChangeStamp;
    HANDLE SectionHandle;
    SIZE_T Size;
} SYSTEM_FEATURE_CONFIGURATION_SECTIONS_INFORMATION_ENTRY, *PSYSTEM_FEATURE_CONFIGURATION_SECTIONS_INFORMATION_ENTRY;

// private
typedef enum _SYSTEM_FEATURE_CONFIGURATION_SECTION_TYPE
{
    SystemFeatureConfigurationSectionTypeBoot = 0,
    SystemFeatureConfigurationSectionTypeRuntime = 1,
    SystemFeatureConfigurationSectionTypeUsageTriggers = 2,
    SystemFeatureConfigurationSectionTypeGoverned = 3,
    SystemFeatureConfigurationSectionTypeCount
} SYSTEM_FEATURE_CONFIGURATION_SECTION_TYPE;

// private
typedef struct _SYSTEM_FEATURE_CONFIGURATION_SECTIONS_REQUEST
{
    RTL_FEATURE_CHANGE_STAMP PreviousChangeStamps[SystemFeatureConfigurationSectionTypeCount];
} SYSTEM_FEATURE_CONFIGURATION_SECTIONS_REQUEST, *PSYSTEM_FEATURE_CONFIGURATION_SECTIONS_REQUEST;

// private
typedef struct _SYSTEM_FEATURE_CONFIGURATION_SECTIONS_INFORMATION
{
    RTL_FEATURE_CHANGE_STAMP OverallChangeStamp;
    SYSTEM_FEATURE_CONFIGURATION_SECTIONS_INFORMATION_ENTRY Descriptors[SystemFeatureConfigurationSectionTypeCount];
} SYSTEM_FEATURE_CONFIGURATION_SECTIONS_INFORMATION, *PSYSTEM_FEATURE_CONFIGURATION_SECTIONS_INFORMATION;

//typedef struct _SYSTEM_FEATURE_USAGE_SUBSCRIPTION_UPDATE
//{
//    ULONG UpdateCount;
//    _Field_size_(UpdateCount) SYSTEM_FEATURE_USAGE_SUBSCRIPTION_UPDATE_ENTRY Updates[ANYSIZE_ARRAY];
//} SYSTEM_FEATURE_USAGE_SUBSCRIPTION_UPDATE, *PSYSTEM_FEATURE_USAGE_SUBSCRIPTION_UPDATE;

// private
typedef struct _RTL_FEATURE_CONFIGURATION_TABLE
{
    ULONG FeatureCount;
    _Field_size_(FeatureCount) RTL_FEATURE_CONFIGURATION_INTERNAL Features[ANYSIZE_ARRAY];
} RTL_FEATURE_CONFIGURATION_TABLE, *PRTL_FEATURE_CONFIGURATION_TABLE;

// private
typedef enum _RTL_FEATURE_CONFIGURATION_PRIORITY
{
    FeatureConfigurationPriorityImageDefault   = 0,
    FeatureConfigurationPriorityEKB            = 1,
    FeatureConfigurationPrioritySafeguard      = 2,
    FeatureConfigurationPriorityPersistent     = FeatureConfigurationPrioritySafeguard,
    FeatureConfigurationPriorityReserved3      = 3,
    FeatureConfigurationPriorityService        = 4,
    FeatureConfigurationPriorityReserved5      = 5,
    FeatureConfigurationPriorityDynamic        = 6,
    FeatureConfigurationPriorityReserved7      = 7,
    FeatureConfigurationPriorityUser           = 8,
    FeatureConfigurationPrioritySecurity       = 9,
    FeatureConfigurationPriorityUserPolicy     = 10,
    FeatureConfigurationPriorityReserved11     = 11,
    FeatureConfigurationPriorityTest           = 12,
    FeatureConfigurationPriorityReserved13     = 13,
    FeatureConfigurationPriorityReserved14     = 14,
    FeatureConfigurationPriorityImageOverride  = 15,
    FeatureConfigurationPriorityMax            = FeatureConfigurationPriorityImageOverride
} RTL_FEATURE_CONFIGURATION_PRIORITY, *PRTL_FEATURE_CONFIGURATION_PRIORITY;

// private
typedef enum _RTL_FEATURE_ENABLED_STATE
{
    FeatureEnabledStateDefault,
    FeatureEnabledStateDisabled,
    FeatureEnabledStateEnabled
} RTL_FEATURE_ENABLED_STATE;

// private
typedef enum _RTL_FEATURE_ENABLED_STATE_OPTIONS
{
    FeatureEnabledStateOptionsNone,
    FeatureEnabledStateOptionsWexpConfig
} RTL_FEATURE_ENABLED_STATE_OPTIONS, *PRTL_FEATURE_ENABLED_STATE_OPTIONS;

// private
typedef enum _RTL_FEATURE_VARIANT_PAYLOAD_KIND
{
    FeatureVariantPayloadKindNone,
    FeatureVariantPayloadKindResident,
    FeatureVariantPayloadKindExternal
} RTL_FEATURE_VARIANT_PAYLOAD_KIND, *PRTL_FEATURE_VARIANT_PAYLOAD_KIND;

// private
typedef enum _RTL_FEATURE_CONFIGURATION_OPERATION
{
    FeatureConfigurationOperationNone         = 0,
    FeatureConfigurationOperationFeatureState = 1,
    FeatureConfigurationOperationVariantState = 2,
    FeatureConfigurationOperationResetState   = 4
} RTL_FEATURE_CONFIGURATION_OPERATION, *PRTL_FEATURE_CONFIGURATION_OPERATION;

// private
typedef struct _RTL_FEATURE_CONFIGURATION_UPDATE
{
    RTL_FEATURE_ID FeatureId;
    RTL_FEATURE_CONFIGURATION_PRIORITY Priority;
    RTL_FEATURE_ENABLED_STATE EnabledState;
    RTL_FEATURE_ENABLED_STATE_OPTIONS EnabledStateOptions;

    union 
    {
        ULONG VariantFlags;
        struct 
        {
            ULONG Variant : 8;
            ULONG ChangeTimeUpgrade : 1;
            ULONG HasGroupBypass : 1;
            ULONG ReservedFlags : 22;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;

    UCHAR Reserved[3]; 
    RTL_FEATURE_VARIANT_PAYLOAD_KIND VariantPayloadKind;
    RTL_FEATURE_VARIANT_PAYLOAD VariantPayload;
    RTL_FEATURE_CONFIGURATION_OPERATION Operation;
} RTL_FEATURE_CONFIGURATION_UPDATE, *PRTL_FEATURE_CONFIGURATION_UPDATE;

// private
typedef struct _RTL_FEATURE_USAGE_SUBSCRIPTION_TARGET
{
    ULONG Data[2];
} RTL_FEATURE_USAGE_SUBSCRIPTION_TARGET, *PRTL_FEATURE_USAGE_SUBSCRIPTION_TARGET;

// private
typedef struct _SYSTEM_FEATURE_USAGE_SUBSCRIPTION_DETAILS
{
    RTL_FEATURE_ID FeatureId;
    USHORT ReportingKind;
    USHORT ReportingOptions;
    RTL_FEATURE_USAGE_SUBSCRIPTION_TARGET ReportingTarget;
} SYSTEM_FEATURE_USAGE_SUBSCRIPTION_DETAILS, *PSYSTEM_FEATURE_USAGE_SUBSCRIPTION_DETAILS;

// private
typedef struct _RTL_FEATURE_USAGE_DATA
{
    RTL_FEATURE_ID FeatureId;
    USHORT ReportingKind;
    USHORT UsageCount;
} RTL_FEATURE_USAGE_DATA, *PRTL_FEATURE_USAGE_DATA;

// private
typedef struct _RTL_FEATURE_USAGE_SUBSCRIPTION_DETAILS
{
    RTL_FEATURE_ID FeatureId;
    USHORT ReportingKind;
    USHORT ReportingOptions;
    RTL_FEATURE_USAGE_SUBSCRIPTION_TARGET ReportingTarget;
} RTL_FEATURE_USAGE_SUBSCRIPTION_DETAILS, *PRTL_FEATURE_USAGE_SUBSCRIPTION_DETAILS;

// private
typedef struct _RTL_FEATURE_USAGE_SUBSCRIPTION_TABLE
{
    ULONG SubscriptionCount;
    _Field_size_(SubscriptionCount) RTL_FEATURE_USAGE_SUBSCRIPTION_DETAILS Subscriptions[ANYSIZE_ARRAY];
} RTL_FEATURE_USAGE_SUBSCRIPTION_TABLE, *PRTL_FEATURE_USAGE_SUBSCRIPTION_TABLE;

// private
typedef struct _SYSTEM_FEATURE_USAGE_SUBSCRIPTION_UPDATE_ENTRY
{
    ULONG Remove;
    RTL_FEATURE_USAGE_SUBSCRIPTION_DETAILS Details;
} SYSTEM_FEATURE_USAGE_SUBSCRIPTION_UPDATE_ENTRY, *PSYSTEM_FEATURE_USAGE_SUBSCRIPTION_UPDATE_ENTRY;

// private
_Function_class_(RTL_FEATURE_CONFIGURATION_CHANGE_CALLBACK)
typedef VOID (NTAPI RTL_FEATURE_CONFIGURATION_CHANGE_CALLBACK)(
    _In_opt_ PVOID Context
    );
typedef RTL_FEATURE_CONFIGURATION_CHANGE_CALLBACK *PRTL_FEATURE_CONFIGURATION_CHANGE_CALLBACK;

// private
typedef struct _SYSTEM_FEATURE_CONFIGURATION_QUERY
{
    RTL_FEATURE_CONFIGURATION_TYPE ConfigurationType;
    RTL_FEATURE_ID FeatureId;
} SYSTEM_FEATURE_CONFIGURATION_QUERY, *PSYSTEM_FEATURE_CONFIGURATION_QUERY;

// private
typedef struct _SYSTEM_FEATURE_CONFIGURATION_INFORMATION
{
    RTL_FEATURE_CHANGE_STAMP ChangeStamp;
    RTL_FEATURE_CONFIGURATION Configuration;
} SYSTEM_FEATURE_CONFIGURATION_INFORMATION, *PSYSTEM_FEATURE_CONFIGURATION_INFORMATION;

// private
typedef enum _SYSTEM_FEATURE_CONFIGURATION_UPDATE_TYPE
{
    SystemFeatureConfigurationUpdateTypeUpdate = 0,
    SystemFeatureConfigurationUpdateTypeOverwrite = 1,
    SystemFeatureConfigurationUpdateTypeCount = 2,
} SYSTEM_FEATURE_CONFIGURATION_UPDATE_TYPE, *PSYSTEM_FEATURE_CONFIGURATION_UPDATE_TYPE;

// private
typedef struct _SYSTEM_FEATURE_CONFIGURATION_UPDATE
{
    SYSTEM_FEATURE_CONFIGURATION_UPDATE_TYPE UpdateType;
    union
    {
        struct
        {
            RTL_FEATURE_CHANGE_STAMP PreviousChangeStamp;
            RTL_FEATURE_CONFIGURATION_TYPE ConfigurationType;
            ULONG UpdateCount;
            _Field_size_(UpdateCount) RTL_FEATURE_CONFIGURATION_UPDATE Updates[ANYSIZE_ARRAY];
        } Update;

        struct
        {
            RTL_FEATURE_CHANGE_STAMP PreviousChangeStamp;
            RTL_FEATURE_CONFIGURATION_TYPE ConfigurationType;
            SIZE_T BufferSize;
            PVOID Buffer;
        } Overwrite;
    };
} SYSTEM_FEATURE_CONFIGURATION_UPDATE, *PSYSTEM_FEATURE_CONFIGURATION_UPDATE;

// private
//typedef struct _SYSTEM_FEATURE_USAGE_SUBSCRIPTION_DETAILS
//{
//    RTL_FEATURE_ID FeatureId;
//    USHORT ReportingKind;
//    USHORT ReportingOptions;
//    RTL_FEATURE_USAGE_SUBSCRIPTION_TARGET ReportingTarget;
//} SYSTEM_FEATURE_USAGE_SUBSCRIPTION_DETAILS, *PSYSTEM_FEATURE_USAGE_SUBSCRIPTION_DETAILS;

//typedef struct _SYSTEM_FEATURE_USAGE_SUBSCRIPTION_UPDATE_ENTRY
//{
//    ULONG Remove;
//    RTL_FEATURE_USAGE_SUBSCRIPTION_DETAILS Details;
//} SYSTEM_FEATURE_USAGE_SUBSCRIPTION_UPDATE_ENTRY, *PSYSTEM_FEATURE_USAGE_SUBSCRIPTION_UPDATE_ENTRY;
//
//typedef struct _SYSTEM_FEATURE_USAGE_SUBSCRIPTION_UPDATE
//{
//    ULONG UpdateCount;
//    _Field_size_(UpdateCount) SYSTEM_FEATURE_USAGE_SUBSCRIPTION_UPDATE_ENTRY Updates[ANYSIZE_ARRAY];
//} SYSTEM_FEATURE_USAGE_SUBSCRIPTION_UPDATE, *PSYSTEM_FEATURE_USAGE_SUBSCRIPTION_UPDATE;

#if (NTDDI_VERSION >= NTDDI_WIN10_MN)

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlNotifyFeatureUsage(
    _In_ PRTL_FEATURE_USAGE_REPORT FeatureUsageReport
    );

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlQueryFeatureConfiguration(
    _In_ RTL_FEATURE_ID FeatureId,
    _In_ RTL_FEATURE_CONFIGURATION_TYPE ConfigurationType,
    _Out_ PRTL_FEATURE_CHANGE_STAMP ChangeStamp,
    _Out_ PRTL_FEATURE_CONFIGURATION FeatureConfiguration
    );

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlSetFeatureConfigurations(
    _In_opt_ PRTL_FEATURE_CHANGE_STAMP PreviousChangeStamp,
    _In_ RTL_FEATURE_CONFIGURATION_TYPE ConfigurationType,
    _In_reads_(ConfigurationUpdateCount) PRTL_FEATURE_CONFIGURATION_UPDATE ConfigurationUpdates,
    _In_ SIZE_T ConfigurationUpdateCount
    );

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlQueryAllFeatureConfigurations(
    _In_ RTL_FEATURE_CONFIGURATION_TYPE ConfigurationType,
    _Out_opt_ PRTL_FEATURE_CHANGE_STAMP ChangeStamp,
    _Out_writes_(*ConfigurationCount) PRTL_FEATURE_CONFIGURATION Configurations,
    _Inout_ PSIZE_T ConfigurationCount
    );

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlQueryAllInternalFeatureConfigurations(
    _In_ RTL_FEATURE_CONFIGURATION_TYPE ConfigurationType,
    _Out_opt_ PRTL_FEATURE_CHANGE_STAMP ChangeStamp,
    _Out_writes_(*ConfigurationCount) PRTL_FEATURE_CONFIGURATION Configurations,
    _Inout_ PSIZE_T ConfigurationCount
    );

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlQueryAllInternalRuntimeFeatureConfigurations(
    _In_ RTL_FEATURE_ID FeatureId,
    _In_ RTL_FEATURE_CONFIGURATION_TYPE ConfigurationType,
    _Out_opt_ PRTL_FEATURE_CHANGE_STAMP ChangeStamp,
    _Out_writes_(*ConfigurationCount) PRTL_FEATURE_CONFIGURATION Configurations,
    _Inout_ PSIZE_T ConfigurationCount
    );

// private
NTSYSAPI
RTL_FEATURE_CHANGE_STAMP
NTAPI
RtlQueryFeatureConfigurationChangeStamp(
    VOID
    );

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlQueryFeatureUsageNotificationSubscriptions(
    _Out_writes_(*SubscriptionCount) PRTL_FEATURE_USAGE_SUBSCRIPTION_DETAILS Subscriptions,
    _Inout_ PSIZE_T SubscriptionCount
    );

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlRegisterFeatureConfigurationChangeNotification(
    _In_ PRTL_FEATURE_CONFIGURATION_CHANGE_CALLBACK Callback,
    _In_opt_ PVOID Context,
    _In_opt_ PRTL_FEATURE_CHANGE_STAMP ObservedChangeStamp,
    _Out_ PRTL_FEATURE_CONFIGURATION_CHANGE_REGISTRATION RegistrationHandle
    );

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlUnregisterFeatureConfigurationChangeNotification(
    _In_ RTL_FEATURE_CONFIGURATION_CHANGE_REGISTRATION RegistrationHandle
    );

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlSubscribeForFeatureUsageNotification(
    _In_reads_(SubscriptionCount) PRTL_FEATURE_USAGE_SUBSCRIPTION_DETAILS SubscriptionDetails,
    _In_ SIZE_T SubscriptionCount
    );

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlUnsubscribeFromFeatureUsageNotifications(
    _In_reads_(SubscriptionCount) PRTL_FEATURE_USAGE_SUBSCRIPTION_DETAILS SubscriptionDetails,
    _In_ SIZE_T SubscriptionCount
    );
#endif

// private
#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)
NTSYSAPI
NTSTATUS
NTAPI
RtlOverwriteFeatureConfigurationBuffer(
    _In_opt_ PRTL_FEATURE_CHANGE_STAMP PreviousChangeStamp,
    _In_ RTL_FEATURE_CONFIGURATION_TYPE ConfigurationType,
    _In_reads_bytes_opt_(ConfigurationBufferSize) PVOID ConfigurationBuffer,
    _In_ ULONG ConfigurationBufferSize
    );
#endif

// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlNotifyFeatureToggleUsage(
    _In_ PRTL_FEATURE_USAGE_REPORT FeatureUsageReport,
    _In_ RTL_FEATURE_ID FeatureId,
    _In_ ULONG Flags
    );

// rev
NTSYSAPI
ULONG
NTAPI
RtlGetFeatureTogglesChangeToken(
    VOID
    );

EXTERN_C_END
