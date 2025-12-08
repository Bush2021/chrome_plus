#pragma once

#include "../MinDef.h"

EXTERN_C_START

#pragma region Driver

/**
 * The NtLoadDriver function loads a driver specified by the DriverServiceName parameter.
 * 
 * @param DriverServiceName A pointer to a UNICODE_STRING structure that specifies the name of the driver service to load.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtLoadDriver(
    _In_ PUNICODE_STRING DriverServiceName);

/**
 * The NtUnloadDriver function unloads a driver specified by the DriverServiceName parameter.
 * 
 * @param DriverServiceName A pointer to a UNICODE_STRING structure that specifies the name of the driver service to unload.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnloadDriver(
    _In_ PUNICODE_STRING DriverServiceName);

#pragma endregion ntifs.h

#pragma region Session

typedef enum _IO_SESSION_EVENT
{
    IoSessionEventIgnore,
    IoSessionEventCreated,
    IoSessionEventTerminated,
    IoSessionEventConnected,
    IoSessionEventDisconnected,
    IoSessionEventLogon,
    IoSessionEventLogoff,
    IoSessionEventMax
} IO_SESSION_EVENT;

typedef enum _IO_SESSION_STATE
{
    IoSessionStateCreated = 1,
    IoSessionStateInitialized = 2,
    IoSessionStateConnected = 3,
    IoSessionStateDisconnected = 4,
    IoSessionStateDisconnectedLoggedOn = 5,
    IoSessionStateLoggedOn = 6,
    IoSessionStateLoggedOff = 7,
    IoSessionStateTerminated = 8,
    IoSessionStateMax
} IO_SESSION_STATE;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenSession(
    _Out_ PHANDLE SessionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtNotifyChangeSession(
    _In_ HANDLE SessionHandle,
    _In_ ULONG ChangeSequenceNumber,
    _In_ PLARGE_INTEGER ChangeTimeStamp,
    _In_ IO_SESSION_EVENT Event,
    _In_ IO_SESSION_STATE NewState,
    _In_ IO_SESSION_STATE PreviousState,
    _In_reads_bytes_opt_(PayloadSize) PVOID Payload,
    _In_ ULONG PayloadSize);

#pragma endregion

#pragma region I/O Ring

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateIoRing(
    _Out_ PHANDLE IoRingHandle,
    _In_ ULONG CreateParametersLength,
    _In_ PVOID CreateParameters,
    _In_ ULONG OutputParametersLength,
    _Out_ PVOID OutputParameters);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSubmitIoRing(
    _In_ HANDLE IoRingHandle,
    _In_ ULONG Flags,
    _In_opt_ ULONG WaitOperations,
    _In_opt_ PLARGE_INTEGER Timeout);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryIoRingCapabilities(
    _In_ SIZE_T IoRingCapabilitiesLength,
    _Out_ PVOID IoRingCapabilities);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationIoRing(
    _In_ HANDLE IoRingHandle,
    _In_ ULONG IoRingInformationClass,
    _In_ ULONG IoRingInformationLength,
    _In_ PVOID IoRingInformation);

#endif

#pragma endregion

#pragma region Reparse

// Reparse structure for FSCTL_SET_REPARSE_POINT, FSCTL_GET_REPARSE_POINT, FSCTL_DELETE_REPARSE_POINT

#define SYMLINK_FLAG_RELATIVE 0x00000001

#if (NTDDI_VERSION >= NTDDI_WIN10_RS4)
#define SYMLINK_DIRECTORY 0x80000000 // If set then this is a directory symlink
#define SYMLINK_FILE 0x40000000 // If set then this is a file symlink
#endif

typedef struct _REPARSE_DATA_BUFFER
{
    ULONG ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;

    _Field_size_bytes_(ReparseDataLength)
        union
    {
        struct
        {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            ULONG Flags;
            WCHAR PathBuffer[1];
        } SymbolicLinkReparseBuffer;
        struct
        {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR PathBuffer[1];
        } MountPointReparseBuffer;
        struct
        {
            ULONG StringCount;
            WCHAR StringList[1];
        } AppExecLinkReparseBuffer;
        struct
        {
            UCHAR DataBuffer[1];
        } GenericReparseBuffer;
    };
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;

#define REPARSE_DATA_BUFFER_HEADER_SIZE UFIELD_OFFSET(REPARSE_DATA_BUFFER, GenericReparseBuffer)

#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)
// Reparse structure for FSCTL_SET_REPARSE_POINT_EX

typedef struct _REPARSE_DATA_BUFFER_EX
{
    ULONG Flags;

    //
    //  This is the existing reparse tag on the file if any,  if the
    //  caller wants to replace the reparse tag too.
    //
    //    - To set the reparse data  along with the reparse tag that
    //      could be different,  pass the current reparse tag of the
    //      file.
    //
    //    - To update the reparse data while having the same reparse
    //      tag,  the caller should give the existing reparse tag in
    //      this ExistingReparseTag field.
    //
    //    - To set the reparse tag along with reparse data on a file
    //      that doesn't have a reparse tag yet, set this to zero.
    //
    //  If the ExistingReparseTag  does not match the reparse tag on
    //  the file,  the FSCTL_SET_REPARSE_POINT_EX  would  fail  with
    //  STATUS_IO_REPARSE_TAG_MISMATCH. NOTE: If a file doesn't have
    //  a reparse tag, ExistingReparseTag should be 0.
    //

    ULONG ExistingReparseTag;

    //  For non-Microsoft reparse tags, this is the existing reparse
    //  guid on the file if any,  if the caller wants to replace the
    //  reparse tag and / or guid along with the data.
    //
    //  If ExistingReparseTag is 0, the file is not expected to have
    //  any reparse tags, so ExistingReparseGuid is ignored. And for
    //  non-Microsoft tags ExistingReparseGuid should match the guid
    //  in the file if ExistingReparseTag is non zero.

    GUID ExistingReparseGuid;

    //
    //  Reserved
    //
    ULONGLONG Reserved;

    //
    //  Reparse data to set
    //
    union
    {
        REPARSE_DATA_BUFFER ReparseDataBuffer;
        REPARSE_GUID_DATA_BUFFER ReparseGuidDataBuffer;
    };
} REPARSE_DATA_BUFFER_EX, *PREPARSE_DATA_BUFFER_EX;

//  REPARSE_DATA_BUFFER_EX Flags
//
//  REPARSE_DATA_EX_FLAG_GIVEN_TAG_OR_NONE - Forces the FSCTL to set the
//  reparse tag if the file has no tag or the tag on the file is same as
//  the one in  ExistingReparseTag.   NOTE: If the ExistingReparseTag is
//  not a Microsoft tag then the ExistingReparseGuid should match if the
//  file has the ExistingReparseTag.
//
#define REPARSE_DATA_EX_FLAG_GIVEN_TAG_OR_NONE              (0x00000001)

#define REPARSE_GUID_DATA_BUFFER_EX_HEADER_SIZE \
    UFIELD_OFFSET(REPARSE_DATA_BUFFER_EX, ReparseGuidDataBuffer.GenericReparseBuffer)

#define REPARSE_DATA_BUFFER_EX_HEADER_SIZE \
    UFIELD_OFFSET(REPARSE_DATA_BUFFER_EX, ReparseDataBuffer.GenericReparseBuffer)

#endif // NTDDI_WIN10_RS1

#pragma endregion

typedef enum _INTERFACE_TYPE
{
    InterfaceTypeUndefined = -1,
    Internal = 0,
    Isa = 1,
    Eisa = 2,
    MicroChannel = 3,
    TurboChannel = 4,
    PCIBus = 5,
    VMEBus = 6,
    NuBus = 7,
    PCMCIABus = 8,
    CBus = 9,
    MPIBus = 10,
    MPSABus = 11,
    ProcessorInternal = 12,
    InternalPowerBus = 13,
    PNPISABus = 14,
    PNPBus = 15,
    Vmcs = 16,
    ACPIBus = 17,
    MaximumInterfaceType
} INTERFACE_TYPE, *PINTERFACE_TYPE;

typedef enum _DMA_WIDTH
{
    Width8Bits,
    Width16Bits,
    Width32Bits,
    Width64Bits,
    WidthNoWrap,
    MaximumDmaWidth
} DMA_WIDTH, *PDMA_WIDTH;

typedef enum _DMA_SPEED
{
    Compatible,
    TypeA,
    TypeB,
    TypeC,
    TypeF,
    MaximumDmaSpeed
} DMA_SPEED, *PDMA_SPEED;

typedef enum _BUS_DATA_TYPE
{
    ConfigurationSpaceUndefined = -1,
    Cmos,
    EisaConfiguration,
    Pos,
    CbusConfiguration,
    PCIConfiguration,
    VMEConfiguration,
    NuBusConfiguration,
    PCMCIAConfiguration,
    MPIConfiguration,
    MPSAConfiguration,
    PNPISAConfiguration,
    SgiInternalConfiguration,
    MaximumBusDataType
} BUS_DATA_TYPE, *PBUS_DATA_TYPE;

// Filter manager

// rev
#define FLT_SYMLINK_NAME     L"\\Global??\\FltMgr"
#define FLT_MSG_SYMLINK_NAME L"\\Global??\\FltMgrMsg"
#define FLT_DEVICE_NAME      L"\\FileSystem\\Filters\\FltMgr"
#define FLT_MSG_DEVICE_NAME  L"\\FileSystem\\Filters\\FltMgrMsg"

// private
typedef struct _FLT_CONNECT_CONTEXT
{
    PUNICODE_STRING PortName;
    PUNICODE_STRING64 PortName64;
    USHORT SizeOfContext;
    UCHAR Padding[6]; // unused
    _Field_size_bytes_(SizeOfContext) UCHAR Context[ANYSIZE_ARRAY];
} FLT_CONNECT_CONTEXT, *PFLT_CONNECT_CONTEXT;

// rev
#define FLT_PORT_EA_NAME "FLTPORT"
#define FLT_PORT_CONTEXT_MAX 0xFFE8

// combined FILE_FULL_EA_INFORMATION and FLT_CONNECT_CONTEXT
typedef struct _FLT_PORT_FULL_EA
{
    ULONG NextEntryOffset; // 0
    UCHAR Flags;           // 0
    UCHAR EaNameLength;    // sizeof(FLT_PORT_EA_NAME) - sizeof(ANSI_NULL)
    USHORT EaValueLength;  // RTL_SIZEOF_THROUGH_FIELD(FLT_CONNECT_CONTEXT, Padding) + SizeOfContext
    CHAR EaName[8];        // FLTPORT\0
    FLT_CONNECT_CONTEXT EaValue;
} FLT_PORT_FULL_EA, *PFLT_PORT_FULL_EA;

#define FLT_PORT_FULL_EA_SIZE \
    (sizeof(FILE_FULL_EA_INFORMATION) + (sizeof(FLT_PORT_EA_NAME) - sizeof(ANSI_NULL)))
#define FLT_PORT_FULL_EA_VALUE_SIZE \
    RTL_SIZEOF_THROUGH_FIELD(FLT_CONNECT_CONTEXT, Padding)

// begin_rev

// IOCTLs for unlinked FltMgr handles
#define FLT_CTL_LOAD                CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 1, METHOD_BUFFERED, FILE_WRITE_ACCESS) // in: FLT_LOAD_PARAMETERS // requires SeLoadDriverPrivilege
#define FLT_CTL_UNLOAD              CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 2, METHOD_BUFFERED, FILE_WRITE_ACCESS) // in: FLT_LOAD_PARAMETERS // requires SeLoadDriverPrivilege
#define FLT_CTL_LINK_HANDLE         CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 3, METHOD_BUFFERED, FILE_READ_ACCESS)  // in: FLT_LINK // specializes the handle
#define FLT_CTL_ATTACH              CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 4, METHOD_BUFFERED, FILE_WRITE_ACCESS) // in: FLT_ATTACH
#define FLT_CTL_DETACH              CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 5, METHOD_BUFFERED, FILE_WRITE_ACCESS) // in: FLT_INSTANCE_PARAMETERS

// IOCTLs for port-specific FltMgrMsg handles (opened using the extended attribute)
#define FLT_CTL_SEND_MESSAGE        CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 6, METHOD_NEITHER, FILE_WRITE_ACCESS)  // in, out: filter-specific
#define FLT_CTL_GET_MESSAGE         CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 7, METHOD_NEITHER, FILE_READ_ACCESS)   // out: filter-specific
#define FLT_CTL_REPLY_MESSAGE       CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 8, METHOD_NEITHER, FILE_WRITE_ACCESS)  // in: filter-specific

// IOCTLs for linked FltMgr handles; depend on previously used FLT_LINK_TYPE
//
// Find first/next:
//   FILTER                - enumerates nested instances; in: INSTANCE_INFORMATION_CLASS
//   FILTER_VOLUME         - enumerates nested instances; in: INSTANCE_INFORMATION_CLASS
//   FILTER_MANAGER        - enumerates all filters;      in: FILTER_INFORMATION_CLASS
//   FILTER_MANAGER_VOLUME - enumerates all volumes;      in: FILTER_VOLUME_INFORMATION_CLASS
//
// Get information:
//   FILTER                - queries filter;              in: FILTER_INFORMATION_CLASS
//   FILTER_INSTANCE       - queries instance;            in: INSTANCE_INFORMATION_CLASS
//
#define FLT_CTL_FIND_FIRST          CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 9, METHOD_BUFFERED, FILE_READ_ACCESS)  // in: *_INFORMATION_CLASS, out: *_INFORMATION (from fltUserStructures.h)
#define FLT_CTL_FIND_NEXT           CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 10, METHOD_BUFFERED, FILE_READ_ACCESS) // in: *_INFORMATION_CLASS, out: *_INFORMATION (from fltUserStructures.h)
#define FLT_CTL_GET_INFORMATION     CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 11, METHOD_BUFFERED, FILE_READ_ACCESS) // in: *_INFORMATION_CLASS, out: *_INFORMATION (from fltUserStructures.h)

// end_rev

// private
typedef struct _FLT_LOAD_PARAMETERS
{
    USHORT FilterNameSize;
    _Field_size_bytes_(FilterNameSize) WCHAR FilterName[ANYSIZE_ARRAY];
} FLT_LOAD_PARAMETERS, *PFLT_LOAD_PARAMETERS;

// private
typedef enum _FLT_LINK_TYPE
{
    FILTER = 0,                // FLT_FILTER_PARAMETERS
    FILTER_INSTANCE = 1,       // FLT_INSTANCE_PARAMETERS
    FILTER_VOLUME = 2,         // FLT_VOLUME_PARAMETERS
    FILTER_MANAGER = 3,        // nothing
    FILTER_MANAGER_VOLUME = 4, // nothing
} FLT_LINK_TYPE, *PFLT_LINK_TYPE;

// private
typedef struct _FLT_LINK
{
    FLT_LINK_TYPE Type;
    ULONG ParametersOffset; // from this struct
} FLT_LINK, *PFLT_LINK;

// rev
typedef struct _FLT_FILTER_PARAMETERS
{
    USHORT FilterNameSize;
    USHORT FilterNameOffset; // to WCHAR[] from this struct
} FLT_FILTER_PARAMETERS, *PFLT_FILTER_PARAMETERS;

// private
typedef struct _FLT_INSTANCE_PARAMETERS
{
    USHORT FilterNameSize;
    USHORT FilterNameOffset; // to WCHAR[] from this struct
    USHORT VolumeNameSize;
    USHORT VolumeNameOffset; // to WCHAR[] from this struct
    USHORT InstanceNameSize;
    USHORT InstanceNameOffset; // to WCHAR[] from this struct
} FLT_INSTANCE_PARAMETERS, *PFLT_INSTANCE_PARAMETERS;

// rev
typedef struct _FLT_VOLUME_PARAMETERS
{
    USHORT VolumeNameSize;
    USHORT VolumeNameOffset; // to WCHAR[] from this struct
} FLT_VOLUME_PARAMETERS, *PFLT_VOLUME_PARAMETERS;

// private
typedef enum _ATTACH_TYPE
{
    AltitudeBased = 0,
    InstanceNameBased = 1,
} ATTACH_TYPE, *PATTACH_TYPE;

// private
typedef struct _FLT_ATTACH
{
    USHORT FilterNameSize;
    USHORT FilterNameOffset; // to WCHAR[] from this struct
    USHORT VolumeNameSize;
    USHORT VolumeNameOffset; // to WCHAR[] from this struct
    ATTACH_TYPE Type;
    USHORT InstanceNameSize;
    USHORT InstanceNameOffset; // to WCHAR[] from this struct
    USHORT AltitudeSize;
    USHORT AltitudeOffset; // to WCHAR[] from this struct
} FLT_ATTACH, *PFLT_ATTACH;

// Multiple UNC Provider

// rev // FSCTLs for \Device\Mup
#define FSCTL_MUP_GET_UNC_CACHE_INFO                CTL_CODE(FILE_DEVICE_MULTI_UNC_PROVIDER, 11, METHOD_BUFFERED, FILE_ANY_ACCESS) // out: MUP_FSCTL_UNC_CACHE_INFORMATION
#define FSCTL_MUP_GET_UNC_PROVIDER_LIST             CTL_CODE(FILE_DEVICE_MULTI_UNC_PROVIDER, 12, METHOD_BUFFERED, FILE_ANY_ACCESS) // out: MUP_FSCTL_UNC_PROVIDER_INFORMATION
#define FSCTL_MUP_GET_SURROGATE_PROVIDER_LIST       CTL_CODE(FILE_DEVICE_MULTI_UNC_PROVIDER, 13, METHOD_BUFFERED, FILE_ANY_ACCESS) // out: MUP_FSCTL_SURROGATE_PROVIDER_INFORMATION
#define FSCTL_MUP_GET_UNC_HARDENING_CONFIGURATION   CTL_CODE(FILE_DEVICE_MULTI_UNC_PROVIDER, 14, METHOD_BUFFERED, FILE_ANY_ACCESS) // out: MUP_FSCTL_UNC_HARDENING_PREFIX_TABLE_ENTRY[]
#define FSCTL_MUP_GET_UNC_HARDENING_CONFIGURATION_FOR_PATH  CTL_CODE(FILE_DEVICE_MULTI_UNC_PROVIDER, 15, METHOD_BUFFERED, FILE_ANY_ACCESS) // in: MUP_FSCTL_QUERY_UNC_HARDENING_CONFIGURATION_IN; out: MUP_FSCTL_QUERY_UNC_HARDENING_CONFIGURATION_OUT

// private
typedef struct _MUP_FSCTL_UNC_CACHE_ENTRY
{
    ULONG TotalLength;
    ULONG UncNameOffset; // to WCHAR[] from this struct
    USHORT UncNameLength; // in bytes
    ULONG ProviderNameOffset; // to WCHAR[] from this struct
    USHORT ProviderNameLength; // in bytes
    ULONG SurrogateNameOffset; // to WCHAR[] from this struct
    USHORT SurrogateNameLength; // in bytes
    ULONG ProviderPriority;
    ULONG EntryTtl;
    WCHAR Strings[ANYSIZE_ARRAY];
} MUP_FSCTL_UNC_CACHE_ENTRY, *PMUP_FSCTL_UNC_CACHE_ENTRY;

// private
typedef struct _MUP_FSCTL_UNC_CACHE_INFORMATION
{
    ULONG MaxCacheSize;
    ULONG CurrentCacheSize;
    ULONG EntryTimeout;
    ULONG TotalEntries;
    MUP_FSCTL_UNC_CACHE_ENTRY CacheEntry[ANYSIZE_ARRAY];
} MUP_FSCTL_UNC_CACHE_INFORMATION, *PMUP_FSCTL_UNC_CACHE_INFORMATION;

// private
typedef struct _MUP_FSCTL_UNC_PROVIDER_ENTRY
{
    ULONG TotalLength;
    LONG ReferenceCount;
    ULONG ProviderPriority;
    ULONG ProviderState;
    ULONG ProviderId;
    USHORT ProviderNameLength; // in bytes
    WCHAR ProviderName[ANYSIZE_ARRAY];
} MUP_FSCTL_UNC_PROVIDER_ENTRY, *PMUP_FSCTL_UNC_PROVIDER_ENTRY;

// private
typedef struct _MUP_FSCTL_UNC_PROVIDER_INFORMATION
{
    ULONG TotalEntries;
    MUP_FSCTL_UNC_PROVIDER_ENTRY ProviderEntry[ANYSIZE_ARRAY];
} MUP_FSCTL_UNC_PROVIDER_INFORMATION, *PMUP_FSCTL_UNC_PROVIDER_INFORMATION;

// private
typedef struct _MUP_FSCTL_SURROGATE_PROVIDER_ENTRY
{
    ULONG TotalLength;
    LONG ReferenceCount;
    ULONG SurrogateType;
    ULONG SurrogateState;
    ULONG SurrogatePriority;
    USHORT SurrogateNameLength; // in bytes
    WCHAR SurrogateName[ANYSIZE_ARRAY];
} MUP_FSCTL_SURROGATE_PROVIDER_ENTRY, *PMUP_FSCTL_SURROGATE_PROVIDER_ENTRY;

// private
typedef struct _MUP_FSCTL_SURROGATE_PROVIDER_INFORMATION
{
    ULONG TotalEntries;
    MUP_FSCTL_SURROGATE_PROVIDER_ENTRY SurrogateEntry[ANYSIZE_ARRAY];
} MUP_FSCTL_SURROGATE_PROVIDER_INFORMATION, *PMUP_FSCTL_SURROGATE_PROVIDER_INFORMATION;

// private
typedef struct _MUP_FSCTL_UNC_HARDENING_PREFIX_TABLE_ENTRY
{
    ULONG NextOffset; // from this struct
    ULONG PrefixNameOffset; // to WCHAR[] from this struct
    USHORT PrefixNameCbLength; // in bytes
    union
    {
        ULONG RequiredHardeningCapabilities;
        struct
        {
            ULONG RequiresMutualAuth : 1;
            ULONG RequiresIntegrity : 1;
            ULONG RequiresPrivacy : 1;
        };
    };
    ULONGLONG OpenCount;
} MUP_FSCTL_UNC_HARDENING_PREFIX_TABLE_ENTRY, *PMUP_FSCTL_UNC_HARDENING_PREFIX_TABLE_ENTRY;

// private
typedef struct _MUP_FSCTL_QUERY_UNC_HARDENING_CONFIGURATION_IN
{
    ULONG Size;
    ULONG UncPathOffset; // to WCHAR[] from this struct
    USHORT UncPathCbLength; // in bytes
} MUP_FSCTL_QUERY_UNC_HARDENING_CONFIGURATION_IN, *PMUP_FSCTL_QUERY_UNC_HARDENING_CONFIGURATION_IN;

// private
typedef struct _MUP_FSCTL_QUERY_UNC_HARDENING_CONFIGURATION_OUT
{
    ULONG Size;
    union
    {
        ULONG RequiredHardeningCapabilities;
        struct
        {
            ULONG RequiresMutualAuth : 1;
            ULONG RequiresIntegrity : 1;
            ULONG RequiresPrivacy : 1;
        };
    };
} MUP_FSCTL_QUERY_UNC_HARDENING_CONFIGURATION_OUT, *PMUP_FSCTL_QUERY_UNC_HARDENING_CONFIGURATION_OUT;

EXTERN_C_END
