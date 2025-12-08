#pragma once

#include "../../MinDef.h"
#include "../Sync.h"
#include "../DataStructures/Bitmap.h"
#include "Process.h"
#include "../../Ps/JobInfo.h"
#include "../../Nls.h"
#include "../../Win32K/Gdi.h"

/* NT\Win32K\Win32KApi.h */
typedef struct _KERNEL_CALLBACK_TABLE KERNEL_CALLBACK_TABLE, *PKERNEL_CALLBACK_TABLE;

EXTERN_C_START

/* phnt & PDB & KNSoft.NDK & Geoff Chappell */

typedef struct _LEAP_SECOND_DATA
{
    UCHAR Enabled;
    UCHAR Padding[3];
    ULONG Count;
    _Field_size_(Count) LARGE_INTEGER Data[ANYSIZE_ARRAY];
} LEAP_SECOND_DATA, *PLEAP_SECOND_DATA;

/**
 * The PEB_LDR_DATA structure contains information about the loaded modules for the process.
 * @sa https://learn.microsoft.com/en-us/windows/win32/api/winternl/ns-winternl-peb_ldr_data
 */

typedef struct _PEB_LDR_DATA
{
    ULONG Length;
    BOOLEAN Initialized;
    HANDLE SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    PVOID EntryInProgress;
    BOOLEAN ShutdownInProgress;
    HANDLE ShutdownThreadId;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB_LDR_DATA64
{
    ULONG Length;
    BOOL Initialized;
    VOID* POINTER_64 SsHandle;
    LIST_ENTRY64 InLoadOrderModuleList;
    LIST_ENTRY64 InMemoryOrderModuleList;
    LIST_ENTRY64 InInitializationOrderModuleList;
    VOID* POINTER_64 EntryInProgress;
    BOOLEAN ShutdownInProgress;
    VOID* POINTER_64 ShutdownThreadId;
} PEB_LDR_DATA64, *PPEB_LDR_DATA64;

typedef struct _PEB_LDR_DATA32
{
    ULONG Length;
    BOOL Initialized;
    VOID* POINTER_32 SsHandle;
    LIST_ENTRY32 InLoadOrderModuleList;
    LIST_ENTRY32 InMemoryOrderModuleList;
    LIST_ENTRY32 InInitializationOrderModuleList;
    VOID* POINTER_32 EntryInProgress;
    BOOLEAN ShutdownInProgress;
    VOID* POINTER_32 ShutdownThreadId;
} PEB_LDR_DATA32, *PPEB_LDR_DATA32;

// PEB->AppCompatFlags
#define KACF_OLDGETSHORTPATHNAME                      0x00000001
#define KACF_VERSIONLIE_NOT_USED                      0x00000002
#define KACF_GETTEMPPATH_NOT_USED                     0x00000004
#define KACF_GETDISKFREESPACE                         0x00000008
#define KACF_FTMFROMCURRENTAPT                        0x00000020
#define KACF_DISALLOWORBINDINGCHANGES                 0x00000040
#define KACF_OLE32VALIDATEPTRS                        0x00000080
#define KACF_DISABLECICERO                            0x00000100
#define KACF_OLE32ENABLEASYNCDOCFILE                  0x00000200
#define KACF_OLE32ENABLELEGACYEXCEPTIONHANDLING       0x00000400
#define KACF_RPCDISABLENDRCLIENTHARDENING             0x00000800
#define KACF_RPCDISABLENDRMAYBENULL_SIZEIS            0x00001000
#define KACF_DISABLEALLDDEHACK_NOT_USED               0x00002000
#define KACF_RPCDISABLENDR61_RANGE                    0x00004000
#define KACF_RPC32ENABLELEGACYEXCEPTIONHANDLING       0x00008000
#define KACF_OLE32DOCFILEUSELEGACYNTFSFLAGS           0x00010000
#define KACF_RPCDISABLENDRCONSTIIDCHECK               0x00020000
#define KACF_USERDISABLEFORWARDERPATCH                0x00040000
#define KACF_OLE32DISABLENEW_WMPAINT_DISPATCH         0x00100000
#define KACF_ADDRESTRICTEDSIDINCOINITIALIZESECURITY   0x00200000
#define KACF_ALLOCDEBUGINFOFORCRITSECTIONS            0x00400000
#define KACF_OLEAUT32ENABLEUNSAFELOADTYPELIBRELATIVE  0x00800000
#define KACF_ALLOWMAXIMIZEDWINDOWGAMMA                0x01000000
#define KACF_DONOTADDTOCACHE                          0x80000000
#define KACF_DISABLEPOSIXDELETEFILE                  0x100000000 // rev KernelBase!InternalDeleteFileW

// private
#define API_SET_SECTION_NAME ".apiset"

// private
#define API_SET_SCHEMA_VERSION_V2 0x00000002 // WIN7, WIN8
#define API_SET_SCHEMA_VERSION_V4 0x00000004 // WINBLUE
#define API_SET_SCHEMA_VERSION_V6 0x00000006 // since THRESHOLD
#define API_SET_SCHEMA_VERSION API_SET_SCHEMA_VERSION_V6

// private
#define API_SET_SCHEMA_FLAGS_SEALED 0x00000001
#define API_SET_SCHEMA_FLAGS_HOST_EXTENSION 0x00000002

// private
#define API_SET_SCHEMA_ENTRY_FLAGS_SEALED 0x00000001
#define API_SET_SCHEMA_ENTRY_FLAGS_EXTENSION 0x00000002

// private
typedef struct _API_SET_VALUE_ENTRY_V2
{
    ULONG NameOffset; // to WCHAR[NameLength / sizeof(WCHAR)], from schema base
    ULONG NameLength;
    ULONG ValueOffset; // to WCHAR[ValueLength / sizeof(WCHAR)], from schema base
    ULONG ValueLength;
} API_SET_VALUE_ENTRY_V2, *PAPI_SET_VALUE_ENTRY_V2;

// private
typedef struct _API_SET_VALUE_ARRAY_V2
{
    ULONG Count;
    _Field_size_full_(Count) API_SET_VALUE_ENTRY_V2 Array[ANYSIZE_ARRAY];
} API_SET_VALUE_ARRAY_V2, *PAPI_SET_VALUE_ARRAY_V2;

// private
typedef struct _API_SET_NAMESPACE_ENTRY_V2
{
    ULONG NameOffset; // to WCHAR[NameLength / sizeof(WCHAR)], from schema base
    ULONG NameLength;
    ULONG DataOffset; // to API_SET_VALUE_ARRAY_V2, from schema base
} API_SET_NAMESPACE_ENTRY_V2, *PAPI_SET_NAMESPACE_ENTRY_V2;

// private // PEB->ApiSetMap on WIN7, WIN8
typedef struct _API_SET_NAMESPACE_ARRAY_V2
{
    ULONG Version; // API_SET_SCHEMA_VERSION_V2
    ULONG Count;
    _Field_size_full_(Count) API_SET_NAMESPACE_ENTRY_V2 Array[ANYSIZE_ARRAY];
} API_SET_NAMESPACE_ARRAY_V2, *PAPI_SET_NAMESPACE_ARRAY_V2;

// private
typedef struct _API_SET_VALUE_ENTRY_V4
{
    ULONG Flags;
    ULONG NameOffset; // to WCHAR[NameLength / sizeof(WCHAR)], from schema base
    ULONG NameLength;
    ULONG ValueOffset; // to WCHAR[ValueLength / sizeof(WCHAR)], from schema base
    ULONG ValueLength;
} API_SET_VALUE_ENTRY_V4, *PAPI_SET_VALUE_ENTRY_V4;

// private
typedef struct _API_SET_VALUE_ARRAY_V4
{
    ULONG Flags;
    ULONG Count;
    _Field_size_full_(Count) API_SET_VALUE_ENTRY_V4 Array[ANYSIZE_ARRAY];
} API_SET_VALUE_ARRAY_V4, *PAPI_SET_VALUE_ARRAY_V4;

// private
typedef struct _API_SET_NAMESPACE_ENTRY_V4
{
    ULONG Flags; // API_SET_SCHEMA_ENTRY_FLAGS_*
    ULONG NameOffset; // to WCHAR[NameLength / sizeof(WCHAR)], from schema base
    ULONG NameLength;
    ULONG AliasOffset; // to WCHAR[AliasLength / sizeof(WCHAR)], from schema base
    ULONG AliasLength;
    ULONG DataOffset; // to API_SET_VALUE_ARRAY_V4, from schema base
} API_SET_NAMESPACE_ENTRY_V4, *PAPI_SET_NAMESPACE_ENTRY_V4;

// private // PEB->ApiSetMap on WINBLUE
typedef struct _API_SET_NAMESPACE_ARRAY_V4
{
    ULONG Version; // API_SET_SCHEMA_VERSION_V4
    ULONG Size;
    ULONG Flags; // API_SET_SCHEMA_FLAGS_*
    ULONG Count;
    _Field_size_full_(Count) API_SET_NAMESPACE_ENTRY_V4 Array[ANYSIZE_ARRAY];
} API_SET_NAMESPACE_ARRAY_V4, *PAPI_SET_NAMESPACE_ARRAY_V4;

// private
typedef struct _API_SET_VALUE_ENTRY
{
    ULONG Flags;
    ULONG NameOffset; // to WCHAR[NameLength / sizeof(WCHAR)], from schema base
    ULONG NameLength;
    ULONG ValueOffset; // to WCHAR[ValueLength / sizeof(WCHAR)], from schema base
    ULONG ValueLength;
} API_SET_VALUE_ENTRY, *PAPI_SET_VALUE_ENTRY;

// private
typedef struct _API_SET_NAMESPACE_ENTRY
{
    ULONG Flags; // API_SET_SCHEMA_ENTRY_FLAGS_*
    ULONG NameOffset; // to WCHAR[NameLength / sizeof(WCHAR)], from schema base
    ULONG NameLength;
    ULONG HashedLength;
    ULONG ValueOffset; // to API_SET_VALUE_ENTRY[ValueCount], from schema base
    ULONG ValueCount;
} API_SET_NAMESPACE_ENTRY, *PAPI_SET_NAMESPACE_ENTRY;

// private
typedef struct _API_SET_HASH_ENTRY
{
    ULONG Hash;
    ULONG Index;
} API_SET_HASH_ENTRY, *PAPI_SET_HASH_ENTRY;

// private // PEB->ApiSetMap since THRESHOLD
typedef struct _API_SET_NAMESPACE
{
    ULONG Version; // API_SET_SCHEMA_VERSION_V6
    ULONG Size;
    ULONG Flags; // API_SET_SCHEMA_FLAGS_*
    ULONG Count;
    ULONG EntryOffset; // to API_SET_NAMESPACE_ENTRY[Count], from this struct base
    ULONG HashOffset; // to API_SET_HASH_ENTRY[Count], from this struct base
    ULONG HashFactor;
} API_SET_NAMESPACE, *PAPI_SET_NAMESPACE;

// PEB->TelemetryCoverageHeader
typedef struct _TELEMETRY_COVERAGE_HEADER
{
    UCHAR MajorVersion;
    UCHAR MinorVersion;
    struct
    {
        USHORT TracingEnabled : 1;
        USHORT Reserved1 : 15;
    };
    ULONG HashTableEntries;
    ULONG HashIndexMask;
    ULONG TableUpdateVersion;
    ULONG TableSizeInBytes;
    ULONG LastResetTick;
    ULONG ResetRound;
    ULONG Reserved2;
    ULONG RecordedCount;
    ULONG Reserved3[4];
    ULONG HashTable[ANYSIZE_ARRAY];
} TELEMETRY_COVERAGE_HEADER, *PTELEMETRY_COVERAGE_HEADER;

typedef struct _WER_RECOVERY_INFO
{
    ULONG Length;
    PVOID Callback;
    PVOID Parameter;
    HANDLE Started;
    HANDLE Finished;
    HANDLE InProgress;
    LONG LastError;
    BOOL Successful;
    ULONG PingInterval;
    ULONG Flags;
} WER_RECOVERY_INFO, *PWER_RECOVERY_INFO;

typedef struct _WER_FILE
{
    USHORT Flags;
    WCHAR Path[MAX_PATH];
} WER_FILE, *PWER_FILE;

typedef struct _WER_MEMORY
{
    PVOID Address;
    ULONG Size;
} WER_MEMORY, *PWER_MEMORY;

typedef struct _WER_GATHER
{
    PVOID Next;
    USHORT Flags;
    union
    {
        WER_FILE File;
        WER_MEMORY Memory;
    } v;
} WER_GATHER, *PWER_GATHER;

typedef struct _WER_METADATA
{
    PVOID Next;
    WCHAR Key[64];
    WCHAR Value[128];
} WER_METADATA, *PWER_METADATA;

typedef struct _WER_RUNTIME_DLL
{
    PVOID Next;
    ULONG Length;
    PVOID Context;
    WCHAR CallbackDllPath[MAX_PATH];
} WER_RUNTIME_DLL, *PWER_RUNTIME_DLL;

typedef struct _WER_DUMP_COLLECTION
{
    PVOID Next;
    ULONG ProcessId;
    ULONG ThreadId;
} WER_DUMP_COLLECTION, *PWER_DUMP_COLLECTION;

typedef struct _WER_HEAP_MAIN_HEADER
{
    WCHAR Signature[16];
    LIST_ENTRY Links;
    HANDLE Mutex;
    PVOID FreeHeap;
    ULONG FreeCount;
} WER_HEAP_MAIN_HEADER, *PWER_HEAP_MAIN_HEADER;

#ifndef RESTART_MAX_CMD_LINE
#define RESTART_MAX_CMD_LINE 1024
#endif

typedef struct _WER_PEB_HEADER_BLOCK
{
    LONG Length;
    WCHAR Signature[16];
    WCHAR AppDataRelativePath[64];
    WCHAR RestartCommandLine[RESTART_MAX_CMD_LINE];
    WER_RECOVERY_INFO RecoveryInfo;
    PWER_GATHER Gather;
    PWER_METADATA MetaData;
    PWER_RUNTIME_DLL RuntimeDll;
    PWER_DUMP_COLLECTION DumpCollection;
    LONG GatherCount;
    LONG MetaDataCount;
    LONG DumpCount;
    LONG Flags;
    WER_HEAP_MAIN_HEADER MainHeader;
    PVOID Reserved;
} WER_PEB_HEADER_BLOCK, *PWER_PEB_HEADER_BLOCK;

typedef _Function_class_(PS_POST_PROCESS_INIT_ROUTINE)
VOID NTAPI PS_POST_PROCESS_INIT_ROUTINE(
    VOID
    );
typedef PS_POST_PROCESS_INIT_ROUTINE* PPS_POST_PROCESS_INIT_ROUTINE;

typedef struct _SDBQUERYRESULT
{
    ULONG Exes[16];
    ULONG ExeFlags[16];
    ULONG Layers[8];
    ULONG LayerFlags;
    ULONG AppHelp;
    ULONG ExeCount;
    ULONG LayerCount;
    GUID ID;
    ULONG ExtraFlags;
    ULONG CustomSDBMap;
    GUID DB[16];
} SDBQUERYRESULT, *PSDBQUERYRESULT;

_STATIC_ASSERT(sizeof(SDBQUERYRESULT) == 0x1c8);

typedef struct _SWITCH_CONTEXT_ATTRIBUTE
{
    ULONG_PTR ContextUpdateCounter;
    BOOL AllowContextUpdate;
    BOOL EnableTrace;
    HANDLE EtwHandle;
} SWITCH_CONTEXT_ATTRIBUTE, *PSWITCH_CONTEXT_ATTRIBUTE;

#ifdef _WIN64
_STATIC_ASSERT(sizeof(SWITCH_CONTEXT_ATTRIBUTE) == 0x18);
#else
_STATIC_ASSERT(sizeof(SWITCH_CONTEXT_ATTRIBUTE) == 0x10);
#endif

typedef struct tagSWITCH_CONTEXT_DATA
{
    ULONGLONG OsMaxVersionTested;
    ULONG TargetPlatform;
    ULONGLONG ContextMinimum;
    GUID Platform;
    GUID MinPlatform;
    ULONG ContextSource;
    ULONG ElementCount;
    GUID Elements[48];
} SWITCH_CONTEXT_DATA, * PSWITCH_CONTEXT_DATA;

_STATIC_ASSERT(sizeof(SWITCH_CONTEXT_DATA) == 0x340);

typedef struct tagSWITCH_CONTEXT
{
    SWITCH_CONTEXT_ATTRIBUTE Attribute;
    SWITCH_CONTEXT_DATA Data;
} SWITCH_CONTEXT, *PSWITCH_CONTEXT;

#ifdef _WIN64
_STATIC_ASSERT(sizeof(SWITCH_CONTEXT) == 0x358);
#else
_STATIC_ASSERT(sizeof(SWITCH_CONTEXT) == 0x350);
#endif

typedef struct _SDB_CSTRUCT_COBALT_PROCFLAG
{
    KAFFINITY AffinityMask;
    ULONG CPUIDEcxOverride;
    ULONG CPUIDEdxOverride;
    USHORT ProcessorGroup;
    USHORT FastSelfModThreshold;
    USHORT Reserved1;
    UCHAR Reserved2;
    UCHAR BackgroundWork : 5;
    UCHAR CPUIDBrand : 4;
    UCHAR Reserved3 : 4;
    UCHAR RdtscScaling : 3;
    UCHAR Reserved4 : 2;
    UCHAR UnalignedAtomicApproach : 2;
    UCHAR Win11Atomics : 2;
    UCHAR RunOnSingleCore : 1;
    UCHAR X64CPUID : 1;
    UCHAR PatchUnaligned : 1;
    UCHAR InterpreterOrJitter : 1;
    UCHAR ForceSegmentHeap : 1;
    UCHAR Reserved5 : 1;
    UCHAR Reserved6 : 1;
    union
    {
        ULONGLONG Group1AsUINT64;
        struct _SDB_CSTRUCT_COBALT_PROCFLAG* Specified;
    };
} SDB_CSTRUCT_COBALT_PROCFLAG, *PSDB_CSTRUCT_COBALT_PROCFLAG;

#ifdef _WIN64
_STATIC_ASSERT(sizeof(SDB_CSTRUCT_COBALT_PROCFLAG) == 0x28);
#else
_STATIC_ASSERT(sizeof(SDB_CSTRUCT_COBALT_PROCFLAG) == 0x20);
#endif

typedef struct _APPCOMPAT_EXE_DATA
{
    ULONG_PTR Reserved[65];
    ULONG Size;
    ULONG Magic;
    BOOL LoadShimEngine;
    USHORT ExeType;
    SDBQUERYRESULT SdbQueryResult;
    ULONG_PTR DbgLogChannels[128];
    SWITCH_CONTEXT SwitchContext;
    ULONG ParentProcessId;
    WCHAR ParentImageName[260];
    WCHAR ParentCompatLayers[256];
    WCHAR ActiveCompatLayers[256];
    ULONG ImageFileSize;
    ULONG ImageCheckSum;
    BOOL LatestOs;
    BOOL PackageId;
    BOOL SwitchBackManifest;
    BOOL UacManifest;
    BOOL LegacyInstaller;
    ULONG RunLevel;
    ULONG_PTR WinRTFlags;
    PVOID HookCOM;
    PVOID ComponentOnDemandEvent;
    PVOID Quirks;
    ULONG QuirksSize;
    SDB_CSTRUCT_COBALT_PROCFLAG CobaltProcFlags;
    ULONG FullMatchDbSizeCb;
    ULONG FullMatchDbOffset;
} APPCOMPAT_EXE_DATA, *PAPPCOMPAT_EXE_DATA;

#ifdef _WIN64
_STATIC_ASSERT(sizeof(APPCOMPAT_EXE_DATA) == 0x11C0);
#else
_STATIC_ASSERT(sizeof(APPCOMPAT_EXE_DATA) == 0xE98);
#endif

/**
 * Process Environment Block (PEB) structure.
 *
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winternl/ns-winternl-peb
 */
typedef struct _PEB
{
    BOOLEAN InheritedAddressSpace;     // The process was cloned with an inherited address space.
    BOOLEAN ReadImageFileExecOptions;  // The process has image file execution options (IFEO).
    BOOLEAN BeingDebugged;             // The process has a debugger attached.

    union
    {
        BOOLEAN BitField;
        struct
        {
            BOOLEAN ImageUsesLargePages : 1;           // The process uses large image regions (4 MB).  
            BOOLEAN IsProtectedProcess : 1;            // The process is a protected process.
            BOOLEAN IsImageDynamicallyRelocated : 1;   // The process image base address was relocated.         
            BOOLEAN SkipPatchingUser32Forwarders : 1;  // The process skipped forwarders for User32.dll functions. 1 for 64-bit, 0 for 32-bit.            
            BOOLEAN IsPackagedProcess : 1;             // The process is a packaged store process (APPX/MSIX).
            BOOLEAN IsAppContainerProcess : 1;         // The process has an AppContainer token.      
            BOOLEAN IsProtectedProcessLight : 1;       // The process is a protected process (light).            
            BOOLEAN IsLongPathAwareProcess : 1;        // The process is long path aware.
        };
    };

    HANDLE Mutant;                                  // Handle to a mutex for synchronization.
    PVOID ImageBaseAddress;                         // Pointer to the base address of the process image.
    PPEB_LDR_DATA Ldr;                              // Pointer to the process loader data.
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters; // Pointer to the process parameters.
    PVOID SubSystemData;                            // Reserved.
    PVOID ProcessHeap;                              // Pointer to the process default heap.
    PRTL_CRITICAL_SECTION FastPebLock;              // Pointer to a critical section used to synchronize access to the PEB.
    PSLIST_HEADER AtlThunkSListPtr;                 // Pointer to a singly linked list used by ATL.
    HANDLE IFEOKey;                                 // Handle to the Image File Execution Options key.

    union
    {
        ULONG CrossProcessFlags;                    // Cross process flags.
        struct
        {
            ULONG ProcessInJob : 1;                 // The process is part of a job.
            ULONG ProcessInitializing : 1;          // The process is initializing. 
            ULONG ProcessUsingVEH : 1;              // The process is using VEH.
            ULONG ProcessUsingVCH : 1;              // The process is using VCH.
            ULONG ProcessUsingFTH : 1;              // The process is using FTH.
            ULONG ProcessPreviouslyThrottled : 1;   // The process was previously throttled.
            ULONG ProcessCurrentlyThrottled : 1;    // The process is currently throttled.
            ULONG ProcessImagesHotPatched : 1;      // The process images are hot patched. // RS5
            ULONG ReservedBits0 : 24;
        };
    };

    // User32 KERNEL_CALLBACK_TABLE (ntuser.h)
    union
    {
        PKERNEL_CALLBACK_TABLE KernelCallbackTable;
        PVOID UserSharedInfoPtr;
    };

    ULONG SystemReserved;                   // Reserved.
    ULONG AtlThunkSListPtr32;               // Pointer to the Active Template Library (ATL) singly linked list (32-bit)
    PAPI_SET_NAMESPACE ApiSetMap;           // Pointer to the API Set Schema.
    ULONG TlsExpansionCounter;              // Counter for TLS expansion.
    PRTL_BITMAP TlsBitmap;                  // Pointer to the TLS bitmap.
    ULONG TlsBitmapBits[2];                 // Bits for the TLS bitmap.
    PVOID ReadOnlySharedMemoryBase;         // Reserved for CSRSS.
    PSILO_USER_SHARED_DATA SharedData;      // Pointer to the USER_SHARED_DATA for the current SILO.
    PVOID* ReadOnlyStaticServerData;        // Reserved for CSRSS.
    PCPTABLEINFO AnsiCodePageData;          // Pointer to the ANSI code page data.
    PCPTABLEINFO OemCodePageData;           // Pointer to the OEM code page data.
    PNLSTABLEINFO UnicodeCaseTableData;     // Pointer to the Unicode case table data.
    ULONG NumberOfProcessors;               // The total number of system processors.
    ULONG NtGlobalFlag;                     // Global flags for the system.
    LARGE_INTEGER CriticalSectionTimeout;   // Timeout for critical sections.
    SIZE_T HeapSegmentReserve;              // Reserved size for heap segments.
    SIZE_T HeapSegmentCommit;               // Committed size for heap segments.
    SIZE_T HeapDeCommitTotalFreeThreshold;  // Threshold for decommitting total free heap.
    SIZE_T HeapDeCommitFreeBlockThreshold;  // Threshold for decommitting free heap blocks.
    ULONG NumberOfHeaps;                    // Number of process heaps.
    ULONG MaximumNumberOfHeaps;             // Maximum number of process heaps.

    //
    // Pointer to an array of process heaps. ProcessHeaps is initialized
    // to point to the first free byte after the PEB and MaximumNumberOfHeaps
    // is computed from the page size used to hold the PEB, less the fixed
    // size of this data structure.
    //
    PVOID* ProcessHeaps;

    PGDI_HANDLE_ENTRY GdiSharedHandleTable; // Pointer to the system GDI shared handle table.
    PVOID ProcessStarterHelper;             // Pointer to the process starter helper.
    ULONG GdiDCAttributeList;               // The maximum number of GDI function calls during batch operations (GdiSetBatchLimit)
    PRTL_CRITICAL_SECTION LoaderLock;       // Pointer to the loader lock critical section.
    ULONG OSMajorVersion;                   // Major version of the operating system.
    ULONG OSMinorVersion;                   // Minor version of the operating system.
    USHORT OSBuildNumber;                   // Build number of the operating system.
    USHORT OSCSDVersion;                    // CSD version of the operating system.
    ULONG OSPlatformId;                     // Platform ID of the operating system.
    ULONG ImageSubsystem;                   // Subsystem version of the current process image (PE Headers).
    ULONG ImageSubsystemMajorVersion;       // Major version of the current process image subsystem (PE Headers).
    ULONG ImageSubsystemMinorVersion;       // Minor version of the current process image subsystem (PE Headers).
    KAFFINITY ActiveProcessAffinityMask;    // Affinity mask for the current process.

    // Temporary buffer for GDI handles accumulated in the current batch.
#ifdef _WIN64
    ULONG GdiHandleBuffer[60];
#else
    ULONG GdiHandleBuffer[34];
#endif

    PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;           // Pointer to the post-process initialization routine available for use by the application.
    PRTL_BITMAP TlsExpansionBitmap;                                 // Pointer to the TLS expansion bitmap.
    ULONG TlsExpansionBitmapBits[32];                               // Bits for the TLS expansion bitmap. TLS_EXPANSION_SLOTS
    ULONG SessionId;                                                // Session ID of the current process.
    ULARGE_INTEGER AppCompatFlags;                                  // Application compatibility flags. KACF_*
    ULARGE_INTEGER AppCompatFlagsUser;                              // Application compatibility flags. KACF_*
    PVOID pShimData;                                                // Pointer to the Application SwitchBack Compatibility Engine.
    PAPPCOMPAT_EXE_DATA AppCompatInfo;                              // Pointer to the Application Compatibility Engine.
    UNICODE_STRING CSDVersion;                                      // CSD version string of the operating system.
    PACTIVATION_CONTEXT_DATA ActivationContextData;                 // Pointer to the process activation context.
    PASSEMBLY_STORAGE_MAP ProcessAssemblyStorageMap;                // Pointer to the process assembly storage map.
    PACTIVATION_CONTEXT_DATA SystemDefaultActivationContextData;    // Pointer to the system default activation context.
    PASSEMBLY_STORAGE_MAP SystemAssemblyStorageMap;                 // Pointer to the system assembly storage map.
    SIZE_T MinimumStackCommit;                                      // Minimum stack commit size.
    PVOID SparePointers[2];                                         // since 19H1 (previously FlsCallback to FlsHighIndex)
    PVOID PatchLoaderData;                                          // Pointer to the patch loader data.
    PVOID ChpeV2ProcessInfo;                                        // Pointer to the CHPE V2 process information. CHPEV2_PROCESS_INFO
    ULONG AppModelFeatureState;                                     // Packaged process feature state.
    ULONG SpareUlongs[2];
    USHORT ActiveCodePage;                                          // Active code page.
    USHORT OemCodePage;                                             // OEM code page.
    USHORT UseCaseMapping;                                          // Code page case mapping.
    USHORT UnusedNlsField;                                          // Unused NLS field.
    PWER_PEB_HEADER_BLOCK WerRegistrationData;                      // Pointer to the application WER registration data.
    PVOID WerShipAssertPtr;                                         // Pointer to the application WER assert pointer.

    // Pointer to the EC bitmap on ARM64. (Windows 11 and above)
    union
    {
        PVOID pContextData; // Pointer to the switchback compatibility engine (Windows 7 and below)
        PVOID EcCodeBitMap; // Pointer to the EC bitmap on ARM64 (Windows 11 and above) // since WIN11
    };

    PVOID pImageHeaderHash; // Reserved.

    union
    {
        ULONG TracingFlags; // ETW tracing flags.
        struct
        {
            ULONG HeapTracingEnabled : 1;       // ETW heap tracing enabled.
            ULONG CritSecTracingEnabled : 1;    // ETW lock tracing enabled.
            ULONG LibLoaderTracingEnabled : 1;  // ETW loader tracing enabled.
            ULONG SpareTracingBits : 29;
        };
    };

    ULONGLONG CsrServerReadOnlySharedMemoryBase;        // Reserved for CSRSS.
    PRTL_CRITICAL_SECTION TppWorkerpListLock;           // Pointer to the thread pool worker list lock.
    LIST_ENTRY TppWorkerpList;                          // Pointer to the thread pool worker list.
    PVOID WaitOnAddressHashTable[128];                  // Wait on address hash table. (RtlWaitOnAddress)
    PTELEMETRY_COVERAGE_HEADER TelemetryCoverageHeader; // Pointer to the telemetry coverage header. // since RS3
    ULONG CloudFileFlags;                               // Cloud file flags. (ProjFs and Cloud Files) // since RS4
    ULONG CloudFileDiagFlags;                           // Cloud file diagnostic flags.
    CHAR PlaceholderCompatibilityMode;                  // Placeholder compatibility mode. (ProjFs and Cloud Files)
    CHAR PlaceholderCompatibilityModeReserved[7];       // Reserved for placeholder compatibility mode.
    PLEAP_SECOND_DATA LeapSecondData;                   // Pointer to leap second data. // since RS5

    union
    {
        ULONG LeapSecondFlags;              // Leap second flags.
        struct
        {
            ULONG SixtySecondEnabled : 1;   // Leap seconds enabled.
            ULONG Reserved : 31;
        };
    };

    ULONG NtGlobalFlag2;                    // Global flags for the process.
    ULONGLONG ExtendedFeatureDisableMask;   // Extended feature disable mask (AVX). // since WIN11
} PEB, *PPEB;

typedef struct _PEB64
{
    /* +0x000 */ BOOLEAN InheritedAddressSpace;
    /* +0x001 */ BOOLEAN ReadImageFileExecOptions;
    /* +0x002 */ BOOLEAN BeingDebugged;
    union
    {
        /* +0x003 */ UCHAR BitField;
        struct
        {
            /* +0x003 */ UCHAR ImageUsesLargePages : 1;
            /* +0x003 */ UCHAR IsProtectedProcess : 1;
            /* +0x003 */ UCHAR IsImageDynamicallyRelocated : 1;
            /* +0x003 */ UCHAR SkipPatchingUser32Forwarders : 1;
            /* +0x003 */ UCHAR IsPackagedProcess : 1;
            /* +0x003 */ UCHAR IsAppContainer : 1;
            /* +0x003 */ UCHAR IsProtectedProcessLight : 1;
            /* +0x003 */ UCHAR IsLongPathAwareProcess : 1;
        };
    };
    /* +0x008 */ VOID* POINTER_64 Mutant;
    /* +0x010 */ VOID* POINTER_64 ImageBaseAddress;
    /* +0x018 */ PEB_LDR_DATA64* POINTER_64 Ldr;
    /* +0x020 */ RTL_USER_PROCESS_PARAMETERS64* POINTER_64 ProcessParameters;
    /* +0x028 */ VOID* POINTER_64 SubSystemData;
    /* +0x030 */ VOID* POINTER_64 ProcessHeap;
    /* +0x038 */ RTL_CRITICAL_SECTION64* POINTER_64 FastPebLock;
    /* +0x040 */ VOID* POINTER_64 AtlThunkSListPtr; // SLIST_HEADER*
    /* +0x048 */ VOID* POINTER_64 IFEOKey;
    union
    {
        /* +0x050 */ ULONG CrossProcessFlags;
        struct
        {
            /* +0x050 */ ULONG ProcessInJob : 1;
            /* +0x050 */ ULONG ProcessInitializing : 1;
            /* +0x050 */ ULONG ProcessUsingVEH : 1;
            /* +0x050 */ ULONG ProcessUsingVCH : 1;
            /* +0x050 */ ULONG ProcessUsingFTH : 1;
            /* +0x050 */ ULONG ProcessPreviouslyThrottled : 1;
            /* +0x050 */ ULONG ProcessCurrentlyThrottled : 1;
            /* +0x050 */ ULONG ProcessImagesHotPatched : 1;
            /* +0x050 */ ULONG ReservedBits0 : 24;
        };
    };
    union
    {
        /* +0x058 */ VOID* POINTER_64 KernelCallbackTable; /* TODO: PKERNEL_CALLBACK_TABLE64 */
        /* +0x058 */ VOID* POINTER_64 UserSharedInfoPtr;
    };
    /* +0x060 */ ULONG SystemReserved;
    /* +0x064 */ ULONG AtlThunkSListPtr32;
    /* +0x068 */ VOID* POINTER_64 ApiSetMap;
    /* +0x070 */ ULONG TlsExpansionCounter;
    /* +0x078 */ RTL_BITMAP64* POINTER_64 TlsBitmap;
    /* +0x080 */ ULONG TlsBitmapBits[2];
    /* +0x088 */ VOID* POINTER_64 ReadOnlySharedMemoryBase;
    /* +0x090 */ VOID* POINTER_64 SharedData;
    /* +0x098 */ VOID* POINTER_64* POINTER_64 ReadOnlyStaticServerData;
    /* +0x0A0 */ VOID* POINTER_64 AnsiCodePageData; /* TODO: PCPTABLEINFO64 */
    /* +0x0A8 */ VOID* POINTER_64 OemCodePageData; /* TODO: PCPTABLEINFO64 */
    /* +0x0B0 */ VOID* POINTER_64 UnicodeCaseTableData; /* TODO: PNLSTABLEINFO64 */
    /* +0x0B8 */ ULONG NumberOfProcessors;
    /* +0x0BC */ ULONG NtGlobalFlag;
    /* +0x0C0 */ LARGE_INTEGER CriticalSectionTimeout;
    /* +0x0C8 */ ULONGLONG HeapSegmentReserve;
    /* +0x0D0 */ ULONGLONG HeapSegmentCommit;
    /* +0x0D8 */ ULONGLONG HeapDeCommitTotalFreeThreshold;
    /* +0x0E0 */ ULONGLONG HeapDeCommitFreeBlockThreshold;
    /* +0x0E8 */ ULONG NumberOfHeaps;
    /* +0x0EC */ ULONG MaximumNumberOfHeaps;
    /* +0x0F0 */ VOID* POINTER_64* POINTER_64 ProcessHeaps;
    /* +0x0F8 */ VOID* POINTER_64 GdiSharedHandleTable; /* TODO: PGDI_HANDLE_ENTRY64 */
    /* +0x100 */ VOID* POINTER_64 ProcessStarterHelper;
    /* +0x108 */ ULONG GdiDCAttributeList;
    /* +0x110 */ RTL_CRITICAL_SECTION64* POINTER_64 LoaderLock;
    /* +0x118 */ ULONG OSMajorVersion;
    /* +0x11C */ ULONG OSMinorVersion;
    /* +0x120 */ USHORT OSBuildNumber;
    /* +0x122 */ USHORT OSCSDVersion;
    /* +0x124 */ ULONG OSPlatformId;
    /* +0x128 */ ULONG ImageSubsystem;
    /* +0x12C */ ULONG ImageSubsystemMajorVersion;
    /* +0x130 */ ULONG ImageSubsystemMinorVersion;
    /* +0x134 */ UCHAR Padding4[4];
    /* +0x138 */ ULONGLONG ActiveProcessAffinityMask;
    /* +0x140 */ ULONG GdiHandleBuffer[60];
    /* +0x230 */ VOID* POINTER_64 PostProcessInitRoutine;
    /* +0x238 */ RTL_BITMAP64* POINTER_64 TlsExpansionBitmap;
    /* +0x240 */ ULONG TlsExpansionBitmapBits[32];
    /* +0x2C0 */ ULONG SessionId;
    /* +0x2C8 */ ULARGE_INTEGER AppCompatFlags;
    /* +0x2D0 */ ULARGE_INTEGER AppCompatFlagsUser;
    /* +0x2D8 */ VOID* POINTER_64 pShimData;
    /* +0x2E0 */ VOID* POINTER_64 AppCompatInfo;
    /* +0x2E8 */ UNICODE_STRING64 CSDVersion;
    /* +0x2F8 */ ACTIVATION_CONTEXT_DATA* POINTER_64 ActivationContextData;
    /* +0x300 */ ASSEMBLY_STORAGE_MAP64* POINTER_64 ProcessAssemblyStorageMap;
    /* +0x308 */ ACTIVATION_CONTEXT_DATA* POINTER_64 SystemDefaultActivationContextData;
    /* +0x310 */ ASSEMBLY_STORAGE_MAP64* POINTER_64 SystemAssemblyStorageMap;
    /* +0x318 */ ULONGLONG MinimumStackCommit;
    /* +0x320 */ VOID* POINTER_64 SparePointers[2];
    /* +0x330 */ VOID* POINTER_64 PatchLoaderData;
    /* +0x338 */ struct CHPEV2_PROCESS_INFO* POINTER_64 ChpeV2ProcessInfo;
    /* +0x340 */ ULONG AppModelFeatureState;
    /* +0x344 */ ULONG SpareUlongs[2];
    /* +0x34C */ USHORT ActiveCodePage;
    /* +0x34E */ USHORT OemCodePage;
    /* +0x350 */ USHORT UseCaseMapping;
    /* +0x352 */ USHORT UnusedNlsField;
    /* +0x358 */ VOID* POINTER_64 WerRegistrationData; // TODO: WER_PEB_HEADER_BLOCK64*
    /* +0x360 */ VOID* POINTER_64 WerShipAssertPtr;
    /* +0x368 */ VOID* POINTER_64 EcCodeBitMap;
    /* +0x370 */ VOID* POINTER_64 pImageHeaderHash;
    union
    {
        /* +0x378 */ ULONG TracingFlags;
        struct
        {
            /* +0x378 */ ULONG HeapTracingEnabled : 1;
            /* +0x378 */ ULONG CritSecTracingEnabled : 1;
            /* +0x378 */ ULONG LibLoaderTracingEnabled : 1;
            /* +0x378 */ ULONG SpareTracingBits : 29;
        };
    };
    /* +0x380 */ ULONGLONG CsrServerReadOnlySharedMemoryBase;
    /* +0x388 */ RTL_CRITICAL_SECTION64* POINTER_64 TppWorkerpListLock;
    /* +0x390 */ LIST_ENTRY64 TppWorkerpList;
    /* +0x3A0 */ VOID* POINTER_64 WaitOnAddressHashTable[128];
    /* +0x7A0 */ TELEMETRY_COVERAGE_HEADER* POINTER_64 TelemetryCoverageHeader;
    /* +0x7A8 */ ULONG CloudFileFlags;
    /* +0x7AC */ ULONG CloudFileDiagFlags;
    /* +0x7B0 */ CHAR PlaceholderCompatibilityMode;
    /* +0x7B1 */ CHAR PlaceholderCompatibilityModeReserved[7];
    /* +0x7B8 */ LEAP_SECOND_DATA* POINTER_64 LeapSecondData;
    union
    {
        /* +0x7C0 */ ULONG LeapSecondFlags;
        struct
        {
            /* +0x7C0 */ ULONG SixtySecondEnabled : 1;
            /* +0x7C0 */ ULONG Reserved : 31;
        };
    };
    /* +0x7C4 */ ULONG NtGlobalFlag2;
    /* +0x7C8 */ ULONGLONG ExtendedFeatureDisableMask;
} PEB64, *PPEB64;

typedef struct _PEB32
{
    /* +0x000 */ BOOLEAN InheritedAddressSpace;
    /* +0x001 */ BOOLEAN ReadImageFileExecOptions;
    /* +0x002 */ BOOLEAN BeingDebugged;
    union
    {
        /* +0x003 */ UCHAR BitField;
        struct
        {
            /* +0x003 */ UCHAR ImageUsesLargePages : 1;
            /* +0x003 */ UCHAR IsProtectedProcess : 1;
            /* +0x003 */ UCHAR IsImageDynamicallyRelocated : 1;
            /* +0x003 */ UCHAR SkipPatchingUser32Forwarders : 1;
            /* +0x003 */ UCHAR IsPackagedProcess : 1;
            /* +0x003 */ UCHAR IsAppContainer : 1;
            /* +0x003 */ UCHAR IsProtectedProcessLight : 1;
            /* +0x003 */ UCHAR IsLongPathAwareProcess : 1;
        };
    };
    /* +0x004 */ VOID* POINTER_32 Mutant;
    /* +0x008 */ VOID* POINTER_32 ImageBaseAddress;
    /* +0x00C */ PEB_LDR_DATA32* POINTER_32 Ldr;
    /* +0x010 */ RTL_USER_PROCESS_PARAMETERS32* POINTER_32 ProcessParameters;
    /* +0x014 */ VOID* POINTER_32 SubSystemData;
    /* +0x018 */ VOID* POINTER_32 ProcessHeap;
    /* +0x01C */ RTL_CRITICAL_SECTION32* POINTER_32 FastPebLock;
    /* +0x020 */ VOID* POINTER_32 AtlThunkSListPtr;
    /* +0x024 */ VOID* POINTER_32 IFEOKey;
    union
    {
        /* +0x028 */ ULONG CrossProcessFlags;
        struct
        {
            /* +0x028 */ ULONG ProcessInJob : 1;
            /* +0x028 */ ULONG ProcessInitializing : 1;
            /* +0x028 */ ULONG ProcessUsingVEH : 1;
            /* +0x028 */ ULONG ProcessUsingVCH : 1;
            /* +0x028 */ ULONG ProcessUsingFTH : 1;
            /* +0x028 */ ULONG ProcessPreviouslyThrottled : 1;
            /* +0x028 */ ULONG ProcessCurrentlyThrottled : 1;
            /* +0x028 */ ULONG ProcessImagesHotPatched : 1;
            /* +0x028 */ ULONG ReservedBits0 : 24;
        };
    };
    union
    {
        /* +0x02C */ VOID* POINTER_32 KernelCallbackTable; /* TODO: PKERNEL_CALLBACK_TABLE32 */
        /* +0x02C */ VOID* POINTER_32 UserSharedInfoPtr;
    };
    /* +0x030 */  ULONG SystemReserved;
    /* +0x034 */ ULONG AtlThunkSListPtr32;
    /* +0x038 */ VOID* POINTER_32 ApiSetMap;
    /* +0x03C */ ULONG TlsExpansionCounter;
    /* +0x040 */ RTL_BITMAP32* POINTER_32 TlsBitmap;
    /* +0x044 */ ULONG TlsBitmapBits[2];
    /* +0x04C */ VOID* POINTER_32 ReadOnlySharedMemoryBase;
    /* +0x050 */ VOID* POINTER_32 SharedData;
    /* +0x054 */ VOID* POINTER_32* POINTER_32 ReadOnlyStaticServerData;
    /* +0x058 */ VOID* POINTER_32 AnsiCodePageData; /* TODO: PCPTABLEINFO32 */
    /* +0x05C */ VOID* POINTER_32 OemCodePageData; /* TODO: PCPTABLEINFO32 */
    /* +0x060 */ VOID* POINTER_32 UnicodeCaseTableData; /* TODO: PNLSTABLEINFO32 */
    /* +0x064 */ ULONG NumberOfProcessors;
    /* +0x068 */ ULONG NtGlobalFlag;
    /* +0x070 */ LARGE_INTEGER CriticalSectionTimeout;
    /* +0x078 */ ULONG HeapSegmentReserve;
    /* +0x07C */ ULONG HeapSegmentCommit;
    /* +0x080 */ ULONG HeapDeCommitTotalFreeThreshold;
    /* +0x084 */ ULONG HeapDeCommitFreeBlockThreshold;
    /* +0x088 */ ULONG NumberOfHeaps;
    /* +0x08C */ ULONG MaximumNumberOfHeaps;
    /* +0x090 */ VOID* POINTER_32* POINTER_32 ProcessHeaps;
    /* +0x094 */ VOID* POINTER_32 GdiSharedHandleTable; /* TODO: PGDI_HANDLE_ENTRY32 */
    /* +0x098 */ VOID* POINTER_32 ProcessStarterHelper;
    /* +0x09C */ ULONG GdiDCAttributeList;
    /* +0x0A0 */ RTL_CRITICAL_SECTION32* POINTER_32 LoaderLock;
    /* +0x0A4 */ ULONG OSMajorVersion;
    /* +0x0A8 */ ULONG OSMinorVersion;
    /* +0x0AC */ USHORT OSBuildNumber;
    /* +0x0Ae */ USHORT OSCSDVersion;
    /* +0x0B0 */ ULONG OSPlatformId;
    /* +0x0B4 */ ULONG ImageSubsystem;
    /* +0x0B8 */ ULONG ImageSubsystemMajorVersion;
    /* +0x0BC */ ULONG ImageSubsystemMinorVersion;
    /* +0x0C0 */ ULONG ActiveProcessAffinityMask;
    /* +0x0C4 */ ULONG GdiHandleBuffer[34];
    /* +0x14C */ VOID* POINTER_32 PostProcessInitRoutine;
    /* +0x150 */ RTL_BITMAP32* POINTER_32 TlsExpansionBitmap;
    /* +0x154 */ ULONG TlsExpansionBitmapBits[32];
    /* +0x1D4 */ ULONG SessionId;
    /* +0x1D8 */ ULARGE_INTEGER AppCompatFlags;
    /* +0x1E0 */ ULARGE_INTEGER AppCompatFlagsUser;
    /* +0x1E8 */ VOID* POINTER_32 pShimData;
    /* +0x1EC */ VOID* POINTER_32 AppCompatInfo;
    /* +0x1F0 */ UNICODE_STRING32 CSDVersion;
    /* +0x1F8 */ ACTIVATION_CONTEXT_DATA* POINTER_32 ActivationContextData;
    /* +0x1FC */ ASSEMBLY_STORAGE_MAP32* POINTER_32 ProcessAssemblyStorageMap;
    /* +0x200 */ ACTIVATION_CONTEXT_DATA* POINTER_32 SystemDefaultActivationContextData;
    /* +0x204 */ ASSEMBLY_STORAGE_MAP32* POINTER_32 SystemAssemblyStorageMap;
    /* +0x208 */ ULONG MinimumStackCommit;
    /* +0x20C */ VOID* POINTER_32 SparePointers[2];
    /* +0x214 */ VOID* POINTER_32 PatchLoaderData;
    /* +0x218 */ struct CHPEV2_PROCESS_INFO* POINTER_32 ChpeV2ProcessInfo;
    /* +0x21C */ ULONG AppModelFeatureState;
    /* +0x220 */ ULONG SpareUlongs[2];
    /* +0x228 */ USHORT ActiveCodePage;
    /* +0x22A */ USHORT OemCodePage;
    /* +0x22C */ USHORT UseCaseMapping;
    /* +0x22E */ USHORT UnusedNlsField;
    /* +0x230 */ VOID* POINTER_32 WerRegistrationData;  // TODO: WER_PEB_HEADER_BLOCK32*
    /* +0x234 */ VOID* POINTER_32 WerShipAssertPtr;
    /* +0x238 */ VOID* POINTER_32 Spare;
    /* +0x23C */ VOID* POINTER_32 pImageHeaderHash;
    union
    {
        /* +0x240 */ ULONG TracingFlags;
        struct
        {
            /* +0x240 */ ULONG HeapTracingEnabled : 1;
            /* +0x240 */ ULONG CritSecTracingEnabled : 1;
            /* +0x240 */ ULONG LibLoaderTracingEnabled : 1;
            /* +0x240 */ ULONG SpareTracingBits : 29;
        };
    };
    /* +0x248 */ ULONGLONG CsrServerReadOnlySharedMemoryBase;
    /* +0x250 */ RTL_CRITICAL_SECTION32* POINTER_32 TppWorkerpListLock;
    /* +0x254 */ LIST_ENTRY32 TppWorkerpList;
    /* +0x25c */ VOID* POINTER_32 WaitOnAddressHashTable[128];
    /* +0x45c */ TELEMETRY_COVERAGE_HEADER* POINTER_32 TelemetryCoverageHeader;
    /* +0x460 */ ULONG CloudFileFlags;
    /* +0x464 */ ULONG CloudFileDiagFlags;
    /* +0x468 */ CHAR PlaceholderCompatibilityMode;
    /* +0x469 */ CHAR PlaceholderCompatibilityModeReserved[7];
    /* +0x470 */ LEAP_SECOND_DATA* POINTER_32 LeapSecondData;
    union
    {
        /* +0x474 */ ULONG LeapSecondFlags;
        struct
        {
            /* +0x474 */ ULONG SixtySecondEnabled : 1;
            /* +0x474 */ ULONG Reserved : 31;
        };
    };
    /* +0x478 */ ULONG NtGlobalFlag2;
    /* +0x480 */ ULONGLONG ExtendedFeatureDisableMask;
} PEB32, *PPEB32;

#if defined(_WIN64)
_STATIC_ASSERT(sizeof(PEB) == sizeof(PEB64));
#else
_STATIC_ASSERT(sizeof(PEB) == sizeof(PEB32));
#endif

/**
 * The GDI_TEB_BATCH structure is used to store information about GDI batch operations.
 */
typedef struct _GDI_TEB_BATCH
{
    struct
    {
        ULONG Offset : 31;
        ULONG HasRenderingCommand : 1;
    };
    ULONG_PTR HDC;
    ULONG Buffer[310];
} GDI_TEB_BATCH, *PGDI_TEB_BATCH;

typedef struct _GDI_TEB_BATCH64
{
    struct
    {
        ULONG Offset : 31;
        ULONG HasRenderingCommand : 1;
    };
    ULONGLONG HDC;
    ULONG Buffer[310];
} GDI_TEB_BATCH64, *PGDI_TEB_BATCH64;

typedef struct _GDI_TEB_BATCH32
{
    struct
    {
        ULONG Offset : 31;
        ULONG HasRenderingCommand : 1;
    };
    ULONG HDC;
    ULONG Buffer[310];
} GDI_TEB_BATCH32, *PGDI_TEB_BATCH32;

#define TEB_ACTIVE_FRAME_CONTEXT_FLAG_EXTENDED (0x00000001)

/**
 * The TEB_ACTIVE_FRAME_CONTEXT structure is used to store information about an active frame context.
 */
typedef struct _TEB_ACTIVE_FRAME_CONTEXT
{
    ULONG Flags;
    PCSTR FrameName;
} TEB_ACTIVE_FRAME_CONTEXT, *PTEB_ACTIVE_FRAME_CONTEXT;

/**
 * The TEB_ACTIVE_FRAME_CONTEXT_EX structure extends TEB_ACTIVE_FRAME_CONTEXT with additional information.
 */
typedef struct _TEB_ACTIVE_FRAME_CONTEXT_EX
{
    TEB_ACTIVE_FRAME_CONTEXT BasicContext;
    PCSTR SourceLocation;
} TEB_ACTIVE_FRAME_CONTEXT_EX, *PTEB_ACTIVE_FRAME_CONTEXT_EX;

#define TEB_ACTIVE_FRAME_FLAG_EXTENDED (0x00000001)

/**
 * The TEB_ACTIVE_FRAME structure is used to store information about an active frame.
 */
typedef struct _TEB_ACTIVE_FRAME TEB_ACTIVE_FRAME, *PTEB_ACTIVE_FRAME;
struct _TEB_ACTIVE_FRAME
{
    ULONG Flags;
    PTEB_ACTIVE_FRAME Previous;
    PTEB_ACTIVE_FRAME_CONTEXT Context;
};

/**
 * The TEB_ACTIVE_FRAME_EX structure extends TEB_ACTIVE_FRAME with additional information.
 */
typedef struct _TEB_ACTIVE_FRAME_EX
{
    TEB_ACTIVE_FRAME BasicFrame;
    PVOID ExtensionIdentifier;
} TEB_ACTIVE_FRAME_EX, *PTEB_ACTIVE_FRAME_EX;

typedef struct _TEB_ACTIVE_FRAME_CONTEXT64
{
    ULONG Flags;
    UCHAR Padding[4];
    CONST CHAR* POINTER_64 FrameName;
} TEB_ACTIVE_FRAME_CONTEXT64, *PTEB_ACTIVE_FRAME_CONTEXT64;

typedef struct _TEB_ACTIVE_FRAME_CONTEXT32
{
    ULONG Flags;
    UCHAR Padding[4];
    CONST CHAR* POINTER_32 FrameName;
} TEB_ACTIVE_FRAME_CONTEXT32, *PTEB_ACTIVE_FRAME_CONTEXT32;

typedef struct _TEB_ACTIVE_FRAME64 TEB_ACTIVE_FRAME64, *PTEB_ACTIVE_FRAME64;
typedef struct _TEB_ACTIVE_FRAME32 TEB_ACTIVE_FRAME32, *PTEB_ACTIVE_FRAME32;

struct _TEB_ACTIVE_FRAME64
{
    DWORD Flags;
    UCHAR Padding[4];
    TEB_ACTIVE_FRAME64* POINTER_64 Previous;
    TEB_ACTIVE_FRAME_CONTEXT64* POINTER_64 Context;
};

struct _TEB_ACTIVE_FRAME32
{
    DWORD Flags;
    TEB_ACTIVE_FRAME32* POINTER_32 Previous;
    TEB_ACTIVE_FRAME_CONTEXT32* POINTER_32 Context;
};

#define OLETLSF_LOCALTID 0x01 // This TID is in the current process.
#define OLETLSF_UUIDINITIALIZED 0x02 // This Logical thread is init'd.
#define OLETLSF_INTHREADDETACH 0x04 // This is in thread detach.
#define OLETLSF_CHANNELTHREADINITIALZED 0x08,// This channel has been init'd
#define OLETLSF_WOWTHREAD 0x10 // This thread is a 16-bit WOW thread.
#define OLETLSF_THREADUNINITIALIZING 0x20 // This thread is in CoUninitialize.
#define OLETLSF_DISABLE_OLE1DDE 0x40 // This thread can't use a DDE window.
#define OLETLSF_APARTMENTTHREADED 0x80 // This is an STA apartment thread
#define OLETLSF_MULTITHREADED 0x100 // This is an MTA apartment thread
#define OLETLSF_IMPERSONATING 0x200 // This thread is impersonating
#define OLETLSF_DISABLE_EVENTLOGGER 0x400 // Prevent recursion in event logger
#define OLETLSF_INNEUTRALAPT 0x800 // This thread is in the NTA
#define OLETLSF_DISPATCHTHREAD 0x1000 // This is a dispatch thread
#define OLETLSF_HOSTTHREAD 0x2000 // This is a host thread
#define OLETLSF_ALLOWCOINIT 0x4000 // This thread allows inits
#define OLETLSF_PENDINGUNINIT 0x8000 // This thread has pending uninit
#define OLETLSF_FIRSTMTAINIT 0x10000,// First thread to attempt an MTA init
#define OLETLSF_FIRSTNTAINIT 0x20000,// First thread to attempt an NTA init
#define OLETLSF_APTINITIALIZING 0x40000 // Apartment Object is initializing
#define OLETLSF_UIMSGSINMODALLOOP 0x80000,
#define OLETLSF_MARSHALING_ERROR_OBJECT 0x100000 // since WIN8
#define OLETLSF_WINRT_INITIALIZE 0x200000 // This thread called RoInitialize
#define OLETLSF_APPLICATION_STA 0x400000,
#define OLETLSF_IN_SHUTDOWN_CALLBACKS 0x800000,
#define OLETLSF_POINTER_INPUT_BLOCKED 0x1000000,
#define OLETLSF_IN_ACTIVATION_FILTER 0x2000000 // since WINBLUE
#define OLETLSF_ASTATOASTAEXEMPT_QUIRK 0x4000000,
#define OLETLSF_ASTATOASTAEXEMPT_PROXY 0x8000000,
#define OLETLSF_ASTATOASTAEXEMPT_INDOUBT 0x10000000,
#define OLETLSF_DETECTED_USER_INITIALIZED 0x20000000 // since RS3
#define OLETLSF_BRIDGE_STA 0x40000000 // since RS5
#define OLETLSF_NAINITIALIZING 0x80000000UL // since 19H1

// private
typedef struct _OLE_TLS_DATA
{
    PVOID ThreadBase;
    PVOID SmAllocator;
    ULONG ApartmentID;
    ULONG Flags; // OLETLSF_*
    LONG TlsMapIndex;
    PVOID *TlsSlot;
    ULONG ComInits;
    ULONG OleInits;
    ULONG Calls;
    PVOID ServerCall; // previously CallInfo (before TH1)
    PVOID CallObjectCache; // previously FreeAsyncCall (before TH1)
    PVOID ContextStack; // previously FreeClientCall (before TH1)
    PVOID ObjServer;
    ULONG TIDCaller;
    // ... (other fields are version-dependant)
} OLE_TLS_DATA, *POLE_TLS_DATA;

typedef struct _OLE_TLS_DATA64
{
    VOID* POINTER_64 ThreadBase;
    VOID* POINTER_64 SmAllocator;
    ULONG ApartmentID;
    ULONG Flags;
    LONG TlsMapIndex;
    PVOID64* POINTER_64 TlsSlot;
    ULONG ComInits;
    ULONG OleInits;
    ULONG Calls;
    VOID* POINTER_64 ServerCall;
    VOID* POINTER_64 CallObjectCache;
    VOID* POINTER_64 ContextStack;
    VOID* POINTER_64 ObjServer;
    ULONG TIDCaller;
} OLE_TLS_DATA64, *POLE_TLS_DATA64;

typedef struct _OLE_TLS_DATA32
{
    VOID* POINTER_32 ThreadBase;
    VOID* POINTER_32 SmAllocator;
    ULONG ApartmentID;
    ULONG Flags;
    LONG TlsMapIndex;
    PVOID32* POINTER_32 TlsSlot;
    ULONG ComInits;
    ULONG OleInits;
    ULONG Calls;
    VOID* POINTER_32 ServerCall;
    VOID* POINTER_32 CallObjectCache;
    VOID* POINTER_32 ContextStack;
    VOID* POINTER_32 ObjServer;
    ULONG TIDCaller;
} OLE_TLS_DATA32, *POLE_TLS_DATA32;

// rev - xor key for ReservedForNtRpc
#define RPC_THREAD_POINTER_KEY64 0xABABABABDEDEDEDEui64
#define RPC_THREAD_POINTER_KEY32 0xABABABAB
#ifdef _WIN64
#define RPC_THREAD_POINTER_KEY RPC_THREAD_POINTER_KEY64i64
#else
#define RPC_THREAD_POINTER_KEY RPC_THREAD_POINTER_KEY32
#endif

typedef struct _RTL_PERTHREAD_CURDIR
{
    PRTL_DRIVE_LETTER_CURDIR CurrentDirectories;
    PUNICODE_STRING ImageName;
    PVOID Environment;
} RTL_PERTHREAD_CURDIR, *PRTL_PERTHREAD_CURDIR;

typedef struct _RTL_PERTHREAD_CURDIR64
{
    RTL_DRIVE_LETTER_CURDIR64* POINTER_64 CurrentDirectories;
    UNICODE_STRING64* POINTER_64 ImageName;
    VOID* POINTER_64 Environment;
} RTL_PERTHREAD_CURDIR64, *PRTL_PERTHREAD_CURDIR64;

typedef struct _RTL_PERTHREAD_CURDIR32
{
    RTL_DRIVE_LETTER_CURDIR32* POINTER_32 CurrentDirectories;
    UNICODE_STRING32* POINTER_32 ImageName;
    VOID* POINTER_32 Environment;
} RTL_PERTHREAD_CURDIR32, *PRTL_PERTHREAD_CURDIR32;

/**
 * Thread Environment Block (TEB) structure.
 *
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winternl/ns-winternl-teb
 */
typedef struct _TEB
{
    NT_TIB NtTib;                                   // Thread Information Block (TIB) contains the thread's stack, base and limit addresses, the current stack pointer, and the exception list.
    PVOID EnvironmentPointer;                       // Reserved.
    CLIENT_ID ClientId;                             // Client ID for this thread.
    PVOID ActiveRpcHandle;                          // A handle to an active Remote Procedure Call (RPC) if the thread is currently involved in an RPC operation.
    PVOID ThreadLocalStoragePointer;                // A pointer to the __declspec(thread) local storage array.
    PPEB ProcessEnvironmentBlock;                   // A pointer to the Process Environment Block (PEB), which contains information about the process.
    ULONG LastErrorValue;                           // The previous Win32 error value for this thread.
    ULONG CountOfOwnedCriticalSections;             // The number of critical sections currently owned by this thread.
    PVOID CsrClientThread;
    PVOID Win32ThreadInfo;                          // Reserved for win32k.sys.
    ULONG User32Reserved[26];                       // Reserved for user32.dll.
    ULONG UserReserved[5];                          // Reserved for winsrv.dll.
    PVOID WOW32Reserved;
    LCID CurrentLocale;                             // The LCID of the current thread. (Kernel32!GetThreadLocale)
    ULONG FpSoftwareStatusRegister;
    PVOID ReservedForDebuggerInstrumentation[16];

#ifdef _WIN64
    PVOID SystemReserved1[25];                      // Reserved for floating-point emulation.
#else
    PVOID SystemReserved1[21];
#endif

    PVOID HeapFlsData;                                          // Per-thread fiber local storage. (Teb->HasFiberData)
    ULONG_PTR RngState[4];
    CHAR PlaceholderCompatibilityMode;                          // Placeholder compatibility mode. (ProjFs and Cloud Files)
    BOOLEAN PlaceholderHydrationAlwaysExplicit;
    CHAR PlaceholderReserved[10];                               // ProjFs and Cloud Files (reparse point) file virtualization.
    ULONG ProxiedProcessId;                                     // The process ID (PID) that the current COM server thread is acting on behalf of.
    ACTIVATION_CONTEXT_STACK ActivationStack;
    UCHAR WorkingOnBehalfTicket[8];                             // Opaque operation on behalf of another user or process.
    NTSTATUS ExceptionCode;                                     // The last exception status for the current thread.
    PACTIVATION_CONTEXT_STACK ActivationContextStackPointer;    // Pointer to the activation context stack for the current thread.
    ULONG_PTR InstrumentationCallbackSp;                        // The stack pointer (SP) of the current system call or exception during instrumentation.
    ULONG_PTR InstrumentationCallbackPreviousPc;                // The program counter (PC) of the previous system call or exception during instrumentation.
    ULONG_PTR InstrumentationCallbackPreviousSp;                // The stack pointer (SP) of the previous system call or exception during instrumentation.

#ifdef _WIN64
    ULONG TxFsContext;                          // The miniversion ID of the current transacted file operation.
    BOOLEAN InstrumentationCallbackDisabled;    // Indicates the state of the system call or exception instrumentation callback.
    BOOLEAN UnalignedLoadStoreExceptions;       // Indicates the state of alignment exceptions for unaligned load/store operations.
#else
    BOOLEAN InstrumentationCallbackDisabled;
    UCHAR SpareBytes[23];
    ULONG TxFsContext;
#endif

    // Reserved for GDI (Win32k).
    GDI_TEB_BATCH GdiTebBatch;
    CLIENT_ID RealClientId;
    HANDLE GdiCachedProcessHandle;
    ULONG GdiClientPID;
    ULONG GdiClientTID;
    PVOID GdiThreadLocalInfo;

    ULONG_PTR Win32ClientInfo[62];  // Reserved for User32 (Win32k).

    // Reserved for opengl32.dll
    PVOID glDispatchTable[233];
    ULONG_PTR glReserved1[29];
    PVOID glReserved2;
    PVOID glSectionInfo;
    PVOID glSection;
    PVOID glTable;
    PVOID glCurrentRC;
    PVOID glContext;

    NTSTATUS LastStatusValue;               // The previous status value for this thread.
    UNICODE_STRING StaticUnicodeString;     // A static string for use by the application.
    WCHAR StaticUnicodeBuffer[261];         // A static buffer for use by the application.
    PVOID DeallocationStack;                // The maximum stack size and indicates the base of the stack.
    PVOID TlsSlots[TLS_MINIMUM_AVAILABLE];  // Data for Thread Local Storage. (TlsGetValue)
    LIST_ENTRY TlsLinks;                    // Reserved for TLS.
    PVOID Vdm;                              // Reserved for NTVDM.
    PVOID ReservedForNtRpc;                 // Reserved for RPC. The pointer is XOR'ed with RPC_THREAD_POINTER_KEY.
    PVOID DbgSsReserved[2];
    ULONG HardErrorMode;                    // The error mode for the current thread. (GetThreadErrorMode)

#ifdef _WIN64
    PVOID Instrumentation[11];
#else
    PVOID Instrumentation[9];
#endif

    GUID ActivityId;
    PVOID SubProcessTag;    // The identifier of the service that created the thread. (svchost)
    PVOID PerflibData;
    PVOID EtwTraceData;
    PVOID WinSockData;      // The address of a socket handle during a blocking socket operation. (WSAStartup)
    ULONG GdiBatchCount;    // The number of function calls accumulated in the current GDI batch. (GdiSetBatchLimit)

    union
    {
        PROCESSOR_NUMBER CurrentIdealProcessor; // The preferred processor for the curremt thread. (SetThreadIdealProcessor/SetThreadIdealProcessorEx)
        ULONG IdealProcessorValue;
        struct
        {
            UCHAR ReservedPad0;
            UCHAR ReservedPad1;
            UCHAR ReservedPad2;
            UCHAR IdealProcessor;
        };
    };

    ULONG GuaranteedStackBytes;         // The minimum size of the stack available during any stack overflow exceptions. (SetThreadStackGuarantee)
    PVOID ReservedForPerf;
    POLE_TLS_DATA ReservedForOle;       // Reserved for Object Linking and Embedding (OLE)
    ULONG WaitingOnLoaderLock;          // Indicates whether the thread is waiting on the loader lock.
    PVOID SavedPriorityState;           // The saved priority state for the thread.
    ULONG_PTR ReservedForCodeCoverage;
    PVOID ThreadPoolData;
    PVOID* TlsExpansionSlots;           // Pointer to the TLS (Thread Local Storage) expansion slots for the thread.

#ifdef _WIN64
    struct CHPEV2_CPUAREA_INFO* ChpeV2CpuAreaInfo;  // CHPEV2_CPUAREA_INFO // previously DeallocationBStore
    PVOID Unused;                                   // previously BStoreLimit
#endif

    ULONG MuiGeneration;                // The generation of the MUI (Multilingual User Interface) data.
    ULONG IsImpersonating;              // Indicates whether the thread is impersonating another security context.
    PVOID NlsCache;                     // Pointer to the NLS (National Language Support) cache.
    PVOID pShimData;                    // Pointer to the AppCompat/Shim Engine data.
    ULONG HeapData;
    HANDLE CurrentTransactionHandle;    // Handle to the current transaction associated with the thread.
    PTEB_ACTIVE_FRAME ActiveFrame;      // Pointer to the active frame for the thread.
    PVOID FlsData;
    PVOID PreferredLanguages;           // Pointer to the preferred languages for the current thread. (GetThreadPreferredUILanguages)
    PVOID UserPrefLanguages;            // Pointer to the user-preferred languages for the current thread. (GetUserPreferredUILanguages)
    PVOID MergedPrefLanguages;          // Pointer to the merged preferred languages for the current thread. (MUI_MERGE_USER_FALLBACK)
    ULONG MuiImpersonation;             // Indicates whether the thread is impersonating another user's language settings.

    union
    {
        USHORT CrossTebFlags;
        USHORT SpareCrossTebBits : 16;
    };

    // SameTebFlags modify the state and behavior of the current thread.
    union
    {
        USHORT SameTebFlags;
        struct
        {
            USHORT SafeThunkCall : 1;
            USHORT InDebugPrint : 1;            // Indicates if the thread is currently in a debug print routine.
            USHORT HasFiberData : 1;            // Indicates if the thread has local fiber-local storage (FLS).
            USHORT SkipThreadAttach : 1;        // Indicates if the thread should suppress DLL_THREAD_ATTACH notifications.
            USHORT WerInShipAssertCode : 1;
            USHORT RanProcessInit : 1;          // Indicates if the thread has run process initialization code.
            USHORT ClonedThread : 1;            // Indicates if the thread is a clone of a different thread.
            USHORT SuppressDebugMsg : 1;        // Indicates if the thread should suppress LOAD_DLL_DEBUG_INFO notifications.
            USHORT DisableUserStackWalk : 1;
            USHORT RtlExceptionAttached : 1;
            USHORT InitialThread : 1;           // Indicates if the thread is the initial thread of the process.
            USHORT SessionAware : 1;
            USHORT LoadOwner : 1;               // Indicates if the thread is the owner of the process loader lock.
            USHORT LoaderWorker : 1;
            USHORT SkipLoaderInit : 1;
            USHORT SkipFileAPIBrokering : 1;
        };
    };

    PVOID TxnScopeEnterCallback;            // Pointer to the callback function that is called when a KTM transaction scope is entered.
    PVOID TxnScopeExitCallback;             // Pointer to the callback function that is called when a KTM transaction scope is exited.
    PVOID TxnScopeContext;                  // Pointer to optional context data for use by the application when a KTM transaction scope callback is called.
    ULONG LockCount;                        // The lock count of critical sections for the current thread.
    LONG WowTebOffset;                      // The offset to the WOW64 (Windows on Windows) TEB for the current thread.
    PLDR_RESLOADER_RET ResourceRetValue;
    PVOID ReservedForWdf;                   // Reserved for Windows Driver Framework (WDF).
    ULONGLONG ReservedForCrt;               // Reserved for the Microsoft C runtime (CRT).
    GUID EffectiveContainerId;              // The Host Compute Service (HCS) container identifier.

    // Reserved for Kernel32!Sleep (SpinWait).
    ULONGLONG LastSleepCounter; // since Win11
    ULONG SpinCallCount;

    ULONGLONG ExtendedFeatureDisableMask;   // Extended feature disable mask (AVX).
    PVOID SchedulerSharedDataSlot;          // since 24H2
    PVOID HeapWalkContext;
    GROUP_AFFINITY PrimaryGroupAffinity;    // The primary processor group affinity of the thread.
    ULONG Rcu[2];                           // Read-copy-update (RCU) synchronization context.
} TEB, *PTEB;

typedef struct _TEB64
{
    /* +0x0000 */ NT_TIB64 NtTib;
    /* +0x0038 */ VOID* POINTER_64 EnvironmentPointer;
    /* +0x0040 */ CLIENT_ID64 ClientId;
    /* +0x0050 */ VOID* POINTER_64 ActiveRpcHandle;
    /* +0x0058 */ VOID* POINTER_64 ThreadLocalStoragePointer;
    /* +0x0060 */ PEB64* POINTER_64 ProcessEnvironmentBlock;
    /* +0x0068 */ ULONG LastErrorValue;
    /* +0x006C */ ULONG CountOfOwnedCriticalSections;
    /* +0x0070 */ VOID* POINTER_64 CsrClientThread;
    /* +0x0078 */ VOID* POINTER_64 Win32ThreadInfo;
    /* +0x0080 */ ULONG User32Reserved[26];
    /* +0x00E8 */ ULONG UserReserved[5];
    /* +0x0100 */ VOID* POINTER_64 WOW32Reserved;
    /* +0x0108 */ LCID CurrentLocale;
    /* +0x010C */ ULONG FpSoftwareStatusRegister;
    /* +0x0110 */ VOID* POINTER_64 ReservedForDebuggerInstrumentation[16];
    /* +0x0190 */ VOID* POINTER_64 SystemReserved1[25];
    /* +0x0258 */ PVOID HeapFlsData;
    /* +0x0260 */ ULONG_PTR RngState[4];
    /* +0x0280 */ CHAR PlaceholderCompatibilityMode;
    /* +0x0281 */ BOOLEAN PlaceholderHydrationAlwaysExplicit;
    /* +0x0282 */ CHAR PlaceholderReserved[10];
    /* +0x028C */ ULONG ProxiedProcessId;
    /* +0x0290 */ ACTIVATION_CONTEXT_STACK64 _ActivationStack;
    /* +0x02B8 */ UCHAR WorkingOnBehalfTicket[8];
    /* +0x02C0 */ NTSTATUS ExceptionCode;
    /* +0x02C8 */ ACTIVATION_CONTEXT_STACK64* POINTER_64 ActivationContextStackPointer;
    /* +0x02D0 */ ULONGLONG InstrumentationCallbackSp;
    /* +0x02D8 */ ULONGLONG InstrumentationCallbackPreviousPc;
    /* +0x02E0 */ ULONGLONG InstrumentationCallbackPreviousSp;
    /* +0x02E8 */ ULONG TxFsContext;
    /* +0x02EC */ BOOLEAN InstrumentationCallbackDisabled;
    /* +0x02ED */ BOOLEAN UnalignedLoadStoreExceptions;
    /* +0x02F0 */ GDI_TEB_BATCH64 GdiTebBatch;
    /* +0x07D8 */ CLIENT_ID64 RealClientId;
    /* +0x07E8 */ VOID* POINTER_64 GdiCachedProcessHandle;
    /* +0x07F0 */ ULONG GdiClientPID;
    /* +0x07F4 */ ULONG GdiClientTID;
    /* +0x07F8 */ VOID* POINTER_64 GdiThreadLocalInfo;
    /* +0x0800 */ ULONGLONG Win32ClientInfo[62];
    /* +0x09F0 */ VOID* POINTER_64 glDispatchTable[233];
    /* +0x1138 */ ULONGLONG glReserved1[29];
    /* +0x1220 */ VOID* POINTER_64 glReserved2;
    /* +0x1228 */ VOID* POINTER_64 glSectionInfo;
    /* +0x1230 */ VOID* POINTER_64 glSection;
    /* +0x1238 */ VOID* POINTER_64 glTable;
    /* +0x1240 */ VOID* POINTER_64 glCurrentRC;
    /* +0x1248 */ VOID* POINTER_64 glContext;
    /* +0x1250 */ NTSTATUS LastStatusValue;
    /* +0x1254 */ UCHAR Padding2[4];
    /* +0x1258 */ UNICODE_STRING64 StaticUnicodeString;
    /* +0x1268 */ WCHAR StaticUnicodeBuffer[261];
    /* +0x1478 */ VOID* POINTER_64 DeallocationStack;
    /* +0x1480 */ VOID* POINTER_64 TlsSlots[TLS_MINIMUM_AVAILABLE];
    /* +0x1680 */ LIST_ENTRY64 TlsLinks;
    /* +0x1690 */ VOID* POINTER_64 Vdm;
    /* +0x1698 */ VOID* POINTER_64 ReservedForNtRpc;
    /* +0x16A0 */ VOID* POINTER_64 DbgSsReserved[2];
    /* +0x16B0 */ ULONG HardErrorMode;
    /* +0x16B8 */ VOID* POINTER_64 Instrumentation[11];
    /* +0x1710 */ GUID ActivityId;
    /* +0x1720 */ VOID* POINTER_64 SubProcessTag;
    /* +0x1728 */ VOID* POINTER_64 PerflibData;
    /* +0x1730 */ VOID* POINTER_64 EtwTraceData;
    /* +0x1738 */ VOID* POINTER_64 WinSockData;
    /* +0x1740 */ ULONG GdiBatchCount;
    union
    {
        /* +0x1744 */ PROCESSOR_NUMBER CurrentIdealProcessor;
        union
        {
            /* +0x1744 */ ULONG IdealProcessorValue;
            struct
            {
                /* +0x1744 */ UCHAR ReservedPad0;
                /* +0x1745 */ UCHAR ReservedPad1;
                /* +0x1746 */ UCHAR ReservedPad2;
                /* +0x1747 */ UCHAR IdealProcessor;
            };
        };
    };
    /* +0x1748 */ ULONG GuaranteedStackBytes;
    /* +0x174C */ UCHAR Padding5[4];
    /* +0x1750 */ VOID* POINTER_64 ReservedForPerf;
    /* +0x1758 */ OLE_TLS_DATA64* POINTER_64 ReservedForOle;
    /* +0x1760 */ ULONG WaitingOnLoaderLock;
    /* +0x1768 */ VOID* POINTER_64 SavedPriorityState;
    /* +0x1770 */ ULONGLONG ReservedForCodeCoverage;
    /* +0x1778 */ VOID* POINTER_64 ThreadPoolData;
    /* +0x1780 */ VOID* POINTER_64* POINTER_64 TlsExpansionSlots;
    /* +0x1788 */ struct CHPEV2_CPUAREA_INFO* POINTER_64 ChpeV2CpuAreaInfo;
    /* +0x1790 */ VOID* POINTER_64 Unused;
    /* +0x1798 */ ULONG MuiGeneration;
    /* +0x179C */ ULONG IsImpersonating;
    /* +0x17A0 */ VOID* POINTER_64 NlsCache;
    /* +0x17A8 */ VOID* POINTER_64 pShimData;
    /* +0x17B0 */ ULONG HeapData;
    /* +0x17B8 */ VOID* POINTER_64 CurrentTransactionHandle;
    /* +0x17C0 */ TEB_ACTIVE_FRAME64* POINTER_64 ActiveFrame;
    /* +0x17C8 */ VOID* POINTER_64 FlsData;
    /* +0x17D0 */ VOID* POINTER_64 PreferredLanguages;
    /* +0x17D8 */ VOID* POINTER_64 UserPrefLanguages;
    /* +0x17E0 */ VOID* POINTER_64 MergedPrefLanguages;
    /* +0x17E8 */ ULONG MuiImpersonation;
    union
    {
        /* +0x17EC */ USHORT CrossTebFlags;
        struct
        {
            /* +0x17EC */ USHORT SpareCrossTebBits : 16;
        };
    };
    union
    {
        /* +0x17EE */ USHORT SameTebFlags;
        struct
        {
            /* +0x17EE */ USHORT SafeThunkCall : 1;
            /* +0x17EE */ USHORT InDebugPrint : 1;
            /* +0x17EE */ USHORT HasFiberData : 1;
            /* +0x17EE */ USHORT SkipThreadAttach : 1;
            /* +0x17EE */ USHORT WerInShipAssertCode : 1;
            /* +0x17EE */ USHORT RanProcessInit : 1;
            /* +0x17EE */ USHORT ClonedThread : 1;
            /* +0x17EE */ USHORT SuppressDebugMsg : 1;
            /* +0x17EE */ USHORT DisableUserStackWalk : 1;
            /* +0x17EE */ USHORT RtlExceptionAttached : 1;
            /* +0x17EE */ USHORT InitialThread : 1;
            /* +0x17EE */ USHORT SessionAware : 1;
            /* +0x17EE */ USHORT LoadOwner : 1;
            /* +0x17EE */ USHORT LoaderWorker : 1;
            /* +0x17EE */ USHORT SkipLoaderInit : 1;
            /* +0x17EE */ USHORT SkipFileAPIBrokering : 1;
        };
    };
    /* +0x17F0 */ VOID* POINTER_64 TxnScopeEnterCallback;
    /* +0x17F8 */ VOID* POINTER_64 TxnScopeExitCallback;
    /* +0x1800 */ VOID* POINTER_64 TxnScopeContext;
    /* +0x1808 */ ULONG LockCount;
    /* +0x180C */ LONG WowTebOffset;
    /* +0x1810 */ LDR_RESLOADER_RET64* POINTER_64 ResourceRetValue;
    /* +0x1818 */ VOID* POINTER_64 ReservedForWdf;
    /* +0x1820 */ ULONGLONG ReservedForCrt;
    /* +0x1828 */ GUID EffectiveContainerId;
    /* +0x1838 */ ULONGLONG LastSleepCounter;
    /* +0x1840 */ ULONG SpinCallCount;
    /* +0x1848 */ ULONGLONG ExtendedFeatureDisableMask;
    /* +0x1850 */ VOID* POINTER_64 SchedulerSharedDataSlot;
    /* +0x1858 */ VOID* POINTER_64 HeapWalkContext;
    /* +0x1860 */ GROUP_AFFINITY PrimaryGroupAffinity;
    /* +0x1870 */ ULONG Rcu[2];
} TEB64, *PTEB64;

typedef struct _TEB32
{
    /* +0x0000 */ NT_TIB32 NtTib;
    /* +0x001C */ VOID* POINTER_32 EnvironmentPointer;
    /* +0x0020 */ CLIENT_ID32 ClientId;
    /* +0x0028 */ VOID* POINTER_32 ActiveRpcHandle;
    /* +0x002C */ VOID* POINTER_32 ThreadLocalStoragePointer;
    /* +0x0030 */ PEB32* POINTER_32 ProcessEnvironmentBlock;
    /* +0x0034 */ ULONG LastErrorValue;
    /* +0x0038 */ ULONG CountOfOwnedCriticalSections;
    /* +0x003C */ VOID* POINTER_32 CsrClientThread;
    /* +0x0040 */ VOID* POINTER_32 Win32ThreadInfo;
    /* +0x0044 */ ULONG User32Reserved[26];
    /* +0x00AC */ ULONG UserReserved[5];
    /* +0x00C0 */ VOID* POINTER_32 WOW32Reserved;
    /* +0x00C4 */ LCID CurrentLocale;
    /* +0x00C8 */ ULONG FpSoftwareStatusRegister;
    /* +0x00CC */ VOID* POINTER_32 ReservedForDebuggerInstrumentation[16];
    /* +0x010C */ VOID* POINTER_32 SystemReserved1[21];
    /* +0x0160 */ VOID* POINTER_32 HeapFlsData;
    /* +0x0164 */ ULONG RngState[4];
    /* +0x0174 */ CHAR PlaceholderCompatibilityMode;
    /* +0x0175 */ BOOLEAN PlaceholderHydrationAlwaysExplicit;
    /* +0x0176 */ CHAR PlaceholderReserved[10];
    /* +0x0180 */ ULONG ProxiedProcessId;
    /* +0x0184 */ ACTIVATION_CONTEXT_STACK32 _ActivationStack;
    /* +0x019C */ UCHAR WorkingOnBehalfTicket[8];
    /* +0x01A4 */ NTSTATUS ExceptionCode;
    /* +0x01A8 */ ACTIVATION_CONTEXT_STACK32* POINTER_32 ActivationContextStackPointer;
    /* +0x01AC */ ULONG InstrumentationCallbackSp;
    /* +0x01B0 */ ULONG InstrumentationCallbackPreviousPc;
    /* +0x01B4 */ ULONG InstrumentationCallbackPreviousSp;
    /* +0x01B8 */ UCHAR InstrumentationCallbackDisabled;
    /* +0x01B9 */ UCHAR SpareBytes[23];
    /* +0x01D0 */ ULONG TxFsContext;
    /* +0x01D4 */ GDI_TEB_BATCH32 GdiTebBatch;
    /* +0x06B4 */ CLIENT_ID32 RealClientId;
    /* +0x06BC */ VOID* POINTER_32 GdiCachedProcessHandle;
    /* +0x06C0 */ ULONG GdiClientPID;
    /* +0x06C4 */ ULONG GdiClientTID;
    /* +0x06C8 */ VOID* POINTER_32 GdiThreadLocalInfo;
    /* +0x06CC */ ULONG Win32ClientInfo[62];
    /* +0x07C4 */ VOID* POINTER_32 glDispatchTable[233];
    /* +0x0B68 */ ULONG glReserved1[29];
    /* +0x0BDC */ VOID* POINTER_32 glReserved2;
    /* +0x0BE0 */ VOID* POINTER_32 glSectionInfo;
    /* +0x0BE4 */ VOID* POINTER_32 glSection;
    /* +0x0BE8 */ VOID* POINTER_32 glTable;
    /* +0x0BEc */ VOID* POINTER_32 glCurrentRC;
    /* +0x0BF0 */ VOID* POINTER_32 glContext;
    /* +0x0BF4 */ NTSTATUS LastStatusValue;
    /* +0x0BF8 */ UNICODE_STRING32 StaticUnicodeString;
    /* +0x0C00 */ WCHAR StaticUnicodeBuffer[261];
    /* +0x0E0C */ VOID* POINTER_32 DeallocationStack;
    /* +0x0E10 */ VOID* POINTER_32 TlsSlots[TLS_MINIMUM_AVAILABLE];
    /* +0x0F10 */ LIST_ENTRY32 TlsLinks;
    /* +0x0F18 */ VOID* POINTER_32 Vdm;
    /* +0x0F1C */ VOID* POINTER_32 ReservedForNtRpc;
    /* +0x0F20 */ VOID* POINTER_32 DbgSsReserved[2];
    /* +0x0F28 */ ULONG HardErrorMode;
    /* +0x0F2C */ VOID* POINTER_32 Instrumentation[9];
    /* +0x0F50 */ GUID ActivityId;
    /* +0x0F60 */ VOID* POINTER_32 SubProcessTag;
    /* +0x0F64 */ VOID* POINTER_32 PerflibData;
    /* +0x0F68 */ VOID* POINTER_32 EtwTraceData;
    /* +0x0F6C */ VOID* POINTER_32 WinSockData;
    /* +0x0F70 */ ULONG GdiBatchCount;
    union
    {
        /* +0x0F74 */ PROCESSOR_NUMBER CurrentIdealProcessor;
        union
        {
            /* +0x0F74 */ ULONG IdealProcessorValue;
            struct
            {
                /* +0x0F74 */ UCHAR ReservedPad0;
                /* +0x0F75 */ UCHAR ReservedPad1;
                /* +0x0F76 */ UCHAR ReservedPad2;
                /* +0x0F77 */ UCHAR IdealProcessor;
            };
        };
    };
    /* +0x0F78 */ ULONG GuaranteedStackBytes;
    /* +0x0F7C */ VOID* POINTER_32 ReservedForPerf;
    /* +0x0F80 */ OLE_TLS_DATA32* POINTER_32 ReservedForOle;
    /* +0x0F84 */ ULONG WaitingOnLoaderLock;
    /* +0x0F88 */ VOID* POINTER_32 SavedPriorityState;
    /* +0x0F8C */ ULONG ReservedForCodeCoverage;
    /* +0x0F90 */ VOID* POINTER_32 ThreadPoolData;
    /* +0x0F94 */ VOID* POINTER_32 TlsExpansionSlots;
    /* +0x0F98 */ ULONG MuiGeneration;
    /* +0x0F9C */ ULONG IsImpersonating;
    /* +0x0FA0 */ VOID* POINTER_32 NlsCache;
    /* +0x0FA4 */ VOID* POINTER_32 pShimData;
    /* +0x0FA8 */ ULONG HeapData;
    /* +0x0FAC */ VOID* POINTER_32 CurrentTransactionHandle;
    /* +0x0FB0 */ TEB_ACTIVE_FRAME32* POINTER_32 ActiveFrame;
    /* +0x0FB4 */ VOID* POINTER_32 FlsData;
    /* +0x0FB8 */ VOID* POINTER_32 PreferredLanguages;
    /* +0x0FBC */ VOID* POINTER_32 UserPrefLanguages;
    /* +0x0FC0 */ VOID* POINTER_32 MergedPrefLanguages;
    /* +0x0FC4 */ ULONG MuiImpersonation;
    union
    {
        /* +0x0FC8 */ USHORT CrossTebFlags;
        struct
        {
            /* +0x0FC8 */ USHORT SpareCrossTebBits : 16;
        };
    };
    union
    {
        /* +0x0FCA */ USHORT SameTebFlags;
        struct
        {
            /* +0x0FCA */ USHORT SafeThunkCall : 1;
            /* +0x0FCA */ USHORT InDebugPrint : 1;
            /* +0x0FCA */ USHORT HasFiberData : 1;
            /* +0x0FCA */ USHORT SkipThreadAttach : 1;
            /* +0x0FCA */ USHORT WerInShipAssertCode : 1;
            /* +0x0FCA */ USHORT RanProcessInit : 1;
            /* +0x0FCA */ USHORT ClonedThread : 1;
            /* +0x0FCA */ USHORT SuppressDebugMsg : 1;
            /* +0x0FCA */ USHORT DisableUserStackWalk : 1;
            /* +0x0FCA */ USHORT RtlExceptionAttached : 1;
            /* +0x0FCA */ USHORT InitialThread : 1;
            /* +0x0FCA */ USHORT SessionAware : 1;
            /* +0x0FCA */ USHORT LoadOwner : 1;
            /* +0x0FCA */ USHORT LoaderWorker : 1;
            /* +0x0FCA */ USHORT SkipLoaderInit : 1;
            /* +0x0FCA */ USHORT SkipFileAPIBrokering : 1;
        };
    };
    /* +0x0FCC */ VOID* POINTER_32 TxnScopeEnterCallback;
    /* +0x0FD0 */ VOID* POINTER_32 TxnScopeExitCallback;
    /* +0x0FD4 */ VOID* POINTER_32 TxnScopeContext;
    /* +0x0FD8 */ ULONG LockCount;
    /* +0x0FDC */ LONG WowTebOffset;
    /* +0x0FE0 */ LDR_RESLOADER_RET32* POINTER_32 ResourceRetValue;
    /* +0x0FE4 */ VOID* POINTER_32 ReservedForWdf;
    /* +0x0FE8 */ ULONGLONG ReservedForCrt;
    /* +0x0FF0 */ GUID EffectiveContainerId;
    /* +0x1000 */ ULONGLONG LastSleepCounter;
    /* +0x1008 */ ULONG SpinCallCount;
    /* +0x1010 */ ULONGLONG ExtendedFeatureDisableMask;
    /* +0x1018 */ VOID* POINTER_32 SchedulerSharedDataSlot;
    /* +0x101C */ VOID* POINTER_32 HeapWalkContext;
    /* +0x1020 */ GROUP_AFFINITY PrimaryGroupAffinity;
    /* +0x102C */ ULONG Rcu[2];
} TEB32, *PTEB32;

#if defined(_WIN64)
_STATIC_ASSERT(sizeof(TEB) == sizeof(TEB64));
#else
_STATIC_ASSERT(sizeof(TEB) == sizeof(TEB32));
#endif

NTSYSAPI
PPEB
NTAPI
RtlGetCurrentPeb(VOID);

NTSYSAPI
NTSTATUS
NTAPI
RtlAcquirePebLock(VOID);

NTSYSAPI
NTSTATUS
NTAPI
RtlReleasePebLock(VOID);

NTSYSAPI
LOGICAL
NTAPI
RtlTryAcquirePebLock(VOID);

EXTERN_C_END
