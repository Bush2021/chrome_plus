#pragma once

#include "../MinDef.h"
#include "Base.h"

EXTERN_C_START

/* phnt */

typedef HANDLE ALPC_HANDLE, *PALPC_HANDLE;

#define ALPC_PORFLG_LPC_MODE 0x1000 // kernel only
#define ALPC_PORFLG_ALLOW_IMPERSONATION 0x10000
#define ALPC_PORFLG_ALLOW_LPC_REQUESTS 0x20000 // rev
#define ALPC_PORFLG_WAITABLE_PORT 0x40000 // dbg
#define ALPC_PORFLG_ALLOW_DUP_OBJECT 0x80000
#define ALPC_PORFLG_SYSTEM_PROCESS 0x100000 // dbg
#define ALPC_PORFLG_WAKE_POLICY1 0x200000
#define ALPC_PORFLG_WAKE_POLICY2 0x400000
#define ALPC_PORFLG_WAKE_POLICY3 0x800000
#define ALPC_PORFLG_DIRECT_MESSAGE 0x1000000
#define ALPC_PORFLG_ALLOW_MULTIHANDLE_ATTRIBUTE 0x2000000

#define ALPC_PORFLG_OBJECT_TYPE_FILE 0x0001
#define ALPC_PORFLG_OBJECT_TYPE_INVALID 0x0002
#define ALPC_PORFLG_OBJECT_TYPE_THREAD 0x0004
#define ALPC_PORFLG_OBJECT_TYPE_SEMAPHORE 0x0008
#define ALPC_PORFLG_OBJECT_TYPE_EVENT 0x0010
#define ALPC_PORFLG_OBJECT_TYPE_PROCESS 0X0020
#define ALPC_PORFLG_OBJECT_TYPE_MUTEX 0x0040
#define ALPC_PORFLG_OBJECT_TYPE_SECTION 0x0080
#define ALPC_PORFLG_OBJECT_TYPE_REGKEY 0x0100
#define ALPC_PORFLG_OBJECT_TYPE_TOKEN 0x0200
#define ALPC_PORFLG_OBJECT_TYPE_COMPOSITION 0x0400
#define ALPC_PORFLG_OBJECT_TYPE_JOB 0x0800
#define ALPC_PORFLG_OBJECT_TYPE_ALL \
    (ALPC_PORFLG_OBJECT_TYPE_FILE | ALPC_PORFLG_OBJECT_TYPE_THREAD | \
     ALPC_PORFLG_OBJECT_TYPE_SEMAPHORE | ALPC_PORFLG_OBJECT_TYPE_EVENT | \
     ALPC_PORFLG_OBJECT_TYPE_PROCESS | ALPC_PORFLG_OBJECT_TYPE_MUTEX | \
     ALPC_PORFLG_OBJECT_TYPE_SECTION | ALPC_PORFLG_OBJECT_TYPE_REGKEY | \
     ALPC_PORFLG_OBJECT_TYPE_TOKEN | ALPC_PORFLG_OBJECT_TYPE_COMPOSITION | \
     ALPC_PORFLG_OBJECT_TYPE_JOB)

typedef struct _ALPC_PORT_ATTRIBUTES
{
    ULONG Flags;
    SECURITY_QUALITY_OF_SERVICE SecurityQos;
    SIZE_T MaxMessageLength;
    SIZE_T MemoryBandwidth;
    SIZE_T MaxPoolUsage;
    SIZE_T MaxSectionSize;
    SIZE_T MaxViewSize;
    SIZE_T MaxTotalSectionSize;
    ULONG DupObjectTypes;
#ifdef _WIN64
    ULONG Reserved;
#endif
} ALPC_PORT_ATTRIBUTES, *PALPC_PORT_ATTRIBUTES;

#define ALPC_MESSAGE_HANDLE_ATTRIBUTE 0x10000000
#define ALPC_MESSAGE_CONTEXT_ATTRIBUTE 0x20000000
#define ALPC_MESSAGE_VIEW_ATTRIBUTE 0x40000000
#define ALPC_MESSAGE_SECURITY_ATTRIBUTE 0x80000000

typedef struct _ALPC_MESSAGE_ATTRIBUTES
{
    ULONG AllocatedAttributes;
    ULONG ValidAttributes;
} ALPC_MESSAGE_ATTRIBUTES, *PALPC_MESSAGE_ATTRIBUTES;

typedef struct _ALPC_COMPLETION_LIST_STATE
{
    union
    {
        struct
        {
            ULONG64 Head : 24;
            ULONG64 Tail : 24;
            ULONG64 ActiveThreadCount : 16;
        } s1;
        ULONG64 Value;
    } u1;
} ALPC_COMPLETION_LIST_STATE, *PALPC_COMPLETION_LIST_STATE;

#define ALPC_COMPLETION_LIST_BUFFER_GRANULARITY_MASK 0x3f // dbg

typedef struct DECLSPEC_ALIGN(128) _ALPC_COMPLETION_LIST_HEADER
{
    ULONG64 StartMagic;

    ULONG TotalSize;
    ULONG ListOffset;
    ULONG ListSize;
    ULONG BitmapOffset;
    ULONG BitmapSize;
    ULONG DataOffset;
    ULONG DataSize;
    ULONG AttributeFlags;
    ULONG AttributeSize;

    DECLSPEC_ALIGN(128) ALPC_COMPLETION_LIST_STATE State;
    ULONG LastMessageId;
    ULONG LastCallbackId;
    DECLSPEC_ALIGN(128) ULONG PostCount;
    DECLSPEC_ALIGN(128) ULONG ReturnCount;
    DECLSPEC_ALIGN(128) ULONG LogSequenceNumber;
    DECLSPEC_ALIGN(128) RTL_SRWLOCK UserLock;

    ULONG64 EndMagic;
} ALPC_COMPLETION_LIST_HEADER, *PALPC_COMPLETION_LIST_HEADER;

typedef struct _ALPC_CONTEXT_ATTR
{
    PVOID PortContext;
    PVOID MessageContext;
    ULONG Sequence;
    ULONG MessageId;
    ULONG CallbackId;
} ALPC_CONTEXT_ATTR, *PALPC_CONTEXT_ATTR;

#define ALPC_HANDLEFLG_DUPLICATE_SAME_ACCESS 0x10000
#define ALPC_HANDLEFLG_DUPLICATE_SAME_ATTRIBUTES 0x20000
#define ALPC_HANDLEFLG_DUPLICATE_INHERIT 0x80000

typedef struct _ALPC_HANDLE_ATTR32
{
    ULONG Flags;
    ULONG Reserved0;
    ULONG SameAccess;
    ULONG SameAttributes;
    ULONG Indirect;
    ULONG Inherit;
    ULONG Reserved1;
    ULONG Handle;
    ULONG ObjectType; // ObjectTypeCode, not ObjectTypeIndex
    ACCESS_MASK DesiredAccess;
    ACCESS_MASK GrantedAccess;
} ALPC_HANDLE_ATTR32, *PALPC_HANDLE_ATTR32;

typedef struct _ALPC_HANDLE_ATTR
{
    ULONG Flags;
    ULONG Reserved0;
    ULONG SameAccess;
    ULONG SameAttributes;
    ULONG Indirect;
    ULONG Inherit;
    ULONG Reserved1;
    HANDLE Handle;
    PALPC_HANDLE_ATTR32 HandleAttrArray;
    ULONG ObjectType; // ObjectTypeCode, not ObjectTypeIndex
    ULONG HandleCount;
    ACCESS_MASK DesiredAccess;
    ACCESS_MASK GrantedAccess;
} ALPC_HANDLE_ATTR, *PALPC_HANDLE_ATTR;

#define ALPC_SECFLG_CREATE_HANDLE 0x20000 // dbg
#define ALPC_SECFLG_NOSECTIONHANDLE 0x40000

typedef struct _ALPC_SECURITY_ATTR
{
    ULONG Flags;
    PSECURITY_QUALITY_OF_SERVICE QoS;
    ALPC_HANDLE ContextHandle; // dbg
} ALPC_SECURITY_ATTR, *PALPC_SECURITY_ATTR;

#define ALPC_VIEWFLG_UNMAP_EXISTING     0x10000
#define ALPC_VIEWFLG_AUTO_RELEASE       0x20000
#define ALPC_VIEWFLG_NOT_SECURE         0x40000

typedef struct _ALPC_DATA_VIEW_ATTR
{
    ULONG Flags;
    ALPC_HANDLE SectionHandle;
    PVOID ViewBase; // must be zero on input
    SIZE_T ViewSize;
} ALPC_DATA_VIEW_ATTR, *PALPC_DATA_VIEW_ATTR;

typedef enum _ALPC_PORT_INFORMATION_CLASS
{
    AlpcBasicInformation, // q: out ALPC_BASIC_INFORMATION
    AlpcPortInformation, // s: in ALPC_PORT_ATTRIBUTES
    AlpcAssociateCompletionPortInformation, // s: in ALPC_PORT_ASSOCIATE_COMPLETION_PORT
    AlpcConnectedSIDInformation, // q: in SID
    AlpcServerInformation, // q: inout ALPC_SERVER_INFORMATION
    AlpcMessageZoneInformation, // s: in ALPC_PORT_MESSAGE_ZONE_INFORMATION
    AlpcRegisterCompletionListInformation, // s: in ALPC_PORT_COMPLETION_LIST_INFORMATION
    AlpcUnregisterCompletionListInformation, // s: VOID
    AlpcAdjustCompletionListConcurrencyCountInformation, // s: in ULONG
    AlpcRegisterCallbackInformation, // s: ALPC_REGISTER_CALLBACK // kernel-mode only
    AlpcCompletionListRundownInformation, // s: VOID // 10
    AlpcWaitForPortReferences,
    AlpcServerSessionInformation // q: ALPC_SERVER_SESSION_INFORMATION // since 19H2
} ALPC_PORT_INFORMATION_CLASS;

typedef struct _ALPC_BASIC_INFORMATION
{
    ULONG Flags;
    ULONG SequenceNo;
    PVOID PortContext;
} ALPC_BASIC_INFORMATION, *PALPC_BASIC_INFORMATION;

typedef struct _ALPC_PORT_ASSOCIATE_COMPLETION_PORT
{
    PVOID CompletionKey;
    HANDLE CompletionPort;
} ALPC_PORT_ASSOCIATE_COMPLETION_PORT, *PALPC_PORT_ASSOCIATE_COMPLETION_PORT;

typedef struct _ALPC_SERVER_INFORMATION
{
    union
    {
        struct
        {
            HANDLE ThreadHandle;
        } In;
        struct
        {
            BOOLEAN ThreadBlocked;
            HANDLE ConnectedProcessId;
            UNICODE_STRING ConnectionPortName;
        } Out;
    };
} ALPC_SERVER_INFORMATION, *PALPC_SERVER_INFORMATION;

typedef struct _ALPC_PORT_MESSAGE_ZONE_INFORMATION
{
    PVOID Buffer;
    ULONG Size;
} ALPC_PORT_MESSAGE_ZONE_INFORMATION, *PALPC_PORT_MESSAGE_ZONE_INFORMATION;

typedef struct _ALPC_PORT_COMPLETION_LIST_INFORMATION
{
    PVOID Buffer; // PALPC_COMPLETION_LIST_HEADER
    ULONG Size;
    ULONG ConcurrencyCount;
    ULONG AttributeFlags;
} ALPC_PORT_COMPLETION_LIST_INFORMATION, *PALPC_PORT_COMPLETION_LIST_INFORMATION;

typedef struct _ALPC_REGISTER_CALLBACK
{
    PVOID CallbackObject; // PCALLBACK_OBJECT
    PVOID CallbackContext;
} ALPC_REGISTER_CALLBACK, *PALPC_REGISTER_CALLBACK;

typedef struct _ALPC_SERVER_SESSION_INFORMATION
{
    ULONG SessionId;
    ULONG ProcessId;
} ALPC_SERVER_SESSION_INFORMATION, *PALPC_SERVER_SESSION_INFORMATION;

typedef enum _ALPC_MESSAGE_INFORMATION_CLASS
{
    AlpcMessageSidInformation, // q: out SID
    AlpcMessageTokenModifiedIdInformation,  // q: out LUID
    AlpcMessageDirectStatusInformation,
    AlpcMessageHandleInformation, // ALPC_MESSAGE_HANDLE_INFORMATION
    MaxAlpcMessageInfoClass
} ALPC_MESSAGE_INFORMATION_CLASS, *PALPC_MESSAGE_INFORMATION_CLASS;

typedef struct _ALPC_MESSAGE_HANDLE_INFORMATION
{
    ULONG Index;
    ULONG Flags;
    ULONG Handle;
    ULONG ObjectType;
    ACCESS_MASK GrantedAccess;
} ALPC_MESSAGE_HANDLE_INFORMATION, *PALPC_MESSAGE_HANDLE_INFORMATION;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcCreatePort(
    _Out_ PHANDLE PortHandle,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PALPC_PORT_ATTRIBUTES PortAttributes);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcDisconnectPort(
    _In_ HANDLE PortHandle,
    _In_ ULONG Flags);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcQueryInformation(
    _In_opt_ HANDLE PortHandle,
    _In_ ALPC_PORT_INFORMATION_CLASS PortInformationClass,
    _Inout_updates_bytes_to_(Length, *ReturnLength) PVOID PortInformation,
    _In_ ULONG Length,
    _Out_opt_ PULONG ReturnLength);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcSetInformation(
    _In_ HANDLE PortHandle,
    _In_ ALPC_PORT_INFORMATION_CLASS PortInformationClass,
    _In_reads_bytes_opt_(Length) PVOID PortInformation,
    _In_ ULONG Length);

#define ALPC_CREATEPORTSECTIONFLG_SECURE 0x40000 // rev

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcCreatePortSection(
    _In_ HANDLE PortHandle,
    _In_ ULONG Flags,
    _In_opt_ HANDLE SectionHandle,
    _In_ SIZE_T SectionSize,
    _Out_ PALPC_HANDLE AlpcSectionHandle,
    _Out_ PSIZE_T ActualSectionSize);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcDeletePortSection(
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _In_ ALPC_HANDLE SectionHandle);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcCreateResourceReserve(
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _In_ SIZE_T MessageSize,
    _Out_ PULONG ResourceId);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcDeleteResourceReserve(
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _In_ PULONG ResourceId);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcCreateSectionView(
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _Inout_ PALPC_DATA_VIEW_ATTR ViewAttributes);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcDeleteSectionView(
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _In_ PVOID ViewBase);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcCreateSecurityContext(
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _Inout_ PALPC_SECURITY_ATTR SecurityAttribute);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcDeleteSecurityContext(
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _In_ ALPC_HANDLE ContextHandle);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcRevokeSecurityContext(
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _In_ ALPC_HANDLE ContextHandle);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcQueryInformationMessage(
    _In_ HANDLE PortHandle,
    _In_ PPORT_MESSAGE PortMessage,
    _In_ ALPC_MESSAGE_INFORMATION_CLASS MessageInformationClass,
    _Out_writes_bytes_to_opt_(Length, *ReturnLength) PVOID MessageInformation,
    _In_ ULONG Length,
    _Out_opt_ PULONG ReturnLength);

#define ALPC_MSGFLG_REPLY_MESSAGE 0x1
#define ALPC_MSGFLG_LPC_MODE 0x2
#define ALPC_MSGFLG_RELEASE_MESSAGE 0x10000 // dbg
#define ALPC_MSGFLG_SYNC_REQUEST 0x20000 // dbg
#define ALPC_MSGFLG_TRACK_PORT_REFERENCES 0x40000
#define ALPC_MSGFLG_WAIT_USER_MODE 0x100000
#define ALPC_MSGFLG_WAIT_ALERTABLE 0x200000
#define ALPC_MSGFLG_WOW64_CALL 0x80000000 // dbg

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcConnectPort(
    _Out_ PHANDLE PortHandle,
    _In_ PUNICODE_STRING PortName,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PALPC_PORT_ATTRIBUTES PortAttributes,
    _In_ ULONG Flags,
    _In_opt_ PSID RequiredServerSid,
    _Inout_updates_bytes_to_opt_(*BufferLength, *BufferLength) PPORT_MESSAGE ConnectionMessage,
    _Inout_opt_ PSIZE_T BufferLength,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES OutMessageAttributes,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES InMessageAttributes,
    _In_opt_ PLARGE_INTEGER Timeout);

#if (NTDDI_VERSION >= NTDDI_WIN8)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcConnectPortEx(
    _Out_ PHANDLE PortHandle,
    _In_ POBJECT_ATTRIBUTES ConnectionPortObjectAttributes,
    _In_opt_ POBJECT_ATTRIBUTES ClientPortObjectAttributes,
    _In_opt_ PALPC_PORT_ATTRIBUTES PortAttributes,
    _In_ ULONG Flags,
    _In_opt_ PSECURITY_DESCRIPTOR ServerSecurityRequirements,
    _Inout_updates_bytes_to_opt_(*BufferLength, *BufferLength) PPORT_MESSAGE ConnectionMessage,
    _Inout_opt_ PSIZE_T BufferLength,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES OutMessageAttributes,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES InMessageAttributes,
    _In_opt_ PLARGE_INTEGER Timeout);
#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcAcceptConnectPort(
    _Out_ PHANDLE PortHandle,
    _In_ HANDLE ConnectionPortHandle,
    _In_ ULONG Flags,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PALPC_PORT_ATTRIBUTES PortAttributes,
    _In_opt_ PVOID PortContext,
    _In_reads_bytes_(ConnectionRequest->u1.s1.TotalLength) PPORT_MESSAGE ConnectionRequest,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES ConnectionMessageAttributes,
    _In_ BOOLEAN AcceptConnection);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcSendWaitReceivePort(
    _In_ HANDLE PortHandle,
    _In_ ULONG Flags,
    _In_reads_bytes_opt_(SendMessage->u1.s1.TotalLength) PPORT_MESSAGE SendMessage,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES SendMessageAttributes,
    _Out_writes_bytes_to_opt_(*BufferLength, *BufferLength) PPORT_MESSAGE ReceiveMessage,
    _Inout_opt_ PSIZE_T BufferLength,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES ReceiveMessageAttributes,
    _In_opt_ PLARGE_INTEGER Timeout);

#define ALPC_CANCELFLG_TRY_CANCEL 0x1 // dbg
#define ALPC_CANCELFLG_NO_CONTEXT_CHECK 0x8
#define ALPC_CANCELFLGP_FLUSH 0x10000 // dbg

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcCancelMessage(
    _In_ HANDLE PortHandle,
    _In_ ULONG Flags,
    _In_ PALPC_CONTEXT_ATTR MessageContext);

#define ALPC_IMPERSONATEFLG_ANONYMOUS 0x1
#define ALPC_IMPERSONATEFLG_REQUIRE_IMPERSONATE 0x2
//ALPC_IMPERSONATEFLG 0x3-0x10 (SECURITY_IMPERSONATION_LEVEL)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcImpersonateClientOfPort(
    _In_ HANDLE PortHandle,
    _In_ PPORT_MESSAGE Message,
    _In_ PVOID Flags);

#if (NTDDI_VERSION >= NTDDI_WIN10)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcImpersonateClientContainerOfPort(
    _In_ HANDLE PortHandle,
    _In_ PPORT_MESSAGE Message,
    _Reserved_ ULONG Flags);
#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcOpenSenderProcess(
    _Out_ PHANDLE ProcessHandle,
    _In_ HANDLE PortHandle,
    _In_ PPORT_MESSAGE PortMessage,
    _Reserved_ ULONG Flags,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlpcOpenSenderThread(
    _Out_ PHANDLE ThreadHandle,
    _In_ HANDLE PortHandle,
    _In_ PPORT_MESSAGE PortMessage,
    _Reserved_ ULONG Flags,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes);

#pragma region Support API in Ntdll.dll

NTSYSAPI
ULONG
NTAPI
AlpcMaxAllowedMessageLength(VOID);

#define ALPC_ATTRFLG_ALLOCATEDATTR 0x20000000
#define ALPC_ATTRFLG_VALIDATTR 0x40000000
#define ALPC_ATTRFLG_KEEPRUNNINGATTR 0x60000000

NTSYSAPI
ULONG
NTAPI
AlpcGetHeaderSize(
    _In_ ULONG Flags);

NTSYSAPI
NTSTATUS
NTAPI
AlpcInitializeMessageAttribute(
    _In_ ULONG AttributeFlags,
    _Out_opt_ PALPC_MESSAGE_ATTRIBUTES Buffer,
    _In_ SIZE_T BufferSize,
    _Out_ PSIZE_T RequiredBufferSize);

NTSYSAPI
PVOID
NTAPI
AlpcGetMessageAttribute(
    _In_ PALPC_MESSAGE_ATTRIBUTES Buffer,
    _In_ ULONG AttributeFlag);

NTSYSAPI
NTSTATUS
NTAPI
AlpcRegisterCompletionList(
    _In_ HANDLE PortHandle,
    _Out_ PALPC_COMPLETION_LIST_HEADER Buffer,
    _In_ ULONG Size,
    _In_ ULONG ConcurrencyCount,
    _In_ ULONG AttributeFlags);

NTSYSAPI
NTSTATUS
NTAPI
AlpcUnregisterCompletionList(
    _In_ HANDLE PortHandle);

NTSYSAPI
NTSTATUS
NTAPI
AlpcRundownCompletionList(
    _In_ HANDLE PortHandle);

NTSYSAPI
NTSTATUS
NTAPI
AlpcAdjustCompletionListConcurrencyCount(
    _In_ HANDLE PortHandle,
    _In_ ULONG ConcurrencyCount);

NTSYSAPI
BOOLEAN
NTAPI
AlpcRegisterCompletionListWorkerThread(
    _Inout_ PVOID CompletionList);

NTSYSAPI
BOOLEAN
NTAPI
AlpcUnregisterCompletionListWorkerThread(
    _Inout_ PVOID CompletionList);

NTSYSAPI
VOID
NTAPI
AlpcGetCompletionListLastMessageInformation(
    _In_ PVOID CompletionList,
    _Out_ PULONG LastMessageId,
    _Out_ PULONG LastCallbackId);

NTSYSAPI
ULONG
NTAPI
AlpcGetOutstandingCompletionListMessageCount(
    _In_ PVOID CompletionList);

NTSYSAPI
PPORT_MESSAGE
NTAPI
AlpcGetMessageFromCompletionList(
    _In_ PVOID CompletionList,
    _Out_opt_ PALPC_MESSAGE_ATTRIBUTES *MessageAttributes);

NTSYSAPI
VOID
NTAPI
AlpcFreeCompletionListMessage(
    _Inout_ PVOID CompletionList,
    _In_ PPORT_MESSAGE Message);

NTSYSAPI
PALPC_MESSAGE_ATTRIBUTES
NTAPI
AlpcGetCompletionListMessageAttributes(
    _In_ PVOID CompletionList,
    _In_ PPORT_MESSAGE Message);

#pragma endregion

EXTERN_C_END
