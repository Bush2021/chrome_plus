#pragma once

#include "../NDK.h"

EXTERN_C_START

__inline
NTSTATUS
NTAPI
_Inline_DbgQueryDebugFilterState(
    _In_ ULONG ComponentId,
    _In_ ULONG Level)
{
    return NtQueryDebugFilterState(ComponentId, Level);
}

__inline
NTSTATUS
NTAPI
_Inline_DbgSetDebugFilterState(
    _In_ ULONG ComponentId,
    _In_ ULONG Level,
    _In_ BOOLEAN State)
{
    return NtSetDebugFilterState(ComponentId, Level, State);
}

__inline
NTSTATUS
NTAPI
_Inline_DbgUiConnectToDbg(VOID)
{
    OBJECT_ATTRIBUTES ObjectAttributes;

    if (NtReadTebPVOID(DbgSsReserved[1]))
    {
        return STATUS_SUCCESS;
    }

    InitializeObjectAttributes(&ObjectAttributes, NULL, 0, NULL, 0);
    return NtCreateDebugObject(&NtCurrentTeb()->DbgSsReserved[1],
                               DEBUG_ALL_ACCESS,
                               &ObjectAttributes,
                               DEBUG_KILL_ON_CLOSE);
}

__inline
HANDLE
NTAPI
_Inline_DbgUiGetThreadDebugObject(VOID)
{
    return NtReadTebPVOID(DbgSsReserved[1]);
}

__inline
VOID
NTAPI
_Inline_DbgUiSetThreadDebugObject(
    _In_ HANDLE DebugObject)
{
    NtWriteTeb(DbgSsReserved[1], DebugObject);
}

__inline
NTSTATUS
NTAPI
_Inline_DbgUiWaitStateChange(
    _Out_ PDBGUI_WAIT_STATE_CHANGE StateChange,
    _In_opt_ PLARGE_INTEGER Timeout)
{
    return NtWaitForDebugEvent(NtReadTebPVOID(DbgSsReserved[1]), TRUE, Timeout, StateChange);
}

__inline
NTSTATUS
NTAPI
_Inline_DbgUiContinue(
    _In_ PCLIENT_ID AppClientId,
    _In_ NTSTATUS ContinueStatus)
{
    return NtDebugContinue(NtReadTebPVOID(DbgSsReserved[1]), AppClientId, ContinueStatus);
}

__inline
NTSTATUS
NTAPI
_Inline_DbgUiStopDebugging(
    _In_ HANDLE Process)
{
    return NtRemoveProcessDebug(Process, NtReadTebPVOID(DbgSsReserved[1]));
}

__inline
NTSTATUS
NTAPI
_Inline_DbgUiDebugActiveProcess(
    _In_ HANDLE Process)
{
    NTSTATUS Status;

    Status = NtDebugActiveProcess(Process, NtReadTebPVOID(DbgSsReserved[1]));
    if (NT_SUCCESS(Status))
    {
        Status = DbgUiIssueRemoteBreakin(Process);
        if (!NT_SUCCESS(Status))
        {
            NtRemoveProcessDebug(Process, NtReadTebPVOID(DbgSsReserved[1]));
        }
    }
    return Status;
}

EXTERN_C_END
