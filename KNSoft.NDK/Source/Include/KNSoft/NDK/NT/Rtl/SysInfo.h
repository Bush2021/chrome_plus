#pragma once

#include "../MinDef.h"

EXTERN_C_START

/* phnt */

// rev
typedef struct _RTL_OSVERSIONINFOEX2
{
    ULONG OSVersionInfoSize;
    ULONG MajorVersion;
    ULONG MinorVersion;
    ULONG BuildNumber;
    ULONG PlatformId;
    WCHAR CSDVersion[128];
    USHORT ServicePackMajor;
    USHORT ServicePackMinor;
    USHORT SuiteMask;
    UCHAR ProductType;
    UCHAR Reserved;
    ULONG SuiteMaskEx;
    ULONG Reserved2;
} RTL_OSVERSIONINFOEX2, *PRTL_OSVERSIONINFOEX2;

// rev
typedef struct _RTL_OSVERSIONINFOEX3
{
    ULONG OSVersionInfoSize;
    ULONG MajorVersion;
    ULONG MinorVersion;
    ULONG BuildNumber;
    union
    {
        ULONG PlatformId;
        ULONG QfeNumber;
    };
    union
    {
        WCHAR CSDVersion[128];
        WCHAR LayerAttrib[128];
    };
    USHORT ServicePackMajor;
    USHORT ServicePackMinor;
    USHORT SuiteMask;
    UCHAR ProductType;
    UCHAR Reserved;
    ULONG SuiteMaskEx;
    ULONG Reserved2;
    union
    {
        USHORT RawInput16;
        USHORT LayerNumber : 12;
        USHORT AttribSelector : 4;
    } Input;
    USHORT LayerCount;
    ULONG LayerFlags;
} RTL_OSVERSIONINFOEX3, *PRTL_OSVERSIONINFOEX3;

/* wdm.h */

NTSYSAPI
NTSTATUS
NTAPI
RtlGetVersion(
    _Out_
    _At_(lpVersionInformation->dwOSVersionInfoSize, _Pre_ _Valid_)
    _When_(lpVersionInformation->dwOSVersionInfoSize == sizeof(RTL_OSVERSIONINFOEXW),
           _At_((PRTL_OSVERSIONINFOEXW)lpVersionInformation, _Out_))
        PRTL_OSVERSIONINFOW lpVersionInformation);

_Must_inspect_result_
NTSYSAPI
NTSTATUS
NTAPI
RtlVerifyVersionInfo(
    _In_ PRTL_OSVERSIONINFOEXW VersionInfo,
    _In_ ULONG TypeMask,
    _In_ ULONGLONG  ConditionMask);

/* ntddk.h */

/**
 * The RtlGetEnabledExtendedFeatures routine returns a mask of extended processor features that are enabled by the system.
 *
 * \param FeatureMask A 64-bit feature mask. This parameter indicates a set of extended processor features for which the caller 
 * requests information about whether the features are enabled.
 * \return A 64-bitmask of enabled extended processor features. The routine calculates this mask as the intersection (bitwise AND) 
 * between all enabled features and the value of the FeatureMask parameter.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-rtlgetenabledextendedfeatures
 */
NTSYSAPI
ULONG64
NTAPI
RtlGetEnabledExtendedFeatures(
    _In_ ULONG64 FeatureMask);

#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)
NTSYSAPI
BOOLEAN
NTAPI
RtlGetNtProductType(
    _Out_ PNT_PRODUCT_TYPE NtProductType);
#endif

// private
NTSYSAPI
BOOLEAN
NTAPI
RtlGetProductInfo(
    _In_ ULONG OSMajorVersion,
    _In_ ULONG OSMinorVersion,
    _In_ ULONG SpMajorVersion,
    _In_ ULONG SpMinorVersion,
    _Out_ PULONG ReturnedProductType);

#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)
NTSYSAPI
ULONG
NTAPI
RtlGetSuiteMask(VOID);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)
_Must_inspect_result_
NTSYSAPI
BOOLEAN
NTAPI
RtlIsMultiSessionSku(VOID);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)
_Must_inspect_result_
NTSYSAPI
BOOLEAN
NTAPI
RtlIsMultiUsersInSessionSku(VOID);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN10_RS5)
_Must_inspect_result_
NTSYSAPI
NTSTATUS
NTAPI
RtlIsApiSetImplemented(
    _In_ PCSTR apiSetName);
#endif

/* phnt */

NTSYSAPI
VOID
NTAPI
RtlGetNtVersionNumbers(
    _Out_opt_ PULONG NtMajorVersion,
    _Out_opt_ PULONG NtMinorVersion,
    _Out_opt_ PULONG NtBuildNumber);

NTSYSAPI
ULONG
NTAPI
RtlGetNtGlobalFlags(VOID);

#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)
NTSYSAPI
BOOLEAN
NTAPI
RtlIsEnclaveFeaturePresent(
    _In_ ULONG FeatureMask);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN10)
// rev
/**
 * The RtlIsProcessorFeaturePresent routine determines whether the specified processor feature is supported by the current computer.
 *
 * \param ProcessorFeature The processor feature to be tested.
 * \return If the feature is supported, the return value is a nonzero value.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-isprocessorfeaturepresent
 */
NTSYSAPI
BOOLEAN
NTAPI
RtlIsProcessorFeaturePresent(
    _In_ ULONG ProcessorFeature);
#endif

// rev
/**
 * The RtlGetCurrentProcessorNumber routine retrieves the number of the processor the current thread was running 
 * on during the call to this function.
 *
 * \return The function returns the current processor number.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentprocessornumber
 */
NTSYSAPI
ULONG
NTAPI
RtlGetCurrentProcessorNumber(VOID);

// rev
/**
 * The RtlGetCurrentProcessorNumberEx routine retrieves the processor group and number of the logical processor 
 * in which the calling thread is running.
 *
 * \param ProcessorNumber A pointer to a PROCESSOR_NUMBER structure that receives the processor group and number 
 * of the logical processor the calling thread is running.
 * \return This function does not return a value.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentprocessornumberex
 */
NTSYSAPI
VOID
NTAPI
RtlGetCurrentProcessorNumberEx(
    _Out_ PPROCESSOR_NUMBER ProcessorNumber);

#if (NTDDI_VERSION >= NTDDI_WIN10_RS4)

NTSYSAPI
ULONG64
NTAPI
RtlGetEnabledExtendedAndSupervisorFeatures(
    _In_ ULONG64 FeatureMask);

_Ret_maybenull_
_Success_(return != NULL)
NTSYSAPI
PVOID
NTAPI
RtlLocateSupervisorFeature(
    _In_ PXSAVE_AREA_HEADER XStateHeader,
    _In_range_(XSTATE_AVX, MAXIMUM_XSTATE_FEATURES - 1) ULONG FeatureId,
    _Out_opt_ PULONG Length);

#endif

#define ELEVATION_FLAG_TOKEN_CHECKS 0x00000001
#define ELEVATION_FLAG_VIRTUALIZATION 0x00000002
#define ELEVATION_FLAG_SHORTCUT_REDIR 0x00000004
#define ELEVATION_FLAG_NO_SIGNATURE_CHECK 0x00000008

// private
typedef struct _RTL_ELEVATION_FLAGS
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG ElevationEnabled : 1;
            ULONG VirtualizationEnabled : 1;
            ULONG InstallerDetectEnabled : 1;
            ULONG AdminApprovalModeType : 2;
            ULONG ReservedBits : 27;
        };
    };
} RTL_ELEVATION_FLAGS, *PRTL_ELEVATION_FLAGS;

NTSYSAPI
NTSTATUS
NTAPI
RtlQueryElevationFlags(
    _Out_ PRTL_ELEVATION_FLAGS Flags);

EXTERN_C_END
