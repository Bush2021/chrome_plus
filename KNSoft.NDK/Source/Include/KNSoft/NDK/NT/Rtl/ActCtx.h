#pragma once

#include "../MinDef.h"
#include "../Sxs.h"
#include "Process/EnvironmentBlock.h"

#define _USE_FULL_PROC_THREAD_ATTRIBUTE
#include <WinBase.h>

EXTERN_C_START

/* phnt */

#define INVALID_ACTIVATION_CONTEXT ((HANDLE)(LONG_PTR)-1)
#define ACTCTX_PROCESS_DEFAULT ((HANDLE)(LONG_PTR)0)
#define ACTCTX_EMPTY ((HANDLE)(LONG_PTR)-3)
#define ACTCTX_SYSTEM_DEFAULT ((HANDLE)(LONG_PTR)-4)
#define IS_SPECIAL_ACTCTX(x) (((((LONG_PTR)(x)) - 1) | 7) == -1)

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlGetActiveActivationContext(
    _Out_ PACTIVATION_CONTEXT ActivationContext
    );

// private
NTSYSAPI
VOID
NTAPI
RtlAddRefActivationContext(
    _In_ PACTIVATION_CONTEXT ActivationContext
    );

// private
NTSYSAPI
VOID
NTAPI
RtlReleaseActivationContext(
    _In_ PACTIVATION_CONTEXT ActivationContext
    );

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlZombifyActivationContext(
    _In_ PACTIVATION_CONTEXT ActivationContext
    );

// private
NTSYSAPI
BOOLEAN
NTAPI
RtlIsActivationContextActive(
    _In_ PACTIVATION_CONTEXT ActivationContext
    );

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlActivateActivationContext(
    _Reserved_ ULONG Flags,
    _In_ PACTIVATION_CONTEXT ActivationContext,
    _Out_ PULONG_PTR Cookie
    );

#define RTL_ACTIVATE_ACTIVATION_CONTEXT_EX_FLAG_RELEASE_ON_STACK_DEALLOCATION 0x00000001

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlActivateActivationContextEx(
    _In_ ULONG Flags,
    _In_ PTEB Teb,
    _In_ PACTIVATION_CONTEXT ActivationContext,
    _Out_ PULONG_PTR Cookie
    );

#define RTL_DEACTIVATE_ACTIVATION_CONTEXT_FLAG_FORCE_EARLY_DEACTIVATION 0x00000001

// private
NTSYSAPI
VOID
NTAPI
RtlDeactivateActivationContext(
    _In_ ULONG Flags,
    _In_ ULONG_PTR Cookie
    );

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlCreateActivationContext(
    _Reserved_ ULONG Flags,
    _In_ PACTIVATION_CONTEXT_DATA ActivationContextData,
    _In_opt_ ULONG ExtraBytes,
    _In_opt_ PACTIVATION_CONTEXT_NOTIFY_ROUTINE NotificationRoutine,
    _In_opt_ PVOID NotificationContext,
    _Out_ PACTIVATION_CONTEXT *ActivationContext
    );

#define FIND_ACTIVATION_CONTEXT_SECTION_KEY_RETURN_ACTIVATION_CONTEXT 0x00000001
#define FIND_ACTIVATION_CONTEXT_SECTION_KEY_RETURN_FLAGS 0x00000002
#define FIND_ACTIVATION_CONTEXT_SECTION_KEY_RETURN_ASSEMBLY_METADATA 0x00000004

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlFindActivationContextSectionString(
    _In_ ULONG Flags,
    _In_opt_ PGUID ExtensionGuid,
    _In_ ULONG SectionId, // ACTIVATION_CONTEXT_SECTION_*
    _In_ PUNICODE_STRING StringToFind,
    _Inout_ PACTCTX_SECTION_KEYED_DATA ReturnedData
    );

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlFindActivationContextSectionGuid(
    _In_ ULONG Flags,
    _In_opt_ PGUID ExtensionGuid,
    _In_ ULONG SectionId, // ACTIVATION_CONTEXT_SECTION_*
    _In_ PGUID GuidToFind,
    _Inout_ PACTCTX_SECTION_KEYED_DATA ReturnedData
    );

// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlQueryActivationContextApplicationSettings(
    _Reserved_ ULONG Flags,
    _In_ PACTIVATION_CONTEXT ActivationContext,
    _In_ PCWSTR SettingsNameSpace,
    _In_ PCWSTR SettingName,
    _Out_writes_bytes_(BufferLength) PWSTR Buffer,
    _In_ SIZE_T BufferLength,
    _Out_opt_ PSIZE_T RequiredLength
    );

// ACTIVATION_CONTEXT_INFO_CLASS
//   ActivationContextBasicInformation                      // q: ACTIVATION_CONTEXT_BASIC_INFORMATION
//   ActivationContextDetailedInformation                   // q: ACTIVATION_CONTEXT_DETAILED_INFORMATION
//   AssemblyDetailedInformationInActivationContext         // q: ACTIVATION_CONTEXT_ASSEMBLY_DETAILED_INFORMATION
//   FileInformationInAssemblyOfAssemblyInActivationContext // q: ASSEMBLY_FILE_DETAILED_INFORMATION
//   RunlevelInformationInActivationContext                 // q: ACTIVATION_CONTEXT_RUN_LEVEL_INFORMATION
//   CompatibilityInformationInActivationContext            // q: ACTIVATION_CONTEXT_COMPATIBILITY_INFORMATION[_LEGACY]
//   ActivationContextManifestResourceName                  // q: ULONG

#define RTL_QUERY_INFORMATION_ACTIVATION_CONTEXT_FLAG_USE_ACTIVE_ACTIVATION_CONTEXT 0x00000001
#define RTL_QUERY_INFORMATION_ACTIVATION_CONTEXT_FLAG_ACTIVATION_CONTEXT_IS_MODULE 0x00000002
#define RTL_QUERY_INFORMATION_ACTIVATION_CONTEXT_FLAG_ACTIVATION_CONTEXT_IS_ADDRESS 0x00000004
#define RTL_QUERY_INFORMATION_ACTIVATION_CONTEXT_FLAG_NO_ADDREF 0x80000000

// private
NTSYSAPI
NTSTATUS
NTAPI
RtlQueryInformationActivationContext(
    _In_ ULONG Flags,
    _In_opt_ PACTIVATION_CONTEXT ActivationContext,
    _In_opt_ PACTIVATION_CONTEXT_QUERY_INDEX SubInstanceIndex,
    _In_ ACTIVATION_CONTEXT_INFO_CLASS ActivationContextInformationClass,
    _Out_writes_bytes_(ActivationContextInformationLength) PVOID ActivationContextInformation,
    _In_ SIZE_T ActivationContextInformationLength,
    _Out_opt_ PSIZE_T ReturnLength
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlQueryInformationActiveActivationContext(
    _In_ ACTIVATION_CONTEXT_INFO_CLASS ActivationContextInformationClass,
    _Out_writes_bytes_(ActivationContextInformationLength) PVOID ActivationContextInformation,
    _In_ SIZE_T ActivationContextInformationLength,
    _Out_opt_ PSIZE_T ReturnLength
    );

EXTERN_C_END
