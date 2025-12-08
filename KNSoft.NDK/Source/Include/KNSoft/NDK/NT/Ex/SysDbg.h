#pragma once

#include "../MinDef.h"
#include "../Io/Io.h"

EXTERN_C_START

/* phnt */

typedef enum _SYSDBG_COMMAND
{
    SysDbgQueryModuleInformation,
    SysDbgQueryTraceInformation,
    SysDbgSetTracepoint,
    SysDbgSetSpecialCall, // PVOID
    SysDbgClearSpecialCalls, // void
    SysDbgQuerySpecialCalls,
    SysDbgBreakPoint,
    SysDbgQueryVersion, // DBGKD_GET_VERSION64
    SysDbgReadVirtual, // SYSDBG_VIRTUAL
    SysDbgWriteVirtual, // SYSDBG_VIRTUAL
    SysDbgReadPhysical, // SYSDBG_PHYSICAL // 10
    SysDbgWritePhysical, // SYSDBG_PHYSICAL
    SysDbgReadControlSpace, // SYSDBG_CONTROL_SPACE
    SysDbgWriteControlSpace, // SYSDBG_CONTROL_SPACE
    SysDbgReadIoSpace, // SYSDBG_IO_SPACE
    SysDbgWriteIoSpace, // SYSDBG_IO_SPACE
    SysDbgReadMsr, // SYSDBG_MSR
    SysDbgWriteMsr, // SYSDBG_MSR
    SysDbgReadBusData, // SYSDBG_BUS_DATA
    SysDbgWriteBusData, // SYSDBG_BUS_DATA
    SysDbgCheckLowMemory, // 20
    SysDbgEnableKernelDebugger,
    SysDbgDisableKernelDebugger,
    SysDbgGetAutoKdEnable,
    SysDbgSetAutoKdEnable,
    SysDbgGetPrintBufferSize,
    SysDbgSetPrintBufferSize,
    SysDbgGetKdUmExceptionEnable,
    SysDbgSetKdUmExceptionEnable,
    SysDbgGetTriageDump, // SYSDBG_TRIAGE_DUMP
    SysDbgGetKdBlockEnable, // 30
    SysDbgSetKdBlockEnable,
    SysDbgRegisterForUmBreakInfo,
    SysDbgGetUmBreakPid,
    SysDbgClearUmBreakPid,
    SysDbgGetUmAttachPid,
    SysDbgClearUmAttachPid,
    SysDbgGetLiveKernelDump, // SYSDBG_LIVEDUMP_CONTROL
    SysDbgKdPullRemoteFile, // SYSDBG_KD_PULL_REMOTE_FILE
    SysDbgMaxInfoClass
} SYSDBG_COMMAND, *PSYSDBG_COMMAND;

typedef struct _SYSDBG_VIRTUAL
{
    PVOID Address;
    PVOID Buffer;
    ULONG Request;
} SYSDBG_VIRTUAL, *PSYSDBG_VIRTUAL;

typedef struct _SYSDBG_PHYSICAL
{
    PHYSICAL_ADDRESS Address;
    PVOID Buffer;
    ULONG Request;
} SYSDBG_PHYSICAL, *PSYSDBG_PHYSICAL;

typedef struct _SYSDBG_CONTROL_SPACE
{
    ULONG64 Address;
    PVOID Buffer;
    ULONG Request;
    ULONG Processor;
} SYSDBG_CONTROL_SPACE, *PSYSDBG_CONTROL_SPACE;

typedef struct _SYSDBG_IO_SPACE
{
    ULONG64 Address;
    PVOID Buffer;
    ULONG Request;
    INTERFACE_TYPE InterfaceType;
    ULONG BusNumber;
    ULONG AddressSpace;
} SYSDBG_IO_SPACE, *PSYSDBG_IO_SPACE;

typedef struct _SYSDBG_MSR
{
    ULONG Msr;
    ULONG64 Data;
} SYSDBG_MSR, *PSYSDBG_MSR;

typedef struct _SYSDBG_BUS_DATA
{
    ULONG Address;
    PVOID Buffer;
    ULONG Request;
    BUS_DATA_TYPE BusDataType;
    ULONG BusNumber;
    ULONG SlotNumber;
} SYSDBG_BUS_DATA, *PSYSDBG_BUS_DATA;

typedef struct _SYSDBG_TRIAGE_DUMP
{
    ULONG Flags;
    ULONG BugCheckCode;
    ULONG_PTR BugCheckParam1;
    ULONG_PTR BugCheckParam2;
    ULONG_PTR BugCheckParam3;
    ULONG_PTR BugCheckParam4;
    ULONG ProcessHandles;
    ULONG ThreadHandles;
    PHANDLE Handles;
} SYSDBG_TRIAGE_DUMP, *PSYSDBG_TRIAGE_DUMP;

typedef union _SYSDBG_LIVEDUMP_CONTROL_FLAGS
{
    struct
    {
        ULONG UseDumpStorageStack : 1;
        ULONG CompressMemoryPagesData : 1;
        ULONG IncludeUserSpaceMemoryPages : 1;
        ULONG AbortIfMemoryPressure : 1; // REDSTONE4
        ULONG SelectiveDump : 1; // WIN11
        ULONG Reserved : 27;
    };
    ULONG AsUlong;
} SYSDBG_LIVEDUMP_CONTROL_FLAGS, *PSYSDBG_LIVEDUMP_CONTROL_FLAGS;

typedef union _SYSDBG_LIVEDUMP_CONTROL_ADDPAGES
{
    struct
    {
        ULONG HypervisorPages : 1;
        ULONG NonEssentialHypervisorPages : 1; // since WIN11
        ULONG Reserved : 30;
    };
    ULONG AsUlong;
} SYSDBG_LIVEDUMP_CONTROL_ADDPAGES, *PSYSDBG_LIVEDUMP_CONTROL_ADDPAGES;

#define SYSDBG_LIVEDUMP_SELECTIVE_CONTROL_VERSION 1

typedef struct _SYSDBG_LIVEDUMP_SELECTIVE_CONTROL
{
    ULONG Version;
    ULONG Size;
    union
    {
        ULONGLONG Flags;
        struct
        {
            ULONGLONG ThreadKernelStacks : 1;
            ULONGLONG ReservedFlags : 63;
        };
    };
    ULONGLONG Reserved[4];
} SYSDBG_LIVEDUMP_SELECTIVE_CONTROL, *PSYSDBG_LIVEDUMP_SELECTIVE_CONTROL;

#define SYSDBG_LIVEDUMP_CONTROL_VERSION_1 1
#define SYSDBG_LIVEDUMP_CONTROL_VERSION_2 2
#define SYSDBG_LIVEDUMP_CONTROL_VERSION SYSDBG_LIVEDUMP_CONTROL_VERSION_2

// private
typedef struct _SYSDBG_LIVEDUMP_CONTROL_V1
{
    ULONG Version;
    ULONG BugCheckCode;
    ULONG_PTR BugCheckParam1;
    ULONG_PTR BugCheckParam2;
    ULONG_PTR BugCheckParam3;
    ULONG_PTR BugCheckParam4;
    HANDLE DumpFileHandle;
    HANDLE CancelEventHandle;
    SYSDBG_LIVEDUMP_CONTROL_FLAGS Flags;
    SYSDBG_LIVEDUMP_CONTROL_ADDPAGES AddPagesControl;
} SYSDBG_LIVEDUMP_CONTROL_V1, *PSYSDBG_LIVEDUMP_CONTROL_V1;

typedef struct _SYSDBG_LIVEDUMP_CONTROL
{
    ULONG Version;
    ULONG BugCheckCode;
    ULONG_PTR BugCheckParam1;
    ULONG_PTR BugCheckParam2;
    ULONG_PTR BugCheckParam3;
    ULONG_PTR BugCheckParam4;
    HANDLE DumpFileHandle;
    HANDLE CancelEventHandle;
    SYSDBG_LIVEDUMP_CONTROL_FLAGS Flags;
    SYSDBG_LIVEDUMP_CONTROL_ADDPAGES AddPagesControl;
    PSYSDBG_LIVEDUMP_SELECTIVE_CONTROL SelectiveControl; // since WIN11
} SYSDBG_LIVEDUMP_CONTROL, *PSYSDBG_LIVEDUMP_CONTROL;

typedef struct _SYSDBG_KD_PULL_REMOTE_FILE
{
    UNICODE_STRING ImageFileName;
} SYSDBG_KD_PULL_REMOTE_FILE, *PSYSDBG_KD_PULL_REMOTE_FILE;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSystemDebugControl(
    _In_ SYSDBG_COMMAND Command,
    _Inout_updates_bytes_opt_(InputBufferLength) PVOID InputBuffer,
    _In_ ULONG InputBufferLength,
    _Out_writes_bytes_opt_(OutputBufferLength) PVOID OutputBuffer,
    _In_ ULONG OutputBufferLength,
    _Out_opt_ PULONG ReturnLength);

EXTERN_C_END
