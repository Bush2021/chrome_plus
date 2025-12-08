#pragma once

#include "../../../NDK.h"

EXTERN_C_START

__inline
NTSTATUS
NTAPI
_Inline_RtlImpersonateSelfEx(
    _In_ SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
    _In_opt_ ACCESS_MASK AdditionalAccess,
    _Out_opt_ PHANDLE ThreadToken)
{
    NTSTATUS Status;
    HANDLE TokenHandle;
    HANDLE NewTokenHandle;
    OBJECT_ATTRIBUTES ObjectAttributes;
    SECURITY_QUALITY_OF_SERVICE Sqos;

    if (ThreadToken == NULL && AdditionalAccess != 0)
    {
        return STATUS_INVALID_PARAMETER_2;
    }
    Status = NtOpenProcessTokenEx(NtCurrentProcess(), TOKEN_DUPLICATE, OBJ_KERNEL_HANDLE, &TokenHandle);
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    Sqos.Length = sizeof(SECURITY_QUALITY_OF_SERVICE);
    Sqos.ImpersonationLevel = ImpersonationLevel;
    Sqos.ContextTrackingMode = SECURITY_DYNAMIC_TRACKING;
    Sqos.EffectiveOnly = FALSE;
    InitializeObjectAttributes(&ObjectAttributes, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);
    ObjectAttributes.SecurityQualityOfService = &Sqos;
    Status = NtDuplicateToken(TokenHandle,
                              AdditionalAccess | TOKEN_IMPERSONATE,
                              &ObjectAttributes,
                              FALSE,
                              TokenImpersonation,
                              &NewTokenHandle);
    if (NT_SUCCESS(Status))
    {
        Status = NtSetInformationThread(NtCurrentThread(),
                                        ThreadImpersonationToken,
                                        &NewTokenHandle,
                                        sizeof(NewTokenHandle));
        if (NT_SUCCESS(Status) && ThreadToken != NULL)
        {
            *ThreadToken = NewTokenHandle;
        } else
        {
            NtClose(NewTokenHandle);
        }
    }
    NtClose(TokenHandle);
    return Status;
}

__inline
NTSTATUS
NTAPI
_Inline_RtlImpersonateSelf(
    _In_ SECURITY_IMPERSONATION_LEVEL ImpersonationLevel)
{
    return _Inline_RtlImpersonateSelfEx(ImpersonationLevel, 0, NULL);
}

__inline
NTSTATUS
NTAPI
_Inline_RtlAdjustPrivilege(
    _In_ ULONG Privilege,
    _In_ BOOLEAN Enable,
    _In_ BOOLEAN Client,
    _Out_ PBOOLEAN WasEnabled)
{
    NTSTATUS Status;
    HANDLE TokenHandle;
    ULONG ReturnLength;
    TOKEN_PRIVILEGES NewState, PreviousState;

    if (Client)
    {
        Status = NtOpenThreadToken(NtCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &TokenHandle);
    } else
    {
        Status = NtOpenProcessToken(NtCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &TokenHandle);
    }
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    NewState.PrivilegeCount = 1;
    NewState.Privileges[0].Luid = RtlConvertUlongToLuid(Privilege);
    NewState.Privileges[0].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;
    Status = NtAdjustPrivilegesToken(TokenHandle, FALSE, &NewState, sizeof(NewState), &PreviousState, &ReturnLength);
    NtClose(TokenHandle);

    if (Status == STATUS_NOT_ALL_ASSIGNED)
    {
        return STATUS_PRIVILEGE_NOT_HELD;
    }
    if (NT_SUCCESS(Status))
    {
        if (PreviousState.PrivilegeCount != 0)
        {
            *WasEnabled = BooleanFlagOn(PreviousState.Privileges[0].Attributes, SE_PRIVILEGE_ENABLED);
        } else
        {
            *WasEnabled = Enable;
        }
    }
    return Status;
}

EXTERN_C_END
