#pragma once

#include "../MinDef.h"
#include "Processor.h"
#include "../Mm/Constant.h"

EXTERN_C_START

/* ntddk.h & phnt & Geoff Chappell & KNSoft.NDK */

//
// Define data shared between kernel and user mode.
//
// N.B. User mode has read only access to this data
//

//
// WARNING: This structure must have exactly the same layout for 32- and
//    64-bit systems. The layout of this structure cannot change and new
//    fields can only be added at the end of the structure (unless a gap
//    can be exploited). Deprecated fields cannot be deleted. Platform
//    specific fields are included on all systems.
//
//    Layout exactness is required for Wow64 support of 32-bit applications
//    on Win64 systems.
//
//    The layout itself cannot change since this structure has been exported
//    in ntddk, ntifs.h, and nthal.h for some time.
//
// Define NX support policy values.
//

#define NX_SUPPORT_POLICY_ALWAYSOFF     0
#define NX_SUPPORT_POLICY_ALWAYSON      1
#define NX_SUPPORT_POLICY_OPTIN         2
#define NX_SUPPORT_POLICY_OPTOUT        3

//
// SEH chain validation policies.
//
// N.B. These constants must not be changed because the ldr relies on their
//      semantic meaning.
//

#define SEH_VALIDATION_POLICY_ON        0
#define SEH_VALIDATION_POLICY_OFF       1
#define SEH_VALIDATION_POLICY_TELEMETRY 2
#define SEH_VALIDATION_POLICY_DEFER     3

//
// Global shared data flags and manipulation macros.
//

#define SHARED_GLOBAL_FLAGS_ERROR_PORT_V                0x0
#define SHARED_GLOBAL_FLAGS_ERROR_PORT                  \
    (1UL << SHARED_GLOBAL_FLAGS_ERROR_PORT_V)

#define SHARED_GLOBAL_FLAGS_ELEVATION_ENABLED_V         0x1
#define SHARED_GLOBAL_FLAGS_ELEVATION_ENABLED           \
    (1UL << SHARED_GLOBAL_FLAGS_ELEVATION_ENABLED_V)

#define SHARED_GLOBAL_FLAGS_VIRT_ENABLED_V              0x2
#define SHARED_GLOBAL_FLAGS_VIRT_ENABLED                \
    (1UL << SHARED_GLOBAL_FLAGS_VIRT_ENABLED_V)

#define SHARED_GLOBAL_FLAGS_INSTALLER_DETECT_ENABLED_V  0x3
#define SHARED_GLOBAL_FLAGS_INSTALLER_DETECT_ENABLED    \
    (1UL << SHARED_GLOBAL_FLAGS_INSTALLER_DETECT_ENABLED_V)

#define SHARED_GLOBAL_FLAGS_LKG_ENABLED_V               0x4
#define SHARED_GLOBAL_FLAGS_LKG_ENABLED                 \
    (1UL << SHARED_GLOBAL_FLAGS_LKG_ENABLED_V)

#define SHARED_GLOBAL_FLAGS_DYNAMIC_PROC_ENABLED_V      0x5
#define SHARED_GLOBAL_FLAGS_DYNAMIC_PROC_ENABLED        \
    (1UL << SHARED_GLOBAL_FLAGS_DYNAMIC_PROC_ENABLED_V)

#define SHARED_GLOBAL_FLAGS_CONSOLE_BROKER_ENABLED_V    0x6
#define SHARED_GLOBAL_FLAGS_CONSOLE_BROKER_ENABLED      \
    (1UL << SHARED_GLOBAL_FLAGS_CONSOLE_BROKER_ENABLED_V)

#define SHARED_GLOBAL_FLAGS_SECURE_BOOT_ENABLED_V       0x7
#define SHARED_GLOBAL_FLAGS_SECURE_BOOT_ENABLED         \
    (1UL << SHARED_GLOBAL_FLAGS_SECURE_BOOT_ENABLED_V)

#define SHARED_GLOBAL_FLAGS_MULTI_SESSION_SKU_V         0x8
#define SHARED_GLOBAL_FLAGS_MULTI_SESSION_SKU           \
    (1UL << SHARED_GLOBAL_FLAGS_MULTI_SESSION_SKU_V)

#define SHARED_GLOBAL_FLAGS_MULTIUSERS_IN_SESSION_SKU_V 0x9
#define SHARED_GLOBAL_FLAGS_MULTIUSERS_IN_SESSION_SKU   \
    (1UL << SHARED_GLOBAL_FLAGS_MULTIUSERS_IN_SESSION_SKU_V)

#define SHARED_GLOBAL_FLAGS_STATE_SEPARATION_ENABLED_V 0xA
#define SHARED_GLOBAL_FLAGS_STATE_SEPARATION_ENABLED   \
    (1UL << SHARED_GLOBAL_FLAGS_STATE_SEPARATION_ENABLED_V)

#define SHARED_GLOBAL_FLAGS_ADMINAPPROVALMODE_TYPE_SPLITTOKEN_V         0xB
#define SHARED_GLOBAL_FLAGS_ADMINAPPROVALMODE_TYPE_SPLITTOKEN           \
    (1UL << SHARED_GLOBAL_FLAGS_ADMINAPPROVALMODE_TYPE_SPLITTOKEN_V)

#define SHARED_GLOBAL_FLAGS_ADMINAPPROVALMODE_TYPE_SHADOWADMIN_V        0xC
#define SHARED_GLOBAL_FLAGS_ADMINAPPROVALMODE_TYPE_SHADOWADMIN          \
    (1UL << SHARED_GLOBAL_FLAGS_ADMINAPPROVALMODE_TYPE_SHADOWADMIN_V)

#define SHARED_GLOBAL_FLAGS_SET_GLOBAL_DATA_FLAG        0x40000000

#define SHARED_GLOBAL_FLAGS_CLEAR_GLOBAL_DATA_FLAG      0x80000000

#define EX_INIT_BITS(Flags, Bit) \
    *((Flags)) |= (Bit)             // Safe to use before concurrently accessible

#define EX_TEST_SET_BIT(Flags, Bit) \
    InterlockedBitTestAndSet ((PLONG)(Flags), (Bit))

#define EX_TEST_CLEAR_BIT(Flags, Bit) \
    InterlockedBitTestAndReset ((PLONG)(Flags), (Bit))

//
// Define legal values for the SystemCall member.
//

#define SYSTEM_CALL_SYSCALL 0
#define SYSTEM_CALL_INT_2E  1

//
// Define flags for QPC bypass information. None of these flags may be set
// unless bypass is enabled. This is for compat with existing code which
// compares this value to zero to detect bypass enablement.
//

#define SHARED_GLOBAL_FLAGS_QPC_BYPASS_ENABLED (0x01)
#define SHARED_GLOBAL_FLAGS_QPC_BYPASS_USE_HV_PAGE (0x02)
#define SHARED_GLOBAL_FLAGS_QPC_BYPASS_DISABLE_32BIT (0x04)
#define SHARED_GLOBAL_FLAGS_QPC_BYPASS_USE_MFENCE (0x10)
#define SHARED_GLOBAL_FLAGS_QPC_BYPASS_USE_LFENCE (0x20)
#define SHARED_GLOBAL_FLAGS_QPC_BYPASS_A73_ERRATA (0x40)
#define SHARED_GLOBAL_FLAGS_QPC_BYPASS_USE_RDTSCP (0x80)

/**
 * The KUSER_SHARED_DATA structure contains information shared with user-mode.
 *
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-kuser_shared_data
 */
typedef struct _KUSER_SHARED_DATA
{
    //
    // Current low 32-bit of tick count and tick count multiplier.
    //
    // N.B. The tick count is updated each time the clock ticks.
    //
    /* +0x000 */ ULONG TickCountLowDeprecated;
    /* +0x004 */ ULONG TickCountMultiplier;

    //
    // Current 64-bit interrupt time in 100ns units.
    //
    /* +0x008 */ volatile KSYSTEM_TIME InterruptTime;

    //
    // Current 64-bit system time in 100ns units.
    //
    /* +0x014 */ volatile KSYSTEM_TIME SystemTime;

    //
    // Current 64-bit time zone bias.
    //
    /* +0x020 */ volatile KSYSTEM_TIME TimeZoneBias;

    //
    // Support image magic number range for the host system.
    //
    // N.B. This is an inclusive range.
    //
    /* +0x02C */ USHORT ImageNumberLow;
    /* +0x02E */ USHORT ImageNumberHigh;

    //
    // Copy of system root in unicode.
    //
    // N.B. This field must be accessed via the RtlGetNtSystemRoot API for
    //      an accurate result.
    //
    /* +0x030 */ WCHAR NtSystemRoot[260];

    //
    // Maximum stack trace depth if tracing enabled.
    //
    /* +0x238 */ ULONG MaxStackTraceDepth;

    //
    // Crypto exponent value.
    //
    /* +0x23C */ ULONG CryptoExponent;

    //
    // Time zone ID.
    //
    /* +0x240 */ ULONG TimeZoneId;
    /* +0x244 */ ULONG LargePageMinimum;

    //
    // This value controls the AIT Sampling rate.
    //
    /* +0x248 */ ULONG AitSamplingValue;

    //
    // This value controls switchback processing.
    //
    /* +0x24C */ ULONG AppCompatFlag;

    //
    // Current Kernel Root RNG state seed version
    //
    /* +0x250 */ ULONGLONG RNGSeedVersion;

    //
    // This value controls assertion failure handling.
    //
    /* +0x258 */ ULONG GlobalValidationRunlevel;

    /* +0x25C */ volatile LONG TimeZoneBiasStamp;

    //
    // The shared collective build number undecorated with C or F.
    // GetVersionEx hides the real number
    //
    /* +0x260 */ ULONG NtBuildNumber;

    //
    // Product type.
    //
    // N.B. This field must be accessed via the RtlGetNtProductType API for
    //      an accurate result.
    //
    /* +0x264 */ NT_PRODUCT_TYPE NtProductType;
    /* +0x268 */ BOOLEAN ProductTypeIsValid;
    /* +0x269 */ BOOLEAN Reserved0[1];
    /* +0x26A */ USHORT NativeProcessorArchitecture;

    //
    // The NT Version.
    //
    // N. B. Note that each process sees a version from its PEB, but if the
    //       process is running with an altered view of the system version,
    //       the following two fields are used to correctly identify the
    //       version
    //
    /* +0x26C */ ULONG NtMajorVersion;
    /* +0x270 */ ULONG NtMinorVersion;

    //
    // Processor features.
    //
    /* +0x274 */ BOOLEAN ProcessorFeatures[PROCESSOR_FEATURE_MAX];

    //
    // Reserved fields - do not use.
    //
    /* +0x2B4 */ ULONG MaximumUserModeAddressDeprecated; // Deprecated, use SystemBasicInformation instead.
    /* +0x2B8 */ ULONG SystemRangeStartDeprecated;       // Deprecated, use SystemRangeStartInformation instead.

    //
    // Time slippage while in debugger.
    //
    /* +0x2BC */ volatile ULONG TimeSlip;

    //
    // Alternative system architecture, e.g., NEC PC98xx on x86.
    //
    /* +0x2C0 */ ALTERNATIVE_ARCHITECTURE_TYPE AlternativeArchitecture;

    //
    // Boot sequence, incremented for each boot attempt by the OS loader.
    //
    /* +0x2C4 */ ULONG BootId;

    //
    // If the system is an evaluation unit, the following field contains the
    // date and time that the evaluation unit expires. A value of 0 indicates
    // that there is no expiration. A non-zero value is the UTC absolute time
    // that the system expires.
    //
    /* +0x2C8 */ LARGE_INTEGER SystemExpirationDate;

    //
    // Suite support.
    //
    // N.B. This field must be accessed via the RtlGetSuiteMask API for
    //      an accurate result.
    //
    /* +0x2D0 */ ULONG SuiteMask;

    //
    // TRUE if a kernel debugger is connected/enabled.
    //
    /* +0x2D4 */ BOOLEAN KdDebuggerEnabled;

    //
    // Mitigation policies.
    //
    union
    {
        /* +0x2D5 */ UCHAR MitigationPolicies;
        struct
        {
            /* +0x2D5 */ UCHAR NXSupportPolicy : 2;
            /* +0x2D5 */ UCHAR SEHValidationPolicy : 2;
            /* +0x2D5 */ UCHAR CurDirDevicesSkippedForDlls : 2;
            /* +0x2D5 */ UCHAR Reserved : 2;
        };
    };

    //
    // Measured duration of a single processor yield, in cycles. This is used by
    // lock packages to determine how many times to spin waiting for a state
    // change before blocking.
    //
    /* +0x2D6 */ USHORT CyclesPerYield;

    //
    // Current console session Id. Always zero on non-TS systems.
    //
    // N.B. This field must be accessed via the RtlGetActiveConsoleId API for an
    //      accurate result.
    //
    /* +0x2D8 */ volatile ULONG ActiveConsoleId;

    //
    // Force-dismounts cause handles to become invalid. Rather than always
    // probe handles, a serial number of dismounts is maintained that clients
    // can use to see if they need to probe handles.
    //
    /* +0x2DC */ volatile ULONG DismountCount;

    //
    // This field indicates the status of the 64-bit COM+ package on the
    // system. It indicates whether the Intermediate Language (IL) COM+
    // images need to use the 64-bit COM+ runtime or the 32-bit COM+ runtime.
    //
    /* +0x2E0 */ ULONG ComPlusPackage;

    //
    // Time in tick count for system-wide last user input across all terminal
    // sessions. For MP performance, it is not updated all the time (e.g. once
    // a minute per session). It is used for idle detection.
    //
    /* +0x2E4 */ ULONG LastSystemRITEventTickCount;

    //
    // Number of physical pages in the system. This can dynamically change as
    // physical memory can be added or removed from a running system.  This
    // cell is too small to hold the non-truncated value on very large memory
    // machines so code that needs the full value should access
    // FullNumberOfPhysicalPages instead.
    //
    /* +0x2E8 */ ULONG NumberOfPhysicalPages;

    //
    // True if the system was booted in safe boot mode.
    //
    /* +0x2EC */ BOOLEAN SafeBootMode;

    //
    // Virtualization flags.
    //
    union
    {
        /* +0x2ED */ UCHAR VirtualizationFlags;

#if defined(_ARM64_)
        //
        // N.B. Keep this bitfield in sync with the one in arc.w.
        //
        struct
        {
            /* +0x2ED */ UCHAR ArchStartedInEl2 : 1;
            /* +0x2ED */ UCHAR QcSlIsSupported : 1;
            /* +0x2ED */ UCHAR : 6;
        };
#endif

    };

    //
    // Reserved (available for reuse).
    //
    /* +0x2EE */ UCHAR Reserved12[2];

    //
    // This is a packed bitfield that contains various flags concerning
    // the system state. They must be manipulated using interlocked
    // operations.
    //
    // N.B. DbgMultiSessionSku must be accessed via the RtlIsMultiSessionSku
    //      API for an accurate result
    //
    union
    {
        /* +0x2F0 */ ULONG SharedDataFlags;
        struct
        {
            //
            // The following bit fields are for the debugger only. Do not use.
            // Use the bit definitions instead.
            //
            /* +0x2F0 */ ULONG DbgErrorPortPresent       : 1;
            /* +0x2F0 */ ULONG DbgElevationEnabled       : 1;
            /* +0x2F0 */ ULONG DbgVirtEnabled            : 1;
            /* +0x2F0 */ ULONG DbgInstallerDetectEnabled : 1;
            /* +0x2F0 */ ULONG DbgLkgEnabled             : 1;
            /* +0x2F0 */ ULONG DbgDynProcessorEnabled    : 1;
            /* +0x2F0 */ ULONG DbgConsoleBrokerEnabled   : 1;
            /* +0x2F0 */ ULONG DbgSecureBootEnabled      : 1;
            /* +0x2F0 */ ULONG DbgMultiSessionSku        : 1;
            /* +0x2F0 */ ULONG DbgMultiUsersInSessionSku : 1;
            /* +0x2F0 */ ULONG DbgStateSeparationEnabled : 1;
            /* +0x2F0 */ ULONG SpareBits                 : 21;
        } DUMMYSTRUCTNAME2;
    } DUMMYUNIONNAME2;

    /* +0x2F4 */ ULONG DataFlagsPad[1];

    //
    // Depending on the processor, the code for fast system call will differ,
    // Stub code is provided pointers below to access the appropriate code.
    //
    // N.B. The following field is only used on 32-bit systems.
    //
    /* +0x2F8 */ ULONGLONG TestRetInstruction;
    /* +0x300 */ LONGLONG QpcFrequency;

    //
    // On AMD64, this value is initialized to a nonzero value if the system
    // operates with an altered view of the system service call mechanism.
    //
    /* +0x308 */ ULONG SystemCall;

    //
    // Reserved field - do not use. Used to be UserCetAvailableEnvironments.
    //
    /* +0x30C */ ULONG Reserved2;

    //
    // Full 64 bit version of the number of physical pages in the system.
    // This can dynamically change as physical memory can be added or removed
    // from a running system.
    //
    /* +0x310 */ ULONGLONG FullNumberOfPhysicalPages;

    //
    // Reserved, available for reuse.
    //
    /* +0x318 */ ULONGLONG SystemCallPad[1];

    //
    // The 64-bit tick count.
    //
    union
    {
        /* +0x320 */ volatile KSYSTEM_TIME TickCount;
        /* +0x320 */ volatile ULONG64 TickCountQuad;
        struct
        {
            /* +0x320 */ ULONG ReservedTickCountOverlay[3];
            /* +0x320 */ ULONG TickCountPad[1];
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME3;

    //
    // Cookie for encoding pointers system wide.
    //
    /* +0x330 */ ULONG Cookie;
    /* +0x334 */ ULONG CookiePad[1];

    //
    // Client id of the process having the focus in the current
    // active console session id.
    //
    // N.B. This field must be accessed via the
    //      RtlGetConsoleSessionForegroundProcessId API for an accurate result.
    //
    /* +0x338 */ LONGLONG ConsoleSessionForegroundProcessId;

    //
    // N.B. The following data is used to implement the precise time
    //      services. It is aligned on a 64-byte cache-line boundary and
    //      arranged in the order of typical accesses.
    //
    // Placeholder for the (internal) time update lock.
    //
    /* +0x340 */ ULONGLONG TimeUpdateLock;

    //
    // The performance counter value used to establish the current system time.
    //
    /* +0x348 */ ULONGLONG BaselineSystemTimeQpc;

    //
    // The performance counter value used to compute the last interrupt time.
    //
    /* +0x350 */ ULONGLONG BaselineInterruptTimeQpc;

    //
    // The scaled number of system time seconds represented by a single
    // performance count (this value may vary to achieve time synchronization).
    //
    /* +0x358 */ ULONGLONG QpcSystemTimeIncrement;

    //
    // The scaled number of interrupt time seconds represented by a single
    // performance count (this value is constant after the system is booted).
    //
    /* +0x360 */ ULONGLONG QpcInterruptTimeIncrement;

    //
    // The scaling shift count applied to the performance counter system time
    // increment.
    //
    /* +0x368 */ UCHAR QpcSystemTimeIncrementShift;

    //
    // The scaling shift count applied to the performance counter interrupt time
    // increment.
    //
    /* +0x369 */ UCHAR QpcInterruptTimeIncrementShift;

    //
    // The count of unparked processors.
    //
    /* +0x36A */ USHORT UnparkedProcessorCount;

    //
    // A bitmask of enclave features supported on this system.
    //
    // N.B. This field must be accessed via the RtlIsEnclareFeaturePresent API for an
    //      accurate result.
    //
    /* +0x36C */ ULONG EnclaveFeatureMask[4];

    //
    // Current coverage round for telemetry based coverage.
    //
    /* +0x37C */ ULONG TelemetryCoverageRound;

    //
    // The following field is used for ETW user mode global logging
    // (UMGL).
    //
    /* +0x380 */ USHORT UserModeGlobalLogger[16];

    //
    // Settings that can enable the use of Image File Execution Options
    // from HKCU in addition to the original HKLM.
    //
    /* +0x3A0 */ ULONG ImageFileExecutionOptions;

    //
    // Generation of the kernel structure holding system language information
    //
    /* +0x3A4 */ ULONG LangGenerationCount;

    //
    // Reserved (available for reuse).
    //
    /* +0x3A8 */ ULONGLONG Reserved4;

    //
    // Current 64-bit interrupt time bias in 100ns units.
    //
    /* +0x3B0 */ volatile ULONGLONG InterruptTimeBias;

    //
    // Current 64-bit performance counter bias, in performance counter units
    // before the shift is applied.
    //
    /* +0x3B8 */ volatile ULONGLONG QpcBias;

    //
    // Number of active processors and groups.
    //
    /* +0x3C0 */ ULONG ActiveProcessorCount;
    /* +0x3C4 */ volatile UCHAR ActiveGroupCount;

    //
    // Reserved (available for re-use).
    //
    /* +0x3C5 */ UCHAR Reserved9;

    union
    {
        /* +0x3C6 */ USHORT QpcData;
        struct
        {

            //
            // A bitfield indicating whether performance counter queries can
            // read the counter directly (bypassing the system call) and flags.
            //
            /* +0x3C6 */ volatile UCHAR QpcBypassEnabled;

            //
            // Reserved, leave as zero for backward compatibility. Was shift
            // applied to the raw counter value to derive QPC count.
            //
            /* +0x3C6 */ UCHAR QpcReserved;
        };
    };

    /* +0x3C8 */ LARGE_INTEGER TimeZoneBiasEffectiveStart;
    /* +0x3D0 */ LARGE_INTEGER TimeZoneBiasEffectiveEnd;

    //
    // Extended processor state configuration (AMD64 and x86).
    //
    /* +0x3D8 */ XSTATE_CONFIGURATION XState;

    /* +0x720 */ KSYSTEM_TIME FeatureConfigurationChangeStamp;
    /* +0x72C */ ULONG Spare;

    /* +0x730 */ ULONG64 UserPointerAuthMask;

    //
    // Extended processor state configuration (ARM64). The reserved space for
    // other architectures is not available for reuse.
    //
#if defined(_ARM64_)
    /* +0x738 */ XSTATE_CONFIGURATION XStateArm64;
#else
    /* +0x738 */ ULONG Reserved10[210];
#endif

} KUSER_SHARED_DATA, *PKUSER_SHARED_DATA;

//
// Mostly enforce earlier comment about the stability and
// architecture-neutrality of this struct.
//

//
// Assembler logic assumes a zero value for syscall and a nonzero value for
// int 2e, and that no other values exist presently for the SystemCall field.
//

_STATIC_ASSERT(SYSTEM_CALL_SYSCALL == 0);
_STATIC_ASSERT(SYSTEM_CALL_INT_2E == 1);

//
// The overall size can change, but it must be the same for all architectures.
//

_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TickCountLowDeprecated) == 0x0);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TickCountMultiplier) == 0x4);
_STATIC_ASSERT(__alignof(KSYSTEM_TIME) == 4);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, InterruptTime) == 0x08);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, SystemTime) == 0x014);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TimeZoneBias) == 0x020);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ImageNumberLow) == 0x02c);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ImageNumberHigh) == 0x02e);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, NtSystemRoot) == 0x030);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, MaxStackTraceDepth) == 0x238);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, CryptoExponent) == 0x23c);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TimeZoneId) == 0x240);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, LargePageMinimum) == 0x244);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, AitSamplingValue) == 0x248);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, AppCompatFlag) == 0x24c);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, RNGSeedVersion) == 0x250);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, GlobalValidationRunlevel) == 0x258);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TimeZoneBiasStamp) == 0x25c);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, NtBuildNumber) == 0x260);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, NtProductType) == 0x264);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ProductTypeIsValid) == 0x268);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, NativeProcessorArchitecture) == 0x26a);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, NtMajorVersion) == 0x26c);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, NtMinorVersion) == 0x270);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ProcessorFeatures) == 0x274);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, MaximumUserModeAddressDeprecated) == 0x2b4);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, SystemRangeStartDeprecated) == 0x2b8);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TimeSlip) == 0x2bc);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, AlternativeArchitecture) == 0x2c0);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, SystemExpirationDate) == 0x2c8);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, SuiteMask) == 0x2d0);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, KdDebuggerEnabled) == 0x2d4);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, MitigationPolicies) == 0x2d5);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, CyclesPerYield) == 0x2d6);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ActiveConsoleId) == 0x2d8);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, DismountCount) == 0x2dc);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ComPlusPackage) == 0x2e0);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, LastSystemRITEventTickCount) == 0x2e4);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, NumberOfPhysicalPages) == 0x2e8);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, SafeBootMode) == 0x2ec);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, VirtualizationFlags) == 0x2ed);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, Reserved12) == 0x2ee);

#if defined(_MSC_EXTENSIONS)

_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, SharedDataFlags) == 0x2f0);

#endif

_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TestRetInstruction) == 0x2f8);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, QpcFrequency) == 0x300);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, SystemCall) == 0x308);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, Reserved2) == 0x30c);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, SystemCallPad) == 0x318);

#if defined(_MSC_EXTENSIONS)

_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TickCount) == 0x320);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TickCountQuad) == 0x320);

#endif

_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, Cookie) == 0x330);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ConsoleSessionForegroundProcessId) == 0x338);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TimeUpdateLock) == 0x340);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, BaselineSystemTimeQpc) == 0x348);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, BaselineInterruptTimeQpc) == 0x350);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, QpcSystemTimeIncrement) == 0x358);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, QpcInterruptTimeIncrement) == 0x360);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, QpcSystemTimeIncrementShift) == 0x368);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, QpcInterruptTimeIncrementShift) == 0x369);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, UnparkedProcessorCount) == 0x36a);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, EnclaveFeatureMask) == 0x36c);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TelemetryCoverageRound) == 0x37c);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, UserModeGlobalLogger) == 0x380);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ImageFileExecutionOptions) == 0x3a0);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, LangGenerationCount) == 0x3a4);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, Reserved4) == 0x3a8);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, InterruptTimeBias) == 0x3b0);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, QpcBias) == 0x3b8);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ActiveProcessorCount) == 0x3c0);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ActiveGroupCount) == 0x3c4);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, Reserved9) == 0x3c5);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, QpcData) == 0x3c6);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, QpcBypassEnabled) == 0x3c6);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, QpcReserved) == 0x3c7);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TimeZoneBiasEffectiveStart) == 0x3c8);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TimeZoneBiasEffectiveEnd) == 0x3d0);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, XState) == 0x3d8);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, FeatureConfigurationChangeStamp) == 0x720);
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, UserPointerAuthMask) == 0x730);
#if defined(_ARM64_)
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, XStateArm64) == 0x738);
#else
_STATIC_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, Reserved10) == 0x738);
#endif
#if !defined(WINDOWS_IGNORE_PACKING_MISMATCH)
_STATIC_ASSERT(sizeof(KUSER_SHARED_DATA) == 0xA80);
#endif

/* wdm.h */
#if !defined(_KERNEL_MODE) && !defined(_BOOT_ENVIRONMENT)
#define SharedUserData ((KUSER_SHARED_DATA * const)MM_SHARED_USER_DATA_VA)
#endif

typedef const struct _KUSER_SHARED_DATA* PCKUSER_SHARED_DATA;

EXTERN_C_END
