#pragma once

#include "../MinDef.h"

EXTERN_C_START

/* phnt */

typedef enum _WNF_STATE_NAME_LIFETIME
{
    WnfWellKnownStateName,
    WnfPermanentStateName,
    WnfPersistentStateName,
    WnfTemporaryStateName
} WNF_STATE_NAME_LIFETIME, *PWNF_STATE_NAME_LIFETIME;

typedef enum _WNF_STATE_NAME_INFORMATION
{
    WnfInfoStateNameExist,
    WnfInfoSubscribersPresent,
    WnfInfoIsQuiescent
} WNF_STATE_NAME_INFORMATION, *PWNF_STATE_NAME_INFORMATION;

typedef enum _WNF_DATA_SCOPE
{
    WnfDataScopeSystem,
    WnfDataScopeSession,
    WnfDataScopeUser,
    WnfDataScopeProcess,
    WnfDataScopeMachine, // REDSTONE3
    WnfDataScopePhysicalMachine, // WIN11
} WNF_DATA_SCOPE;

typedef struct _WNF_TYPE_ID
{
    GUID TypeId;
} WNF_TYPE_ID, *PWNF_TYPE_ID;

typedef const WNF_TYPE_ID *PCWNF_TYPE_ID;

typedef ULONG WNF_CHANGE_STAMP, *PWNF_CHANGE_STAMP;

typedef struct _WNF_DELIVERY_DESCRIPTOR
{
    ULONGLONG SubscriptionId;
    WNF_STATE_NAME StateName;
    WNF_CHANGE_STAMP ChangeStamp;
    ULONG StateDataSize;
    ULONG EventMask;
    WNF_TYPE_ID TypeId;
    ULONG StateDataOffset;
} WNF_DELIVERY_DESCRIPTOR, *PWNF_DELIVERY_DESCRIPTOR;

#if (NTDDI_VERSION >= NTDDI_WIN8)

/**
 * The NtCreateWnfStateName routine creates a new WNF (Windows Notification Facility) state name.
 *
 * \param StateName Pointer to a WNF_STATE_NAME structure that receives the created state name.
 * \param NameLifetime The lifetime of the state name (see WNF_STATE_NAME_LIFETIME).
 * \param DataScope The data scope for the state name (see WNF_DATA_SCOPE).
 * \param PersistData If TRUE, the state data is persistent.
 * \param TypeId Optional pointer to a WNF_TYPE_ID structure specifying the type of the state data.
 * \param MaximumStateSize The maximum size, in bytes, of the state data.
 * \param SecurityDescriptor Pointer to a security descriptor for the state name.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateWnfStateName(
    _Out_ PWNF_STATE_NAME StateName,
    _In_ WNF_STATE_NAME_LIFETIME NameLifetime,
    _In_ WNF_DATA_SCOPE DataScope,
    _In_ BOOLEAN PersistData,
    _In_opt_ PCWNF_TYPE_ID TypeId,
    _In_ ULONG MaximumStateSize,
    _In_ PSECURITY_DESCRIPTOR SecurityDescriptor);

/**
 * The NtDeleteWnfStateName routine deletes an existing WNF state name.
 *
 * \param StateName Pointer to the WNF_STATE_NAME to delete.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteWnfStateName(
    _In_ PCWNF_STATE_NAME StateName);

/**
 * The NtUpdateWnfStateData routine updates the data associated with a WNF state name.
 *
 * \param StateName Pointer to the WNF_STATE_NAME to update.
 * \param Buffer Pointer to the data buffer to write.
 * \param Length Length, in bytes, of the data buffer.
 * \param TypeId Optional pointer to a WNF_TYPE_ID structure specifying the type of the state data.
 * \param ExplicitScope Optional pointer to a security identifier (SID) for explicit scope.
 * \param MatchingChangeStamp The change stamp to match for update.
 * \param CheckStamp If TRUE, the change stamp is checked before updating.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtUpdateWnfStateData(
    _In_ PCWNF_STATE_NAME StateName,
    _In_reads_bytes_opt_(Length) const VOID* Buffer,
    _In_opt_ ULONG Length,
    _In_opt_ PCWNF_TYPE_ID TypeId,
    _In_opt_ PCSID ExplicitScope,
    _In_ WNF_CHANGE_STAMP MatchingChangeStamp,
    _In_ LOGICAL CheckStamp
    );

/**
 * The NtDeleteWnfStateData routine Deletes the data associated with a WNF state name.
 *
 * \param StateName Pointer to the WNF_STATE_NAME whose data is to be deleted.
 * \param ExplicitScope Optional pointer to a security identifier (SID) for explicit scope.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteWnfStateData(
    _In_ PCWNF_STATE_NAME StateName,
    _In_opt_ PCSID ExplicitScope
    );

/**
 * The NtQueryWnfStateData routine Queries the data associated with a WNF state name.
 *
 * \param StateName Pointer to the WNF_STATE_NAME to query.
 * \param TypeId Optional pointer to a WNF_TYPE_ID structure specifying the type of the state data.
 * \param ExplicitScope Optional pointer to a security identifier (SID) for explicit scope.
 * \param ChangeStamp Pointer to a variable that receives the change stamp.
 * \param Buffer Pointer to a buffer that receives the state data.
 * \param BufferLength On input, the size of the buffer in bytes; on output, the number of bytes written.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryWnfStateData(
    _In_ PCWNF_STATE_NAME StateName,
    _In_opt_ PCWNF_TYPE_ID TypeId,
    _In_opt_ PCSID ExplicitScope,
    _Out_ PWNF_CHANGE_STAMP ChangeStamp,
    _Out_writes_bytes_opt_(*BufferLength) PVOID Buffer,
    _Inout_ PULONG BufferLength
    );

/**
 * The NtQueryWnfStateNameInformation routine queries information about a WNF state name.
 *
 * \param StateName Pointer to the WNF_STATE_NAME to query.
 * \param NameInfoClass The information class to query (see WNF_STATE_NAME_INFORMATION).
 * \param ExplicitScope Optional pointer to a security identifier (SID) for explicit scope.
 * \param Buffer Pointer to a buffer that receives the requested information.
 * \param BufferLength The size, in bytes, of the buffer.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryWnfStateNameInformation(
    _In_ PCWNF_STATE_NAME StateName,
    _In_ WNF_STATE_NAME_INFORMATION NameInfoClass,
    _In_opt_ PCSID ExplicitScope,
    _Out_writes_bytes_(BufferLength) PVOID Buffer,
    _In_ ULONG BufferLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSubscribeWnfStateChange(
    _In_ PCWNF_STATE_NAME StateName,
    _In_opt_ WNF_CHANGE_STAMP ChangeStamp,
    _In_ ULONG EventMask,
    _Out_opt_ PULONG64 SubscriptionId);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnsubscribeWnfStateChange(
    _In_ PCWNF_STATE_NAME StateName);

#endif

#if (NTDDI_VERSION >= NTDDI_WIN10)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtGetCompleteWnfStateSubscription(
    _In_opt_ PWNF_STATE_NAME OldDescriptorStateName,
    _In_opt_ ULONG64 *OldSubscriptionId,
    _In_opt_ ULONG OldDescriptorEventMask,
    _In_opt_ ULONG OldDescriptorStatus,
    _Out_writes_bytes_(DescriptorSize) PWNF_DELIVERY_DESCRIPTOR NewDeliveryDescriptor,
    _In_ ULONG DescriptorSize);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetWnfProcessNotificationEvent(
    _In_ HANDLE NotificationEvent);

#endif

EXTERN_C_END
