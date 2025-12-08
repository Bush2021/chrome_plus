#pragma once

#include "../MinDef.h"
#include "../Ps/PsApi.h"

EXTERN_C_START

/* phnt */

#ifdef _WIN64

// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlWow64GetThreadContext(
    _In_ HANDLE ThreadHandle,
    _Inout_ PWOW64_CONTEXT ThreadContext
);

// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlWow64SetThreadContext(
    _In_ HANDLE ThreadHandle,
    _In_ PWOW64_CONTEXT ThreadContext
);

#endif

#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)
// rev from Wow64DetermineEnvironment
NTSYSAPI
USHORT
NTAPI
RtlWow64GetCurrentMachine(
    VOID
);

// rev from Wow64DetermineEnvironment
NTSYSAPI
NTSTATUS
NTAPI
RtlWow64IsWowGuestMachineSupported(
    _In_ USHORT NativeMachine,
    _Out_ PBOOLEAN IsWowGuestMachineSupported
);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN10_NI)
// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlWow64GetProcessMachines(
    _In_ HANDLE ProcessHandle,
    _Out_ PUSHORT ProcessMachine,
    _Out_ PUSHORT NativeMachine
);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)

// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlWow64ChangeProcessState(
    _In_ HANDLE ProcessStateChangeHandle,
    _In_ HANDLE ProcessHandle,
    _In_ PROCESS_STATE_CHANGE_TYPE StateChangeType
);

// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlWow64ChangeThreadState(
    _In_ HANDLE ThreadStateChangeHandle,
    _In_ HANDLE ThreadHandle,
    _In_ THREAD_STATE_CHANGE_TYPE StateChangeType
);

#endif

// WOW64

NTSYSAPI
NTSTATUS
NTAPI
RtlGetNativeSystemInformation(
    _In_ SYSTEM_INFORMATION_CLASS SystemInformationClass,
    _In_ PVOID NativeSystemInformation,
    _In_ ULONG InformationLength,
    _Out_opt_ PULONG ReturnLength
);

NTSYSAPI
NTSTATUS
NTAPI
RtlQueueApcWow64Thread(
    _In_ HANDLE ThreadHandle,
    _In_ PPS_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcArgument1,
    _In_opt_ PVOID ApcArgument2,
    _In_opt_ PVOID ApcArgument3
);

/**
 * The RtlWow64EnableFsRedirection routine enables or disables file system redirection for the calling thread.
 *
 * \param Wow64FsEnableRedirection If TRUE, requests redirection be enabled; if FALSE, requests redirection be disabled.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/wow64apiset/nf-wow64apiset-wow64enablewow64fsredirection
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlWow64EnableFsRedirection(
    _In_ BOOLEAN Wow64FsEnableRedirection
);

/**
 * The RtlWow64EnableFsRedirectionEx routine enables or disables file system redirection for the calling thread.
 *
 * \param Wow64FsEnableRedirection If TRUE, requests redirection be enabled; if FALSE, requests redirection be disabled.
 * \param OldFsRedirectionLevel The WOW64 file system redirection value. The system uses this parameter to store information
 *  necessary to revert (re-enable) file system redirection.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/wow64apiset/nf-wow64apiset-wow64disablewow64fsredirection
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlWow64EnableFsRedirectionEx(
    _In_ PVOID Wow64FsEnableRedirection,
    _Out_ PVOID* OldFsRedirectionLevel
);

EXTERN_C_END
