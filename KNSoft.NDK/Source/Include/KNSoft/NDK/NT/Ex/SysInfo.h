#pragma once

#include "../MinDef.h"
#include "../Ke/Ke.h"
#include "../Mm/Misc.h"
#include "../Ldr.h"

EXTERN_C_START

/* phnt */

typedef struct _TEB *PTEB;

typedef enum _SYSTEM_INFORMATION_CLASS
{
    SystemBasicInformation,                                 // q: SYSTEM_BASIC_INFORMATION
    SystemProcessorInformation,                             // q: SYSTEM_PROCESSOR_INFORMATION
    SystemPerformanceInformation,                           // q: SYSTEM_PERFORMANCE_INFORMATION
    SystemTimeOfDayInformation,                             // q: SYSTEM_TIMEOFDAY_INFORMATION
    SystemPathInformation,                                  // q: not implemented
    SystemProcessInformation,                               // q: SYSTEM_PROCESS_INFORMATION
    SystemCallCountInformation,                             // q: SYSTEM_CALL_COUNT_INFORMATION
    SystemDeviceInformation,                                // q: SYSTEM_DEVICE_INFORMATION
    SystemProcessorPerformanceInformation,                  // q: SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION (EX in: USHORT ProcessorGroup)
    SystemFlagsInformation,                                 // qs: SYSTEM_FLAGS_INFORMATION
    SystemCallTimeInformation,                              // q: SYSTEM_CALL_TIME_INFORMATION // not implemented // 10
    SystemModuleInformation,                                // q: RTL_PROCESS_MODULES
    SystemLocksInformation,                                 // q: RTL_PROCESS_LOCKS
    SystemStackTraceInformation,                            // q: RTL_PROCESS_BACKTRACES
    SystemPagedPoolInformation,                             // q: not implemented
    SystemNonPagedPoolInformation,                          // q: not implemented
    SystemHandleInformation,                                // q: SYSTEM_HANDLE_INFORMATION
    SystemObjectInformation,                                // q: SYSTEM_OBJECTTYPE_INFORMATION mixed with SYSTEM_OBJECT_INFORMATION
    SystemPageFileInformation,                              // q: SYSTEM_PAGEFILE_INFORMATION
    SystemVdmInstemulInformation,                           // q: SYSTEM_VDM_INSTEMUL_INFO
    SystemVdmBopInformation,                                // q: not implemented // 20
    SystemFileCacheInformation,                             // qs: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemCache)
    SystemPoolTagInformation,                               // q: SYSTEM_POOLTAG_INFORMATION
    SystemInterruptInformation,                             // q: SYSTEM_INTERRUPT_INFORMATION (EX in: USHORT ProcessorGroup)
    SystemDpcBehaviorInformation,                           // qs: SYSTEM_DPC_BEHAVIOR_INFORMATION; s: SYSTEM_DPC_BEHAVIOR_INFORMATION (requires SeLoadDriverPrivilege)
    SystemFullMemoryInformation,                            // q: SYSTEM_MEMORY_USAGE_INFORMATION // not implemented
    SystemLoadGdiDriverInformation,                         // s: (kernel-mode only)
    SystemUnloadGdiDriverInformation,                       // s: (kernel-mode only)
    SystemTimeAdjustmentInformation,                        // qs: SYSTEM_QUERY_TIME_ADJUST_INFORMATION; s: SYSTEM_SET_TIME_ADJUST_INFORMATION (requires SeSystemtimePrivilege)
    SystemSummaryMemoryInformation,                         // q: SYSTEM_MEMORY_USAGE_INFORMATION // not implemented
    SystemMirrorMemoryInformation,                          // qs: (requires license value "Kernel-MemoryMirroringSupported") (requires SeShutdownPrivilege) // 30
    SystemPerformanceTraceInformation,                      // qs: (type depends on EVENT_TRACE_INFORMATION_CLASS)
    SystemObsolete0,                                        // q: not implemented
    SystemExceptionInformation,                             // q: SYSTEM_EXCEPTION_INFORMATION
    SystemCrashDumpStateInformation,                        // s: SYSTEM_CRASH_DUMP_STATE_INFORMATION (requires SeDebugPrivilege)
    SystemKernelDebuggerInformation,                        // q: SYSTEM_KERNEL_DEBUGGER_INFORMATION
    SystemContextSwitchInformation,                         // q: SYSTEM_CONTEXT_SWITCH_INFORMATION
    SystemRegistryQuotaInformation,                         // qs: SYSTEM_REGISTRY_QUOTA_INFORMATION; s (requires SeIncreaseQuotaPrivilege)
    SystemExtendServiceTableInformation,                    // s: (requires SeLoadDriverPrivilege) // loads win32k only
    SystemPrioritySeparation,                               // s: (requires SeTcbPrivilege)
    SystemVerifierAddDriverInformation,                     // s: UNICODE_STRING (requires SeDebugPrivilege) // 40
    SystemVerifierRemoveDriverInformation,                  // s: UNICODE_STRING (requires SeDebugPrivilege)
    SystemProcessorIdleInformation,                         // q: SYSTEM_PROCESSOR_IDLE_INFORMATION (EX in: USHORT ProcessorGroup)
    SystemLegacyDriverInformation,                          // q: SYSTEM_LEGACY_DRIVER_INFORMATION
    SystemCurrentTimeZoneInformation,                       // qs: RTL_TIME_ZONE_INFORMATION
    SystemLookasideInformation,                             // q: SYSTEM_LOOKASIDE_INFORMATION
    SystemTimeSlipNotification,                             // s: HANDLE (NtCreateEvent) (requires SeSystemtimePrivilege)
    SystemSessionCreate,                                    // q: not implemented
    SystemSessionDetach,                                    // q: not implemented
    SystemSessionInformation,                               // q: not implemented (SYSTEM_SESSION_INFORMATION)
    SystemRangeStartInformation,                            // q: SYSTEM_RANGE_START_INFORMATION // 50
    SystemVerifierInformation,                              // qs: SYSTEM_VERIFIER_INFORMATION; s (requires SeDebugPrivilege)
    SystemVerifierThunkExtend,                              // qs: (kernel-mode only)
    SystemSessionProcessInformation,                        // q: SYSTEM_SESSION_PROCESS_INFORMATION
    SystemLoadGdiDriverInSystemSpace,                       // qs: SYSTEM_GDI_DRIVER_INFORMATION (kernel-mode only) (same as SystemLoadGdiDriverInformation)
    SystemNumaProcessorMap,                                 // q: SYSTEM_NUMA_INFORMATION
    SystemPrefetcherInformation,                            // qs: PREFETCHER_INFORMATION // PfSnQueryPrefetcherInformation
    SystemExtendedProcessInformation,                       // q: SYSTEM_EXTENDED_PROCESS_INFORMATION
    SystemRecommendedSharedDataAlignment,                   // q: ULONG // KeGetRecommendedSharedDataAlignment
    SystemComPlusPackage,                                   // qs: ULONG
    SystemNumaAvailableMemory,                              // q: SYSTEM_NUMA_INFORMATION // 60
    SystemProcessorPowerInformation,                        // q: SYSTEM_PROCESSOR_POWER_INFORMATION (EX in: USHORT ProcessorGroup)
    SystemEmulationBasicInformation,                        // q: SYSTEM_BASIC_INFORMATION
    SystemEmulationProcessorInformation,                    // q: SYSTEM_PROCESSOR_INFORMATION
    SystemExtendedHandleInformation,                        // q: SYSTEM_HANDLE_INFORMATION_EX
    SystemLostDelayedWriteInformation,                      // q: ULONG
    SystemBigPoolInformation,                               // q: SYSTEM_BIGPOOL_INFORMATION
    SystemSessionPoolTagInformation,                        // q: SYSTEM_SESSION_POOLTAG_INFORMATION
    SystemSessionMappedViewInformation,                     // q: SYSTEM_SESSION_MAPPED_VIEW_INFORMATION
    SystemHotpatchInformation,                              // qs: SYSTEM_HOTPATCH_CODE_INFORMATION
    SystemObjectSecurityMode,                               // q: ULONG // 70
    SystemWatchdogTimerHandler,                             // s: SYSTEM_WATCHDOG_HANDLER_INFORMATION // (kernel-mode only)
    SystemWatchdogTimerInformation,                         // qs: out: SYSTEM_WATCHDOG_TIMER_INFORMATION (EX in: ULONG WATCHDOG_INFORMATION_CLASS) // NtQuerySystemInformationEx
    SystemLogicalProcessorInformation,                      // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION (EX in: USHORT ProcessorGroup) // NtQuerySystemInformationEx
    SystemWow64SharedInformationObsolete,                   // q: not implemented
    SystemRegisterFirmwareTableInformationHandler,          // s: SYSTEM_FIRMWARE_TABLE_HANDLER // (kernel-mode only)
    SystemFirmwareTableInformation,                         // q: SYSTEM_FIRMWARE_TABLE_INFORMATION
    SystemModuleInformationEx,                              // q: RTL_PROCESS_MODULE_INFORMATION_EX // since VISTA
    SystemVerifierTriageInformation,                        // q: not implemented
    SystemSuperfetchInformation,                            // qs: SUPERFETCH_INFORMATION // PfQuerySuperfetchInformation
    SystemMemoryListInformation,                            // q: SYSTEM_MEMORY_LIST_INFORMATION; s: SYSTEM_MEMORY_LIST_COMMAND (requires SeProfileSingleProcessPrivilege) // 80
    SystemFileCacheInformationEx,                           // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (same as SystemFileCacheInformation)
    SystemThreadPriorityClientIdInformation,                // s: SYSTEM_THREAD_CID_PRIORITY_INFORMATION (requires SeIncreaseBasePriorityPrivilege) // NtQuerySystemInformationEx
    SystemProcessorIdleCycleTimeInformation,                // q: SYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION[] (EX in: USHORT ProcessorGroup) // NtQuerySystemInformationEx
    SystemVerifierCancellationInformation,                  // q: SYSTEM_VERIFIER_CANCELLATION_INFORMATION // name:wow64:whNT32QuerySystemVerifierCancellationInformation
    SystemProcessorPowerInformationEx,                      // q: not implemented
    SystemRefTraceInformation,                              // qs: SYSTEM_REF_TRACE_INFORMATION // ObQueryRefTraceInformation
    SystemSpecialPoolInformation,                           // qs: SYSTEM_SPECIAL_POOL_INFORMATION (requires SeDebugPrivilege) // MmSpecialPoolTag, then MmSpecialPoolCatchOverruns != 0
    SystemProcessIdInformation,                             // q: SYSTEM_PROCESS_ID_INFORMATION
    SystemErrorPortInformation,                             // s: (requires SeTcbPrivilege)
    SystemBootEnvironmentInformation,                       // q: SYSTEM_BOOT_ENVIRONMENT_INFORMATION // 90
    SystemHypervisorInformation,                            // q: SYSTEM_HYPERVISOR_QUERY_INFORMATION
    SystemVerifierInformationEx,                            // qs: SYSTEM_VERIFIER_INFORMATION_EX
    SystemTimeZoneInformation,                              // qs: RTL_TIME_ZONE_INFORMATION (requires SeTimeZonePrivilege)
    SystemImageFileExecutionOptionsInformation,             // s: SYSTEM_IMAGE_FILE_EXECUTION_OPTIONS_INFORMATION (requires SeTcbPrivilege)
    SystemCoverageInformation,                              // q: COVERAGE_MODULES s: COVERAGE_MODULE_REQUEST // ExpCovQueryInformation (requires SeDebugPrivilege)
    SystemPrefetchPatchInformation,                         // q: SYSTEM_PREFETCH_PATCH_INFORMATION
    SystemVerifierFaultsInformation,                        // s: SYSTEM_VERIFIER_FAULTS_INFORMATION (requires SeDebugPrivilege)
    SystemSystemPartitionInformation,                       // q: SYSTEM_SYSTEM_PARTITION_INFORMATION
    SystemSystemDiskInformation,                            // q: SYSTEM_SYSTEM_DISK_INFORMATION
    SystemProcessorPerformanceDistribution,                 // q: SYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION (EX in: USHORT ProcessorGroup) // NtQuerySystemInformationEx // 100
    SystemNumaProximityNodeInformation,                     // qs: SYSTEM_NUMA_PROXIMITY_MAP
    SystemDynamicTimeZoneInformation,                       // qs: RTL_DYNAMIC_TIME_ZONE_INFORMATION (requires SeTimeZonePrivilege)
    SystemCodeIntegrityInformation,                         // q: SYSTEM_CODEINTEGRITY_INFORMATION // SeCodeIntegrityQueryInformation
    SystemProcessorMicrocodeUpdateInformation,              // s: SYSTEM_PROCESSOR_MICROCODE_UPDATE_INFORMATION
    SystemProcessorBrandString,                             // q: CHAR[] // HaliQuerySystemInformation -> HalpGetProcessorBrandString, info class 23
    SystemVirtualAddressInformation,                        // q: SYSTEM_VA_LIST_INFORMATION[]; s: SYSTEM_VA_LIST_INFORMATION[] (requires SeIncreaseQuotaPrivilege) // MmQuerySystemVaInformation
    SystemLogicalProcessorAndGroupInformation,              // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX (EX in: LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType) // since WIN7 // NtQuerySystemInformationEx // KeQueryLogicalProcessorRelationship
    SystemProcessorCycleTimeInformation,                    // q: SYSTEM_PROCESSOR_CYCLE_TIME_INFORMATION[] (EX in: USHORT ProcessorGroup) // NtQuerySystemInformationEx
    SystemStoreInformation,                                 // qs: SYSTEM_STORE_INFORMATION (requires SeProfileSingleProcessPrivilege) // SmQueryStoreInformation
    SystemRegistryAppendString,                             // s: SYSTEM_REGISTRY_APPEND_STRING_PARAMETERS // 110
    SystemAitSamplingValue,                                 // s: ULONG (requires SeProfileSingleProcessPrivilege)
    SystemVhdBootInformation,                               // q: SYSTEM_VHD_BOOT_INFORMATION
    SystemCpuQuotaInformation,                              // qs: PS_CPU_QUOTA_QUERY_INFORMATION
    SystemNativeBasicInformation,                           // q: SYSTEM_BASIC_INFORMATION
    SystemErrorPortTimeouts,                                // q: SYSTEM_ERROR_PORT_TIMEOUTS
    SystemLowPriorityIoInformation,                         // q: SYSTEM_LOW_PRIORITY_IO_INFORMATION
    SystemTpmBootEntropyInformation,                        // q: BOOT_ENTROPY_NT_RESULT // ExQueryBootEntropyInformation
    SystemVerifierCountersInformation,                      // q: SYSTEM_VERIFIER_COUNTERS_INFORMATION
    SystemPagedPoolInformationEx,                           // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypePagedPool)
    SystemSystemPtesInformationEx,                          // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemPtes) // 120
    SystemNodeDistanceInformation,                          // q: USHORT[4*NumaNodes] // (EX in: USHORT NodeNumber) // NtQuerySystemInformationEx
    SystemAcpiAuditInformation,                             // q: SYSTEM_ACPI_AUDIT_INFORMATION // HaliQuerySystemInformation -> HalpAuditQueryResults, info class 26
    SystemBasicPerformanceInformation,                      // q: SYSTEM_BASIC_PERFORMANCE_INFORMATION // name:wow64:whNtQuerySystemInformation_SystemBasicPerformanceInformation
    SystemQueryPerformanceCounterInformation,               // q: SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION // since WIN7 SP1
    SystemSessionBigPoolInformation,                        // q: SYSTEM_SESSION_POOLTAG_INFORMATION // since WIN8
    SystemBootGraphicsInformation,                          // qs: SYSTEM_BOOT_GRAPHICS_INFORMATION (kernel-mode only)
    SystemScrubPhysicalMemoryInformation,                   // qs: MEMORY_SCRUB_INFORMATION
    SystemBadPageInformation,                               // q: SYSTEM_BAD_PAGE_INFORMATION
    SystemProcessorProfileControlArea,                      // qs: SYSTEM_PROCESSOR_PROFILE_CONTROL_AREA
    SystemCombinePhysicalMemoryInformation,                 // s: MEMORY_COMBINE_INFORMATION, MEMORY_COMBINE_INFORMATION_EX, MEMORY_COMBINE_INFORMATION_EX2 // 130
    SystemEntropyInterruptTimingInformation,                // qs: SYSTEM_ENTROPY_TIMING_INFORMATION
    SystemConsoleInformation,                               // qs: SYSTEM_CONSOLE_INFORMATION // (requires SeLoadDriverPrivilege)
    SystemPlatformBinaryInformation,                        // q: SYSTEM_PLATFORM_BINARY_INFORMATION (requires SeTcbPrivilege)
    SystemPolicyInformation,                                // q: SYSTEM_POLICY_INFORMATION (Warbird/Encrypt/Decrypt/Execute)
    SystemHypervisorProcessorCountInformation,              // q: SYSTEM_HYPERVISOR_PROCESSOR_COUNT_INFORMATION
    SystemDeviceDataInformation,                            // q: SYSTEM_DEVICE_DATA_INFORMATION
    SystemDeviceDataEnumerationInformation,                 // q: SYSTEM_DEVICE_DATA_INFORMATION
    SystemMemoryTopologyInformation,                        // q: SYSTEM_MEMORY_TOPOLOGY_INFORMATION
    SystemMemoryChannelInformation,                         // q: SYSTEM_MEMORY_CHANNEL_INFORMATION
    SystemBootLogoInformation,                              // q: SYSTEM_BOOT_LOGO_INFORMATION // 140
    SystemProcessorPerformanceInformationEx,                // q: SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION_EX // (EX in: USHORT ProcessorGroup) // NtQuerySystemInformationEx // since WINBLUE
    SystemCriticalProcessErrorLogInformation,               // q: CRITICAL_PROCESS_EXCEPTION_DATA
    SystemSecureBootPolicyInformation,                      // q: SYSTEM_SECUREBOOT_POLICY_INFORMATION
    SystemPageFileInformationEx,                            // q: SYSTEM_PAGEFILE_INFORMATION_EX
    SystemSecureBootInformation,                            // q: SYSTEM_SECUREBOOT_INFORMATION
    SystemEntropyInterruptTimingRawInformation,             // qs: SYSTEM_ENTROPY_TIMING_INFORMATION
    SystemPortableWorkspaceEfiLauncherInformation,          // q: SYSTEM_PORTABLE_WORKSPACE_EFI_LAUNCHER_INFORMATION
    SystemFullProcessInformation,                           // q: SYSTEM_EXTENDED_PROCESS_INFORMATION with SYSTEM_PROCESS_INFORMATION_EXTENSION (requires admin)
    SystemKernelDebuggerInformationEx,                      // q: SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX
    SystemBootMetadataInformation,                          // q: (requires SeTcbPrivilege) // 150
    SystemSoftRebootInformation,                            // q: ULONG
    SystemElamCertificateInformation,                       // s: SYSTEM_ELAM_CERTIFICATE_INFORMATION
    SystemOfflineDumpConfigInformation,                     // q: OFFLINE_CRASHDUMP_CONFIGURATION_TABLE_V2
    SystemProcessorFeaturesInformation,                     // q: SYSTEM_PROCESSOR_FEATURES_INFORMATION
    SystemRegistryReconciliationInformation,                // s: NULL (requires admin) (flushes registry hives)
    SystemEdidInformation,                                  // q: SYSTEM_EDID_INFORMATION
    SystemManufacturingInformation,                         // q: SYSTEM_MANUFACTURING_INFORMATION // since THRESHOLD
    SystemEnergyEstimationConfigInformation,                // q: SYSTEM_ENERGY_ESTIMATION_CONFIG_INFORMATION
    SystemHypervisorDetailInformation,                      // q: SYSTEM_HYPERVISOR_DETAIL_INFORMATION
    SystemProcessorCycleStatsInformation,                   // q: SYSTEM_PROCESSOR_CYCLE_STATS_INFORMATION (EX in: USHORT ProcessorGroup) // NtQuerySystemInformationEx // 160
    SystemVmGenerationCountInformation,                     // s: 
    SystemTrustedPlatformModuleInformation,                 // q: SYSTEM_TPM_INFORMATION
    SystemKernelDebuggerFlags,                              // q: SYSTEM_KERNEL_DEBUGGER_FLAGS
    SystemCodeIntegrityPolicyInformation,                   // qs: SYSTEM_CODEINTEGRITYPOLICY_INFORMATION
    SystemIsolatedUserModeInformation,                      // q: SYSTEM_ISOLATED_USER_MODE_INFORMATION
    SystemHardwareSecurityTestInterfaceResultsInformation,  // q: 
    SystemSingleModuleInformation,                          // q: SYSTEM_SINGLE_MODULE_INFORMATION
    SystemAllowedCpuSetsInformation,                        // s: SYSTEM_WORKLOAD_ALLOWED_CPU_SET_INFORMATION
    SystemVsmProtectionInformation,                         // q: SYSTEM_VSM_PROTECTION_INFORMATION (previously SystemDmaProtectionInformation)
    SystemInterruptCpuSetsInformation,                      // q: SYSTEM_INTERRUPT_CPU_SET_INFORMATION // 170
    SystemSecureBootPolicyFullInformation,                  // q: SYSTEM_SECUREBOOT_POLICY_FULL_INFORMATION
    SystemCodeIntegrityPolicyFullInformation,               // q: 
    SystemAffinitizedInterruptProcessorInformation,         // q: KAFFINITY_EX // (requires SeIncreaseBasePriorityPrivilege)
    SystemRootSiloInformation,                              // q: SYSTEM_ROOT_SILO_INFORMATION
    SystemCpuSetInformation,                                // q: SYSTEM_CPU_SET_INFORMATION // since THRESHOLD2
    SystemCpuSetTagInformation,                             // q: SYSTEM_CPU_SET_TAG_INFORMATION
    SystemWin32WerStartCallout,                             // s: 
    SystemSecureKernelProfileInformation,                   // q: SYSTEM_SECURE_KERNEL_HYPERGUARD_PROFILE_INFORMATION
    SystemCodeIntegrityPlatformManifestInformation,         // q: SYSTEM_SECUREBOOT_PLATFORM_MANIFEST_INFORMATION // NtQuerySystemInformationEx // since REDSTONE
    SystemInterruptSteeringInformation,                     // q: in: SYSTEM_INTERRUPT_STEERING_INFORMATION_INPUT, out: SYSTEM_INTERRUPT_STEERING_INFORMATION_OUTPUT // NtQuerySystemInformationEx
    SystemSupportedProcessorArchitectures,                  // p: in opt: HANDLE, out: SYSTEM_SUPPORTED_PROCESSOR_ARCHITECTURES_INFORMATION[] // NtQuerySystemInformationEx // 180
    SystemMemoryUsageInformation,                           // q: SYSTEM_MEMORY_USAGE_INFORMATION
    SystemCodeIntegrityCertificateInformation,              // q: SYSTEM_CODEINTEGRITY_CERTIFICATE_INFORMATION
    SystemPhysicalMemoryInformation,                        // q: SYSTEM_PHYSICAL_MEMORY_INFORMATION // since REDSTONE2
    SystemControlFlowTransition,                            // qs: (Warbird/Encrypt/Decrypt/Execute)
    SystemKernelDebuggingAllowed,                           // s: ULONG
    SystemActivityModerationExeState,                       // s: SYSTEM_ACTIVITY_MODERATION_EXE_STATE
    SystemActivityModerationUserSettings,                   // q: SYSTEM_ACTIVITY_MODERATION_USER_SETTINGS
    SystemCodeIntegrityPoliciesFullInformation,             // qs: NtQuerySystemInformationEx
    SystemCodeIntegrityUnlockInformation,                   // q: SYSTEM_CODEINTEGRITY_UNLOCK_INFORMATION // 190
    SystemIntegrityQuotaInformation,                        // s: SYSTEM_INTEGRITY_QUOTA_INFORMATION (requires SeDebugPrivilege)
    SystemFlushInformation,                                 // q: SYSTEM_FLUSH_INFORMATION
    SystemProcessorIdleMaskInformation,                     // q: ULONG_PTR[ActiveGroupCount] // since REDSTONE3
    SystemSecureDumpEncryptionInformation,                  // qs: NtQuerySystemInformationEx // (q: requires SeDebugPrivilege) (s: requires SeTcbPrivilege)
    SystemWriteConstraintInformation,                       // q: SYSTEM_WRITE_CONSTRAINT_INFORMATION
    SystemKernelVaShadowInformation,                        // q: SYSTEM_KERNEL_VA_SHADOW_INFORMATION
    SystemHypervisorSharedPageInformation,                  // q: SYSTEM_HYPERVISOR_SHARED_PAGE_INFORMATION // since REDSTONE4
    SystemFirmwareBootPerformanceInformation,               // q: 
    SystemCodeIntegrityVerificationInformation,             // q: SYSTEM_CODEINTEGRITYVERIFICATION_INFORMATION
    SystemFirmwarePartitionInformation,                     // q: SYSTEM_FIRMWARE_PARTITION_INFORMATION // 200
    SystemSpeculationControlInformation,                    // q: SYSTEM_SPECULATION_CONTROL_INFORMATION // (CVE-2017-5715) REDSTONE3 and above.
    SystemDmaGuardPolicyInformation,                        // q: SYSTEM_DMA_GUARD_POLICY_INFORMATION
    SystemEnclaveLaunchControlInformation,                  // q: SYSTEM_ENCLAVE_LAUNCH_CONTROL_INFORMATION
    SystemWorkloadAllowedCpuSetsInformation,                // q: SYSTEM_WORKLOAD_ALLOWED_CPU_SET_INFORMATION // since REDSTONE5
    SystemCodeIntegrityUnlockModeInformation,               // q: SYSTEM_CODEINTEGRITY_UNLOCK_INFORMATION
    SystemLeapSecondInformation,                            // qs: SYSTEM_LEAP_SECOND_INFORMATION // (s: requires SeSystemtimePrivilege)
    SystemFlags2Information,                                // q: SYSTEM_FLAGS_INFORMATION // (s: requires SeDebugPrivilege)
    SystemSecurityModelInformation,                         // q: SYSTEM_SECURITY_MODEL_INFORMATION // since 19H1
    SystemCodeIntegritySyntheticCacheInformation,           // qs: NtQuerySystemInformationEx
    SystemFeatureConfigurationInformation,                  // q: in: SYSTEM_FEATURE_CONFIGURATION_QUERY, out: SYSTEM_FEATURE_CONFIGURATION_INFORMATION; s: SYSTEM_FEATURE_CONFIGURATION_UPDATE // NtQuerySystemInformationEx // since 20H1 // 210
    SystemFeatureConfigurationSectionInformation,           // q: in: SYSTEM_FEATURE_CONFIGURATION_SECTIONS_REQUEST, out: SYSTEM_FEATURE_CONFIGURATION_SECTIONS_INFORMATION // NtQuerySystemInformationEx
    SystemFeatureUsageSubscriptionInformation,              // q: SYSTEM_FEATURE_USAGE_SUBSCRIPTION_DETAILS; s: SYSTEM_FEATURE_USAGE_SUBSCRIPTION_UPDATE
    SystemSecureSpeculationControlInformation,              // q: SECURE_SPECULATION_CONTROL_INFORMATION
    SystemSpacesBootInformation,                            // qs: // since 20H2
    SystemFwRamdiskInformation,                             // q: SYSTEM_FIRMWARE_RAMDISK_INFORMATION
    SystemWheaIpmiHardwareInformation,                      // q: 
    SystemDifSetRuleClassInformation,                       // s: SYSTEM_DIF_VOLATILE_INFORMATION (requires SeDebugPrivilege)
    SystemDifClearRuleClassInformation,                     // s: NULL (requires SeDebugPrivilege)
    SystemDifApplyPluginVerificationOnDriver,               // q: SYSTEM_DIF_PLUGIN_DRIVER_INFORMATION (requires SeDebugPrivilege)
    SystemDifRemovePluginVerificationOnDriver,              // q: SYSTEM_DIF_PLUGIN_DRIVER_INFORMATION (requires SeDebugPrivilege) // 220
    SystemShadowStackInformation,                           // q: SYSTEM_SHADOW_STACK_INFORMATION
    SystemBuildVersionInformation,                          // q: in: ULONG (LayerNumber), out: SYSTEM_BUILD_VERSION_INFORMATION // NtQuerySystemInformationEx
    SystemPoolLimitInformation,                             // q: SYSTEM_POOL_LIMIT_INFORMATION (requires SeIncreaseQuotaPrivilege) // NtQuerySystemInformationEx
    SystemCodeIntegrityAddDynamicStore,                     // q: CodeIntegrity-AllowConfigurablePolicy-CustomKernelSigners
    SystemCodeIntegrityClearDynamicStores,                  // q: CodeIntegrity-AllowConfigurablePolicy-CustomKernelSigners
    SystemDifPoolTrackingInformation,                       // s: SYSTEM_DIF_POOL_TRACKING_INFORMATION (requires SeDebugPrivilege)
    SystemPoolZeroingInformation,                           // q: SYSTEM_POOL_ZEROING_INFORMATION
    SystemDpcWatchdogInformation,                           // qs: SYSTEM_DPC_WATCHDOG_CONFIGURATION_INFORMATION
    SystemDpcWatchdogInformation2,                          // qs: SYSTEM_DPC_WATCHDOG_CONFIGURATION_INFORMATION_V2
    SystemSupportedProcessorArchitectures2,                 // q: in opt: HANDLE, out: SYSTEM_SUPPORTED_PROCESSOR_ARCHITECTURES_INFORMATION[] // NtQuerySystemInformationEx // 230
    SystemSingleProcessorRelationshipInformation,           // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX // (EX in: PROCESSOR_NUMBER Processor) // NtQuerySystemInformationEx
    SystemXfgCheckFailureInformation,                       // q: SYSTEM_XFG_FAILURE_INFORMATION
    SystemIommuStateInformation,                            // q: SYSTEM_IOMMU_STATE_INFORMATION // since 22H1
    SystemHypervisorMinrootInformation,                     // q: SYSTEM_HYPERVISOR_MINROOT_INFORMATION
    SystemHypervisorBootPagesInformation,                   // q: SYSTEM_HYPERVISOR_BOOT_PAGES_INFORMATION
    SystemPointerAuthInformation,                           // q: SYSTEM_POINTER_AUTH_INFORMATION
    SystemSecureKernelDebuggerInformation,                  // qs: NtQuerySystemInformationEx
    SystemOriginalImageFeatureInformation,                  // q: in: SYSTEM_ORIGINAL_IMAGE_FEATURE_INFORMATION_INPUT, out: SYSTEM_ORIGINAL_IMAGE_FEATURE_INFORMATION_OUTPUT // NtQuerySystemInformationEx
    SystemMemoryNumaInformation,                            // q: SYSTEM_MEMORY_NUMA_INFORMATION_INPUT, SYSTEM_MEMORY_NUMA_INFORMATION_OUTPUT // NtQuerySystemInformationEx
    SystemMemoryNumaPerformanceInformation,                 // q: SYSTEM_MEMORY_NUMA_PERFORMANCE_INFORMATION_INPUTSYSTEM_MEMORY_NUMA_PERFORMANCE_INFORMATION_INPUT, SYSTEM_MEMORY_NUMA_PERFORMANCE_INFORMATION_OUTPUT // since 24H2 // 240
    SystemCodeIntegritySignedPoliciesFullInformation,       // qs: NtQuerySystemInformationEx
    SystemSecureCoreInformation,                            // qs: SystemSecureSecretsInformation
    SystemTrustedAppsRuntimeInformation,                    // q: SYSTEM_TRUSTEDAPPS_RUNTIME_INFORMATION
    SystemBadPageInformationEx,                             // q: SYSTEM_BAD_PAGE_INFORMATION
    SystemResourceDeadlockTimeout,                          // q: ULONG
    SystemBreakOnContextUnwindFailureInformation,           // q: ULONG (requires SeDebugPrivilege)
    SystemOslRamdiskInformation,                            // q: SYSTEM_OSL_RAMDISK_INFORMATION
    SystemCodeIntegrityPolicyManagementInformation,         // q: SYSTEM_CODEINTEGRITYPOLICY_MANAGEMENT // since 25H2
    SystemMemoryNumaCacheInformation,                       // q: 
    SystemProcessorFeaturesBitMapInformation,               // q: // 250
    SystemRefTraceInformationEx,                            // q: SYSTEM_REF_TRACE_INFORMATION_EX
    SystemBasicProcessInformation,                          // q: SYSTEM_BASICPROCESS_INFORMATION
    SystemHandleCountInformation,                           // q: SYSTEM_HANDLECOUNT_INFORMATION
    MaxSystemInfoClass
} SYSTEM_INFORMATION_CLASS;

/**
 * The SYSTEM_BASIC_INFORMATION structure contains basic information about the current system.
 */
typedef struct _SYSTEM_BASIC_INFORMATION
{
    ULONG Reserved;                         // Reserved
    ULONG TimerResolution;                  // The resolution of the timer, in milliseconds. // NtQueryTimerResolution
    ULONG PageSize;                         // The page size and the granularity of page protection and commitment.
    ULONG NumberOfPhysicalPages;            // The number of physical pages in the system. // KUSER_SHARED_DATA->NumberOfPhysicalPages
    ULONG LowestPhysicalPageNumber;         // The lowest memory page accessible to applications and dynamic-link libraries (DLLs).
    ULONG HighestPhysicalPageNumber;        // The highest memory page accessible to applications and dynamic-link libraries (DLLs).
    ULONG AllocationGranularity;            // The granularity for the starting address at which virtual memory can be allocated.
    ULONG_PTR MinimumUserModeAddress;       // A pointer to the lowest memory address accessible to applications and dynamic-link libraries (DLLs).
    ULONG_PTR MaximumUserModeAddress;       // A pointer to the highest memory address accessible to applications and dynamic-link libraries (DLLs).
    KAFFINITY ActiveProcessorsAffinityMask; // A mask representing the set of processors configured in the current processor group. // deprecated 
    UCHAR NumberOfProcessors;               // The number of logical processors in the current processor group. // deprecated
} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;

// SYSTEM_PROCESSOR_INFORMATION // ProcessorFeatureBits (see also SYSTEM_PROCESSOR_FEATURES_INFORMATION)
#define KF_V86_VIS      0x00000001 // Virtual 8086 mode.
#define KF_RDTSC        0x00000002 // RDTSC (Read Time-Stamp Counter) instruction.
#define KF_CR4          0x00000004 // CR4 (Control Register 4) register.
#define KF_CMOV         0x00000008 // CMOV (Conditional Move) instruction.
#define KF_GLOBAL_PAGE  0x00000010 // Global memory pages.
#define KF_LARGE_PAGE   0x00000020 // Large memory pages.
#define KF_MTRR         0x00000040 // MTRR (Memory Type Range Registers).
#define KF_CMPXCHG8B    0x00000080 // CMPXCHG8B (CompareExchange) instruction.
#define KF_MMX          0x00000100 // MMX (MultiMedia eXtensions).
#define KF_WORKING_PTE  0x00000200 // PTE (Page Table Entries).
#define KF_PAT          0x00000400 // PAT (Page Attribute Table).
#define KF_FXSR         0x00000800 // FXSR (Floating Point Extended Save and Restore).
#define KF_FAST_SYSCALL 0x00001000 // Fast system calls.
#define KF_XMMI         0x00002000 // XMMI (Streaming SIMD Extensions - 32-bit).
#define KF_3DNOW        0x00004000 // AMD 3DNow! technology.
#define KF_AMDK6MTRR    0x00008000 // AMD K6 MTRR.
#define KF_XMMI64       0x00010000 // XMMI (Streaming SIMD Extensions - 64-bit).
#define KF_DTS          0x00020000 // DTS (Digital Thermal Sensor).
#define KF_NOEXECUTE    0x20000000 // No-Execute (NX) bit.
#define KF_GLOBAL_32BIT_EXECUTE 0x40000000
#define KF_GLOBAL_32BIT_NOEXECUTE 0x80000000
#define KF_GLOBAL_32BIT_NOEXECUTE 0x80000000

typedef struct _SYSTEM_PROCESSOR_INFORMATION
{
    USHORT ProcessorArchitecture;
    USHORT ProcessorLevel;
    USHORT ProcessorRevision;
    USHORT MaximumProcessors;
    ULONG ProcessorFeatureBits;
} SYSTEM_PROCESSOR_INFORMATION, *PSYSTEM_PROCESSOR_INFORMATION;

typedef struct _SYSTEM_PERFORMANCE_INFORMATION
{
    LARGE_INTEGER IdleProcessTime;
    LARGE_INTEGER IoReadTransferCount;
    LARGE_INTEGER IoWriteTransferCount;
    LARGE_INTEGER IoOtherTransferCount;
    ULONG IoReadOperationCount;
    ULONG IoWriteOperationCount;
    ULONG IoOtherOperationCount;
    ULONG AvailablePages;
    ULONG CommittedPages;
    ULONG CommitLimit;
    ULONG PeakCommitment;
    ULONG PageFaultCount;
    ULONG CopyOnWriteCount;
    ULONG TransitionCount;
    ULONG CacheTransitionCount;
    ULONG DemandZeroCount;
    ULONG PageReadCount;
    ULONG PageReadIoCount;
    ULONG CacheReadCount;
    ULONG CacheIoCount;
    ULONG DirtyPagesWriteCount;
    ULONG DirtyWriteIoCount;
    ULONG MappedPagesWriteCount;
    ULONG MappedWriteIoCount;
    ULONG PagedPoolPages;
    ULONG NonPagedPoolPages;
    ULONG PagedPoolAllocs;
    ULONG PagedPoolFrees;
    ULONG NonPagedPoolAllocs;
    ULONG NonPagedPoolFrees;
    ULONG FreeSystemPtes;
    ULONG ResidentSystemCodePage;
    ULONG TotalSystemDriverPages;
    ULONG TotalSystemCodePages;
    ULONG NonPagedPoolLookasideHits;
    ULONG PagedPoolLookasideHits;
    ULONG AvailablePagedPoolPages;
    ULONG ResidentSystemCachePage;
    ULONG ResidentPagedPoolPage;
    ULONG ResidentSystemDriverPage;
    ULONG CcFastReadNoWait;
    ULONG CcFastReadWait;
    ULONG CcFastReadResourceMiss;
    ULONG CcFastReadNotPossible;
    ULONG CcFastMdlReadNoWait;
    ULONG CcFastMdlReadWait;
    ULONG CcFastMdlReadResourceMiss;
    ULONG CcFastMdlReadNotPossible;
    ULONG CcMapDataNoWait;
    ULONG CcMapDataWait;
    ULONG CcMapDataNoWaitMiss;
    ULONG CcMapDataWaitMiss;
    ULONG CcPinMappedDataCount;
    ULONG CcPinReadNoWait;
    ULONG CcPinReadWait;
    ULONG CcPinReadNoWaitMiss;
    ULONG CcPinReadWaitMiss;
    ULONG CcCopyReadNoWait;
    ULONG CcCopyReadWait;
    ULONG CcCopyReadNoWaitMiss;
    ULONG CcCopyReadWaitMiss;
    ULONG CcMdlReadNoWait;
    ULONG CcMdlReadWait;
    ULONG CcMdlReadNoWaitMiss;
    ULONG CcMdlReadWaitMiss;
    ULONG CcReadAheadIos;
    ULONG CcLazyWriteIos;
    ULONG CcLazyWritePages;
    ULONG CcDataFlushes;
    ULONG CcDataPages;
    ULONG ContextSwitches;
    ULONG FirstLevelTbFills;
    ULONG SecondLevelTbFills;
    ULONG SystemCalls;
    ULONGLONG CcTotalDirtyPages; // since THRESHOLD
    ULONGLONG CcDirtyPageThreshold;
    LONGLONG ResidentAvailablePages;
    ULONGLONG SharedCommittedPages;
    ULONGLONG MdlPagesAllocated; // since 24H2
    ULONGLONG PfnDatabaseCommittedPages;
    ULONGLONG SystemPageTableCommittedPages;
    ULONGLONG ContiguousPagesAllocated;
} SYSTEM_PERFORMANCE_INFORMATION, *PSYSTEM_PERFORMANCE_INFORMATION;

/**
 * The SYSTEM_TIMEOFDAY_INFORMATION structure contains information about the system uptime.
 */
typedef struct _SYSTEM_TIMEOFDAY_INFORMATION
{
    LARGE_INTEGER BootTime;         // Number of 100-nanosecond intervals since the system was started.
    LARGE_INTEGER CurrentTime;      // The current system date and time.
    LARGE_INTEGER TimeZoneBias;     // Number of 100-nanosecond intervals between local time and Coordinated Universal Time (UTC).
    ULONG TimeZoneId;               // The current system time zone identifier.
    ULONG Reserved;                 // Reserved
    ULONGLONG BootTimeBias;         // Number of 100-nanosecond intervals between the boot time and Coordinated Universal Time (UTC).
    ULONGLONG SleepTimeBias;        // Number of 100-nanosecond intervals between the sleep time and Coordinated Universal Time (UTC).
} SYSTEM_TIMEOFDAY_INFORMATION, *PSYSTEM_TIMEOFDAY_INFORMATION;

/**
 * The SYSTEM_THREAD_INFORMATION structure contains information about a thread running on a system.
 * https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-tsts/e82d73e4-cedb-4077-9099-d58f3459722f
 */
typedef struct _SYSTEM_THREAD_INFORMATION
{
    LARGE_INTEGER KernelTime;       // Number of 100-nanosecond intervals spent executing kernel code.
    LARGE_INTEGER UserTime;         // Number of 100-nanosecond intervals spent executing user code.
    LARGE_INTEGER CreateTime;       // The date and time when the thread was created.
    ULONG WaitTime;                 // The current time spent in ready queue or waiting (depending on the thread state).
    PVOID StartAddress;             // The initial start address of the thread.
    CLIENT_ID ClientId;             // The identifier of the thread and the process owning the thread.
    KPRIORITY Priority;             // The dynamic priority of the thread.
    KPRIORITY BasePriority;         // The starting priority of the thread.
    ULONG ContextSwitches;          // The total number of context switches performed.
    KTHREAD_STATE ThreadState;      // The current state of the thread.
    KWAIT_REASON WaitReason;        // The current reason the thread is waiting.
} SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

/**
 * The SYSTEM_PROCESS_INFORMATION structure contains information about a process running on a system.
 */
typedef struct _SYSTEM_PROCESS_INFORMATION
{
    ULONG NextEntryOffset;                  // The address of the previous item plus the value in the NextEntryOffset member. For the last item in the array, NextEntryOffset is 0.
    ULONG NumberOfThreads;                  // The NumberOfThreads member contains the number of threads in the process.
    ULONGLONG WorkingSetPrivateSize;        // The total private memory that a process currently has allocated and is physically resident in memory. // since VISTA
    ULONG HardFaultCount;                   // The total number of hard faults for data from disk rather than from in-memory pages. // since WIN7
    ULONG NumberOfThreadsHighWatermark;     // The peak number of threads that were running at any given point in time, indicative of potential performance bottlenecks related to thread management.
    ULONGLONG CycleTime;                    // The sum of the cycle time of all threads in the process.
    LARGE_INTEGER CreateTime;               // Number of 100-nanosecond intervals since the creation time of the process. Not updated during system timezone changes.
    LARGE_INTEGER UserTime;                 // Number of 100-nanosecond intervals the process has executed in user mode.
    LARGE_INTEGER KernelTime;               // Number of 100-nanosecond intervals the process has executed in kernel mode.
    UNICODE_STRING ImageName;               // The file name of the executable image.
    KPRIORITY BasePriority;                 // The starting priority of the process.
    HANDLE UniqueProcessId;                 // The identifier of the process.
    HANDLE InheritedFromUniqueProcessId;    // The identifier of the process that created this process. Not updated and incorrectly refers to processes with recycled identifiers. 
    ULONG HandleCount;                      // The current number of open handles used by the process.
    ULONG SessionId;                        // The identifier of the Remote Desktop Services session under which the specified process is running. 
    ULONG_PTR UniqueProcessKey;             // since VISTA (requires SystemExtendedProcessInformation)
    SIZE_T PeakVirtualSize;                 // The peak size, in bytes, of the virtual memory used by the process.
    SIZE_T VirtualSize;                     // The current size, in bytes, of virtual memory used by the process.
    ULONG PageFaultCount;                   // The total number of page faults for data that is not currently in memory. The value wraps around to zero on average 24 hours.
    SIZE_T PeakWorkingSetSize;              // The peak size, in kilobytes, of the working set of the process.
    SIZE_T WorkingSetSize;                  // The number of pages visible to the process in physical memory. These pages are resident and available for use without triggering a page fault.
    SIZE_T QuotaPeakPagedPoolUsage;         // The peak quota charged to the process for pool usage, in bytes.
    SIZE_T QuotaPagedPoolUsage;             // The quota charged to the process for paged pool usage, in bytes.
    SIZE_T QuotaPeakNonPagedPoolUsage;      // The peak quota charged to the process for nonpaged pool usage, in bytes.
    SIZE_T QuotaNonPagedPoolUsage;          // The current quota charged to the process for nonpaged pool usage.
    SIZE_T PagefileUsage;                   // The total number of bytes of page file storage in use by the process.
    SIZE_T PeakPagefileUsage;               // The maximum number of bytes of page-file storage used by the process.
    SIZE_T PrivatePageCount;                // The number of memory pages allocated for the use by the process.
    LARGE_INTEGER ReadOperationCount;       // The total number of read operations performed.
    LARGE_INTEGER WriteOperationCount;      // The total number of write operations performed.
    LARGE_INTEGER OtherOperationCount;      // The total number of I/O operations performed other than read and write operations.
    LARGE_INTEGER ReadTransferCount;        // The total number of bytes read during a read operation.
    LARGE_INTEGER WriteTransferCount;       // The total number of bytes written during a write operation.
    LARGE_INTEGER OtherTransferCount;       // The total number of bytes transferred during operations other than read and write operations.
    SYSTEM_THREAD_INFORMATION Threads[];    // This type is not defined in the structure but was added for convenience.
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

// private
typedef struct _SYSTEM_EXTENDED_THREAD_INFORMATION
{
    union
    {
        SYSTEM_THREAD_INFORMATION ThreadInfo;
        struct
        {
            ULONGLONG KernelTime;           // Number of 100-nanosecond intervals spent executing kernel code.
            ULONGLONG UserTime;             // Number of 100-nanosecond intervals spent executing user code.
            ULONGLONG CreateTime;           // The date and time when the thread was created.
            ULONG WaitTime;                 // The current time spent in ready queue or waiting (depending on the thread state).
            PVOID StartAddress;             // The initial start address of the thread.
            CLIENT_ID ClientId;             // The identifier of the thread and the process owning the thread.
            KPRIORITY Priority;             // The dynamic priority of the thread.
            KPRIORITY BasePriority;         // The starting priority of the thread.
            ULONG ContextSwitches;          // The total number of context switches performed.
            KTHREAD_STATE ThreadState;      // The current state of the thread.
            KWAIT_REASON WaitReason;        // The current reason the thread is waiting.
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    PVOID StackBase;                        // The lower boundary of the current thread stack.
    PVOID StackLimit;                       // The upper boundary of the current thread stack.
    PVOID Win32StartAddress;                // The thread's Win32 start address.
    PVOID TebBaseAddress;                   // The base address of the memory region containing the TEB structure. // since VISTA
    ULONG_PTR Reserved2;
    ULONG_PTR Reserved3;
    ULONG_PTR Reserved4;
} SYSTEM_EXTENDED_THREAD_INFORMATION, *PSYSTEM_EXTENDED_THREAD_INFORMATION;

typedef struct _SYSTEM_EXTENDED_PROCESS_INFORMATION
{
    ULONG NextEntryOffset;                  // The address of the previous item plus the value in the NextEntryOffset member. For the last item in the array, NextEntryOffset is 0.
    ULONG NumberOfThreads;                  // The NumberOfThreads member contains the number of threads in the process.
    ULONGLONG WorkingSetPrivateSize;        // The total private memory that a process currently has allocated and is physically resident in memory. // since VISTA
    ULONG HardFaultCount;                   // The total number of hard faults for data from disk rather than from in-memory pages. // since WIN7
    ULONG NumberOfThreadsHighWatermark;     // The peak number of threads that were running at any given point in time, indicative of potential performance bottlenecks related to thread management.
    ULONGLONG CycleTime;                    // The sum of the cycle time of all threads in the process.
    ULONGLONG CreateTime;                   // Number of 100-nanosecond intervals since the creation time of the process. Not updated during system timezone changes.
    ULONGLONG UserTime;                     // Number of 100-nanosecond intervals the process has executed in user mode.
    ULONGLONG KernelTime;                   // Number of 100-nanosecond intervals the process has executed in kernel mode.
    UNICODE_STRING ImageName;               // The file name of the executable image.
    KPRIORITY BasePriority;                 // The starting priority of the process.
    HANDLE UniqueProcessId;                 // The identifier of the process.
    HANDLE InheritedFromUniqueProcessId;    // The identifier of the process that created this process. Not updated and incorrectly refers to processes with recycled identifiers. 
    ULONG HandleCount;                      // The current number of open handles used by the process.
    ULONG SessionId;                        // The identifier of the Remote Desktop Services session under which the specified process is running. 
    HANDLE UniqueProcessKey;                // since VISTA (requires SystemExtendedProcessInformation)
    SIZE_T PeakVirtualSize;                 // The peak size, in bytes, of the virtual memory used by the process.
    SIZE_T VirtualSize;                     // The current size, in bytes, of virtual memory used by the process.
    ULONG PageFaultCount;                   // The total number of page faults for data that is not currently in memory. The value wraps around to zero on average 24 hours.
    SIZE_T PeakWorkingSetSize;              // The peak size, in kilobytes, of the working set of the process.
    SIZE_T WorkingSetSize;                  // The number of pages visible to the process in physical memory. These pages are resident and available for use without triggering a page fault.
    SIZE_T QuotaPeakPagedPoolUsage;         // The peak quota charged to the process for pool usage, in bytes.
    SIZE_T QuotaPagedPoolUsage;             // The quota charged to the process for paged pool usage, in bytes.
    SIZE_T QuotaPeakNonPagedPoolUsage;      // The peak quota charged to the process for nonpaged pool usage, in bytes.
    SIZE_T QuotaNonPagedPoolUsage;          // The current quota charged to the process for nonpaged pool usage.
    SIZE_T PagefileUsage;                   // The total number of bytes of page file storage in use by the process.
    SIZE_T PeakPagefileUsage;               // The maximum number of bytes of page-file storage used by the process.
    SIZE_T PrivatePageCount;                // The number of memory pages allocated for the use by the process.
    ULONGLONG ReadOperationCount;           // The total number of read operations performed.
    ULONGLONG WriteOperationCount;          // The total number of write operations performed.
    ULONGLONG OtherOperationCount;          // The total number of I/O operations performed other than read and write operations.
    ULONGLONG ReadTransferCount;            // The total number of bytes read during a read operation.
    ULONGLONG WriteTransferCount;           // The total number of bytes written during a write operation.
    ULONGLONG OtherTransferCount;           // The total number of bytes transferred during operations other than read and write operations.
    SYSTEM_EXTENDED_THREAD_INFORMATION Threads[];
    // SYSTEM_PROCESS_INFORMATION_EXTENSION // SystemFullProcessInformation
} SYSTEM_EXTENDED_PROCESS_INFORMATION, *PSYSTEM_EXTENDED_PROCESS_INFORMATION;

typedef struct _SYSTEM_CALL_COUNT_INFORMATION
{
    ULONG Length;
    ULONG NumberOfTables;
} SYSTEM_CALL_COUNT_INFORMATION, *PSYSTEM_CALL_COUNT_INFORMATION;

typedef struct _SYSTEM_DEVICE_INFORMATION
{
    ULONG NumberOfDisks;
    ULONG NumberOfFloppies;
    ULONG NumberOfCdRoms;
    ULONG NumberOfTapes;
    ULONG NumberOfSerialPorts;
    ULONG NumberOfParallelPorts;
} SYSTEM_DEVICE_INFORMATION, *PSYSTEM_DEVICE_INFORMATION;

/**
 * The SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION structure contains information about the performance of each processor installed in the system.
 * https://learn.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-ntquerysysteminformation#system_processor_performance_information
 */
typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION
{
    LARGE_INTEGER IdleTime;         // The IdleTime member contains the amount of time that the system has been idle, in 100-nanosecond intervals.
    LARGE_INTEGER KernelTime;       // The KernelTime member contains the amount of time that the system has spent executing in Kernel mode (including all threads in all processes, on all processors), in 100-nanosecond intervals.
    LARGE_INTEGER UserTime;         // The UserTime member contains the amount of time that the system has spent executing in User mode (including all threads in all processes, on all processors), in 100-nanosecond intervals.
    LARGE_INTEGER DpcTime;          // The DpcTime member contains the amount of time that the system has spent processing deferred procedure calls (DPCs), in 100-nanosecond intervals.
    LARGE_INTEGER InterruptTime;    // The InterruptTime member contains the amount of time that the system has spent processing hardware interrupts, in 100-nanosecond intervals.
    ULONG InterruptCount;           // The InterruptCount member contains the number of interrupts that have occurred, as counted by the system.
    ULONG Spare0;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION, *PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

typedef struct _SYSTEM_FLAGS_INFORMATION
{
    union
    {
        ULONG Flags; // NtGlobalFlag
        struct
        {
            ULONG StopOnException : 1;          // FLG_STOP_ON_EXCEPTION
            ULONG ShowLoaderSnaps : 1;          // FLG_SHOW_LDR_SNAPS
            ULONG DebugInitialCommand : 1;      // FLG_DEBUG_INITIAL_COMMAND
            ULONG StopOnHungGUI : 1;            // FLG_STOP_ON_HUNG_GUI
            ULONG HeapEnableTailCheck : 1;      // FLG_HEAP_ENABLE_TAIL_CHECK
            ULONG HeapEnableFreeCheck : 1;      // FLG_HEAP_ENABLE_FREE_CHECK
            ULONG HeapValidateParameters : 1;   // FLG_HEAP_VALIDATE_PARAMETERS
            ULONG HeapValidateAll : 1;          // FLG_HEAP_VALIDATE_ALL
            ULONG ApplicationVerifier : 1;      // FLG_APPLICATION_VERIFIER
            ULONG MonitorSilentProcessExit : 1; // FLG_MONITOR_SILENT_PROCESS_EXIT
            ULONG PoolEnableTagging : 1;        // FLG_POOL_ENABLE_TAGGING
            ULONG HeapEnableTagging : 1;        // FLG_HEAP_ENABLE_TAGGING
            ULONG UserStackTraceDb : 1;         // FLG_USER_STACK_TRACE_DB
            ULONG KernelStackTraceDb : 1;       // FLG_KERNEL_STACK_TRACE_DB
            ULONG MaintainObjectTypeList : 1;   // FLG_MAINTAIN_OBJECT_TYPELIST
            ULONG HeapEnableTagByDll : 1;       // FLG_HEAP_ENABLE_TAG_BY_DLL
            ULONG DisableStackExtension : 1;    // FLG_DISABLE_STACK_EXTENSION
            ULONG EnableCsrDebug : 1;           // FLG_ENABLE_CSRDEBUG
            ULONG EnableKDebugSymbolLoad : 1;   // FLG_ENABLE_KDEBUG_SYMBOL_LOAD
            ULONG DisablePageKernelStacks : 1;  // FLG_DISABLE_PAGE_KERNEL_STACKS
            ULONG EnableSystemCritBreaks : 1;   // FLG_ENABLE_SYSTEM_CRIT_BREAKS
            ULONG HeapDisableCoalescing : 1;    // FLG_HEAP_DISABLE_COALESCING
            ULONG EnableCloseExceptions : 1;    // FLG_ENABLE_CLOSE_EXCEPTIONS
            ULONG EnableExceptionLogging : 1;   // FLG_ENABLE_EXCEPTION_LOGGING
            ULONG EnableHandleTypeTagging : 1;  // FLG_ENABLE_HANDLE_TYPE_TAGGING
            ULONG HeapPageAllocs : 1;           // FLG_HEAP_PAGE_ALLOCS
            ULONG DebugInitialCommandEx : 1;    // FLG_DEBUG_INITIAL_COMMAND_EX
            ULONG DisableDbgPrint : 1;          // FLG_DISABLE_DBGPRINT
            ULONG CritSecEventCreation : 1;     // FLG_CRITSEC_EVENT_CREATION
            ULONG LdrTopDown : 1;               // FLG_LDR_TOP_DOWN
            ULONG EnableHandleExceptions : 1;   // FLG_ENABLE_HANDLE_EXCEPTIONS
            ULONG DisableProtDlls : 1;          // FLG_DISABLE_PROTDLLS
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} SYSTEM_FLAGS_INFORMATION, *PSYSTEM_FLAGS_INFORMATION;

// private
typedef struct _SYSTEM_CALL_TIME_INFORMATION
{
    ULONG Length;
    ULONG TotalCalls;
    LARGE_INTEGER TimeOfCalls[1];
} SYSTEM_CALL_TIME_INFORMATION, *PSYSTEM_CALL_TIME_INFORMATION;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
    USHORT UniqueProcessId;
    USHORT CreatorBackTraceIndex;
    UCHAR ObjectTypeIndex;
    UCHAR HandleAttributes;
    USHORT HandleValue;
    PVOID Object;
    ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
    ULONG NumberOfHandles;
    _Field_size_(NumberOfHandles) SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef struct _SYSTEM_OBJECTTYPE_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG NumberOfObjects;
    ULONG NumberOfHandles;
    ULONG TypeIndex;
    ULONG InvalidAttributes;
    GENERIC_MAPPING GenericMapping;
    ACCESS_MASK ValidAccessMask;
    ULONG PoolType;
    BOOLEAN SecurityRequired;
    BOOLEAN WaitableObject;
    UNICODE_STRING TypeName;
} SYSTEM_OBJECTTYPE_INFORMATION, *PSYSTEM_OBJECTTYPE_INFORMATION;

typedef struct _SYSTEM_OBJECT_INFORMATION
{
    ULONG NextEntryOffset;
    PVOID Object;
    HANDLE CreatorUniqueProcess;
    USHORT CreatorBackTraceIndex;
    USHORT Flags;
    LONG PointerCount;
    LONG HandleCount;
    ULONG PagedPoolCharge;
    ULONG NonPagedPoolCharge;
    HANDLE ExclusiveProcessId;
    PVOID SecurityDescriptor;
    UNICODE_STRING NameInfo;
} SYSTEM_OBJECT_INFORMATION, *PSYSTEM_OBJECT_INFORMATION;

typedef struct _SYSTEM_PAGEFILE_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG TotalSize;
    ULONG TotalInUse;
    ULONG PeakUsage;
    UNICODE_STRING PageFileName;
} SYSTEM_PAGEFILE_INFORMATION, *PSYSTEM_PAGEFILE_INFORMATION;

typedef struct _SYSTEM_VDM_INSTEMUL_INFO
{
    ULONG SegmentNotPresent;
    ULONG VdmOpcode0F;
    ULONG OpcodeESPrefix;
    ULONG OpcodeCSPrefix;
    ULONG OpcodeSSPrefix;
    ULONG OpcodeDSPrefix;
    ULONG OpcodeFSPrefix;
    ULONG OpcodeGSPrefix;
    ULONG OpcodeOPER32Prefix;
    ULONG OpcodeADDR32Prefix;
    ULONG OpcodeINSB;
    ULONG OpcodeINSW;
    ULONG OpcodeOUTSB;
    ULONG OpcodeOUTSW;
    ULONG OpcodePUSHF;
    ULONG OpcodePOPF;
    ULONG OpcodeINTnn;
    ULONG OpcodeINTO;
    ULONG OpcodeIRET;
    ULONG OpcodeINBimm;
    ULONG OpcodeINWimm;
    ULONG OpcodeOUTBimm;
    ULONG OpcodeOUTWimm;
    ULONG OpcodeINB;
    ULONG OpcodeINW;
    ULONG OpcodeOUTB;
    ULONG OpcodeOUTW;
    ULONG OpcodeLOCKPrefix;
    ULONG OpcodeREPNEPrefix;
    ULONG OpcodeREPPrefix;
    ULONG OpcodeHLT;
    ULONG OpcodeCLI;
    ULONG OpcodeSTI;
    ULONG BopCount;
} SYSTEM_VDM_INSTEMUL_INFO, *PSYSTEM_VDM_INSTEMUL_INFO;

#define MM_WORKING_SET_MAX_HARD_ENABLE 0x1
#define MM_WORKING_SET_MAX_HARD_DISABLE 0x2
#define MM_WORKING_SET_MIN_HARD_ENABLE 0x4
#define MM_WORKING_SET_MIN_HARD_DISABLE 0x8

typedef struct _SYSTEM_FILECACHE_INFORMATION
{
    SIZE_T CurrentSize;
    SIZE_T PeakSize;
    ULONG PageFaultCount;
    SIZE_T MinimumWorkingSet;
    SIZE_T MaximumWorkingSet;
    SIZE_T CurrentSizeIncludingTransitionInPages;
    SIZE_T PeakSizeIncludingTransitionInPages;
    ULONG TransitionRePurposeCount;
    ULONG Flags;
} SYSTEM_FILECACHE_INFORMATION, *PSYSTEM_FILECACHE_INFORMATION;

// Can be used instead of SYSTEM_FILECACHE_INFORMATION
typedef struct _SYSTEM_BASIC_WORKING_SET_INFORMATION
{
    SIZE_T CurrentSize;
    SIZE_T PeakSize;
    ULONG PageFaultCount;
} SYSTEM_BASIC_WORKING_SET_INFORMATION, *PSYSTEM_BASIC_WORKING_SET_INFORMATION;

typedef struct _SYSTEM_POOLTAG
{
    union
    {
        UCHAR Tag[4];
        ULONG TagUlong;
    } DUMMYUNIONNAME;
    ULONG PagedAllocs;
    ULONG PagedFrees;
    SIZE_T PagedUsed;
    ULONG NonPagedAllocs;
    ULONG NonPagedFrees;
    SIZE_T NonPagedUsed;
} SYSTEM_POOLTAG, *PSYSTEM_POOLTAG;

typedef struct _SYSTEM_POOLTAG_INFORMATION
{
    ULONG Count;
    _Field_size_(Count) SYSTEM_POOLTAG TagInfo[1];
} SYSTEM_POOLTAG_INFORMATION, *PSYSTEM_POOLTAG_INFORMATION;

typedef struct _SYSTEM_INTERRUPT_INFORMATION
{
    ULONG ContextSwitches;
    ULONG DpcCount;
    ULONG DpcRate;
    ULONG TimeIncrement;
    ULONG DpcBypassCount;
    ULONG ApcBypassCount;
} SYSTEM_INTERRUPT_INFORMATION, *PSYSTEM_INTERRUPT_INFORMATION;

typedef struct _SYSTEM_DPC_BEHAVIOR_INFORMATION
{
    ULONG Spare;
    ULONG DpcQueueDepth;
    ULONG MinimumDpcRate;
    ULONG AdjustDpcThreshold;
    ULONG IdealDpcRate;
} SYSTEM_DPC_BEHAVIOR_INFORMATION, *PSYSTEM_DPC_BEHAVIOR_INFORMATION;

typedef struct _SYSTEM_QUERY_TIME_ADJUST_INFORMATION
{
    ULONG TimeAdjustment;
    ULONG TimeIncrement;
    BOOLEAN Enable;
} SYSTEM_QUERY_TIME_ADJUST_INFORMATION, *PSYSTEM_QUERY_TIME_ADJUST_INFORMATION;

typedef struct _SYSTEM_QUERY_TIME_ADJUST_INFORMATION_PRECISE
{
    ULONGLONG TimeAdjustment;
    ULONGLONG TimeIncrement;
    BOOLEAN Enable;
} SYSTEM_QUERY_TIME_ADJUST_INFORMATION_PRECISE, *PSYSTEM_QUERY_TIME_ADJUST_INFORMATION_PRECISE;

typedef struct _SYSTEM_SET_TIME_ADJUST_INFORMATION
{
    ULONG TimeAdjustment;
    BOOLEAN Enable;
} SYSTEM_SET_TIME_ADJUST_INFORMATION, *PSYSTEM_SET_TIME_ADJUST_INFORMATION;

typedef struct _SYSTEM_SET_TIME_ADJUST_INFORMATION_PRECISE
{
    ULONGLONG TimeAdjustment;
    BOOLEAN Enable;
} SYSTEM_SET_TIME_ADJUST_INFORMATION_PRECISE, *PSYSTEM_SET_TIME_ADJUST_INFORMATION_PRECISE;

typedef struct _SYSTEM_EXCEPTION_INFORMATION
{
    ULONG AlignmentFixupCount;
    ULONG ExceptionDispatchCount;
    ULONG FloatingEmulationCount;
    ULONG ByteWordEmulationCount;
} SYSTEM_EXCEPTION_INFORMATION, *PSYSTEM_EXCEPTION_INFORMATION;

typedef enum _SYSTEM_CRASH_DUMP_CONFIGURATION_CLASS
{
    SystemCrashDumpDisable,
    SystemCrashDumpReconfigure,
    SystemCrashDumpInitializationComplete
} SYSTEM_CRASH_DUMP_CONFIGURATION_CLASS, *PSYSTEM_CRASH_DUMP_CONFIGURATION_CLASS;

typedef struct _SYSTEM_CRASH_DUMP_STATE_INFORMATION
{
    SYSTEM_CRASH_DUMP_CONFIGURATION_CLASS CrashDumpConfigurationClass;
} SYSTEM_CRASH_DUMP_STATE_INFORMATION, *PSYSTEM_CRASH_DUMP_STATE_INFORMATION;

typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION
{
    BOOLEAN KernelDebuggerEnabled;
    BOOLEAN KernelDebuggerNotPresent;
} SYSTEM_KERNEL_DEBUGGER_INFORMATION, *PSYSTEM_KERNEL_DEBUGGER_INFORMATION;

typedef struct _SYSTEM_CONTEXT_SWITCH_INFORMATION
{
    ULONG ContextSwitches;
    ULONG FindAny;
    ULONG FindLast;
    ULONG FindIdeal;
    ULONG IdleAny;
    ULONG IdleCurrent;
    ULONG IdleLast;
    ULONG IdleIdeal;
    ULONG PreemptAny;
    ULONG PreemptCurrent;
    ULONG PreemptLast;
    ULONG SwitchToIdle;
} SYSTEM_CONTEXT_SWITCH_INFORMATION, *PSYSTEM_CONTEXT_SWITCH_INFORMATION;

typedef struct _SYSTEM_REGISTRY_QUOTA_INFORMATION
{
    ULONG RegistryQuotaAllowed;
    ULONG RegistryQuotaUsed;
    SIZE_T PagedPoolSize;
} SYSTEM_REGISTRY_QUOTA_INFORMATION, *PSYSTEM_REGISTRY_QUOTA_INFORMATION;

typedef struct _SYSTEM_PROCESSOR_IDLE_INFORMATION
{
    ULONGLONG IdleTime;
    ULONGLONG C1Time;
    ULONGLONG C2Time;
    ULONGLONG C3Time;
    ULONG C1Transitions;
    ULONG C2Transitions;
    ULONG C3Transitions;
    ULONG Padding;
} SYSTEM_PROCESSOR_IDLE_INFORMATION, *PSYSTEM_PROCESSOR_IDLE_INFORMATION;

typedef struct _SYSTEM_LEGACY_DRIVER_INFORMATION
{
    ULONG VetoType;
    UNICODE_STRING VetoList;
} SYSTEM_LEGACY_DRIVER_INFORMATION, *PSYSTEM_LEGACY_DRIVER_INFORMATION;

typedef struct _SYSTEM_LOOKASIDE_INFORMATION
{
    USHORT CurrentDepth;
    USHORT MaximumDepth;
    ULONG TotalAllocates;
    ULONG AllocateMisses;
    ULONG TotalFrees;
    ULONG FreeMisses;
    ULONG Type;
    ULONG Tag;
    ULONG Size;
} SYSTEM_LOOKASIDE_INFORMATION, *PSYSTEM_LOOKASIDE_INFORMATION;

// private
typedef struct _SYSTEM_RANGE_START_INFORMATION
{
    ULONG_PTR SystemRangeStart;
} SYSTEM_RANGE_START_INFORMATION, *PSYSTEM_RANGE_START_INFORMATION;

typedef struct _SYSTEM_VERIFIER_INFORMATION_LEGACY // pre-19H1
{
    ULONG NextEntryOffset;
    ULONG Level;
    UNICODE_STRING DriverName;

    ULONG RaiseIrqls;
    ULONG AcquireSpinLocks;
    ULONG SynchronizeExecutions;
    ULONG AllocationsAttempted;

    ULONG AllocationsSucceeded;
    ULONG AllocationsSucceededSpecialPool;
    ULONG AllocationsWithNoTag;
    ULONG TrimRequests;

    ULONG Trims;
    ULONG AllocationsFailed;
    ULONG AllocationsFailedDeliberately;
    ULONG Loads;

    ULONG Unloads;
    ULONG UnTrackedPool;
    ULONG CurrentPagedPoolAllocations;
    ULONG CurrentNonPagedPoolAllocations;

    ULONG PeakPagedPoolAllocations;
    ULONG PeakNonPagedPoolAllocations;

    SIZE_T PagedPoolUsageInBytes;
    SIZE_T NonPagedPoolUsageInBytes;
    SIZE_T PeakPagedPoolUsageInBytes;
    SIZE_T PeakNonPagedPoolUsageInBytes;
} SYSTEM_VERIFIER_INFORMATION_LEGACY, *PSYSTEM_VERIFIER_INFORMATION_LEGACY;

typedef struct _SYSTEM_VERIFIER_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG Level;
    ULONG RuleClasses[2];
    ULONG TriageContext;
    ULONG AreAllDriversBeingVerified;

    UNICODE_STRING DriverName;

    ULONG RaiseIrqls;
    ULONG AcquireSpinLocks;
    ULONG SynchronizeExecutions;
    ULONG AllocationsAttempted;

    ULONG AllocationsSucceeded;
    ULONG AllocationsSucceededSpecialPool;
    ULONG AllocationsWithNoTag;
    ULONG TrimRequests;

    ULONG Trims;
    ULONG AllocationsFailed;
    ULONG AllocationsFailedDeliberately;
    ULONG Loads;

    ULONG Unloads;
    ULONG UnTrackedPool;
    ULONG CurrentPagedPoolAllocations;
    ULONG CurrentNonPagedPoolAllocations;

    ULONG PeakPagedPoolAllocations;
    ULONG PeakNonPagedPoolAllocations;

    SIZE_T PagedPoolUsageInBytes;
    SIZE_T NonPagedPoolUsageInBytes;
    SIZE_T PeakPagedPoolUsageInBytes;
    SIZE_T PeakNonPagedPoolUsageInBytes;
} SYSTEM_VERIFIER_INFORMATION, *PSYSTEM_VERIFIER_INFORMATION;

// private
typedef struct _SYSTEM_SESSION_PROCESS_INFORMATION
{
    ULONG SessionId;
    ULONG BufferSize;
    PVOID Buffer;
} SYSTEM_SESSION_PROCESS_INFORMATION, *PSYSTEM_SESSION_PROCESS_INFORMATION;

#if !defined(_KERNEL_MODE)

// private
typedef struct _SYSTEM_GDI_DRIVER_INFORMATION
{
    UNICODE_STRING DriverName;
    PVOID ImageAddress;
    PVOID SectionPointer;
    PVOID EntryPoint;
    PIMAGE_EXPORT_DIRECTORY ExportSectionPointer;
    ULONG ImageLength;
} SYSTEM_GDI_DRIVER_INFORMATION, *PSYSTEM_GDI_DRIVER_INFORMATION;
#endif

// geoffchappell
#ifdef _WIN64
#define MAXIMUM_NODE_COUNT 0x40
#else
#define MAXIMUM_NODE_COUNT 0x10
#endif

// private
typedef struct _SYSTEM_NUMA_INFORMATION
{
    ULONG HighestNodeNumber;
    ULONG Reserved;
    union
    {
        GROUP_AFFINITY ActiveProcessorsGroupAffinity[MAXIMUM_NODE_COUNT];
        ULONGLONG AvailableMemory[MAXIMUM_NODE_COUNT];
        ULONGLONG Pad[MAXIMUM_NODE_COUNT * 2];
    } DUMMYUNIONNAME;
} SYSTEM_NUMA_INFORMATION, *PSYSTEM_NUMA_INFORMATION;

typedef struct _SYSTEM_PROCESSOR_POWER_INFORMATION
{
    UCHAR CurrentFrequency;
    UCHAR ThermalLimitFrequency;
    UCHAR ConstantThrottleFrequency;
    UCHAR DegradedThrottleFrequency;
    UCHAR LastBusyFrequency;
    UCHAR LastC3Frequency;
    UCHAR LastAdjustedBusyFrequency;
    UCHAR ProcessorMinThrottle;
    UCHAR ProcessorMaxThrottle;
    ULONG NumberOfFrequencies;
    ULONG PromotionCount;
    ULONG DemotionCount;
    ULONG ErrorCount;
    ULONG RetryCount;
    ULONGLONG CurrentFrequencyTime;
    ULONGLONG CurrentProcessorTime;
    ULONGLONG CurrentProcessorIdleTime;
    ULONGLONG LastProcessorTime;
    ULONGLONG LastProcessorIdleTime;
    ULONGLONG Energy;
} SYSTEM_PROCESSOR_POWER_INFORMATION, *PSYSTEM_PROCESSOR_POWER_INFORMATION;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX
{
    PVOID Object;
    HANDLE UniqueProcessId;
    HANDLE HandleValue;
    ACCESS_MASK GrantedAccess;
    USHORT CreatorBackTraceIndex;
    USHORT ObjectTypeIndex;
    ULONG HandleAttributes;
    ULONG Reserved;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO_EX;

typedef struct _SYSTEM_HANDLE_INFORMATION_EX
{
    ULONG_PTR NumberOfHandles;
    ULONG_PTR Reserved;
    _Field_size_(NumberOfHandles) SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX Handles[1];
} SYSTEM_HANDLE_INFORMATION_EX, *PSYSTEM_HANDLE_INFORMATION_EX;

typedef struct _SYSTEM_BIGPOOL_ENTRY
{
    union
    {
        PVOID VirtualAddress;
        ULONG_PTR NonPaged : 1;
    };
    SIZE_T SizeInBytes;
    union
    {
        UCHAR Tag[4];
        ULONG TagUlong;
    };
} SYSTEM_BIGPOOL_ENTRY, *PSYSTEM_BIGPOOL_ENTRY;

typedef struct _SYSTEM_BIGPOOL_INFORMATION
{
    ULONG Count;
    _Field_size_(Count) SYSTEM_BIGPOOL_ENTRY AllocatedInfo[1];
} SYSTEM_BIGPOOL_INFORMATION, *PSYSTEM_BIGPOOL_INFORMATION;

typedef struct _SYSTEM_POOL_ENTRY
{
    BOOLEAN Allocated;
    BOOLEAN Spare0;
    USHORT AllocatorBackTraceIndex;
    ULONG Size;
    union
    {
        UCHAR Tag[4];
        ULONG TagUlong;
        PVOID ProcessChargedQuota;
    };
} SYSTEM_POOL_ENTRY, *PSYSTEM_POOL_ENTRY;

typedef struct _SYSTEM_POOL_INFORMATION
{
    SIZE_T TotalSize;
    PVOID FirstEntry;
    USHORT EntryOverhead;
    BOOLEAN PoolTagPresent;
    BOOLEAN Spare0;
    ULONG NumberOfEntries;
    _Field_size_(NumberOfEntries) SYSTEM_POOL_ENTRY Entries[1];
} SYSTEM_POOL_INFORMATION, *PSYSTEM_POOL_INFORMATION;

typedef struct _SYSTEM_SESSION_POOLTAG_INFORMATION
{
    SIZE_T NextEntryOffset;
    ULONG SessionId;
    ULONG Count;
    _Field_size_(Count) SYSTEM_POOLTAG TagInfo[1];
} SYSTEM_SESSION_POOLTAG_INFORMATION, *PSYSTEM_SESSION_POOLTAG_INFORMATION;

typedef struct _SYSTEM_SESSION_MAPPED_VIEW_INFORMATION
{
    SIZE_T NextEntryOffset;
    ULONG SessionId;
    ULONG ViewFailures;
    SIZE_T NumberOfBytesAvailable;
    SIZE_T NumberOfBytesAvailableContiguous;
} SYSTEM_SESSION_MAPPED_VIEW_INFORMATION, *PSYSTEM_SESSION_MAPPED_VIEW_INFORMATION;

typedef enum _WATCHDOG_HANDLER_ACTION
{
    WdActionSetTimeoutValue,
    WdActionQueryTimeoutValue,
    WdActionResetTimer,
    WdActionStopTimer,
    WdActionStartTimer,
    WdActionSetTriggerAction,
    WdActionQueryTriggerAction,
    WdActionQueryState
} WATCHDOG_HANDLER_ACTION;

typedef
_Function_class_(SYSTEM_WATCHDOG_HANDLER)
NTSTATUS
NTAPI
SYSTEM_WATCHDOG_HANDLER(
    _In_ WATCHDOG_HANDLER_ACTION Action,
    _In_ PVOID Context,
    _Inout_ PULONG DataValue,
    _In_ BOOLEAN NoLocks);
typedef SYSTEM_WATCHDOG_HANDLER* PSYSTEM_WATCHDOG_HANDLER;

// private
typedef struct _SYSTEM_WATCHDOG_HANDLER_INFORMATION
{
    PSYSTEM_WATCHDOG_HANDLER WdHandler;
    PVOID Context;
} SYSTEM_WATCHDOG_HANDLER_INFORMATION, *PSYSTEM_WATCHDOG_HANDLER_INFORMATION;

typedef enum _WATCHDOG_INFORMATION_CLASS
{
    WdInfoTimeoutValue = 0,
    WdInfoResetTimer = 1,
    WdInfoStopTimer = 2,
    WdInfoStartTimer = 3,
    WdInfoTriggerAction = 4,
    WdInfoState = 5,
    WdInfoTriggerReset = 6,
    WdInfoNop = 7,
    WdInfoGeneratedLastReset = 8,
    WdInfoInvalid = 9,
} WATCHDOG_INFORMATION_CLASS;

// private
typedef struct _SYSTEM_WATCHDOG_TIMER_INFORMATION
{
    WATCHDOG_INFORMATION_CLASS WdInfoClass;
    ULONG DataValue;
} SYSTEM_WATCHDOG_TIMER_INFORMATION, *PSYSTEM_WATCHDOG_TIMER_INFORMATION;

#if !defined(_KERNEL_MODE)
// private
typedef enum _SYSTEM_FIRMWARE_TABLE_ACTION
{
    SystemFirmwareTableEnumerate,
    SystemFirmwareTableGet,
    SystemFirmwareTableMax
} SYSTEM_FIRMWARE_TABLE_ACTION;

// private
typedef struct _SYSTEM_FIRMWARE_TABLE_INFORMATION
{
    ULONG ProviderSignature; // (same as the GetSystemFirmwareTable function)
    SYSTEM_FIRMWARE_TABLE_ACTION Action;
    ULONG TableID;
    ULONG TableBufferLength;
    _Field_size_bytes_(TableBufferLength) UCHAR TableBuffer[1];
} SYSTEM_FIRMWARE_TABLE_INFORMATION, *PSYSTEM_FIRMWARE_TABLE_INFORMATION;
#endif

#if !defined(_KERNEL_MODE)

// private
typedef _Function_class_(FNFTH)
NTSTATUS STDAPIVCALLTYPE FNFTH(
    _Inout_ PSYSTEM_FIRMWARE_TABLE_INFORMATION SystemFirmwareTableInfo
    );
typedef FNFTH* PFNFTH;

// private
typedef struct _SYSTEM_FIRMWARE_TABLE_HANDLER
{
    ULONG ProviderSignature;
    BOOLEAN Register;
    PFNFTH FirmwareTableHandler;
    PVOID DriverObject;
} SYSTEM_FIRMWARE_TABLE_HANDLER, *PSYSTEM_FIRMWARE_TABLE_HANDLER;

#endif

// private
typedef struct _SYSTEM_MEMORY_LIST_INFORMATION
{
    SIZE_T ZeroPageCount;
    SIZE_T FreePageCount;
    SIZE_T ModifiedPageCount;
    SIZE_T ModifiedNoWritePageCount;
    SIZE_T BadPageCount;
    SIZE_T PageCountByPriority[8];
    SIZE_T RepurposedPagesByPriority[8];
    SIZE_T ModifiedPageCountPageFile;
} SYSTEM_MEMORY_LIST_INFORMATION, *PSYSTEM_MEMORY_LIST_INFORMATION;

// private
typedef enum _SYSTEM_MEMORY_LIST_COMMAND
{
    MemoryCaptureAccessedBits,
    MemoryCaptureAndResetAccessedBits,
    MemoryEmptyWorkingSets,
    MemoryFlushModifiedList,
    MemoryPurgeStandbyList,
    MemoryPurgeLowPriorityStandbyList,
    MemoryCommandMax
} SYSTEM_MEMORY_LIST_COMMAND;

// private
typedef struct _SYSTEM_THREAD_CID_PRIORITY_INFORMATION
{
    CLIENT_ID ClientId;
    KPRIORITY Priority;
} SYSTEM_THREAD_CID_PRIORITY_INFORMATION, *PSYSTEM_THREAD_CID_PRIORITY_INFORMATION;

// private
/**
 * The SYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION structure contains the cumulative number of clock cycles a logical processor
 * has spent running its idle thread, deferred procedure calls (DPCs) and interrupt service routines (ISRs) since it became active.
 */
typedef struct _SYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION
{
    ULONGLONG CycleTime;
} SYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION, *PSYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION;

// private
typedef struct _SYSTEM_VERIFIER_ISSUE
{
    ULONGLONG IssueType;
    PVOID Address;
    ULONGLONG Parameters[2];
} SYSTEM_VERIFIER_ISSUE, *PSYSTEM_VERIFIER_ISSUE;

// private
typedef struct _SYSTEM_VERIFIER_CANCELLATION_INFORMATION
{
    ULONG CancelProbability;
    ULONG CancelThreshold;
    ULONG CompletionThreshold;
    ULONG CancellationVerifierDisabled;
    ULONG AvailableIssues;
    SYSTEM_VERIFIER_ISSUE Issues[128];
} SYSTEM_VERIFIER_CANCELLATION_INFORMATION, *PSYSTEM_VERIFIER_CANCELLATION_INFORMATION;

// private
typedef struct _SYSTEM_REF_TRACE_INFORMATION
{
    BOOLEAN TraceEnable;
    BOOLEAN TracePermanent;
    UNICODE_STRING TraceProcessName;
    UNICODE_STRING TracePoolTags;
} SYSTEM_REF_TRACE_INFORMATION, *PSYSTEM_REF_TRACE_INFORMATION;

// private
typedef struct _SYSTEM_SPECIAL_POOL_INFORMATION
{
    ULONG PoolTag;
    ULONG Flags;
} SYSTEM_SPECIAL_POOL_INFORMATION, *PSYSTEM_SPECIAL_POOL_INFORMATION;

// private
typedef struct _SYSTEM_PROCESS_ID_INFORMATION
{
    HANDLE ProcessId;
    UNICODE_STRING ImageName;
} SYSTEM_PROCESS_ID_INFORMATION, *PSYSTEM_PROCESS_ID_INFORMATION;

// private
typedef struct _SYSTEM_HYPERVISOR_QUERY_INFORMATION
{
    BOOLEAN HypervisorConnected;
    BOOLEAN HypervisorDebuggingEnabled;
    BOOLEAN HypervisorPresent;
    BOOLEAN Spare0[5];
    ULONGLONG EnabledEnlightenments;
} SYSTEM_HYPERVISOR_QUERY_INFORMATION, *PSYSTEM_HYPERVISOR_QUERY_INFORMATION;

// private
typedef struct _SYSTEM_BOOT_ENVIRONMENT_INFORMATION
{
    GUID BootIdentifier;
    FIRMWARE_TYPE FirmwareType;
    union
    {
        ULONGLONG BootFlags;
        struct
        {
            ULONGLONG DbgMenuOsSelection : 1; // REDSTONE4
            ULONGLONG DbgHiberBoot : 1;
            ULONGLONG DbgSoftBoot : 1;
            ULONGLONG DbgMeasuredLaunch : 1;
            ULONGLONG DbgMeasuredLaunchCapable : 1; // 19H1
            ULONGLONG DbgSystemHiveReplace : 1;
            ULONGLONG DbgMeasuredLaunchSmmProtections : 1;
            ULONGLONG DbgMeasuredLaunchSmmLevel : 7; // 20H1
            ULONGLONG DbgBugCheckRecovery : 1; // 24H2
            ULONGLONG DbgFASR : 1;
            ULONGLONG DbgUseCachedBcd : 1;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} SYSTEM_BOOT_ENVIRONMENT_INFORMATION, *PSYSTEM_BOOT_ENVIRONMENT_INFORMATION;

// private
typedef struct _SYSTEM_IMAGE_FILE_EXECUTION_OPTIONS_INFORMATION
{
    ULONG FlagsToEnable;
    ULONG FlagsToDisable;
} SYSTEM_IMAGE_FILE_EXECUTION_OPTIONS_INFORMATION, *PSYSTEM_IMAGE_FILE_EXECUTION_OPTIONS_INFORMATION;

// private
typedef enum _COVERAGE_REQUEST_CODES
{
    CoverageAllModules = 0,
    CoverageSearchByHash = 1,
    CoverageSearchByName = 2
} COVERAGE_REQUEST_CODES;

// private
typedef struct _COVERAGE_MODULE_REQUEST
{
    COVERAGE_REQUEST_CODES RequestType;
    union
    {
        UCHAR MD5Hash[16];
        UNICODE_STRING ModuleName;
    } SearchInfo;
} COVERAGE_MODULE_REQUEST, *PCOVERAGE_MODULE_REQUEST;

// private
typedef struct _COVERAGE_MODULE_INFO
{
    ULONG ModuleInfoSize;
    ULONG IsBinaryLoaded;
    UNICODE_STRING ModulePathName;
    ULONG CoverageSectionSize;
    UCHAR CoverageSection[1];
} COVERAGE_MODULE_INFO, *PCOVERAGE_MODULE_INFO;

// private
typedef struct _COVERAGE_MODULES
{
    ULONG ListAndReset;
    ULONG NumberOfModules;
    COVERAGE_MODULE_REQUEST ModuleRequestInfo;
    COVERAGE_MODULE_INFO Modules[1];
} COVERAGE_MODULES, *PCOVERAGE_MODULES;

// private
typedef struct _SYSTEM_PREFETCH_PATCH_INFORMATION
{
    ULONG PrefetchPatchCount;
} SYSTEM_PREFETCH_PATCH_INFORMATION, *PSYSTEM_PREFETCH_PATCH_INFORMATION;

// private
typedef struct _SYSTEM_VERIFIER_FAULTS_INFORMATION
{
    ULONG Probability;
    ULONG MaxProbability;
    UNICODE_STRING PoolTags;
    UNICODE_STRING Applications;
} SYSTEM_VERIFIER_FAULTS_INFORMATION, *PSYSTEM_VERIFIER_FAULTS_INFORMATION;

// private
typedef struct _SYSTEM_VERIFIER_INFORMATION_EX
{
    ULONG VerifyMode;
    ULONG OptionChanges;
    UNICODE_STRING PreviousBucketName;
    ULONG IrpCancelTimeoutMsec;
    ULONG VerifierExtensionEnabled;
#ifdef _WIN64
    ULONG Reserved[1];
#else
    ULONG Reserved[3];
#endif
} SYSTEM_VERIFIER_INFORMATION_EX, *PSYSTEM_VERIFIER_INFORMATION_EX;

// private
typedef struct _SYSTEM_SYSTEM_PARTITION_INFORMATION
{
    UNICODE_STRING SystemPartition;
} SYSTEM_SYSTEM_PARTITION_INFORMATION, *PSYSTEM_SYSTEM_PARTITION_INFORMATION;

// private
typedef struct _SYSTEM_SYSTEM_DISK_INFORMATION
{
    UNICODE_STRING SystemDisk;
} SYSTEM_SYSTEM_DISK_INFORMATION, *PSYSTEM_SYSTEM_DISK_INFORMATION;

// private
typedef struct _SYSTEM_NUMA_PROXIMITY_MAP
{
    ULONG NodeProximityId;
    USHORT NodeNumber;
} SYSTEM_NUMA_PROXIMITY_MAP, *PSYSTEM_NUMA_PROXIMITY_MAP;

// private (Windows 8.1 and above)
typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_HITCOUNT
{
    ULONGLONG Hits;
    UCHAR PercentFrequency;
} SYSTEM_PROCESSOR_PERFORMANCE_HITCOUNT, *PSYSTEM_PROCESSOR_PERFORMANCE_HITCOUNT;

// private (Windows 8.1 and above)
typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION
{
    ULONG ProcessorNumber;
    ULONG StateCount;
    _Field_size_(StateCount) SYSTEM_PROCESSOR_PERFORMANCE_HITCOUNT States[1];
} SYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION, *PSYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION;

// private (Windows 7 and Windows 8)
typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_HITCOUNT_WIN8
{
    ULONG Hits;
    UCHAR PercentFrequency;
} SYSTEM_PROCESSOR_PERFORMANCE_HITCOUNT_WIN8, *PSYSTEM_PROCESSOR_PERFORMANCE_HITCOUNT_WIN8;

// private (Windows 7 and Windows 8)
typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION_WIN8
{
    ULONG ProcessorNumber;
    ULONG StateCount;
    _Field_size_(StateCount) SYSTEM_PROCESSOR_PERFORMANCE_HITCOUNT_WIN8 States[1];
} SYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION_WIN8, *PSYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION_WIN8;

// private
typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION
{
    ULONG ProcessorCount;
    ULONG Offsets[1];
} SYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION, *PSYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION;

#define CODEINTEGRITY_OPTION_ENABLED 0x01
#define CODEINTEGRITY_OPTION_TESTSIGN 0x02
#define CODEINTEGRITY_OPTION_UMCI_ENABLED 0x04
#define CODEINTEGRITY_OPTION_UMCI_AUDITMODE_ENABLED 0x08
#define CODEINTEGRITY_OPTION_UMCI_EXCLUSIONPATHS_ENABLED 0x10
#define CODEINTEGRITY_OPTION_TEST_BUILD 0x20
#define CODEINTEGRITY_OPTION_PREPRODUCTION_BUILD 0x40
#define CODEINTEGRITY_OPTION_DEBUGMODE_ENABLED 0x80
#define CODEINTEGRITY_OPTION_FLIGHT_BUILD 0x100
#define CODEINTEGRITY_OPTION_FLIGHTING_ENABLED 0x200
#define CODEINTEGRITY_OPTION_HVCI_KMCI_ENABLED 0x400
#define CODEINTEGRITY_OPTION_HVCI_KMCI_AUDITMODE_ENABLED 0x800
#define CODEINTEGRITY_OPTION_HVCI_KMCI_STRICTMODE_ENABLED 0x1000
#define CODEINTEGRITY_OPTION_HVCI_IUM_ENABLED 0x2000
#define CODEINTEGRITY_OPTION_WHQL_ENFORCEMENT_ENABLED 0x4000
#define CODEINTEGRITY_OPTION_WHQL_AUDITMODE_ENABLED 0x8000

// private
typedef struct _SYSTEM_CODEINTEGRITY_INFORMATION
{
    ULONG Length;
    union
    {
        ULONG CodeIntegrityOptions;
        struct
        {
            ULONG Enabled : 1;
            ULONG TestSign : 1;
            ULONG UmciEnabled : 1;
            ULONG UmciAuditModeEnabled : 1;
            ULONG UmciExclusionPathsEnabled : 1;
            ULONG TestBuild : 1;
            ULONG PreproductionBuild : 1;
            ULONG DebugModeEnabled : 1;
            ULONG FlightBuild : 1;
            ULONG FlightingEnabled : 1;
            ULONG HvciKmciEnabled : 1;
            ULONG HvciKmciAuditModeEnabled : 1;
            ULONG HvciKmciStrictModeEnabled : 1;
            ULONG HvciIumEnabled : 1;
            ULONG WhqlEnforcementEnabled : 1;
            ULONG WhqlAuditModeEnabled : 1;
            ULONG Spare : 16;
        };
    };
} SYSTEM_CODEINTEGRITY_INFORMATION, *PSYSTEM_CODEINTEGRITY_INFORMATION;

// private
typedef struct _SYSTEM_PROCESSOR_MICROCODE_UPDATE_INFORMATION
{
    ULONG Operation;
} SYSTEM_PROCESSOR_MICROCODE_UPDATE_INFORMATION, *PSYSTEM_PROCESSOR_MICROCODE_UPDATE_INFORMATION;

// private
typedef enum _SYSTEM_VA_TYPE
{
    SystemVaTypeAll,
    SystemVaTypeNonPagedPool,
    SystemVaTypePagedPool,
    SystemVaTypeSystemCache,
    SystemVaTypeSystemPtes,
    SystemVaTypeSessionSpace,
    SystemVaTypeMax
} SYSTEM_VA_TYPE, *PSYSTEM_VA_TYPE;

// private
typedef struct _SYSTEM_VA_LIST_INFORMATION
{
    SIZE_T VirtualSize;
    SIZE_T VirtualPeak;
    SIZE_T VirtualLimit;
    SIZE_T AllocationFailures;
} SYSTEM_VA_LIST_INFORMATION, *PSYSTEM_VA_LIST_INFORMATION;

// rev
typedef enum _STORE_INFORMATION_CLASS
{
    StorePageRequest = 1,
    StoreStatsRequest = 2, // q: SM_STATS_REQUEST // SmProcessStatsRequest
    StoreCreateRequest = 3, // s: SM_CREATE_REQUEST (requires SeProfileSingleProcessPrivilege)
    StoreDeleteRequest = 4, // s: SM_DELETE_REQUEST (requires SeProfileSingleProcessPrivilege)
    StoreListRequest = 5, // q: SM_STORE_LIST_REQUEST / SM_STORE_LIST_REQUEST_EX // SmProcessListRequest
    Available1 = 6,
    StoreEmptyRequest = 7,
    CacheListRequest = 8, // q: SMC_CACHE_LIST_REQUEST // SmcProcessListRequest
    CacheCreateRequest = 9, // s: SMC_CACHE_CREATE_REQUEST (requires SeProfileSingleProcessPrivilege)
    CacheDeleteRequest = 10, // s: SMC_CACHE_DELETE_REQUEST (requires SeProfileSingleProcessPrivilege)
    CacheStoreCreateRequest = 11, // s: SMC_STORE_CREATE_REQUEST (requires SeProfileSingleProcessPrivilege)
    CacheStoreDeleteRequest = 12, // s: SMC_STORE_DELETE_REQUEST (requires SeProfileSingleProcessPrivilege)
    CacheStatsRequest = 13, // q: SMC_CACHE_STATS_REQUEST // SmcProcessStatsRequest
    Available2 = 14,
    RegistrationRequest = 15, // q: SM_REGISTRATION_REQUEST (requires SeProfileSingleProcessPrivilege) // SmProcessRegistrationRequest
    GlobalCacheStatsRequest = 16,
    StoreResizeRequest = 17, // s: SM_STORE_RESIZE_REQUEST (requires SeProfileSingleProcessPrivilege)
    CacheStoreResizeRequest = 18, // s: SM_STORE_CACHE_RESIZE_REQUEST (requires SeProfileSingleProcessPrivilege)
    SmConfigRequest = 19, // s: SM_CONFIG_REQUEST (requires SeProfileSingleProcessPrivilege)
    StoreHighMemoryPriorityRequest = 20, // s: SM_STORE_HIGH_MEMORY_PRIORITY_REQUEST (requires SeProfileSingleProcessPrivilege)
    SystemStoreTrimRequest = 21, // s: SM_SYSTEM_STORE_TRIM_REQUEST (requires SeProfileSingleProcessPrivilege) // SmProcessSystemStoreTrimRequest
    MemCompressionInfoRequest = 22,  // q: SM_STORE_COMPRESSION_INFORMATION_REQUEST // SmProcessCompressionInfoRequest
    ProcessStoreInfoRequest = 23, // SmProcessProcessStoreInfoRequest
    StoreInformationMax
} STORE_INFORMATION_CLASS;

// rev
#define SYSTEM_STORE_INFORMATION_VERSION 1

// rev
typedef struct _SYSTEM_STORE_INFORMATION
{
    _In_ ULONG Version;
    _In_ STORE_INFORMATION_CLASS StoreInformationClass;
    _Inout_ PVOID Data;
    _Inout_ ULONG Length;
} SYSTEM_STORE_INFORMATION, *PSYSTEM_STORE_INFORMATION;

#define SYSTEM_STORE_STATS_INFORMATION_VERSION 2

typedef enum _ST_STATS_LEVEL
{
    StStatsLevelBasic = 0,
    StStatsLevelIoStats = 1,
    StStatsLevelRegionSpace = 2, // requires SeProfileSingleProcessPrivilege
    StStatsLevelSpaceBitmap = 3, // requires SeProfileSingleProcessPrivilege
    StStatsLevelMax = 4
} ST_STATS_LEVEL;

typedef struct _SM_STATS_REQUEST
{
    ULONG Version : 8; // SYSTEM_STORE_STATS_INFORMATION_VERSION
    ULONG DetailLevel : 8; // ST_STATS_LEVEL
    ULONG StoreId : 16;
    ULONG BufferSize;
    PVOID Buffer; // PST_STATS
} SM_STATS_REQUEST, *PSM_STATS_REQUEST;

typedef struct _ST_DATA_MGR_STATS
{
    ULONG RegionCount;
    ULONG PagesStored;
    ULONG UniquePagesStored;
    ULONG LazyCleanupRegionCount;
    struct
    {
        ULONG RegionsInUse;
        ULONG SpaceUsed;
    } Space[8];
} ST_DATA_MGR_STATS, *PST_DATA_MGR_STATS;

typedef struct _ST_IO_STATS_PERIOD
{
    ULONG PageCounts[5];
} ST_IO_STATS_PERIOD, *PST_IO_STATS_PERIOD;

typedef struct _ST_IO_STATS
{
    ULONG PeriodCount;
    ST_IO_STATS_PERIOD Periods[64];
} ST_IO_STATS, *PST_IO_STATS;

typedef struct _ST_READ_LATENCY_BUCKET
{
    ULONG LatencyUs;
    ULONG Count;
} ST_READ_LATENCY_BUCKET, *PST_READ_LATENCY_BUCKET;

typedef struct _ST_READ_LATENCY_STATS
{
    ST_READ_LATENCY_BUCKET Buckets[8];
} ST_READ_LATENCY_STATS, *PST_READ_LATENCY_STATS;

// rev
typedef struct _ST_STATS_REGION_INFO
{
    USHORT SpaceUsed;
    UCHAR Priority;
    UCHAR Spare;
} ST_STATS_REGION_INFO, *PST_STATS_REGION_INFO;

// rev
typedef struct _ST_STATS_SPACE_BITMAP
{
    SIZE_T CompressedBytes;
    ULONG BytesPerBit;
    UCHAR StoreBitmap[1];
} ST_STATS_SPACE_BITMAP, *PST_STATS_SPACE_BITMAP;

// rev
typedef struct _ST_STATS
{
    ULONG Version : 8;
    ULONG Level : 4;
    ULONG StoreType : 4;
    ULONG NoDuplication : 1;
    ULONG NoCompression : 1;
    ULONG EncryptionStrength : 12;
    ULONG VirtualRegions : 1;
    ULONG Spare0 : 1;
    ULONG Size;
    USHORT CompressionFormat;
    USHORT Spare;

    struct
    {
        ULONG RegionSize;
        ULONG RegionCount;
        ULONG RegionCountMax;
        ULONG Granularity;
        ST_DATA_MGR_STATS UserData;
        ST_DATA_MGR_STATS Metadata;
    } Basic;

    struct
    {
        ST_IO_STATS IoStats;
        ST_READ_LATENCY_STATS ReadLatencyStats;
    } Io;

    // ST_STATS_REGION_INFO[RegionCountMax]
    // ST_STATS_SPACE_BITMAP
} ST_STATS, *PST_STATS;

#define SYSTEM_STORE_CREATE_INFORMATION_VERSION 6

typedef enum _SM_STORE_TYPE
{
    StoreTypeInMemory = 0,
    StoreTypeFile = 1,
    StoreTypeMax = 2
} SM_STORE_TYPE;

typedef struct _SM_STORE_BASIC_PARAMS
{
    union
    {
        struct
        {
            ULONG StoreType : 8; // SM_STORE_TYPE
            ULONG NoDuplication : 1;
            ULONG FailNoCompression : 1;
            ULONG NoCompression : 1;
            ULONG NoEncryption : 1;
            ULONG NoEvictOnAdd : 1;
            ULONG PerformsFileIo : 1;
            ULONG VdlNotSet : 1;
            ULONG UseIntermediateAddBuffer : 1;
            ULONG CompressNoHuff : 1;
            ULONG LockActiveRegions : 1;
            ULONG VirtualRegions : 1;
            ULONG Spare : 13;
        } DUMMYSTRUCTNAME;
        ULONG StoreFlags;
    } DUMMYUNIONNAME;
    ULONG Granularity;
    ULONG RegionSize;
    ULONG RegionCountMax;
} SM_STORE_BASIC_PARAMS, *PSM_STORE_BASIC_PARAMS;

typedef struct _SMKM_REGION_EXTENT
{
    ULONG RegionCount;
    SIZE_T ByteOffset;
} SMKM_REGION_EXTENT, *PSMKM_REGION_EXTENT;

typedef struct _RTL_BITMAP *PRTL_BITMAP;

typedef struct _SMKM_FILE_INFO
{
    HANDLE FileHandle;
    PFILE_OBJECT FileObject;
    PFILE_OBJECT VolumeFileObject;
    PDEVICE_OBJECT VolumeDeviceObject;
    HANDLE VolumePnpHandle;
    PIRP UsageNotificationIrp;
    PSMKM_REGION_EXTENT Extents;
    ULONG ExtentCount;
} SMKM_FILE_INFO, *PSMKM_FILE_INFO;

typedef struct _SM_STORE_CACHE_BACKED_PARAMS
{
    ULONG SectorSize;
    PCHAR EncryptionKey;
    ULONG EncryptionKeySize;
    PSMKM_FILE_INFO FileInfo;
    PVOID EtaContext;
    PRTL_BITMAP StoreRegionBitmap;
} SM_STORE_CACHE_BACKED_PARAMS, *PSM_STORE_CACHE_BACKED_PARAMS;

typedef struct _SM_STORE_PARAMETERS
{
    SM_STORE_BASIC_PARAMS Store;
    ULONG Priority;
    ULONG Flags;
    SM_STORE_CACHE_BACKED_PARAMS CacheBacked;
} SM_STORE_PARAMETERS, *PSM_STORE_PARAMETERS;

typedef struct _SM_CREATE_REQUEST
{
    ULONG Version : 8; // SYSTEM_STORE_CREATE_INFORMATION_VERSION
    ULONG AcquireReference : 1;
    ULONG KeyedStore : 1;
    ULONG Spare : 22;
    SM_STORE_PARAMETERS Params;
    ULONG StoreId;
} SM_CREATE_REQUEST, *PSM_CREATE_REQUEST;

#define SYSTEM_STORE_DELETE_INFORMATION_VERSION 1

typedef struct _SM_DELETE_REQUEST
{
    ULONG Version : 8; // SYSTEM_STORE_DELETE_INFORMATION_VERSION
    ULONG Spare : 24;
    ULONG StoreId;
} SM_DELETE_REQUEST, *PSM_DELETE_REQUEST;

#define SYSTEM_STORE_LIST_INFORMATION_VERSION 2

typedef struct _SM_STORE_LIST_REQUEST
{
    ULONG Version : 8; // SYSTEM_STORE_LIST_INFORMATION_VERSION
    ULONG StoreCount : 8; // = 0
    ULONG ExtendedRequest : 1; // SM_STORE_LIST_REQUEST_EX if set
    ULONG Spare : 15;
    ULONG StoreId[32];
} SM_STORE_LIST_REQUEST, *PSM_STORE_LIST_REQUEST;

typedef struct _SM_STORE_LIST_REQUEST_EX
{
    SM_STORE_LIST_REQUEST Request;
    WCHAR NameBuffer[32][64];
} SM_STORE_LIST_REQUEST_EX, *PSM_STORE_LIST_REQUEST_EX;

#define SYSTEM_CACHE_LIST_INFORMATION_VERSION 2

typedef struct _SMC_CACHE_LIST_REQUEST
{
    ULONG Version : 8; // SYSTEM_CACHE_LIST_INFORMATION_VERSION
    ULONG CacheCount : 8; // = 0
    ULONG Spare : 16;
    ULONG CacheId[16];
} SMC_CACHE_LIST_REQUEST, *PSMC_CACHE_LIST_REQUEST;

#define SYSTEM_CACHE_CREATE_INFORMATION_VERSION 3

typedef struct _SMC_CACHE_PARAMETERS
{
    SIZE_T CacheFileSize;
    ULONG StoreAlignment;
    ULONG PerformsFileIo : 1;
    ULONG VdlNotSet : 1;
    ULONG Spare : 30;
    ULONG CacheFlags;
    ULONG Priority;
} SMC_CACHE_PARAMETERS, *PSMC_CACHE_PARAMETERS;

typedef struct _SMC_CACHE_CREATE_PARAMETERS
{
    SMC_CACHE_PARAMETERS CacheParameters;
    WCHAR TemplateFilePath[512];
} SMC_CACHE_CREATE_PARAMETERS, *PSMC_CACHE_CREATE_PARAMETERS;

typedef struct _SMC_CACHE_CREATE_REQUEST
{
    ULONG Version : 8; // SYSTEM_CACHE_CREATE_INFORMATION_VERSION
    ULONG Spare : 24;
    ULONG CacheId;
    SMC_CACHE_CREATE_PARAMETERS CacheCreateParams;
} SMC_CACHE_CREATE_REQUEST, *PSMC_CACHE_CREATE_REQUEST;

#define SYSTEM_CACHE_DELETE_INFORMATION_VERSION 1

typedef struct _SMC_CACHE_DELETE_REQUEST
{
    ULONG Version : 8; // SYSTEM_CACHE_DELETE_INFORMATION_VERSION
    ULONG Spare : 24;
    ULONG CacheId;
} SMC_CACHE_DELETE_REQUEST, *PSMC_CACHE_DELETE_REQUEST;

#define SYSTEM_CACHE_STORE_CREATE_INFORMATION_VERSION 2

typedef enum _SM_STORE_MANAGER_TYPE
{
    SmStoreManagerTypePhysical = 0,
    SmStoreManagerTypeVirtual = 1,
    SmStoreManagerTypeMax = 2
} SM_STORE_MANAGER_TYPE;

typedef struct _SMC_STORE_CREATE_REQUEST
{
    ULONG Version : 8; // SYSTEM_CACHE_STORE_CREATE_INFORMATION_VERSION
    ULONG Spare : 24;
    SM_STORE_BASIC_PARAMS StoreParams;
    ULONG CacheId;
    SM_STORE_MANAGER_TYPE StoreManagerType;
    ULONG StoreId;
} SMC_STORE_CREATE_REQUEST, *PSMC_STORE_CREATE_REQUEST;

#define SYSTEM_CACHE_STORE_DELETE_INFORMATION_VERSION 1

typedef struct _SMC_STORE_DELETE_REQUEST
{
    ULONG Version : 8; // SYSTEM_CACHE_STORE_DELETE_INFORMATION_VERSION
    ULONG Spare : 24;
    ULONG CacheId;
    SM_STORE_MANAGER_TYPE StoreManagerType;
    ULONG StoreId;
} SMC_STORE_DELETE_REQUEST, *PSMC_STORE_DELETE_REQUEST;

#define SYSTEM_CACHE_STATS_INFORMATION_VERSION 3

typedef struct _SMC_CACHE_STATS
{
    SIZE_T TotalFileSize;
    ULONG StoreCount;
    ULONG RegionCount;
    ULONG RegionSizeBytes;
    ULONG FileCount : 6;
    ULONG PerformsFileIo : 1;
    ULONG Spare : 25;
    ULONG StoreIds[16];
    ULONG PhysicalStoreBitmap;
    ULONG Priority;
    WCHAR TemplateFilePath[512];
} SMC_CACHE_STATS, *PSMC_CACHE_STATS;

typedef struct _SMC_CACHE_STATS_REQUEST
{
    ULONG Version : 8; // SYSTEM_CACHE_STATS_INFORMATION_VERSION
    ULONG NoFilePath : 1;
    ULONG Spare : 23;
    ULONG CacheId;
    SMC_CACHE_STATS CacheStats;
} SMC_CACHE_STATS_REQUEST, *PSMC_CACHE_STATS_REQUEST;

#define SYSTEM_STORE_REGISTRATION_INFORMATION_VERSION 2

typedef struct _SM_REGISTRATION_INFO
{
    HANDLE CachesUpdatedEvent;
} SM_REGISTRATION_INFO, *PSM_REGISTRATION_INFO;

typedef struct _SM_REGISTRATION_REQUEST
{
    ULONG Version : 8; // SYSTEM_STORE_REGISTRATION_INFORMATION_VERSION
    ULONG Spare : 24;
    SM_REGISTRATION_INFO RegInfo;
} SM_REGISTRATION_REQUEST, *PSM_REGISTRATION_REQUEST;

#define SYSTEM_STORE_RESIZE_INFORMATION_VERSION 6

typedef struct _SM_STORE_RESIZE_REQUEST
{
    ULONG Version : 8; // SYSTEM_STORE_RESIZE_INFORMATION_VERSION
    ULONG AddRegions : 1;
    ULONG Spare : 23;
    ULONG StoreId;
    ULONG NumberOfRegions;
    PRTL_BITMAP RegionBitmap;
} SM_STORE_RESIZE_REQUEST, *PSM_STORE_RESIZE_REQUEST;

#define SYSTEM_CACHE_STORE_RESIZE_INFORMATION_VERSION 1

typedef struct _SM_STORE_CACHE_RESIZE_REQUEST
{
    ULONG Version : 8; // SYSTEM_CACHE_STORE_RESIZE_INFORMATION_VERSION
    ULONG AddRegions : 1;
    ULONG Spare : 23;
    ULONG CacheId;
    ULONG StoreId;
    SM_STORE_MANAGER_TYPE StoreManagerType;
    ULONG RegionCount;
} SM_STORE_CACHE_RESIZE_REQUEST, *PSM_STORE_CACHE_RESIZE_REQUEST;

#define SYSTEM_STORE_CONFIG_INFORMATION_VERSION 4

typedef enum _SM_CONFIG_TYPE
{
    SmConfigDirtyPageCompression = 0,
    SmConfigAsyncInswap = 1,
    SmConfigPrefetchSeekThreshold = 2,
    SmConfigTypeMax = 3
} SM_CONFIG_TYPE;

typedef struct _SM_CONFIG_REQUEST
{
    ULONG Version : 8; // SYSTEM_STORE_CONFIG_INFORMATION_VERSION
    ULONG Spare : 16;
    ULONG ConfigType : 8; // SM_CONFIG_TYPE
    ULONG ConfigValue;
} SM_CONFIG_REQUEST, *PSM_CONFIG_REQUEST;

#define SYSTEM_STORE_HIGH_MEM_PRIORITY_INFORMATION_VERSION 1

// rev
typedef struct _SM_STORE_HIGH_MEMORY_PRIORITY_REQUEST
{
    ULONG Version : 8; // SYSTEM_STORE_HIGH_MEMORY_PRIORITY_INFORMATION_VERSION
    ULONG SetHighMemoryPriority : 1;
    ULONG Spare : 23;
    HANDLE ProcessHandle;
} SM_STORE_HIGH_MEMORY_PRIORITY_REQUEST, *PSM_STORE_HIGH_MEMORY_PRIORITY_REQUEST;

// rev
typedef struct _SM_SYSTEM_STORE_TRIM_REQUEST
{
    ULONG Version : 8;  // SYSTEM_STORE_TRIM_INFORMATION_VERSION
    ULONG Spare : 24;
    SIZE_T PagesToTrim;
    HANDLE PartitionHandle; // since 24H2
} SM_SYSTEM_STORE_TRIM_REQUEST, *PSM_SYSTEM_STORE_TRIM_REQUEST;

// rev
#define SYSTEM_STORE_TRIM_INFORMATION_VERSION_V1 1 // WIN10
#define SYSTEM_STORE_TRIM_INFORMATION_VERSION_V2 2 // 24H2
#define SYSTEM_STORE_TRIM_INFORMATION_VERSION SYSTEM_STORE_TRIM_INFORMATION_VERSION_V1

// rev
#define SYSTEM_STORE_TRIM_INFORMATION_SIZE_V1 RTL_SIZEOF_THROUGH_FIELD(SM_SYSTEM_STORE_TRIM_REQUEST, PagesToTrim) // WIN10
#define SYSTEM_STORE_TRIM_INFORMATION_SIZE_V2 RTL_SIZEOF_THROUGH_FIELD(SM_SYSTEM_STORE_TRIM_REQUEST, PartitionHandle) // 24H2
#define SYSTEM_STORE_TRIM_INFORMATION_SIZE SYSTEM_STORE_TRIM_INFORMATION_SIZE_V2

#ifdef _WIN64
_STATIC_ASSERT(SYSTEM_STORE_TRIM_INFORMATION_SIZE_V1 == 16);
_STATIC_ASSERT(SYSTEM_STORE_TRIM_INFORMATION_SIZE_V2 == 24);
#else
_STATIC_ASSERT(SYSTEM_STORE_TRIM_INFORMATION_SIZE_V1 == 8);
_STATIC_ASSERT(SYSTEM_STORE_TRIM_INFORMATION_SIZE_V2 == 12);
#endif

// rev
typedef struct _SM_STORE_COMPRESSION_INFORMATION_REQUEST
{
    ULONG Version : 8; // SYSTEM_STORE_COMPRESSION_INFORMATION_VERSION
    ULONG Spare : 24;
    ULONG CompressionPid;
    ULONG WorkingSetSize;
    SIZE_T TotalDataCompressed;
    SIZE_T TotalCompressedSize;
    SIZE_T TotalUniqueDataCompressed;
    HANDLE PartitionHandle; // since 24H2
} SM_STORE_COMPRESSION_INFORMATION_REQUEST, *PSM_STORE_COMPRESSION_INFORMATION_REQUEST;

// rev
#define SYSTEM_STORE_COMPRESSION_INFORMATION_VERSION_V1 3 // WIN10
#define SYSTEM_STORE_COMPRESSION_INFORMATION_VERSION_V2 4 // 24H2
#define SYSTEM_STORE_COMPRESSION_INFORMATION_VERSION SYSTEM_STORE_COMPRESSION_INFORMATION_VERSION_V2

// rev
#define SYSTEM_STORE_COMPRESSION_INFORMATION_SIZE_V1 RTL_SIZEOF_THROUGH_FIELD(SM_STORE_COMPRESSION_INFORMATION_REQUEST, TotalUniqueDataCompressed) // WIN10
#define SYSTEM_STORE_COMPRESSION_INFORMATION_SIZE_V2 RTL_SIZEOF_THROUGH_FIELD(SM_STORE_COMPRESSION_INFORMATION_REQUEST, PartitionHandle) // 24H2
#define SYSTEM_STORE_COMPRESSION_INFORMATION_SIZE SYSTEM_STORE_COMPRESSION_INFORMATION_SIZE_V2

#ifdef _WIN64
_STATIC_ASSERT(SYSTEM_STORE_COMPRESSION_INFORMATION_SIZE_V1 == 40);
_STATIC_ASSERT(SYSTEM_STORE_COMPRESSION_INFORMATION_SIZE_V2 == 48);
#else
_STATIC_ASSERT(SYSTEM_STORE_COMPRESSION_INFORMATION_SIZE_V1 == 24);
_STATIC_ASSERT(SYSTEM_STORE_COMPRESSION_INFORMATION_SIZE_V2 == 28);
#endif

// private
typedef struct _SYSTEM_REGISTRY_APPEND_STRING_PARAMETERS
{
    HANDLE KeyHandle;
    PUNICODE_STRING ValueNamePointer;
    PULONG RequiredLengthPointer;
    PUCHAR Buffer;
    ULONG BufferLength;
    ULONG Type;
    PUCHAR AppendBuffer;
    ULONG AppendBufferLength;
    BOOLEAN CreateIfDoesntExist;
    BOOLEAN TruncateExistingValue;
} SYSTEM_REGISTRY_APPEND_STRING_PARAMETERS, *PSYSTEM_REGISTRY_APPEND_STRING_PARAMETERS;

// msdn
typedef struct _SYSTEM_VHD_BOOT_INFORMATION
{
    BOOLEAN OsDiskIsVhd;
    ULONG OsVhdFilePathOffset;
    WCHAR OsVhdParentVolume[1];
} SYSTEM_VHD_BOOT_INFORMATION, *PSYSTEM_VHD_BOOT_INFORMATION;

// private
typedef struct _PS_CPU_QUOTA_QUERY_ENTRY
{
    ULONG SessionId;
    ULONG Weight;
} PS_CPU_QUOTA_QUERY_ENTRY, *PPS_CPU_QUOTA_QUERY_ENTRY;

// private
typedef struct _PS_CPU_QUOTA_QUERY_INFORMATION
{
    ULONG SessionCount;
    PS_CPU_QUOTA_QUERY_ENTRY SessionInformation[1];
} PS_CPU_QUOTA_QUERY_INFORMATION, *PPS_CPU_QUOTA_QUERY_INFORMATION;

// private
typedef struct _SYSTEM_ERROR_PORT_TIMEOUTS
{
    ULONG StartTimeout;
    ULONG CommTimeout;
} SYSTEM_ERROR_PORT_TIMEOUTS, *PSYSTEM_ERROR_PORT_TIMEOUTS;

// private
typedef struct _SYSTEM_LOW_PRIORITY_IO_INFORMATION
{
    ULONG LowPriReadOperations;
    ULONG LowPriWriteOperations;
    ULONG KernelBumpedToNormalOperations;
    ULONG LowPriPagingReadOperations;
    ULONG KernelPagingReadsBumpedToNormal;
    ULONG LowPriPagingWriteOperations;
    ULONG KernelPagingWritesBumpedToNormal;
    ULONG BoostedIrpCount;
    ULONG BoostedPagingIrpCount;
    ULONG BlanketBoostCount;
} SYSTEM_LOW_PRIORITY_IO_INFORMATION, *PSYSTEM_LOW_PRIORITY_IO_INFORMATION;

// symbols
typedef enum _BOOT_ENTROPY_SOURCE_RESULT_CODE
{
    BootEntropySourceStructureUninitialized,
    BootEntropySourceDisabledByPolicy,
    BootEntropySourceNotPresent,
    BootEntropySourceError,
    BootEntropySourceSuccess
} BOOT_ENTROPY_SOURCE_RESULT_CODE;

typedef enum _BOOT_ENTROPY_SOURCE_ID
{
    BootEntropySourceNone = 0,
    BootEntropySourceSeedfile = 1,
    BootEntropySourceExternal = 2,
    BootEntropySourceTpm = 3,
    BootEntropySourceRdrand = 4,
    BootEntropySourceTime = 5,
    BootEntropySourceAcpiOem0 = 6,
    BootEntropySourceUefi = 7,
    BootEntropySourceCng = 8,
    BootEntropySourceTcbTpm = 9,
    BootEntropySourceTcbRdrand = 10,
    BootMaxEntropySources = 10
} BOOT_ENTROPY_SOURCE_ID, *PBOOT_ENTROPY_SOURCE_ID;

// Contents of KeLoaderBlock->Extension->TpmBootEntropyResult (TPM_BOOT_ENTROPY_LDR_RESULT).
// EntropyData is truncated to 40 bytes.

// private
typedef struct _TPM_BOOT_ENTROPY_NT_RESULT
{
    ULONGLONG Policy;
    BOOT_ENTROPY_SOURCE_RESULT_CODE ResultCode;
    NTSTATUS ResultStatus;
    ULONGLONG Time;
    ULONG EntropyLength;
    UCHAR EntropyData[40];
} TPM_BOOT_ENTROPY_NT_RESULT, *PTPM_BOOT_ENTROPY_NT_RESULT;

// private
typedef struct _BOOT_ENTROPY_SOURCE_NT_RESULT
{
    BOOT_ENTROPY_SOURCE_ID SourceId;
    ULONG64 Policy;
    BOOT_ENTROPY_SOURCE_RESULT_CODE ResultCode;
    NTSTATUS ResultStatus;
    ULONGLONG Time;
    ULONG EntropyLength;
    UCHAR EntropyData[64];
} BOOT_ENTROPY_SOURCE_NT_RESULT, *PBOOT_ENTROPY_SOURCE_NT_RESULT;

// private
typedef struct _BOOT_ENTROPY_NT_RESULT
{
    ULONG maxEntropySources;
    BOOT_ENTROPY_SOURCE_NT_RESULT EntropySourceResult[10];
    UCHAR SeedBytesForCng[48];
} BOOT_ENTROPY_NT_RESULT, *PBOOT_ENTROPY_NT_RESULT;

// private
typedef struct _SYSTEM_VERIFIER_COUNTERS_INFORMATION
{
    SYSTEM_VERIFIER_INFORMATION Legacy;
    ULONG RaiseIrqls;
    ULONG AcquireSpinLocks;
    ULONG SynchronizeExecutions;
    ULONG AllocationsWithNoTag;
    ULONG AllocationsFailed;
    ULONG AllocationsFailedDeliberately;
    SIZE_T LockedBytes;
    SIZE_T PeakLockedBytes;
    SIZE_T MappedLockedBytes;
    SIZE_T PeakMappedLockedBytes;
    SIZE_T MappedIoSpaceBytes;
    SIZE_T PeakMappedIoSpaceBytes;
    SIZE_T PagesForMdlBytes;
    SIZE_T PeakPagesForMdlBytes;
    SIZE_T ContiguousMemoryBytes;
    SIZE_T PeakContiguousMemoryBytes;
    ULONG ExecutePoolTypes; // REDSTONE2
    ULONG ExecutePageProtections;
    ULONG ExecutePageMappings;
    ULONG ExecuteWriteSections;
    ULONG SectionAlignmentFailures;
    ULONG UnsupportedRelocs;
    ULONG IATInExecutableSection;
} SYSTEM_VERIFIER_COUNTERS_INFORMATION, *PSYSTEM_VERIFIER_COUNTERS_INFORMATION;

// private
typedef struct _SYSTEM_ACPI_AUDIT_INFORMATION
{
    ULONG RsdpCount;
    ULONG SameRsdt : 1;
    ULONG SlicPresent : 1;
    ULONG SlicDifferent : 1;
} SYSTEM_ACPI_AUDIT_INFORMATION, *PSYSTEM_ACPI_AUDIT_INFORMATION;

// private
typedef struct _SYSTEM_BASIC_PERFORMANCE_INFORMATION
{
    SIZE_T AvailablePages;
    SIZE_T CommittedPages;
    SIZE_T CommitLimit;
    SIZE_T PeakCommitment;
} SYSTEM_BASIC_PERFORMANCE_INFORMATION, *PSYSTEM_BASIC_PERFORMANCE_INFORMATION;

// begin_msdn

typedef struct _QUERY_PERFORMANCE_COUNTER_FLAGS
{
    union
    {
        struct
        {
            ULONG KernelTransition : 1;
            ULONG Reserved : 31;
        };
        ULONG ul;
    };
} QUERY_PERFORMANCE_COUNTER_FLAGS;

typedef struct _SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION
{
    ULONG Version;
    QUERY_PERFORMANCE_COUNTER_FLAGS Flags;
    QUERY_PERFORMANCE_COUNTER_FLAGS ValidFlags;
} SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION, *PSYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION;

// end_msdn

// private
typedef enum _SYSTEM_PIXEL_FORMAT
{
    SystemPixelFormatUnknown,
    SystemPixelFormatR8G8B8,
    SystemPixelFormatR8G8B8X8,
    SystemPixelFormatB8G8R8,
    SystemPixelFormatB8G8R8X8
} SYSTEM_PIXEL_FORMAT;

// private
typedef struct _SYSTEM_BOOT_GRAPHICS_INFORMATION
{
    LARGE_INTEGER FrameBuffer;
    ULONG Width;
    ULONG Height;
    ULONG PixelStride;
    ULONG Flags;
    SYSTEM_PIXEL_FORMAT Format;
    ULONG DisplayRotation;
} SYSTEM_BOOT_GRAPHICS_INFORMATION, *PSYSTEM_BOOT_GRAPHICS_INFORMATION;

// private
typedef struct _MEMORY_SCRUB_INFORMATION
{
    HANDLE Handle;
    SIZE_T PagesScrubbed;
} MEMORY_SCRUB_INFORMATION, *PMEMORY_SCRUB_INFORMATION;

// private
typedef union _SYSTEM_BAD_PAGE_INFORMATION
{
#ifdef _WIN64
    ULONG_PTR PhysicalPageNumber : 52;
#else
    ULONG PhysicalPageNumber : 20;
#endif
    ULONG_PTR Reserved : 10;
    ULONG_PTR Pending : 1;
    ULONG_PTR Poisoned : 1;
} SYSTEM_BAD_PAGE_INFORMATION, *PSYSTEM_BAD_PAGE_INFORMATION;

// private
typedef struct _PEBS_DS_SAVE_AREA32
{
    ULONG BtsBufferBase;
    ULONG BtsIndex;
    ULONG BtsAbsoluteMaximum;
    ULONG BtsInterruptThreshold;
    ULONG PebsBufferBase;
    ULONG PebsIndex;
    ULONG PebsAbsoluteMaximum;
    ULONG PebsInterruptThreshold;
    ULONG PebsGpCounterReset[8];
    ULONG PebsFixedCounterReset[4];
} PEBS_DS_SAVE_AREA32, *PPEBS_DS_SAVE_AREA32;

// private
typedef struct _PEBS_DS_SAVE_AREA64
{
    ULONGLONG BtsBufferBase;
    ULONGLONG BtsIndex;
    ULONGLONG BtsAbsoluteMaximum;
    ULONGLONG BtsInterruptThreshold;
    ULONGLONG PebsBufferBase;
    ULONGLONG PebsIndex;
    ULONGLONG PebsAbsoluteMaximum;
    ULONGLONG PebsInterruptThreshold;
    ULONGLONG PebsGpCounterReset[8];
    ULONGLONG PebsFixedCounterReset[4];
} PEBS_DS_SAVE_AREA64, *PPEBS_DS_SAVE_AREA64;

// private
typedef union _PEBS_DS_SAVE_AREA
{
    PEBS_DS_SAVE_AREA32 As32Bit;
    PEBS_DS_SAVE_AREA64 As64Bit;
} PEBS_DS_SAVE_AREA, *PPEBS_DS_SAVE_AREA;

// private
typedef struct _PROCESSOR_PROFILE_CONTROL_AREA
{
    PEBS_DS_SAVE_AREA PebsDsSaveArea;
} PROCESSOR_PROFILE_CONTROL_AREA, *PPROCESSOR_PROFILE_CONTROL_AREA;

// private
typedef struct _SYSTEM_PROCESSOR_PROFILE_CONTROL_AREA
{
    PROCESSOR_PROFILE_CONTROL_AREA ProcessorProfileControlArea;
    BOOLEAN Allocate;
} SYSTEM_PROCESSOR_PROFILE_CONTROL_AREA, *PSYSTEM_PROCESSOR_PROFILE_CONTROL_AREA;

// private
typedef struct _MEMORY_COMBINE_INFORMATION
{
    HANDLE Handle;
    SIZE_T PagesCombined;
} MEMORY_COMBINE_INFORMATION, *PMEMORY_COMBINE_INFORMATION;

// rev
#define MEMORY_COMBINE_FLAGS_COMMON_PAGES_ONLY 0x4

// private
typedef struct _MEMORY_COMBINE_INFORMATION_EX
{
    HANDLE Handle;
    SIZE_T PagesCombined;
    ULONG Flags;
} MEMORY_COMBINE_INFORMATION_EX, *PMEMORY_COMBINE_INFORMATION_EX;

// private
typedef struct _MEMORY_COMBINE_INFORMATION_EX2
{
    HANDLE Handle;
    SIZE_T PagesCombined;
    ULONG Flags;
    HANDLE ProcessHandle;
} MEMORY_COMBINE_INFORMATION_EX2, *PMEMORY_COMBINE_INFORMATION_EX2;

// private
typedef struct _SYSTEM_ENTROPY_TIMING_INFORMATION
{
    VOID(NTAPI *EntropyRoutine)(PVOID, ULONG);
    VOID(NTAPI *InitializationRoutine)(PVOID, ULONG, PVOID);
    PVOID InitializationContext;
} SYSTEM_ENTROPY_TIMING_INFORMATION, *PSYSTEM_ENTROPY_TIMING_INFORMATION;

// private
typedef struct _SYSTEM_CONSOLE_INFORMATION
{
    ULONG DriverLoaded : 1;
    ULONG Spare : 31;
} SYSTEM_CONSOLE_INFORMATION, *PSYSTEM_CONSOLE_INFORMATION;

// private
typedef struct _SYSTEM_PLATFORM_BINARY_INFORMATION
{
    ULONG64 PhysicalAddress;
    PVOID HandoffBuffer;
    PVOID CommandLineBuffer;
    ULONG HandoffBufferSize;
    ULONG CommandLineBufferSize;
} SYSTEM_PLATFORM_BINARY_INFORMATION, *PSYSTEM_PLATFORM_BINARY_INFORMATION;

// private
typedef struct _SYSTEM_POLICY_INFORMATION
{
    PVOID InputData;
    PVOID OutputData;
    ULONG InputDataSize;
    ULONG OutputDataSize;
    ULONG Version;
} SYSTEM_POLICY_INFORMATION, *PSYSTEM_POLICY_INFORMATION;

// private
typedef struct _SYSTEM_HYPERVISOR_PROCESSOR_COUNT_INFORMATION
{
    ULONG NumberOfLogicalProcessors;
    ULONG NumberOfCores;
} SYSTEM_HYPERVISOR_PROCESSOR_COUNT_INFORMATION, *PSYSTEM_HYPERVISOR_PROCESSOR_COUNT_INFORMATION;

// private
typedef struct _SYSTEM_DEVICE_DATA_INFORMATION
{
    UNICODE_STRING DeviceId;
    UNICODE_STRING DataName;
    ULONG DataType;
    ULONG DataBufferLength;
    PVOID DataBuffer;
} SYSTEM_DEVICE_DATA_INFORMATION, *PSYSTEM_DEVICE_DATA_INFORMATION;

// private
typedef struct _PHYSICAL_CHANNEL_RUN
{
    ULONG NodeNumber;
    ULONG ChannelNumber;
    ULONGLONG BasePage;
    ULONGLONG PageCount;
    ULONG Flags;
} PHYSICAL_CHANNEL_RUN, *PPHYSICAL_CHANNEL_RUN;

// private
typedef struct _SYSTEM_MEMORY_TOPOLOGY_INFORMATION
{
    ULONGLONG NumberOfRuns;
    ULONG NumberOfNodes;
    ULONG NumberOfChannels;
    PHYSICAL_CHANNEL_RUN Run[1];
} SYSTEM_MEMORY_TOPOLOGY_INFORMATION, *PSYSTEM_MEMORY_TOPOLOGY_INFORMATION;

// private
typedef struct _SYSTEM_MEMORY_CHANNEL_INFORMATION
{
    ULONG ChannelNumber;
    ULONG ChannelHeatIndex;
    ULONGLONG TotalPageCount;
    ULONGLONG ZeroPageCount;
    ULONGLONG FreePageCount;
    ULONGLONG StandbyPageCount;
} SYSTEM_MEMORY_CHANNEL_INFORMATION, *PSYSTEM_MEMORY_CHANNEL_INFORMATION;

// private
typedef struct _SYSTEM_BOOT_LOGO_INFORMATION
{
    ULONG Flags;
    ULONG BitmapOffset;
} SYSTEM_BOOT_LOGO_INFORMATION, *PSYSTEM_BOOT_LOGO_INFORMATION;

// private
typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION_EX
{
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER DpcTime;
    LARGE_INTEGER InterruptTime;
    ULONG InterruptCount;
    ULONG Spare0;
    LARGE_INTEGER AvailableTime;
    LARGE_INTEGER Spare1;
    LARGE_INTEGER Spare2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION_EX, *PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION_EX;

// private
typedef struct _CRITICAL_PROCESS_EXCEPTION_DATA
{
    GUID ReportId;
    UNICODE_STRING ModuleName;
    ULONG ModuleTimestamp;
    ULONG ModuleSize;
    ULONG_PTR Offset;
} CRITICAL_PROCESS_EXCEPTION_DATA, *PCRITICAL_PROCESS_EXCEPTION_DATA;

// private
typedef struct _SYSTEM_SECUREBOOT_POLICY_INFORMATION
{
    GUID PolicyPublisher;
    ULONG PolicyVersion;
    ULONG PolicyOptions;
} SYSTEM_SECUREBOOT_POLICY_INFORMATION, *PSYSTEM_SECUREBOOT_POLICY_INFORMATION;

// private
typedef struct _SYSTEM_PAGEFILE_INFORMATION_EX
{
    union // HACK union declaration for convenience (dmex)
    {
        SYSTEM_PAGEFILE_INFORMATION Info;
        struct
        {
            ULONG NextEntryOffset;
            ULONG TotalSize;
            ULONG TotalInUse;
            ULONG PeakUsage;
            UNICODE_STRING PageFileName;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;

    ULONG MinimumSize;
    ULONG MaximumSize;
} SYSTEM_PAGEFILE_INFORMATION_EX, *PSYSTEM_PAGEFILE_INFORMATION_EX;

// private
typedef struct _SYSTEM_SECUREBOOT_INFORMATION
{
    BOOLEAN SecureBootEnabled;
    BOOLEAN SecureBootCapable;
} SYSTEM_SECUREBOOT_INFORMATION, *PSYSTEM_SECUREBOOT_INFORMATION;

// private
typedef struct _PROCESS_DISK_COUNTERS
{
    ULONGLONG BytesRead;
    ULONGLONG BytesWritten;
    ULONGLONG ReadOperationCount;
    ULONGLONG WriteOperationCount;
    ULONGLONG FlushOperationCount;
} PROCESS_DISK_COUNTERS, *PPROCESS_DISK_COUNTERS;

// private
typedef union _ENERGY_STATE_DURATION
{
    ULONGLONG Value;
    struct
    {
        ULONG LastChangeTime;
        ULONG Duration : 31;
        ULONG IsInState : 1;
    } DUMMYSTRUCTNAME;
} ENERGY_STATE_DURATION, *PENERGY_STATE_DURATION;

typedef struct _PROCESS_ENERGY_VALUES
{
    ULONGLONG Cycles[4][2];
    ULONGLONG DiskEnergy;
    ULONGLONG NetworkTailEnergy;
    ULONGLONG MBBTailEnergy;
    ULONGLONG NetworkTxRxBytes;
    ULONGLONG MBBTxRxBytes;
    union
    {
        ENERGY_STATE_DURATION Durations[3];
        struct
        {
            ENERGY_STATE_DURATION ForegroundDuration;
            ENERGY_STATE_DURATION DesktopVisibleDuration;
            ENERGY_STATE_DURATION PSMForegroundDuration;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    ULONG CompositionRendered;
    ULONG CompositionDirtyGenerated;
    ULONG CompositionDirtyPropagated;
    ULONG Reserved1;
    ULONGLONG AttributedCycles[4][2];
    ULONGLONG WorkOnBehalfCycles[4][2];
} PROCESS_ENERGY_VALUES, *PPROCESS_ENERGY_VALUES;

typedef union _TIMELINE_BITMAP
{
    ULONGLONG Value;
    struct
    {
        ULONG EndTime;
        ULONG Bitmap;
    };
} TIMELINE_BITMAP, *PTIMELINE_BITMAP;

typedef struct _PROCESS_ENERGY_VALUES_EXTENSION
{
    union
    {
        TIMELINE_BITMAP Timelines[14]; // 9 for REDSTONE2, 14 for REDSTONE3/4/5
        struct
        {
            TIMELINE_BITMAP CpuTimeline;
            TIMELINE_BITMAP DiskTimeline;
            TIMELINE_BITMAP NetworkTimeline;
            TIMELINE_BITMAP MBBTimeline;
            TIMELINE_BITMAP ForegroundTimeline;
            TIMELINE_BITMAP DesktopVisibleTimeline;
            TIMELINE_BITMAP CompositionRenderedTimeline;
            TIMELINE_BITMAP CompositionDirtyGeneratedTimeline;
            TIMELINE_BITMAP CompositionDirtyPropagatedTimeline;
            TIMELINE_BITMAP InputTimeline; // REDSTONE3
            TIMELINE_BITMAP AudioInTimeline;
            TIMELINE_BITMAP AudioOutTimeline;
            TIMELINE_BITMAP DisplayRequiredTimeline;
            TIMELINE_BITMAP KeyboardInputTimeline;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;

    union // REDSTONE3
    {
        ENERGY_STATE_DURATION Durations[5];
        struct
        {
            ENERGY_STATE_DURATION InputDuration;
            ENERGY_STATE_DURATION AudioInDuration;
            ENERGY_STATE_DURATION AudioOutDuration;
            ENERGY_STATE_DURATION DisplayRequiredDuration;
            ENERGY_STATE_DURATION PSMBackgroundDuration;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;

    ULONG KeyboardInput;
    ULONG MouseInput;
} PROCESS_ENERGY_VALUES_EXTENSION, *PPROCESS_ENERGY_VALUES_EXTENSION;

typedef struct _PROCESS_EXTENDED_ENERGY_VALUES
{
    PROCESS_ENERGY_VALUES Base;
    PROCESS_ENERGY_VALUES_EXTENSION Extension;
} PROCESS_EXTENDED_ENERGY_VALUES, *PPROCESS_EXTENDED_ENERGY_VALUES;

typedef struct _PROCESS_EXTENDED_ENERGY_VALUES_V1
{
    PROCESS_ENERGY_VALUES Base;
    PROCESS_ENERGY_VALUES_EXTENSION Extension;
    ULONG64 NpuWorkUnits;
} PROCESS_EXTENDED_ENERGY_VALUES_V1, *PPROCESS_EXTENDED_ENERGY_VALUES_V1;

// private
typedef enum _SYSTEM_PROCESS_CLASSIFICATION
{
    SystemProcessClassificationNormal,
    SystemProcessClassificationSystem,
    SystemProcessClassificationSecureSystem,
    SystemProcessClassificationMemCompression,
    SystemProcessClassificationRegistry, // REDSTONE4
    SystemProcessClassificationMaximum
} SYSTEM_PROCESS_CLASSIFICATION;

// private
typedef struct _SYSTEM_PROCESS_INFORMATION_EXTENSION
{
    PROCESS_DISK_COUNTERS DiskCounters;
    ULONGLONG ContextSwitches;
    union
    {
        ULONG Flags;
        struct
        {
            ULONG HasStrongId : 1;
            ULONG Classification : 4; // SYSTEM_PROCESS_CLASSIFICATION
            ULONG BackgroundActivityModerated : 1;
            ULONG Spare : 26;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    ULONG UserSidOffset;
    ULONG PackageFullNameOffset; // since THRESHOLD
    PROCESS_ENERGY_VALUES EnergyValues; // since THRESHOLD
    ULONG AppIdOffset; // since THRESHOLD
    SIZE_T SharedCommitCharge; // since THRESHOLD2
    ULONG JobObjectId; // since REDSTONE
    ULONG SpareUlong; // since REDSTONE
    ULONGLONG ProcessSequenceNumber;
} SYSTEM_PROCESS_INFORMATION_EXTENSION, *PSYSTEM_PROCESS_INFORMATION_EXTENSION;

// private
typedef struct _SYSTEM_PORTABLE_WORKSPACE_EFI_LAUNCHER_INFORMATION
{
    BOOLEAN EfiLauncherEnabled;
} SYSTEM_PORTABLE_WORKSPACE_EFI_LAUNCHER_INFORMATION, *PSYSTEM_PORTABLE_WORKSPACE_EFI_LAUNCHER_INFORMATION;

// private
typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX
{
    BOOLEAN DebuggerAllowed;
    BOOLEAN DebuggerEnabled;
    BOOLEAN DebuggerPresent;
} SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX, *PSYSTEM_KERNEL_DEBUGGER_INFORMATION_EX;

// private
typedef struct _SYSTEM_ELAM_CERTIFICATE_INFORMATION
{
    HANDLE ElamDriverFile;
} SYSTEM_ELAM_CERTIFICATE_INFORMATION, *PSYSTEM_ELAM_CERTIFICATE_INFORMATION;

// private
typedef struct _OFFLINE_CRASHDUMP_CONFIGURATION_TABLE_V2
{
    ULONG Version;
    ULONG AbnormalResetOccurred;
    ULONG OfflineMemoryDumpCapable;
    PVOID ResetDataAddress;
    ULONG ResetDataSize;
} OFFLINE_CRASHDUMP_CONFIGURATION_TABLE_V2, *POFFLINE_CRASHDUMP_CONFIGURATION_TABLE_V2;

// private
typedef struct _OFFLINE_CRASHDUMP_CONFIGURATION_TABLE_V1
{
    ULONG Version;
    ULONG AbnormalResetOccurred;
    ULONG OfflineMemoryDumpCapable;
} OFFLINE_CRASHDUMP_CONFIGURATION_TABLE_V1, *POFFLINE_CRASHDUMP_CONFIGURATION_TABLE_V1;

// SYSTEM_PROCESSOR_FEATURES_INFORMATION // ProcessorFeatureBits
#define KF_BRANCH 0x0000000000020000
#define KF_XSTATE 0x0000000000800000
#define KF_RDTSCP 0x0000000400000000
#define KF_CET_SS 0x0000400000000000
#define KF_XFD 0x0080000000000000

// private
typedef struct _SYSTEM_PROCESSOR_FEATURES_INFORMATION
{
    ULONGLONG ProcessorFeatureBits;
    ULONGLONG Reserved[3];
} SYSTEM_PROCESSOR_FEATURES_INFORMATION, *PSYSTEM_PROCESSOR_FEATURES_INFORMATION;

// EDID v1.4 standard data format
typedef struct _SYSTEM_EDID_INFORMATION
{
    UCHAR Edid[128];
} SYSTEM_EDID_INFORMATION, *PSYSTEM_EDID_INFORMATION;

// private
typedef struct _SYSTEM_MANUFACTURING_INFORMATION
{
    ULONG Options;
    UNICODE_STRING ProfileName;
} SYSTEM_MANUFACTURING_INFORMATION, *PSYSTEM_MANUFACTURING_INFORMATION;

// private
typedef struct _SYSTEM_ENERGY_ESTIMATION_CONFIG_INFORMATION
{
    BOOLEAN Enabled;
} SYSTEM_ENERGY_ESTIMATION_CONFIG_INFORMATION, *PSYSTEM_ENERGY_ESTIMATION_CONFIG_INFORMATION;

// private
typedef struct _HV_DETAILS
{
    ULONG Data[4];
} HV_DETAILS, *PHV_DETAILS;

// private
typedef struct _SYSTEM_HYPERVISOR_DETAIL_INFORMATION
{
    HV_DETAILS HvVendorAndMaxFunction;
    HV_DETAILS HypervisorInterface;
    HV_DETAILS HypervisorVersion;
    HV_DETAILS HvFeatures;
    HV_DETAILS HwFeatures;
    HV_DETAILS EnlightenmentInfo;
    HV_DETAILS ImplementationLimits;
} SYSTEM_HYPERVISOR_DETAIL_INFORMATION, *PSYSTEM_HYPERVISOR_DETAIL_INFORMATION;

// private
typedef struct _SYSTEM_PROCESSOR_CYCLE_STATS_INFORMATION
{
    //
    // First index is bucket (see: PoGetFrequencyBucket) selected based on latest frequency percent
    // using _KPRCB.PowerState.FrequencyBucketThresholds.
    //
    // Second index is _KPRCB.PowerState.ArchitecturalEfficiencyClass, accounting for architecture
    // dependent KeHeteroSystem and using _KPRCB.PowerState.EarlyBootArchitecturalEfficiencyClass
    // instead, when appropriate.
    //
    ULONGLONG Cycles[4][2];
} SYSTEM_PROCESSOR_CYCLE_STATS_INFORMATION, *PSYSTEM_PROCESSOR_CYCLE_STATS_INFORMATION;

// private
typedef struct _SYSTEM_TPM_INFORMATION
{
    ULONG Flags;
} SYSTEM_TPM_INFORMATION, *PSYSTEM_TPM_INFORMATION;

// private
typedef struct _SYSTEM_VSM_PROTECTION_INFORMATION
{
    BOOLEAN DmaProtectionsAvailable;
    BOOLEAN DmaProtectionsInUse;
    BOOLEAN HardwareMbecAvailable; // REDSTONE4 (CVE-2018-3639)
    BOOLEAN ApicVirtualizationAvailable; // 20H1
} SYSTEM_VSM_PROTECTION_INFORMATION, *PSYSTEM_VSM_PROTECTION_INFORMATION;

// private
typedef struct _SYSTEM_KERNEL_DEBUGGER_FLAGS
{
    BOOLEAN KernelDebuggerIgnoreUmExceptions;
} SYSTEM_KERNEL_DEBUGGER_FLAGS, *PSYSTEM_KERNEL_DEBUGGER_FLAGS;

// SYSTEM_CODEINTEGRITYPOLICY_INFORMATION Options
#define CODEINTEGRITYPOLICY_OPTION_ENABLED 0x01
#define CODEINTEGRITYPOLICY_OPTION_AUDIT 0x02
#define CODEINTEGRITYPOLICY_OPTION_REQUIRE_WHQL 0x04
#define CODEINTEGRITYPOLICY_OPTION_DISABLED_FLIGHTSIGNING 0x08
#define CODEINTEGRITYPOLICY_OPTION_ENABLED_UMCI 0x10
#define CODEINTEGRITYPOLICY_OPTION_ENABLED_UPDATE_POLICY_NOREBOOT 0x20
#define CODEINTEGRITYPOLICY_OPTION_ENABLED_SECURE_SETTING_POLICY 0x40
#define CODEINTEGRITYPOLICY_OPTION_ENABLED_UNSIGNED_SYSTEMINTEGRITY_POLICY 0x80
#define CODEINTEGRITYPOLICY_OPTION_DYNAMIC_CODE_POLICY_ENABLED 0x100
#define CODEINTEGRITYPOLICY_OPTION_RELOAD_POLICY_NO_REBOOT 0x10000000 // NtSetSystemInformation reloads SiPolicy.p7b
#define CODEINTEGRITYPOLICY_OPTION_CONDITIONAL_LOCKDOWN 0x20000000
#define CODEINTEGRITYPOLICY_OPTION_NOLOCKDOWN 0x40000000
#define CODEINTEGRITYPOLICY_OPTION_LOCKDOWN 0x80000000

// SYSTEM_CODEINTEGRITYPOLICY_INFORMATION HVCIOptions
#define CODEINTEGRITYPOLICY_HVCIOPTION_ENABLED 0x01
#define CODEINTEGRITYPOLICY_HVCIOPTION_STRICT 0x02
#define CODEINTEGRITYPOLICY_HVCIOPTION_DEBUG 0x04

// private
typedef struct _SYSTEM_CODEINTEGRITYPOLICY_INFORMATION
{
    union
    {
        ULONG Options;
        struct
        {
            ULONG Enabled : 1;
            ULONG Audit : 1;
            ULONG RequireWHQL : 1;
            ULONG DisabledFlightSigning : 1;
            ULONG EnabledUMCI : 1;
            ULONG EnabledUpdatePolicyNoReboot : 1;
            ULONG EnabledSecureSettingPolicy : 1;
            ULONG EnabledUnsignedSystemIntegrityPolicy : 1;
            ULONG DynamicCodePolicyEnabled : 1;
            ULONG Spare : 19;
            ULONG ReloadPolicyNoReboot : 1;
            ULONG ConditionalLockdown : 1;
            ULONG NoLockdown : 1;
            ULONG Lockdown : 1;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    union
    {
        ULONG HVCIOptions;
        struct
        {
            ULONG HVCIEnabled : 1;
            ULONG HVCIStrict : 1;
            ULONG HVCIDebug : 1;
            ULONG HVCISpare : 29;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    ULONGLONG Version;
    GUID PolicyGuid;
} SYSTEM_CODEINTEGRITYPOLICY_INFORMATION, *PSYSTEM_CODEINTEGRITYPOLICY_INFORMATION;

// private
typedef struct _SYSTEM_ISOLATED_USER_MODE_INFORMATION
{
    BOOLEAN SecureKernelRunning : 1;
    BOOLEAN HvciEnabled : 1;
    BOOLEAN HvciStrictMode : 1;
    BOOLEAN DebugEnabled : 1;
    BOOLEAN FirmwarePageProtection : 1;
    BOOLEAN EncryptionKeyAvailable : 1;
    BOOLEAN SpareFlags : 2;
    BOOLEAN TrustletRunning : 1;
    BOOLEAN HvciDisableAllowed : 1;
    BOOLEAN HardwareEnforcedVbs : 1;
    BOOLEAN NoSecrets : 1;
    BOOLEAN EncryptionKeyPersistent : 1;
    BOOLEAN HardwareEnforcedHvpt : 1;
    BOOLEAN HardwareHvptAvailable : 1;
    BOOLEAN SpareFlags2 : 1;
    BOOLEAN Spare0[6];
    ULONGLONG Spare1;
} SYSTEM_ISOLATED_USER_MODE_INFORMATION, *PSYSTEM_ISOLATED_USER_MODE_INFORMATION;

// private
typedef struct _SYSTEM_SINGLE_MODULE_INFORMATION
{
    PVOID TargetModuleAddress;
    RTL_PROCESS_MODULE_INFORMATION_EX ExInfo;
} SYSTEM_SINGLE_MODULE_INFORMATION, *PSYSTEM_SINGLE_MODULE_INFORMATION;

// private
typedef struct _SYSTEM_INTERRUPT_CPU_SET_INFORMATION
{
    ULONG Gsiv;
    USHORT Group;
    ULONGLONG CpuSets;
} SYSTEM_INTERRUPT_CPU_SET_INFORMATION, *PSYSTEM_INTERRUPT_CPU_SET_INFORMATION;

// private
typedef struct _SYSTEM_SECUREBOOT_POLICY_FULL_INFORMATION
{
    SYSTEM_SECUREBOOT_POLICY_INFORMATION PolicyInformation;
    ULONG PolicySize;
    UCHAR Policy[1];
} SYSTEM_SECUREBOOT_POLICY_FULL_INFORMATION, *PSYSTEM_SECUREBOOT_POLICY_FULL_INFORMATION;

// private
typedef struct _KAFFINITY_EX
{
    USHORT Count;
    USHORT Size;
    ULONG Reserved;
    union
    {
        ULONG_PTR Bitmap[1];
        ULONG_PTR StaticBitmap[32];
    } DUMMYUNIONNAME;
} KAFFINITY_EX, *PKAFFINITY_EX;

// private
typedef struct _SYSTEM_ROOT_SILO_INFORMATION
{
    ULONG NumberOfSilos;
    ULONG SiloIdList[1];
} SYSTEM_ROOT_SILO_INFORMATION, *PSYSTEM_ROOT_SILO_INFORMATION;

// private
typedef struct _SYSTEM_CPU_SET_TAG_INFORMATION
{
    ULONGLONG Tag;
    ULONGLONG CpuSets[1];
} SYSTEM_CPU_SET_TAG_INFORMATION, *PSYSTEM_CPU_SET_TAG_INFORMATION;

// private
typedef struct _SYSTEM_SECURE_KERNEL_HYPERGUARD_PROFILE_INFORMATION
{
    ULONG ExtentCount;
    ULONG ValidStructureSize;
    ULONG NextExtentIndex;
    ULONG ExtentRestart;
    ULONG CycleCount;
    ULONG TimeoutCount;
    ULONGLONG CycleTime;
    ULONGLONG CycleTimeMax;
    ULONGLONG ExtentTime;
    ULONG ExtentTimeIndex;
    ULONG ExtentTimeMaxIndex;
    ULONGLONG ExtentTimeMax;
    ULONGLONG HyperFlushTimeMax;
    ULONGLONG TranslateVaTimeMax;
    ULONGLONG DebugExemptionCount;
    ULONGLONG TbHitCount;
    ULONGLONG TbMissCount;
    ULONGLONG VinaPendingYield;
    ULONGLONG HashCycles;
    ULONG HistogramOffset;
    ULONG HistogramBuckets;
    ULONG HistogramShift;
    ULONG Reserved1;
    ULONGLONG PageNotPresentCount;
} SYSTEM_SECURE_KERNEL_HYPERGUARD_PROFILE_INFORMATION, *PSYSTEM_SECURE_KERNEL_HYPERGUARD_PROFILE_INFORMATION;

// private
typedef struct _SYSTEM_SECUREBOOT_PLATFORM_MANIFEST_INFORMATION
{
    ULONG PlatformManifestSize;
    UCHAR PlatformManifest[1];
} SYSTEM_SECUREBOOT_PLATFORM_MANIFEST_INFORMATION, *PSYSTEM_SECUREBOOT_PLATFORM_MANIFEST_INFORMATION;

// private
typedef struct _SYSTEM_INTERRUPT_STEERING_INFORMATION_INPUT
{
    ULONG Gsiv;
    UCHAR ControllerInterrupt;
    UCHAR EdgeInterrupt;
    UCHAR IsPrimaryInterrupt;
    GROUP_AFFINITY TargetAffinity;
} SYSTEM_INTERRUPT_STEERING_INFORMATION_INPUT, *PSYSTEM_INTERRUPT_STEERING_INFORMATION_INPUT;

// private
typedef union _SYSTEM_INTERRUPT_STEERING_INFORMATION_OUTPUT
{
    ULONG AsULONG;
    struct
    {
        ULONG Enabled : 1;
        ULONG Reserved : 31;
    };
} SYSTEM_INTERRUPT_STEERING_INFORMATION_OUTPUT, *PSYSTEM_INTERRUPT_STEERING_INFORMATION_OUTPUT;

#if !defined(NTDDI_WIN10_FE) || (NTDDI_VERSION < NTDDI_WIN10_FE)
// private
typedef struct _SYSTEM_SUPPORTED_PROCESSOR_ARCHITECTURES_INFORMATION
{
    ULONG Machine : 16;
    ULONG KernelMode : 1;
    ULONG UserMode : 1;
    ULONG Native : 1;
    ULONG Process : 1;
    ULONG WoW64Container : 1;
    ULONG ReservedZero0 : 11;
} SYSTEM_SUPPORTED_PROCESSOR_ARCHITECTURES_INFORMATION, *PSYSTEM_SUPPORTED_PROCESSOR_ARCHITECTURES_INFORMATION;
#endif

// private
/**
 * The SYSTEM_MEMORY_USAGE_INFORMATION structure contains information about the memory usage of the system.
 */
typedef struct _SYSTEM_MEMORY_USAGE_INFORMATION
{
    ULONGLONG TotalPhysicalBytes;
    ULONGLONG AvailableBytes;
    LONGLONG ResidentAvailableBytes;
    ULONGLONG CommittedBytes;
    ULONGLONG SharedCommittedBytes;
    ULONGLONG CommitLimitBytes;
    ULONGLONG PeakCommitmentBytes;
} SYSTEM_MEMORY_USAGE_INFORMATION, *PSYSTEM_MEMORY_USAGE_INFORMATION;

// rev
typedef enum _SYSTEM_CODEINTEGRITY_IMAGE_TYPE
{
    SystemCodeIntegrityImageTypeUser,
    SystemCodeIntegrityImageTypeKernel,
    SystemCodeIntegrityImageTypeBoot
} SYSTEM_CODEINTEGRITY_IMAGE_TYPE;

/**
 * @def SYSTEM_CODEINTEGRITY_IMAGE_TYPE_USER
 * @brief Flag for validating user-mode images (EXE/DLL).
 *
 * Validation includes:
 * - Digital signature
 * - User-mode certificate chain validity.
 * - Compliance policies for user-mode binaries.
 */
#define SYSTEM_CODEINTEGRITY_IMAGE_TYPE_USER     0

/**
 * @def SYSTEM_CODEINTEGRITY_IMAGE_TYPE_KERNEL
 * @brief Flag for validating kernel-mode images (SYS/Native).
 *
 * Validation includes:
 * - Signed by a trusted certificate authority (or cross-signed).
 * - Compliance policies for kernel-mode binaries.
 */
#define SYSTEM_CODEINTEGRITY_IMAGE_TYPE_KERNEL   1

/**
 * @def SYSTEM_CODEINTEGRITY_IMAGE_TYPE_BOOT
 * @brief Flag for validating boot-critical images (SYS/Native).
 *
 * Validation includes:
 * - Signed only by Microsoft.
 * - Compliance policies for boot-critical binaries (Strict WHQL, Secure Boot requirements).
 */
#define SYSTEM_CODEINTEGRITY_IMAGE_TYPE_BOOT

/**
 * The SYSTEM_CODEINTEGRITY_CERTIFICATE_INFORMATION structure contains information to validate the integrity of an image.
 *
 * \note The return status of NtQuerySystemInformation indicates the result of the code integrity validation as determined by the type specified.
 */
typedef struct _SYSTEM_CODEINTEGRITY_CERTIFICATE_INFORMATION
{
    HANDLE ImageFile;   // in: Handle to a file or image to validate.
    ULONG Type;         // in: The type of code integrity policy. // REDSTONE4
} SYSTEM_CODEINTEGRITY_CERTIFICATE_INFORMATION, *PSYSTEM_CODEINTEGRITY_CERTIFICATE_INFORMATION;

/**
 * The SYSTEM_PHYSICAL_MEMORY_INFORMATION structure retrieves the physical memory layout of the system.
 * \remarks The addresses are physical, not virtual.
 */
typedef struct _SYSTEM_PHYSICAL_MEMORY_INFORMATION
{
    ULONGLONG TotalPhysicalBytes;           // Total amount of physical RAM present, in bytes.
    ULONGLONG LowestPhysicalAddress;        // Lowest accessible physical address (byte address).
    ULONGLONG HighestPhysicalAddress;       // Highest accessible physical address (byte address, inclusive).
} SYSTEM_PHYSICAL_MEMORY_INFORMATION, *PSYSTEM_PHYSICAL_MEMORY_INFORMATION;

// private
typedef enum _SYSTEM_ACTIVITY_MODERATION_STATE
{
    SystemActivityModerationStateSystemManaged,
    SystemActivityModerationStateUserManagedAllowThrottling,
    SystemActivityModerationStateUserManagedDisableThrottling,
    MaxSystemActivityModerationState
} SYSTEM_ACTIVITY_MODERATION_STATE;

// private - REDSTONE2
typedef struct _SYSTEM_ACTIVITY_MODERATION_EXE_STATE // REDSTONE3: Renamed SYSTEM_ACTIVITY_MODERATION_INFO
{
    UNICODE_STRING ExePathNt;
    SYSTEM_ACTIVITY_MODERATION_STATE ModerationState;
} SYSTEM_ACTIVITY_MODERATION_EXE_STATE, *PSYSTEM_ACTIVITY_MODERATION_EXE_STATE;

typedef enum _SYSTEM_ACTIVITY_MODERATION_APP_TYPE
{
    SystemActivityModerationAppTypeClassic,
    SystemActivityModerationAppTypePackaged,
    MaxSystemActivityModerationAppType
} SYSTEM_ACTIVITY_MODERATION_APP_TYPE;

// private - REDSTONE3
typedef struct _SYSTEM_ACTIVITY_MODERATION_INFO
{
    UNICODE_STRING Identifier;
    SYSTEM_ACTIVITY_MODERATION_STATE ModerationState;
    SYSTEM_ACTIVITY_MODERATION_APP_TYPE AppType;
} SYSTEM_ACTIVITY_MODERATION_INFO, *PSYSTEM_ACTIVITY_MODERATION_INFO;

// rev
#include <pshpack1.h>
typedef struct _SYSTEM_ACTIVITY_MODERATION_APP_SETTINGS
{
    LARGE_INTEGER LastUpdatedTime; // QuerySystemTime
    SYSTEM_ACTIVITY_MODERATION_STATE ModerationState;
    UCHAR Reserved[4];
    SYSTEM_ACTIVITY_MODERATION_APP_TYPE AppType;
    UCHAR Flags[4];
} SYSTEM_ACTIVITY_MODERATION_APP_SETTINGS, *PSYSTEM_ACTIVITY_MODERATION_APP_SETTINGS;
#include <poppack.h>

// private
typedef struct _SYSTEM_ACTIVITY_MODERATION_USER_SETTINGS
{
    HANDLE UserKeyHandle;
} SYSTEM_ACTIVITY_MODERATION_USER_SETTINGS, *PSYSTEM_ACTIVITY_MODERATION_USER_SETTINGS;

// private
typedef struct _SYSTEM_CODEINTEGRITY_UNLOCK_INFORMATION
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG Locked : 1;
            ULONG UnlockApplied : 1; // Unlockable field removed 19H1
            ULONG UnlockIdValid : 1;
            ULONG Reserved : 29;
        };
    };
    UCHAR UnlockId[32]; // REDSTONE4
} SYSTEM_CODEINTEGRITY_UNLOCK_INFORMATION, *PSYSTEM_CODEINTEGRITY_UNLOCK_INFORMATION;

// private
typedef struct _SYSTEM_FLUSH_INFORMATION
{
    ULONG SupportedFlushMethods;
    ULONG ProcessorCacheFlushSize;
    ULONGLONG SystemFlushCapabilities;
    ULONGLONG Reserved[2];
} SYSTEM_FLUSH_INFORMATION, *PSYSTEM_FLUSH_INFORMATION;

// private
typedef struct _SYSTEM_WRITE_CONSTRAINT_INFORMATION
{
    ULONG WriteConstraintPolicy;
    ULONG Reserved;
} SYSTEM_WRITE_CONSTRAINT_INFORMATION, *PSYSTEM_WRITE_CONSTRAINT_INFORMATION;

// private
typedef struct _SYSTEM_KERNEL_VA_SHADOW_INFORMATION
{
    union
    {
        ULONG KvaShadowFlags;
        struct
        {
            ULONG KvaShadowEnabled : 1;
            ULONG KvaShadowUserGlobal : 1;
            ULONG KvaShadowPcid : 1;
            ULONG KvaShadowInvpcid : 1;
            ULONG KvaShadowRequired : 1; // REDSTONE4
            ULONG KvaShadowRequiredAvailable : 1;
            ULONG InvalidPteBit : 6;
            ULONG L1DataCacheFlushSupported : 1;
            ULONG L1TerminalFaultMitigationPresent : 1;
            ULONG Reserved : 18;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} SYSTEM_KERNEL_VA_SHADOW_INFORMATION, *PSYSTEM_KERNEL_VA_SHADOW_INFORMATION;

// private
typedef struct _SYSTEM_CODEINTEGRITYVERIFICATION_INFORMATION
{
    HANDLE FileHandle;
    ULONG ImageSize;
    PVOID Image;
} SYSTEM_CODEINTEGRITYVERIFICATION_INFORMATION, *PSYSTEM_CODEINTEGRITYVERIFICATION_INFORMATION;

// rev
typedef struct _SYSTEM_HYPERVISOR_USER_SHARED_DATA
{
    ULONGLONG TimeUpdateLock; // QpcSystemTimeIncrement?
    volatile ULONGLONG QpcMultiplier;
    volatile ULONGLONG QpcBias; // HvlGetQpcBias
} SYSTEM_HYPERVISOR_USER_SHARED_DATA, *PSYSTEM_HYPERVISOR_USER_SHARED_DATA;

// private
typedef struct _SYSTEM_HYPERVISOR_SHARED_PAGE_INFORMATION
{
    PSYSTEM_HYPERVISOR_USER_SHARED_DATA HypervisorSharedUserVa;
} SYSTEM_HYPERVISOR_SHARED_PAGE_INFORMATION, *PSYSTEM_HYPERVISOR_SHARED_PAGE_INFORMATION;

// private
typedef struct _SYSTEM_FIRMWARE_PARTITION_INFORMATION
{
    UNICODE_STRING FirmwarePartition;
} SYSTEM_FIRMWARE_PARTITION_INFORMATION, *PSYSTEM_FIRMWARE_PARTITION_INFORMATION;

// private
typedef struct _SYSTEM_SPECULATION_CONTROL_INFORMATION
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG BpbEnabled : 1;
            ULONG BpbDisabledSystemPolicy : 1;
            ULONG BpbDisabledNoHardwareSupport : 1;
            ULONG SpecCtrlEnumerated : 1;
            ULONG SpecCmdEnumerated : 1;
            ULONG IbrsPresent : 1;
            ULONG StibpPresent : 1;
            ULONG SmepPresent : 1;
            ULONG SpeculativeStoreBypassDisableAvailable : 1; // REDSTONE4 (CVE-2018-3639)
            ULONG SpeculativeStoreBypassDisableSupported : 1;
            ULONG SpeculativeStoreBypassDisabledSystemWide : 1;
            ULONG SpeculativeStoreBypassDisabledKernel : 1;
            ULONG SpeculativeStoreBypassDisableRequired : 1;
            ULONG BpbDisabledKernelToUser : 1;
            ULONG SpecCtrlRetpolineEnabled : 1;
            ULONG SpecCtrlImportOptimizationEnabled : 1;
            ULONG EnhancedIbrs : 1; // since 19H1
            ULONG HvL1tfStatusAvailable : 1;
            ULONG HvL1tfProcessorNotAffected : 1;
            ULONG HvL1tfMigitationEnabled : 1;
            ULONG HvL1tfMigitationNotEnabled_Hardware : 1;
            ULONG HvL1tfMigitationNotEnabled_LoadOption : 1;
            ULONG HvL1tfMigitationNotEnabled_CoreScheduler : 1;
            ULONG EnhancedIbrsReported : 1;
            ULONG MdsHardwareProtected : 1; // since 19H2
            ULONG MbClearEnabled : 1;
            ULONG MbClearReported : 1;
            ULONG ReservedTaa : 4;
            ULONG Reserved : 1;
        };
    } SpeculationControlFlags;
    union
    {
        ULONG Flags; // since 23H2
        struct
        {
            ULONG SbdrSsdpHardwareProtected : 1;
            ULONG FbsdpHardwareProtected : 1;
            ULONG PsdpHardwareProtected : 1;
            ULONG FbClearEnabled : 1;
            ULONG FbClearReported : 1;
            ULONG BhbEnabled : 1;
            ULONG BhbDisabledSystemPolicy : 1;
            ULONG BhbDisabledNoHardwareSupport : 1;
            ULONG BranchConfusionStatus : 2;
            ULONG BranchConfusionReported : 1;
            ULONG RdclHardwareProtectedReported : 1;
            ULONG RdclHardwareProtected : 1;
            ULONG Reserved3 : 4;
            ULONG Reserved4 : 3;
            ULONG DivideByZeroReported : 1;
            ULONG DivideByZeroStatus : 1;
            ULONG Reserved5 : 3;
            ULONG Reserved : 7;
        };
    } SpeculationControlFlags2;
} SYSTEM_SPECULATION_CONTROL_INFORMATION, *PSYSTEM_SPECULATION_CONTROL_INFORMATION;

// private
typedef struct _SYSTEM_DMA_GUARD_POLICY_INFORMATION
{
    BOOLEAN DmaGuardPolicyEnabled;
} SYSTEM_DMA_GUARD_POLICY_INFORMATION, *PSYSTEM_DMA_GUARD_POLICY_INFORMATION;

// private
typedef struct _SYSTEM_ENCLAVE_LAUNCH_CONTROL_INFORMATION
{
    UCHAR EnclaveLaunchSigner[32];
} SYSTEM_ENCLAVE_LAUNCH_CONTROL_INFORMATION, *PSYSTEM_ENCLAVE_LAUNCH_CONTROL_INFORMATION;

// private
typedef struct _SYSTEM_WORKLOAD_ALLOWED_CPU_SET_INFORMATION
{
    ULONGLONG WorkloadClass;
    ULONGLONG CpuSets[1];
} SYSTEM_WORKLOAD_ALLOWED_CPU_SET_INFORMATION, *PSYSTEM_WORKLOAD_ALLOWED_CPU_SET_INFORMATION;

// private
typedef struct _SYSTEM_SECURITY_MODEL_INFORMATION
{
    union
    {
        ULONG SecurityModelFlags;
        struct
        {
            ULONG ReservedFlag : 1; // SModeAdminlessEnabled
            ULONG AllowDeviceOwnerProtectionDowngrade : 1;
            ULONG Reserved : 30;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} SYSTEM_SECURITY_MODEL_INFORMATION, *PSYSTEM_SECURITY_MODEL_INFORMATION;

// private
typedef union _SECURE_SPECULATION_CONTROL_INFORMATION
{
    ULONG KvaShadowSupported : 1;
    ULONG KvaShadowEnabled : 1;
    ULONG KvaShadowUserGlobal : 1;
    ULONG KvaShadowPcid : 1;
    ULONG MbClearEnabled : 1;
    ULONG L1TFMitigated : 1; // since 20H2
    ULONG BpbEnabled : 1;
    ULONG IbrsPresent : 1;
    ULONG EnhancedIbrs : 1;
    ULONG StibpPresent : 1;
    ULONG SsbdSupported : 1;
    ULONG SsbdRequired : 1;
    ULONG BpbKernelToUser : 1;
    ULONG BpbUserToKernel : 1;
    ULONG ReturnSpeculate : 1;
    ULONG BranchConfusionSafe : 1;
    ULONG SsbsEnabledAlways : 1; // 24H2
    ULONG SsbsEnabledKernel : 1;
    ULONG Reserved : 14;
} SECURE_SPECULATION_CONTROL_INFORMATION, *PSECURE_SPECULATION_CONTROL_INFORMATION;

// private
typedef struct _SYSTEM_FIRMWARE_RAMDISK_INFORMATION
{
    ULONG Version;
    ULONG BlockSize;
    ULONG_PTR BaseAddress;
    SIZE_T Size;
} SYSTEM_FIRMWARE_RAMDISK_INFORMATION, *PSYSTEM_FIRMWARE_RAMDISK_INFORMATION;

// private
typedef struct _SYSTEM_SHADOW_STACK_INFORMATION
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG CetCapable : 1;
            ULONG UserCetAllowed : 1;
            ULONG ReservedForUserCet : 6;
            ULONG KernelCetEnabled : 1;
            ULONG KernelCetAuditModeEnabled : 1;
            ULONG ReservedForKernelCet : 6; // since Windows 10 build 21387
            ULONG Reserved : 16;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} SYSTEM_SHADOW_STACK_INFORMATION, *PSYSTEM_SHADOW_STACK_INFORMATION;

// private
typedef union _SYSTEM_BUILD_VERSION_INFORMATION_FLAGS
{
    ULONG Value32;
    struct
    {
        ULONG IsTopLevel : 1;
        ULONG IsChecked : 1;
    };
} SYSTEM_BUILD_VERSION_INFORMATION_FLAGS, *PSYSTEM_BUILD_VERSION_INFORMATION_FLAGS;

// private
typedef struct _SYSTEM_BUILD_VERSION_INFORMATION
{
    USHORT LayerNumber;
    USHORT LayerCount;
    ULONG OsMajorVersion;
    ULONG OsMinorVersion;
    ULONG NtBuildNumber;
    ULONG NtBuildQfe;
    UCHAR LayerName[128];
    UCHAR NtBuildBranch[128];
    UCHAR NtBuildLab[128];
    UCHAR NtBuildLabEx[128];
    UCHAR NtBuildStamp[26];
    UCHAR NtBuildArch[16];
    SYSTEM_BUILD_VERSION_INFORMATION_FLAGS Flags;
} SYSTEM_BUILD_VERSION_INFORMATION, *PSYSTEM_BUILD_VERSION_INFORMATION;

// private
typedef struct _SYSTEM_POOL_LIMIT_MEM_INFO
{
    ULONGLONG MemoryLimit;
    ULONGLONG NotificationLimit;
} SYSTEM_POOL_LIMIT_MEM_INFO, *PSYSTEM_POOL_LIMIT_MEM_INFO;

// private
typedef struct _SYSTEM_POOL_LIMIT_INFO
{
    ULONG PoolTag;
    SYSTEM_POOL_LIMIT_MEM_INFO MemLimits[2];
    WNF_STATE_NAME NotificationHandle;
} SYSTEM_POOL_LIMIT_INFO, *PSYSTEM_POOL_LIMIT_INFO;

// private
typedef struct _SYSTEM_POOL_LIMIT_INFORMATION
{
    ULONG Version;
    ULONG EntryCount;
    _Field_size_(EntryCount) SYSTEM_POOL_LIMIT_INFO LimitEntries[1];
} SYSTEM_POOL_LIMIT_INFORMATION, *PSYSTEM_POOL_LIMIT_INFORMATION;

// private
//typedef struct _SYSTEM_POOL_ZEROING_INFORMATION
//{
//    BOOLEAN PoolZeroingSupportPresent;
//} SYSTEM_POOL_ZEROING_INFORMATION, *PSYSTEM_POOL_ZEROING_INFORMATION;

// private
typedef struct _HV_MINROOT_NUMA_LPS
{
    ULONG NodeIndex;
    ULONG_PTR Mask[16];
} HV_MINROOT_NUMA_LPS, *PHV_MINROOT_NUMA_LPS;

// private
typedef struct _SYSTEM_XFG_FAILURE_INFORMATION
{
    PVOID ReturnAddress;
    PVOID TargetAddress;
    ULONG DispatchMode;
    ULONGLONG XfgValue;
} SYSTEM_XFG_FAILURE_INFORMATION, *PSYSTEM_XFG_FAILURE_INFORMATION;

// private
typedef enum _SYSTEM_IOMMU_STATE
{
    IommuStateBlock,
    IommuStateUnblock
} SYSTEM_IOMMU_STATE;

// private
typedef struct _SYSTEM_IOMMU_STATE_INFORMATION
{
    SYSTEM_IOMMU_STATE State;
    PVOID Pdo;
} SYSTEM_IOMMU_STATE_INFORMATION, *PSYSTEM_IOMMU_STATE_INFORMATION;

// private
typedef struct _SYSTEM_HYPERVISOR_MINROOT_INFORMATION
{
    ULONG NumProc;
    ULONG RootProc;
    ULONG RootProcNumaNodesSpecified;
    USHORT RootProcNumaNodes[64];
    ULONG RootProcPerCore;
    ULONG RootProcPerNode;
    ULONG RootProcNumaNodesLpsSpecified;
    HV_MINROOT_NUMA_LPS RootProcNumaNodeLps[64];
} SYSTEM_HYPERVISOR_MINROOT_INFORMATION, *PSYSTEM_HYPERVISOR_MINROOT_INFORMATION;

// private
typedef struct _SYSTEM_HYPERVISOR_BOOT_PAGES_INFORMATION
{
    ULONG RangeCount;
    ULONG_PTR RangeArray[1];
} SYSTEM_HYPERVISOR_BOOT_PAGES_INFORMATION, *PSYSTEM_HYPERVISOR_BOOT_PAGES_INFORMATION;

// private
typedef struct _SYSTEM_POINTER_AUTH_INFORMATION
{
    union
    {
        USHORT SupportedFlags;
        struct
        {
            USHORT AddressAuthSupported : 1;
            USHORT AddressAuthQarma : 1;
            USHORT GenericAuthSupported : 1;
            USHORT GenericAuthQarma : 1;
            USHORT AddressAuthFaulting : 1;
            USHORT SupportedReserved : 11;
        };
    };
    union
    {
        USHORT EnabledFlags;
        struct
        {
            USHORT UserPerProcessIpAuthEnabled : 1;
            USHORT UserGlobalIpAuthEnabled : 1;
            USHORT UserEnabledReserved : 6;
            USHORT KernelIpAuthEnabled : 1;
            USHORT KernelEnabledReserved : 7;
        };
    };
} SYSTEM_POINTER_AUTH_INFORMATION, *PSYSTEM_POINTER_AUTH_INFORMATION;

// rev
#define SYSTEM_ORIGINAL_IMAGE_FEATURE_INFORMATION_VERSION 1

// private
typedef struct _SYSTEM_ORIGINAL_IMAGE_FEATURE_INFORMATION_INPUT
{
    ULONG Version;
    PWSTR FeatureName;
    ULONG BornOnVersion;
} SYSTEM_ORIGINAL_IMAGE_FEATURE_INFORMATION_INPUT, *PSYSTEM_ORIGINAL_IMAGE_FEATURE_INFORMATION_INPUT;

// private
typedef struct _SYSTEM_ORIGINAL_IMAGE_FEATURE_INFORMATION_OUTPUT
{
    ULONG Version;
    BOOLEAN FeatureIsEnabled;
} SYSTEM_ORIGINAL_IMAGE_FEATURE_INFORMATION_OUTPUT, *PSYSTEM_ORIGINAL_IMAGE_FEATURE_INFORMATION_OUTPUT;

// private
typedef struct _SYSTEM_MEMORY_NUMA_INFORMATION_INPUT
{
    ULONG Version;
    ULONG TargetNodeNumber;
    ULONG Flags;
} SYSTEM_MEMORY_NUMA_INFORMATION_INPUT, *PSYSTEM_MEMORY_NUMA_INFORMATION_INPUT;

// private
typedef struct _SYSTEM_MEMORY_NUMA_INFORMATION_OUTPUT
{
    ULONG Version;
    ULONG Size;
    ULONG InitiatorNode;
    union
    {
        ULONG Flags;
        struct
        {
            ULONG IsAttached : 1;
            ULONG Reserved : 31;
        };
    };
} SYSTEM_MEMORY_NUMA_INFORMATION_OUTPUT, *PSYSTEM_MEMORY_NUMA_INFORMATION_OUTPUT;

// private
typedef enum _SYSTEM_MEMORY_NUMA_PERFORMANCE_QUERY_DATA_TYPES
{
    SystemMemoryNumaPerformanceQuery_ReadLatency,
    SystemMemoryNumaPerformanceQuery_ReadBandwidth,
    SystemMemoryNumaPerformanceQuery_WriteLatency,
    SystemMemoryNumaPerformanceQuery_WriteBandwidth,
    SystemMemoryNumaPerformanceQuery_Latency,
    SystemMemoryNumaPerformanceQuery_Bandwidth,
    SystemMemoryNumaPerformanceQuery_AllDataTypes,
    SystemMemoryNumaPerformanceQuery_MaxDataType
} SYSTEM_MEMORY_NUMA_PERFORMANCE_QUERY_DATA_TYPES;

// private
typedef struct _SYSTEM_MEMORY_NUMA_PERFORMANCE_INFORMATION_INPUT
{
    ULONG Version;
    ULONG TargetNodeNumber;
    SYSTEM_MEMORY_NUMA_PERFORMANCE_QUERY_DATA_TYPES QueryDataType;
    ULONG Flags;
} SYSTEM_MEMORY_NUMA_PERFORMANCE_INFORMATION_INPUT, *PSYSTEM_MEMORY_NUMA_PERFORMANCE_INFORMATION_INPUT;

// private
typedef struct _SYSTEM_MEMORY_NUMA_PERFORMANCE_ENTRY
{
    ULONG InitiatorNodeNumber;
    ULONG TargetNodeNumber;
    SYSTEM_MEMORY_NUMA_PERFORMANCE_QUERY_DATA_TYPES DataType;
    union
    {
        BOOLEAN Flags;
        struct
        {
            BOOLEAN MinTransferSizeToAchieveValues : 1;
            BOOLEAN NonSequentialTransfers : 1;
            BOOLEAN Reserved : 6;
        };
    };
    SIZE_T MinTransferSizeInBytes;
    ULONG_PTR EntryValue;
} SYSTEM_MEMORY_NUMA_PERFORMANCE_ENTRY, *PSYSTEM_MEMORY_NUMA_PERFORMANCE_ENTRY;

// private
typedef struct _SYSTEM_MEMORY_NUMA_PERFORMANCE_INFORMATION_OUTPUT
{
    ULONG Version;
    ULONG Size;
    ULONG EntryCount;
    SYSTEM_MEMORY_NUMA_PERFORMANCE_ENTRY PerformanceEntries[1];
} SYSTEM_MEMORY_NUMA_PERFORMANCE_INFORMATION_OUTPUT, *PSYSTEM_MEMORY_NUMA_PERFORMANCE_INFORMATION_OUTPUT;

// private
typedef struct _SYSTEM_OSL_RAMDISK_ENTRY
{
    ULONG BlockSize;
    ULONG_PTR BaseAddress;
    SIZE_T Size;
} SYSTEM_OSL_RAMDISK_ENTRY, *PSYSTEM_OSL_RAMDISK_ENTRY;

// private
typedef struct _SYSTEM_TRUSTEDAPPS_RUNTIME_INFORMATION
{
    union
    {
        ULONGLONG Flags;
        struct
        {
            ULONGLONG Supported : 1;
            ULONGLONG Spare : 63;
        };
    };
    PVOID RemoteBreakingRoutine;
} SYSTEM_TRUSTEDAPPS_RUNTIME_INFORMATION, *PSYSTEM_TRUSTEDAPPS_RUNTIME_INFORMATION;

// private
typedef struct _SYSTEM_OSL_RAMDISK_INFORMATION
{
    ULONG Version;
    ULONG Count;
    SYSTEM_OSL_RAMDISK_ENTRY Entries[1];
} SYSTEM_OSL_RAMDISK_INFORMATION, *PSYSTEM_OSL_RAMDISK_INFORMATION;

// private
typedef enum _CI_POLICY_MGMT_OPERATION 
{
    CI_POLICY_MGMT_OPERATION_NONE = 0,
    CI_POLICY_MGMT_OPERATION_OPEN_TX = 1,
    CI_POLICY_MGMT_OPERATION_COMMIT_TX = 2,
    CI_POLICY_MGMT_OPERATION_CLOSE_TX = 3,
    CI_POLICY_MGMT_OPERATION_ADD_POLICY = 4,
    CI_POLICY_MGMT_OPERATION_REMOVE_POLICY = 5,
    CI_POLICY_MGMT_OPERATION_GET_POLICY = 6,
    CI_POLICY_MGMT_OPERATION_GET_POLICY_IDS = 7,
    CI_POLICY_MGMT_OPERATION_MAX = 8
} CI_POLICY_MGMT_OPERATION;

// private
typedef struct _SYSTEM_CODEINTEGRITYPOLICY_MANAGEMENT 
{
    CI_POLICY_MGMT_OPERATION Operation;
    UCHAR UseInProgressState; 
    ULONG Arg1Len;
    PUCHAR Arg1;
    ULONG Arg2Len;
    PUCHAR Arg2;
} SYSTEM_CODEINTEGRITYPOLICY_MANAGEMENT, *PSYSTEM_CODEINTEGRITYPOLICY_MANAGEMENT;

// private
typedef struct _SYSTEM_REF_TRACE_INFORMATION_EX 
{
    ULONG Version;
    ULONGLONG MemoryLimits;
    union 
    {
        ULONG Flags;
        struct 
        {
            ULONG TraceEnable        : 1;
            ULONG TracePermanent     : 1;
            ULONG UseTracePoolTags   : 1;
            ULONG TraceByStacksOnly  : 1;
            ULONG ReservedFlags      : 28;
        };
    };
    UNICODE_STRING TraceProcessName;
    UNICODE_STRING TracePoolTags;
    ULONG MaxObjectRefTraces;
    ULONG TracedObjectLimit;
} SYSTEM_REF_TRACE_INFORMATION_EX, *PSYSTEM_REF_TRACE_INFORMATION_EX;

// private
typedef struct _SYSTEM_BASICPROCESS_INFORMATION
{
    ULONG NextEntryOffset;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG64 SequenceNumber;
    UNICODE_STRING ImageName;
} SYSTEM_BASICPROCESS_INFORMATION, *PSYSTEM_BASICPROCESS_INFORMATION;

// private
typedef struct _SYSTEM_HANDLECOUNT_INFORMATION 
{
    ULONG ProcessCount;
    ULONG ThreadCount;
    ULONG HandleCount;
} SYSTEM_HANDLECOUNT_INFORMATION, *PSYSTEM_HANDLECOUNT_INFORMATION;

#pragma region Prefetch

typedef enum _PF_BOOT_PHASE_ID
{
    PfKernelInitPhase = 0,
    PfBootDriverInitPhase = 90,
    PfSystemDriverInitPhase = 120,
    PfSessionManagerInitPhase = 150,
    PfSMRegistryInitPhase = 180,
    PfVideoInitPhase = 210,
    PfPostVideoInitPhase = 240,
    PfBootAcceptedRegistryInitPhase = 270,
    PfUserShellReadyPhase = 300,
    PfMaxBootPhaseId = 900
} PF_BOOT_PHASE_ID;

typedef enum _PF_ENABLE_STATUS
{
    PfSvNotSpecified,
    PfSvEnabled,
    PfSvDisabled,
    PfSvMaxEnableStatus
} PF_ENABLE_STATUS;

typedef struct _PF_TRACE_LIMITS
{
    ULONG MaxNumPages;
    ULONG MaxNumSections;
    LONGLONG TimerPeriod;
} PF_TRACE_LIMITS, *PPF_TRACE_LIMITS;

typedef struct _PF_SYSTEM_PREFETCH_PARAMETERS
{
    PF_ENABLE_STATUS EnableStatus[2];
    PF_TRACE_LIMITS TraceLimits[2];
    ULONG MaxNumActiveTraces;
    ULONG MaxNumSavedTraces;
    WCHAR RootDirPath[32];
    WCHAR HostingApplicationList[128];
} PF_SYSTEM_PREFETCH_PARAMETERS, *PPF_SYSTEM_PREFETCH_PARAMETERS;

#define PF_BOOT_CONTROL_VERSION 1

typedef struct _PF_BOOT_CONTROL
{
    ULONG Version;
    ULONG DisableBootPrefetching;
} PF_BOOT_CONTROL, *PPF_BOOT_CONTROL;

typedef enum _PREFETCHER_INFORMATION_CLASS
{
    PrefetcherRetrieveTrace = 1, // q: CHAR[]
    PrefetcherSystemParameters, // q: PF_SYSTEM_PREFETCH_PARAMETERS
    PrefetcherBootPhase, // s: PF_BOOT_PHASE_ID
    PrefetcherSpare1, // PrefetcherRetrieveBootLoaderTrace // q: CHAR[]
    PrefetcherBootControl, // s: PF_BOOT_CONTROL
    PrefetcherScenarioPolicyControl,
    PrefetcherSpare2,
    PrefetcherAppLaunchScenarioControl,
    PrefetcherInformationMax
} PREFETCHER_INFORMATION_CLASS;

#define PREFETCHER_INFORMATION_VERSION 23 // rev
#define PREFETCHER_INFORMATION_MAGIC ('kuhC') // rev

typedef struct _PREFETCHER_INFORMATION
{
    _In_ ULONG Version;
    _In_ ULONG Magic;
    _In_ PREFETCHER_INFORMATION_CLASS PrefetcherInformationClass;
    _Inout_ PVOID PrefetcherInformation;
    _Inout_ ULONG PrefetcherInformationLength;
} PREFETCHER_INFORMATION, *PPREFETCHER_INFORMATION;

// rev
typedef struct _PF_SYSTEM_SUPERFETCH_RETRIEVE_TRACE 
{
    union 
    {
        struct
        {
            ULONGLONG RequestType;     // Must be 2 for "get completed trace"
            ULONGLONG Reserved;        // Ignored on input
            HANDLE    PartitionHandle;
        } Input;
        struct
        {
            ULONGLONG TypeFlags;       // 0x0000000000180002 on success
            ULONGLONG Timestamp;       // Scaled TSC value
            HANDLE    PartitionHandle;
        } Output;

        //
        // Raw view of the buffer for opaque access
        //
        UCHAR Buffer[ANYSIZE_ARRAY];
    };
} PF_SYSTEM_SUPERFETCH_RETRIEVE_TRACE, *PPF_SYSTEM_SUPERFETCH_RETRIEVE_TRACE;

// rev
typedef struct _PF_SYSTEM_SUPERFETCH_PARAMETERS
{
    ULONG EnabledComponents;
    ULONG BootID;
    ULONG SavedSectInfoTracesMax;
    ULONG SavedPageAccessTracesMax;
    ULONG ScenarioPrefetchTimeoutStandby;
    ULONG ScenarioPrefetchTimeoutHibernate;
    ULONG ScenarioPrefetchTimeoutHiberBoot;
} PF_SYSTEM_SUPERFETCH_PARAMETERS, *PPF_SYSTEM_SUPERFETCH_PARAMETERS;

// rev
typedef enum _PF_EVENT_TYPE
{
    PfEventTypeImageLoad = 0,
    PfEventTypeAppLaunch = 1,
    PfEventTypeStartTrace = 2,
    PfEventTypeEndTrace = 3,
    PfEventTypeTimestamp = 4,
    PfEventTypeOperation = 5,
    PfEventTypeRepurpose = 6,
    PfEventTypeForegroundProcess = 7,
    PfEventTypeTimeRange = 8,
    PfEventTypeUserInput = 9,
    PfEventTypeFileAccess = 10,
    PfEventTypeUnmap = 11,
    PfEventTypeUtilization = 11,
    PfEventTypeMemInfo = 12,
    PfEventTypeFileDelete = 13,
    PfEventTypeAppExit = 14,
    PfEventTypeSystemTime = 15,
    PfEventTypePower = 16,
    PfEventTypeSessionChange = 17,
    PfEventTypeHardFaultTimeStamp = 18,
    PfEventTypeVirtualFree = 19,
    PfEventTypePerfInfo = 20,
    PfEventTypeProcessSnapshot = 21,
    PfEventTypeUserSnapshot = 22,
    PfEventTypeStreamSequenceNumber = 23,
    PfEventTypeFileTruncate = 24,
    PfEventTypeFileRename = 25,
    PfEventTypeFileCreate = 26,
    PfEventTypeAgCxContext = 27,
    PfEventTypePowerAction = 28,
    PfEventTypeHardFaultTS = 29,
    PfEventTypeRobustInfo = 30,
    PfEventTypeFileDefrag = 31,
    PfEventTypeMax = 32
} PF_EVENT_TYPE;

#define PF_LOG_EVENT_DATA_VERSION 1

typedef struct _PF_LOG_EVENT_DATA 
{
    ULONG Version; // PF_LOG_EVENT_DATA_VERSION
    union 
    {
        ULONG Packed; // [31:7]=DataSize, [6:5]=Flags, [4:0]=EventType (PF_EVENT_TYPE)
        struct 
        {
            ULONG DataSize : 25; // in bytes
            ULONG Flags    : 2;
            ULONG EventType: 5; // 2,3,5,27 accepted by the handler // PF_EVENT_TYPE
        };
    };
    PVOID EventData;
    HANDLE PartitionHandle;
} PF_LOG_EVENT_DATA , *PPF_LOG_EVENT_DATA ;

// PFN operations (classes 0x06/0x07/0x16/0x1D/0x1D/0x29) — header 192 bytes, entries 24B each
typedef struct _PFN_TRIPLET
{
    ULONGLONG MaskOrKey;        // Compared against identity with 0x1FFFFFFFFFFFE00 mask
    ULONGLONG Pfn;              // Page frame number
    ULONGLONG Flags;            // Request/result flags
} PFN_TRIPLET, *PPFN_TRIPLET;

#define PF_PFN_PRIO_REQUEST_VERSION 1
#define PF_PFN_PRIO_REQUEST_QUERY_MEMORY_LIST 0x1
#define PF_PFN_PRIO_REQUEST_VALID_FLAGS 0x1

typedef struct _PF_PFN_PRIO_REQUEST
{
    ULONG Version;
    ULONG RequestFlags;
    SIZE_T PfnCount;
    SYSTEM_MEMORY_LIST_INFORMATION MemInfo;
    union
    {
        // Input: (class 6/16) MmQueryPfnList fills identities here
        MMPFN_IDENTITY PageIdentities[256]; // ANYSIZE_ARRAY
        // Output: (class 7/29/1D) caller supplies PFN_TRIPLETs here
        PFN_TRIPLET Entries[256]; // ANYSIZE_ARRAY
    };
} PF_PFN_PRIO_REQUEST, *PPF_PFN_PRIO_REQUEST;

typedef enum _PFS_PRIVATE_PAGE_SOURCE_TYPE
{
    PfsPrivateSourceKernel,
    PfsPrivateSourceSession,
    PfsPrivateSourceProcess,
    PfsPrivateSourceMax
} PFS_PRIVATE_PAGE_SOURCE_TYPE;

typedef struct _PFS_PRIVATE_PAGE_SOURCE
{
    PFS_PRIVATE_PAGE_SOURCE_TYPE Type;
    union
    {
        ULONG SessionId;
        ULONG ProcessId;
    };
    ULONG ImagePathHash;
    ULONG_PTR UniqueProcessHash;
} PFS_PRIVATE_PAGE_SOURCE, *PPFS_PRIVATE_PAGE_SOURCE;

typedef struct _PF_PRIVSOURCE_INFO
{
    PFS_PRIVATE_PAGE_SOURCE DbInfo;
    PVOID EProcess;
    SIZE_T WsPrivatePages;
    SIZE_T TotalPrivatePages;
    ULONG SessionID;
    CHAR ImageName[16];
    union {
        SIZE_T WsSwapPages;                 // process only PF_PRIVSOURCE_QUERY_WS_SWAP_PAGES.
        SIZE_T SessionPagedPoolPages;       // session only.
        SIZE_T StoreSizePages;              // process only PF_PRIVSOURCE_QUERY_STORE_INFO.
    };
    SIZE_T WsTotalPages;            // process/session only.
    ULONG DeepFreezeTimeMs;         // process only.
    ULONG ModernApp : 1;            // process only.
    ULONG DeepFrozen : 1;           // process only. If set, DeepFreezeTimeMs contains the time at which the freeze occurred
    ULONG Foreground : 1;           // process only.
    ULONG PerProcessStore : 1;      // process only.
    ULONG Spare : 28;
} PF_PRIVSOURCE_INFO, *PPF_PRIVSOURCE_INFO;

// rev
#define PF_PRIVSOURCE_QUERY_REQUEST_VERSION 8
#define PF_PRIVSOURCE_QUERY_REQUEST_FLAGS_QUERYWSPAGES 0x1
#define PF_PRIVSOURCE_QUERY_REQUEST_FLAGS_QUERYCOMPRESSEDPAGES 0x2
#define PF_PRIVSOURCE_QUERY_REQUEST_FLAGS_QUERYSKIPPAGES 0x4 // ??

// rev
typedef struct _PF_PRIVSOURCE_QUERY_REQUEST
{
    ULONG Version;
    ULONG Flags;
    ULONG InfoCount;
    PF_PRIVSOURCE_INFO InfoArray[1];
} PF_PRIVSOURCE_QUERY_REQUEST, *PPF_PRIVSOURCE_QUERY_REQUEST;

// rev
typedef enum _PF_PHASED_SCENARIO_TYPE
{
    PfScenarioTypeNone,
    PfScenarioTypeStandby,
    PfScenarioTypeHibernate,
    PfScenarioTypeFUS,
    PfScenarioTypeMax
} PF_PHASED_SCENARIO_TYPE;

// rev
#define PF_SCENARIO_PHASE_INFO_VERSION 4

// rev
typedef struct _PF_SCENARIO_PHASE_INFO
{
    ULONG Version;
    PF_PHASED_SCENARIO_TYPE ScenType;
    ULONG PhaseId;
    ULONG SequenceNumber;
    ULONG Flags;
    ULONG FUSUserId;
    ULONG Reserved0; // pad to 32 bytes (handler expects 32)
    ULONG Reserved1;
} PF_SCENARIO_PHASE_INFO, *PPF_SCENARIO_PHASE_INFO;

// rev
typedef struct _PF_WORKER_PRIORITY_CONTROL 
{
    ULONG Version;
    KPRIORITY Priority; // 0..31 (STATUS_INVALID_PARAMETER if >31)
    HANDLE PartitionHandle;
} PF_WORKER_PRIORITY_CONTROL, *PPF_WORKER_PRIORITY_CONTROL;

// rev
typedef struct _PF_MEMORY_LIST_NODE
{
    ULONGLONG Node : 8;
    ULONGLONG Spare : 56;
    ULONGLONG StandbyLowPageCount;
    ULONGLONG StandbyMediumPageCount;
    ULONGLONG StandbyHighPageCount;
    ULONGLONG FreePageCount;
    ULONGLONG ModifiedPageCount;
} PF_MEMORY_LIST_NODE, *PPF_MEMORY_LIST_NODE;

// rev
typedef struct _PF_ROBUST_PROCESS_ENTRY
{
    ULONG ImagePathHash;
    ULONG Pid;
    ULONG Alignment;
} PF_ROBUST_PROCESS_ENTRY, *PPF_ROBUST_PROCESS_ENTRY;

// rev
typedef struct _PF_ROBUST_FILE_ENTRY
{
    ULONG FilePathHash;
} PF_ROBUST_FILE_ENTRY, *PPF_ROBUST_FILE_ENTRY;

// rev
typedef enum _PF_ROBUSTNESS_CONTROL_COMMAND
{
    PfRpControlUpdate = 0,
    PfRpControlReset = 1,
    PfRpControlRobustAllStart = 2,
    PfRpControlRobustAllStop = 3,
    PfRpControlCommandMax = 4
} PF_ROBUSTNESS_CONTROL_COMMAND, *PPF_ROBUSTNESS_CONTROL_COMMAND;

// rev
typedef struct _PF_SCENARIO_PREFETCH_INFO
{
    ULONG Version;
    ULONG State;
} PF_SCENARIO_PREFETCH_INFO, *PPF_SCENARIO_PREFETCH_INFO;

// rev
#define PF_TRIM_WHILE_AGING_CONTROL_VERSION_1 1

// rev
typedef enum _PF_TRIM_WHILE_AGING_STATE
{
    PfTrimWhileAgingOff = 0,
    PfTrimWhileAgingLowPriority = 1,
    PfTrimWhileAgingPassive = 2,
    PfTrimWhileAgingNormal = 3,
    PfTrimWhileAgingAggressive = 4,
    PfTrimWhileAgingMax = 5,
} PF_TRIM_WHILE_AGING_STATE, *PPF_TRIM_WHILE_AGING_STATE;

// rev
typedef struct _PF_TRIM_WHILE_AGING_CONTROL_1
{
    ULONG Version;
    PF_TRIM_WHILE_AGING_STATE TrimWhileAgingState;
    BOOLEAN PrivatePageTrimAge;
    BOOLEAN SharedPageTrimAge;
    USHORT Spare;
} PF_TRIM_WHILE_AGING_CONTROL_1, *PPF_TRIM_WHILE_AGING_CONTROL_1;

#define PF_TRIM_WHILE_AGING_CONTROL_VERSION_2 2

// rev
typedef struct _PF_TRIM_WHILE_AGING_CONTROL_2
{
    ULONG Version;
    PF_TRIM_WHILE_AGING_STATE TrimWhileAgingState;
    UCHAR PrivatePageTrimAge;      // 0..7
    UCHAR SharedPageTrimAge;       // 0..7
    USHORT Spare;                  // must be 0
} PF_TRIM_WHILE_AGING_CONTROL_2, *PPF_TRIM_WHILE_AGING_CONTROL_2;

// rev
#define PF_ROBUSTNESS_CONTROL_VERSION 1

// rev
typedef struct _PF_ROBUSTNESS_CONTROL
{
    ULONG Version;
    PF_ROBUSTNESS_CONTROL_COMMAND Command;
    ULONG DeprioProcessCount;
    ULONG ExemptProcessCount;
    ULONG DeprioFileCount;
    ULONG ExemptFileCount;
    PF_ROBUST_PROCESS_ENTRY ProcessEntries[1];
    PF_ROBUST_FILE_ENTRY FileEntries[1];
} PF_ROBUSTNESS_CONTROL, *PPF_ROBUSTNESS_CONTROL;

// rev
typedef struct _PF_TIME_CONTROL
{
    LONG TimeAdjustment;
} PF_TIME_CONTROL, *PPF_TIME_CONTROL;

#define PF_MEMORY_LIST_INFO_VERSION 1

typedef struct _PF_MEMORY_LIST_INFO
{
    ULONG Version;
    ULONG Size;
    ULONG NodeCount;
    PF_MEMORY_LIST_NODE Nodes[1];
} PF_MEMORY_LIST_INFO, *PPF_MEMORY_LIST_INFO;

typedef struct _PF_PHYSICAL_MEMORY_RANGE
{
    ULONG_PTR BasePfn;
    ULONG_PTR PageCount;
} PF_PHYSICAL_MEMORY_RANGE, *PPF_PHYSICAL_MEMORY_RANGE;

#define PF_PHYSICAL_MEMORY_RANGE_INFO_V1_VERSION 1

typedef struct _PF_PHYSICAL_MEMORY_RANGE_INFO_V1
{
    ULONG Version;
    ULONG RangeCount;
    PF_PHYSICAL_MEMORY_RANGE Ranges[1];
} PF_PHYSICAL_MEMORY_RANGE_INFO_V1, *PPF_PHYSICAL_MEMORY_RANGE_INFO_V1;

#define PF_PHYSICAL_MEMORY_RANGE_INFO_V2_VERSION 2

typedef struct _PF_PHYSICAL_MEMORY_RANGE_INFO_V2
{
    ULONG Version;
    ULONG Flags;
    ULONG RangeCount;
    PF_PHYSICAL_MEMORY_RANGE Ranges[ANYSIZE_ARRAY];
} PF_PHYSICAL_MEMORY_RANGE_INFO_V2, *PPF_PHYSICAL_MEMORY_RANGE_INFO_V2;

// rev
typedef struct _PF_START_TRACE_CONTROL 
{
    struct 
    {
        ULONG Type;
        ULONG Mode;
        ULONG Flags;
        ULONG Restart;
    };
    struct 
    {
        HANDLE PartitionHandle; // in
        HANDLE TraceHandleOut;  // out when Restart == 0
    };
} PF_START_TRACE_CONTROL, *PPF_START_TRACE_CONTROL;

// rev
#define PF_ACCESS_TRACING_CONTROL_VERSION 1

// rev
typedef struct _PF_ACCESS_TRACING_CONTROL
{
    ULONG Version;
    ULONG Command;
    ULONG ComponentMask;
} PF_ACCESS_TRACING_CONTROL, *PPF_ACCESS_TRACING_CONTROL;

// rev
#define PF_REPURPOSED_BY_PREFETCH_INFO_VERSION 1

// rev
typedef struct _PF_REPURPOSED_BY_PREFETCH_INFO
{
    ULONG Version; // PF_REPURPOSED_BY_PREFETCH_INFO_VERSION
    ULONG Reserved;
    SIZE_T RepurposedByPrefetch;
} PF_REPURPOSED_BY_PREFETCH_INFO, *PPF_REPURPOSED_BY_PREFETCH_INFO;

// rev
#define PF_VIRTUAL_QUERY_VERSION 1

// rev
typedef struct _PF_VIRTUAL_QUERY
{
    ULONG Version;
    union
    {
        ULONG Flags;
        struct
        {
            ULONG FaultInPageTables : 1;
            ULONG ReportPageTables : 1;
            ULONG Spare : 30;
        };
    };
    PVOID QueryBuffer; // MEMORY_WORKING_SET_EX_INFORMATION[NumberOfPages] (input: VirtualAddress[], output: VirtualAttributes[])
    SIZE_T QueryBufferSize; // NumberOfPages * sizeof(MEMORY_WORKING_SET_EX_INFORMATION)
    HANDLE ProcessHandle;
} PF_VIRTUAL_QUERY, *PPF_VIRTUAL_QUERY;

// rev
#define PF_PAGECOMBINE_AGGREGATE_STAT_VERSION 1

// rev
typedef struct _PF_PAGECOMBINE_AGGREGATE_STAT
{
    ULONG Version;
    ULONG CombineScanCount;
    ULONG CombinedBlocksInUse;
    ULONG SumCombinedBlocksReferenceCount;
} PF_PAGECOMBINE_AGGREGATE_STAT, *PPF_PAGECOMBINE_AGGREGATE_STAT;

// rev
#define PF_MIN_WS_AGE_RATE_CONTROL_VERSION 1

// rev
typedef struct _PF_MIN_WS_AGE_RATE_CONTROL
{
    ULONG Version;
    ULONG SecondsToOldestAgeRate;
} PF_MIN_WS_AGE_RATE_CONTROL, *PPF_MIN_WS_AGE_RATE_CONTROL;

// rev
#define PF_DEPRIORITIZE_OLD_PAGES_VERSION 3

// rev
typedef struct _PF_DEPRIORITIZE_OLD_PAGES
{
    ULONG Version;
    HANDLE ProcessHandle;
    union
    {
        ULONG Flags;
        struct
        {
            ULONG TargetPriority : 4;
            ULONG TrimPages : 2;
            ULONG Spare : 26;
        };
    };
} PF_DEPRIORITIZE_OLD_PAGES, *PPF_DEPRIORITIZE_OLD_PAGES;

// rev
#define PF_FILE_EXTENTS_INFO_VERSION 1

// rev
typedef struct _PF_FILE_EXTENTS_INFO
{
    ULONG Version;
    PWSTR FilePath;
    ULONG FilePathSize;
    ULONG VolumePathSize;
    LARGE_INTEGER FileIndexNumber;
    ULONG VolumeSerialNumber;
    RETRIEVAL_POINTERS_BUFFER ExtentsBuffer;
    ULONGLONG ExtentsBufferSize;
} PF_FILE_EXTENTS_INFO, *PPF_FILE_EXTENTS_INFO;

// rev
#define PF_GPU_UTILIZATION_INFO_VERSION 1

// rev
typedef struct _PF_GPU_UTILIZATION_INFO
{
    ULONG Version;
    ULONG SessionId;
    ULONGLONG GpuTime;
} PF_GPU_UTILIZATION_INFO, *PPF_GPU_UTILIZATION_INFO;

// rev
typedef enum _SUPERFETCH_INFORMATION_CLASS
{
    SuperfetchRetrieveTrace = 1,               // q: PF_SYSTEM_SUPERFETCH_RETRIEVE_TRACE // PfGetCompletedTrace
    SuperfetchSystemParameters,                // q: PF_SYSTEM_SUPERFETCH_PARAMETERS
    SuperfetchLogEvent,                        // s: PF_LOG_EVENT_DATA
    SuperfetchGenerateTrace,                   // s: PF_GENERATE_TRACE_CONTROL
    SuperfetchPrefetch,
    SuperfetchPfnQuery,                        // q: PF_PFN_PRIO_REQUEST
    SuperfetchPfnSetPriority,                  // s: PF_PFN_PRIO_REQUEST // MmSetPfnListInfo
    SuperfetchPrivSourceQuery,                 // q: PF_PRIVSOURCE_QUERY_REQUEST
    SuperfetchSequenceNumberQuery,             // q: ULONG
    SuperfetchScenarioPhase,                   // s: PF_SCENARIO_PHASE_INFO // 10
    SuperfetchWorkerPriority,                  // s: PF_WORKER_PRIORITY_CONTROL
    SuperfetchScenarioQuery,
    SuperfetchScenarioPrefetch,                // s: PF_SCENARIO_PREFETCH_INFO
    SuperfetchRobustnessControl,               // s: PF_ROBUSTNESS_CONTROL
    SuperfetchTimeControl,                     // s: PF_TIME_CONTROL
    SuperfetchMemoryListQuery,                 // q: PF_MEMORY_LIST_INFO
    SuperfetchMemoryRangesQuery,               // q: PF_PHYSICAL_MEMORY_RANGE_INFO_V1/V2
    SuperfetchTracingControl,                  // s: PF_ACCESS_TRACING_CONTROL
    SuperfetchTrimWhileAgingControl,           // s: PF_TRIM_WHILE_AGING_CONTROL
    SuperfetchRepurposedByPrefetch,            // q: PF_REPURPOSED_BY_PREFETCH_INFO // 20
    SuperfetchChannelPowerRequest,
    SuperfetchMovePages,                       // s: PF_PFN_PRIO_REQUEST // MmRelocatePfnList
    SuperfetchVirtualQuery,                    // q: PF_VIRTUAL_QUERY
    SuperfetchCombineStatsQuery,               // q: PF_PAGECOMBINE_AGGREGATE_STAT
    SuperfetchSetMinWsAgeRate,                 // s: PF_MIN_WS_AGE_RATE_CONTROL
    SuperfetchDeprioritizeOldPagesInWs,        // s: PF_DEPRIORITIZE_OLD_PAGES 
    SuperfetchFileExtentsQuery,                // q: PF_FILE_EXTENTS_INFO
    SuperfetchGpuUtilizationQuery,             // q: PF_GPU_UTILIZATION_INFO
    SuperfetchPfnSet,                          // s: PF_PFN_PRIO_REQUEST // since WIN11
    SuperfetchInformationMax
} SUPERFETCH_INFORMATION_CLASS;

#define SUPERFETCH_INFORMATION_VERSION 45 // rev
#define SUPERFETCH_INFORMATION_MAGIC ('kuhC') // rev

typedef struct _SUPERFETCH_INFORMATION
{
    _In_ ULONG Version;
    _In_ ULONG Magic;
    _In_ SUPERFETCH_INFORMATION_CLASS SuperfetchInformationClass;
    _Inout_ PVOID SuperfetchInformation;
    _Inout_ ULONG SuperfetchInformationLength;
} SUPERFETCH_INFORMATION, *PSUPERFETCH_INFORMATION;

#pragma endregion

#if !defined(_KERNEL_MODE)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQuerySystemInformation(
    _In_ SYSTEM_INFORMATION_CLASS SystemInformationClass,
    _Out_writes_bytes_opt_(SystemInformationLength) PVOID SystemInformation,
    _In_ ULONG SystemInformationLength,
    _Out_opt_ PULONG ReturnLength
);

/**
 * The NtQuerySystemInformationEx routine queries information about the system.
 *
 * @param SystemInformationClass The type of information to be retrieved.
 * @param InputBuffer Pointer to a caller-allocated input buffer that contains class-specific information.
 * @param InputBufferLength The size of the buffer pointed to by InputBuffer.
 * @param SystemInformation A pointer to a buffer that receives the requested information.
 * @param SystemInformationLength The size of the buffer pointed to by SystemInformation.
 * @param ReturnLength A pointer to a variable that receives the size of the data returned in the buffer.
 * @return NTSTATUS Successful or errant status.
 * @see https://learn.microsoft.com/en-us/windows/win32/sysinfo/zwquerysysteminformation
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQuerySystemInformationEx(
    _In_ SYSTEM_INFORMATION_CLASS SystemInformationClass,
    _In_reads_bytes_(InputBufferLength) PVOID InputBuffer,
    _In_ ULONG InputBufferLength,
    _Out_writes_bytes_opt_(SystemInformationLength) PVOID SystemInformation,
    _In_ ULONG SystemInformationLength,
    _Out_opt_ PULONG ReturnLength
);
#endif

/**
 * The NtSetSystemInformation routine sets information about the system.
 *
 * @param SystemInformationClass The type of information to be set.
 * @param SystemInformation A pointer to a buffer that receives the requested information.
 * @param SystemInformationLength The size of the buffer pointed to by SystemInformation.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetSystemInformation(
    _In_ SYSTEM_INFORMATION_CLASS SystemInformationClass,
    _In_reads_bytes_opt_(SystemInformationLength) PVOID SystemInformation,
    _In_ ULONG SystemInformationLength
);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryLicenseValue(
    _In_ PUNICODE_STRING ValueName,
    _Out_opt_ PULONG Type,
    _Out_writes_bytes_to_opt_(DataSize, *ResultDataSize) PVOID Data,
    _In_ ULONG DataSize,
    _Out_ PULONG ResultDataSize);

EXTERN_C_END
