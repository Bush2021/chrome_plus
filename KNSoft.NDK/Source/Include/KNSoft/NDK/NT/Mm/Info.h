#pragma once

#include "../MinDef.h"

EXTERN_C_START

/* phnt */

#pragma region Memory Information

typedef enum _MEMORY_INFORMATION_CLASS
{
    MemoryBasicInformation, // q: MEMORY_BASIC_INFORMATION
    MemoryWorkingSetInformation, // q: MEMORY_WORKING_SET_INFORMATION
    MemoryMappedFilenameInformation, // q: UNICODE_STRING
    MemoryRegionInformation, // q: MEMORY_REGION_INFORMATION
    MemoryWorkingSetExInformation, // q: MEMORY_WORKING_SET_EX_INFORMATION // since VISTA
    MemorySharedCommitInformation, // q: MEMORY_SHARED_COMMIT_INFORMATION // since WIN8
    MemoryImageInformation, // q: MEMORY_IMAGE_INFORMATION
    MemoryRegionInformationEx, // MEMORY_REGION_INFORMATION
    MemoryPrivilegedBasicInformation, // MEMORY_BASIC_INFORMATION
    MemoryEnclaveImageInformation, // MEMORY_ENCLAVE_IMAGE_INFORMATION // since REDSTONE3
    MemoryBasicInformationCapped, // 10
    MemoryPhysicalContiguityInformation, // MEMORY_PHYSICAL_CONTIGUITY_INFORMATION // since 20H1
    MemoryBadInformation, // since WIN11
    MemoryBadInformationAllProcesses, // since 22H1
    MemoryImageExtensionInformation, // MEMORY_IMAGE_EXTENSION_INFORMATION // since 24H2
    MaxMemoryInfoClass
} MEMORY_INFORMATION_CLASS;

// MEMORY_WORKING_SET_BLOCK->Protection
#define MEMORY_BLOCK_NOT_ACCESSED 0
#define MEMORY_BLOCK_READONLY 1
#define MEMORY_BLOCK_EXECUTABLE 2
#define MEMORY_BLOCK_EXECUTABLE_READONLY 3
#define MEMORY_BLOCK_READWRITE 4
#define MEMORY_BLOCK_COPYONWRITE 5
#define MEMORY_BLOCK_EXECUTABLE_READWRITE 6
#define MEMORY_BLOCK_EXECUTABLE_COPYONWRITE 7
#define MEMORY_BLOCK_NOT_ACCESSED_2 8
#define MEMORY_BLOCK_NON_CACHEABLE_READONLY 9
#define MEMORY_BLOCK_NON_CACHEABLE_EXECUTABLE 10
#define MEMORY_BLOCK_NON_CACHEABLE_EXECUTABLE_READONLY 11
#define MEMORY_BLOCK_NON_CACHEABLE_READWRITE 12
#define MEMORY_BLOCK_NON_CACHEABLE_COPYONWRITE 13
#define MEMORY_BLOCK_NON_CACHEABLE_EXECUTABLE_READWRITE 14
#define MEMORY_BLOCK_NON_CACHEABLE_EXECUTABLE_COPYONWRITE 15
#define MEMORY_BLOCK_NOT_ACCESSED_3 16
#define MEMORY_BLOCK_GUARD_READONLY 17
#define MEMORY_BLOCK_GUARD_EXECUTABLE 18
#define MEMORY_BLOCK_GUARD_EXECUTABLE_READONLY 19
#define MEMORY_BLOCK_GUARD_READWRITE 20
#define MEMORY_BLOCK_GUARD_COPYONWRITE 21
#define MEMORY_BLOCK_GUARD_EXECUTABLE_READWRITE 22
#define MEMORY_BLOCK_GUARD_EXECUTABLE_COPYONWRITE 23
#define MEMORY_BLOCK_NOT_ACCESSED_4 24
#define MEMORY_BLOCK_NON_CACHEABLE_GUARD_READONLY 25
#define MEMORY_BLOCK_NON_CACHEABLE_GUARD_EXECUTABLE 26
#define MEMORY_BLOCK_NON_CACHEABLE_GUARD_EXECUTABLE_READONLY 27
#define MEMORY_BLOCK_NON_CACHEABLE_GUARD_READWRITE 28
#define MEMORY_BLOCK_NON_CACHEABLE_GUARD_COPYONWRITE 29
#define MEMORY_BLOCK_NON_CACHEABLE_GUARD_EXECUTABLE_READWRITE 30
#define MEMORY_BLOCK_NON_CACHEABLE_GUARD_EXECUTABLE_COPYONWRITE 31

/**
 * The MEMORY_WORKING_SET_BLOCK structure contains working set information for a page.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-psapi_working_set_block
 */
typedef struct _MEMORY_WORKING_SET_BLOCK
{
    ULONG_PTR Protection : 5;       // The protection attributes of the page. This member can be one of above MEMORY_BLOCK_* values.
    ULONG_PTR ShareCount : 3;       // The number of processes that share this page. The maximum value of this member is 7.
    ULONG_PTR Shared : 1;           // If this bit is 1, the page is sharable; otherwise, the page is not sharable.
    ULONG_PTR Node : 3;             // The NUMA node where the physical memory should reside.
#ifdef _WIN64
    ULONG_PTR VirtualPage : 52;     // The address of the page in the virtual address space.
#else
    ULONG VirtualPage : 20;         // The address of the page in the virtual address space.
#endif
} MEMORY_WORKING_SET_BLOCK, *PMEMORY_WORKING_SET_BLOCK;

/**
 * The MEMORY_WORKING_SET_INFORMATION structure contains working set information for a process.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-psapi_working_set_information
 */
typedef struct _MEMORY_WORKING_SET_INFORMATION
{
    ULONG_PTR NumberOfEntries;
    _Field_size_(NumberOfEntries) MEMORY_WORKING_SET_BLOCK WorkingSetInfo[ANYSIZE_ARRAY];
} MEMORY_WORKING_SET_INFORMATION, *PMEMORY_WORKING_SET_INFORMATION;

// private
typedef struct _MEMORY_REGION_INFORMATION
{
    PVOID AllocationBase;
    ULONG AllocationProtect;
    union
    {
        ULONG RegionType;
        struct
        {
            ULONG Private : 1;
            ULONG MappedDataFile : 1;
            ULONG MappedImage : 1;
            ULONG MappedPageFile : 1;
            ULONG MappedPhysical : 1;
            ULONG DirectMapped : 1;
            ULONG SoftwareEnclave : 1; // REDSTONE3
            ULONG PageSize64K : 1;
            ULONG PlaceholderReservation : 1; // REDSTONE4
            ULONG MappedAwe : 1; // 21H1
            ULONG MappedWriteWatch : 1;
            ULONG PageSizeLarge : 1;
            ULONG PageSizeHuge : 1;
            ULONG Reserved : 19;
        };
    };
    SIZE_T RegionSize;
    SIZE_T CommitSize;
    ULONG_PTR PartitionId; // 19H1
    ULONG_PTR NodePreference; // 20H1
} MEMORY_REGION_INFORMATION, *PMEMORY_REGION_INFORMATION;

// private
typedef enum _MEMORY_WORKING_SET_EX_LOCATION
{
    MemoryLocationInvalid,
    MemoryLocationResident,
    MemoryLocationPagefile,
    MemoryLocationReserved
} MEMORY_WORKING_SET_EX_LOCATION;

/**
 * The MEMORY_WORKING_SET_EX_BLOCK structure contains extended working set information for a page.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-psapi_working_set_ex_block
 */
typedef union _MEMORY_WORKING_SET_EX_BLOCK
{
    ULONG_PTR Flags;
    union
    {
        struct
        {
            ULONG_PTR Valid : 1;                    // If this bit is 1, the subsequent members are valid; otherwise they should be ignored.
            ULONG_PTR ShareCount : 3;               // The number of processes that share this page. The maximum value of this member is 7.
            ULONG_PTR Win32Protection : 11;         // The memory protection attributes of the page.
            ULONG_PTR Shared : 1;                   // If this bit is 1, the page can be shared.
            ULONG_PTR Node : 6;                     // The NUMA node. The maximum value of this member is 63.
            ULONG_PTR Locked : 1;                   // If this bit is 1, the virtual page is locked in physical memory.
            ULONG_PTR LargePage : 1;                // If this bit is 1, the page is a large page.
            ULONG_PTR Priority : 3;                 // The memory priority attributes of the page.
            ULONG_PTR Reserved : 3;
            ULONG_PTR SharedOriginal : 1;           // If this bit is 1, the page was not modified.
            ULONG_PTR Bad : 1;                      // If this bit is 1, the page is has been reported as bad.
#ifdef _WIN64
            ULONG_PTR Win32GraphicsProtection : 4;  // The memory protection attributes of the page. // since 19H1
            ULONG_PTR ReservedUlong : 28;
#endif
        };
        struct
        {
            ULONG_PTR Valid : 1;                    // If this bit is 0, the subsequent members are valid; otherwise they should be ignored.
            ULONG_PTR Reserved0 : 14;
            ULONG_PTR Shared : 1;                   // If this bit is 1, the page can be shared.
            ULONG_PTR Reserved1 : 5;
            ULONG_PTR PageTable : 1;
            ULONG_PTR Location : 2;                 // The memory location of the page.  MEMORY_WORKING_SET_EX_LOCATION
            ULONG_PTR Priority : 3;                 // The memory priority of the page.
            ULONG_PTR ModifiedList : 1;
            ULONG_PTR Reserved2 : 2;
            ULONG_PTR SharedOriginal : 1;           // If this bit is 1, the page was not modified.
            ULONG_PTR Bad : 1;                      // If this bit is 1, the page is has been reported as bad.
#ifdef _WIN64
            ULONG_PTR ReservedUlong : 32;
#endif
        } Invalid;
    };
} MEMORY_WORKING_SET_EX_BLOCK, *PMEMORY_WORKING_SET_EX_BLOCK;

/**
 * The MEMORY_WORKING_SET_EX_INFORMATION structure contains extended working set information for a process.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-psapi_working_set_ex_information
 */
typedef struct _MEMORY_WORKING_SET_EX_INFORMATION
{
    PVOID VirtualAddress;                             // The virtual address.
    MEMORY_WORKING_SET_EX_BLOCK VirtualAttributes;    // The attributes of the page at VirtualAddress.
} MEMORY_WORKING_SET_EX_INFORMATION, *PMEMORY_WORKING_SET_EX_INFORMATION;

// private
typedef struct _MEMORY_SHARED_COMMIT_INFORMATION
{
    SIZE_T CommitSize;
} MEMORY_SHARED_COMMIT_INFORMATION, *PMEMORY_SHARED_COMMIT_INFORMATION;

// private
typedef struct _MEMORY_IMAGE_INFORMATION
{
    PVOID ImageBase;
    SIZE_T SizeOfImage;
    union
    {
        ULONG ImageFlags;
        struct
        {
            ULONG ImagePartialMap : 1;
            ULONG ImageNotExecutable : 1;
            ULONG ImageSigningLevel : 4; // REDSTONE3
            ULONG ImageExtensionPresent : 1; // since 24H2
            ULONG Reserved : 25;
        };
    };
} MEMORY_IMAGE_INFORMATION, *PMEMORY_IMAGE_INFORMATION;

// private
typedef struct _MEMORY_ENCLAVE_IMAGE_INFORMATION
{
    MEMORY_IMAGE_INFORMATION ImageInfo;
    UCHAR UniqueID[32];
    UCHAR AuthorID[32];
} MEMORY_ENCLAVE_IMAGE_INFORMATION, *PMEMORY_ENCLAVE_IMAGE_INFORMATION;

// private
typedef enum _MEMORY_PHYSICAL_CONTIGUITY_UNIT_STATE
{
    MemoryNotContiguous,
    MemoryAlignedAndContiguous,
    MemoryNotResident,
    MemoryNotEligibleToMakeContiguous,
    MemoryContiguityStateMax,
} MEMORY_PHYSICAL_CONTIGUITY_UNIT_STATE;

// private
typedef struct _MEMORY_PHYSICAL_CONTIGUITY_UNIT_INFORMATION
{
    union
    {
        struct
        {
            ULONG State : 2;
            ULONG Reserved : 30;
        };
        ULONG AllInformation;
    };
} MEMORY_PHYSICAL_CONTIGUITY_UNIT_INFORMATION, *PMEMORY_PHYSICAL_CONTIGUITY_UNIT_INFORMATION;

// private
typedef struct _MEMORY_PHYSICAL_CONTIGUITY_INFORMATION
{
    PVOID VirtualAddress;
    ULONG_PTR Size;
    ULONG_PTR ContiguityUnitSize;
    ULONG Flags;
    PMEMORY_PHYSICAL_CONTIGUITY_UNIT_INFORMATION ContiguityUnitInformation;
} MEMORY_PHYSICAL_CONTIGUITY_INFORMATION, *PMEMORY_PHYSICAL_CONTIGUITY_INFORMATION;

// private
typedef struct _RTL_SCP_CFG_ARM64_HEADER
{
    ULONG EcInvalidCallHandlerRva;
    ULONG EcCfgCheckRva;
    ULONG EcCfgCheckESRva;
    ULONG EcCallCheckRva;
    ULONG CpuInitializationCompleteLoadRva;
    ULONG LdrpValidateEcCallTargetInitRva;
    ULONG SyscallFfsSizeRva;
    ULONG SyscallFfsBaseRva;
} RTL_SCP_CFG_ARM64_HEADER, *PRTL_SCP_CFG_ARM64_HEADER;

// private
typedef enum _RTL_SCP_CFG_PAGE_TYPE
{
    RtlScpCfgPageTypeNop,
    RtlScpCfgPageTypeDefault,
    RtlScpCfgPageTypeExportSuppression,
    RtlScpCfgPageTypeFptr,
    RtlScpCfgPageTypeMax,
    RtlScpCfgPageTypeNone
} RTL_SCP_CFG_PAGE_TYPE;

// private
typedef struct _RTL_SCP_CFG_COMMON_HEADER
{
    ULONG CfgDispatchRva;
    ULONG CfgDispatchESRva;
    ULONG CfgCheckRva;
    ULONG CfgCheckESRva;
    ULONG InvalidCallHandlerRva;
    ULONG FnTableRva;
} RTL_SCP_CFG_COMMON_HEADER, *PRTL_SCP_CFG_COMMON_HEADER;

// private
typedef struct _RTL_SCP_CFG_HEADER
{
    RTL_SCP_CFG_COMMON_HEADER Common;
} RTL_SCP_CFG_HEADER, *PRTL_SCP_CFG_HEADER;

// private
typedef struct _RTL_SCP_CFG_REGION_BOUNDS
{
    PVOID StartAddress;
    PVOID EndAddress;
} RTL_SCP_CFG_REGION_BOUNDS, *PRTL_SCP_CFG_REGION_BOUNDS;

// private
typedef struct _RTL_SCP_CFG_NTDLL_EXPORTS
{
    RTL_SCP_CFG_REGION_BOUNDS ScpRegions[4];
    PVOID CfgDispatchFptr;
    PVOID CfgDispatchESFptr;
    PVOID CfgCheckFptr;
    PVOID CfgCheckESFptr;
    PVOID IllegalCallHandler;
} RTL_SCP_CFG_NTDLL_EXPORTS, *PRTL_SCP_CFG_NTDLL_EXPORTS;

// private
typedef struct _RTL_SCP_CFG_NTDLL_EXPORTS_ARM64EC
{
    PVOID EcInvalidCallHandler;
    PVOID EcCfgCheckFptr;
    PVOID EcCfgCheckESFptr;
    PVOID EcCallCheckFptr;
    PVOID CpuInitializationComplete;
    PVOID LdrpValidateEcCallTargetInit;
    struct
    {
        PVOID SyscallFfsSize;
        union
        {
            PVOID Ptr;
            ULONG Value;
        };
    };
    PVOID SyscallFfsBase;
} RTL_SCP_CFG_NTDLL_EXPORTS_ARM64EC, *PRTL_SCP_CFG_NTDLL_EXPORTS_ARM64EC;

// private
typedef struct _RTL_RETPOLINE_ROUTINES
{
    ULONG SwitchtableJump[16];
    ULONG CfgIndirectRax;
    ULONG NonCfgIndirectRax;
    ULONG ImportR10;
    ULONG JumpHpat;
} RTL_RETPOLINE_ROUTINES, *PRTL_RETPOLINE_ROUTINES;

// private
typedef struct _RTL_KSCP_ROUTINES
{
    ULONG UnwindDataOffset;
    RTL_RETPOLINE_ROUTINES RetpolineRoutines;
    ULONG CfgDispatchSmep;
    ULONG CfgDispatchNoSmep;
} RTL_KSCP_ROUTINES, *PRTL_KSCP_ROUTINES;

// private
typedef enum _MEMORY_IMAGE_EXTENSION_TYPE
{
    MemoryImageExtensionCfgScp,
    MemoryImageExtensionCfgEmulatedScp,
    MemoryImageExtensionTypeMax,
} MEMORY_IMAGE_EXTENSION_TYPE;

// private
typedef struct _MEMORY_IMAGE_EXTENSION_INFORMATION
{
    MEMORY_IMAGE_EXTENSION_TYPE ExtensionType;
    ULONG Flags;
    PVOID ExtensionImageBaseRva;
    SIZE_T ExtensionSize;
} MEMORY_IMAGE_EXTENSION_INFORMATION, *PMEMORY_IMAGE_EXTENSION_INFORMATION;

/**
 * Queries information about a region of virtual memory in a process.
 *
 * @param ProcessHandle A handle to the process whose memory information is to be queried.
 * @param BaseAddress A pointer to the base address of the region of pages to be queried.
 * @param MemoryInformationClass The type of information to be queried.
 * @param MemoryInformation A pointer to a buffer that receives the memory information.
 * @param MemoryInformationLength The size of the buffer pointed to by the MemoryInformation parameter.
 * @param ReturnLength A pointer to a variable that receives the number of bytes returned in the MemoryInformation buffer.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryVirtualMemory(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID BaseAddress,
    _In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
    _Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
    _In_ SIZE_T MemoryInformationLength,
    _Out_opt_ PSIZE_T ReturnLength);

#pragma endregion

#pragma region Section Information

typedef enum _SECTION_INFORMATION_CLASS
{
    SectionBasicInformation, // q; SECTION_BASIC_INFORMATION
    SectionImageInformation, // q; SECTION_IMAGE_INFORMATION
    SectionRelocationInformation, // q; ULONG_PTR RelocationDelta // name:wow64:whNtQuerySection_SectionRelocationInformation // since WIN7
    SectionOriginalBaseInformation, // q; PVOID BaseAddress // since REDSTONE
    SectionInternalImageInformation, // q; SECTION_INTERNAL_IMAGE_INFORMATION // since REDSTONE2
    MaxSectionInfoClass
} SECTION_INFORMATION_CLASS;

/**
 * The SECTION_BASIC_INFORMATION structure contains basic information about an image section.
 * \sa https://learn.microsoft.com/en-us/windows/win32/devnotes/ntquerysection
 */
typedef struct _SECTION_BASIC_INFORMATION
{
    PVOID BaseAddress;              // The base virtual address of the section if the section is based.
    ULONG AllocationAttributes;     // The allocation attributes flags.
    LARGE_INTEGER MaximumSize;      // The maximum size of the section in bytes.
} SECTION_BASIC_INFORMATION, *PSECTION_BASIC_INFORMATION;

/**
 * The SECTION_IMAGE_INFORMATION structure contains detailed information about an image section.
 */
typedef struct _SECTION_IMAGE_INFORMATION
{
    PVOID TransferAddress;          // The address of the image entry point function.
    ULONG ZeroBits;                 // The number of high-order address bits that must be zero in the image base address.
    SIZE_T MaximumStackSize;        // The maximum stack size of threads from the PE file header.
    SIZE_T CommittedStackSize;      // The initial stack size of threads from the PE file header.
    ULONG SubSystemType;            // The image subsystem from the PE file header (e.g., Windows GUI, Windows CUI, POSIX).
    union
    {
        struct
        {
            USHORT SubSystemMinorVersion;
            USHORT SubSystemMajorVersion;
        };
        ULONG SubSystemVersion;
    };
    union
    {
        struct
        {
            USHORT MajorOperatingSystemVersion;
            USHORT MinorOperatingSystemVersion;
        };
        ULONG OperatingSystemVersion;
    };
    USHORT ImageCharacteristics;    // The image characteristics from the PE file header.
    USHORT DllCharacteristics;      // The DLL characteristics flags (e.g., ASLR, NX compatibility).
    USHORT Machine;                 // The image architecture (e.g., x86, x64, ARM).
    BOOLEAN ImageContainsCode;      // The image contains native executable code.
    union
    {
        UCHAR ImageFlags;
        struct
        {
            UCHAR ComPlusNativeReady : 1;           // The image contains precompiled .NET assembly generated by NGEN (Native Image Generator).
            UCHAR ComPlusILOnly : 1;                // the image contains only Microsoft Intermediate Language (IL) assembly.
            UCHAR ImageDynamicallyRelocated : 1;    // The image was mapped using a random base address rather than the preferred base address.
            UCHAR ImageMappedFlat : 1;              // The image was mapped using a single contiguous region, rather than separate regions for each section.
            UCHAR BaseBelow4gb : 1;                 // The image was mapped using a base address below the 4 GB boundary.
            UCHAR ComPlusPrefer32bit : 1;           // The image prefers to run as a 32-bit process, even on a 64-bit system.
            UCHAR Reserved : 2;
        };
    };
    ULONG LoaderFlags;               // Reserved by ntdll.dll for the Windows loader.
    ULONG ImageFileSize;             // The size of the image, in bytes, including all headers.
    ULONG CheckSum;                  // The image file checksum, from the PE optional header.
} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION;

/**
 * The SECTION_INTERNAL_IMAGE_INFORMATION structure contains information about Control Flow Guard (CFG) features required by the image section.
 */
typedef struct _SECTION_INTERNAL_IMAGE_INFORMATION
{
    SECTION_IMAGE_INFORMATION SectionInformation;
    union
    {
        ULONG ExtendedFlags;
        struct
        {
            ULONG ImageExportSuppressionEnabled : 1;
            ULONG ImageCetShadowStacksReady : 1; // 20H1
            ULONG ImageXfgEnabled : 1; // 20H2
            ULONG ImageCetShadowStacksStrictMode : 1;
            ULONG ImageCetSetContextIpValidationRelaxedMode : 1;
            ULONG ImageCetDynamicApisAllowInProc : 1;
            ULONG ImageCetDowngradeReserved1 : 1;
            ULONG ImageCetDowngradeReserved2 : 1;
            ULONG ImageExportSuppressionInfoPresent : 1;
            ULONG ImageCfgEnabled : 1;
            ULONG Reserved : 22;
        };
    };
} SECTION_INTERNAL_IMAGE_INFORMATION, *PSECTION_INTERNAL_IMAGE_INFORMATION;

/**
 * Provides the capability to determine the base address, size, granted access, and allocation of an opened section object.
 *
 * @param SectionHandle An open handle to a section object.
 * @param SectionInformationClass The section information class about which to retrieve information.
 * @param SectionInformation A pointer to a buffer that receives the specified information. The format and content of the buffer depend on the specified section class.
 * @param SectionInformationLength Specifies the length in bytes of the section information buffer.
 * @param ReturnLength An optional pointer which, if specified, receives the number of bytes placed in the section information buffer.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows/win32/devnotes/ntquerysection
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQuerySection(
    _In_ HANDLE SectionHandle,
    _In_ SECTION_INFORMATION_CLASS SectionInformationClass,
    _Out_writes_bytes_(SectionInformationLength) PVOID SectionInformation,
    _In_ SIZE_T SectionInformationLength,
    _Out_opt_ PSIZE_T ReturnLength);

#pragma endregion

#pragma region Virtual Memory Information

typedef enum _VIRTUAL_MEMORY_INFORMATION_CLASS
{
    VmPrefetchInformation, // MEMORY_PREFETCH_INFORMATION
    VmPagePriorityInformation, // MEMORY_PAGE_PRIORITY_INFORMATION
    VmCfgCallTargetInformation, // CFG_CALL_TARGET_LIST_INFORMATION // REDSTONE2
    VmPageDirtyStateInformation, // MEMORY_PAGE_DIRTY_STATE_INFORMATION // REDSTONE3
    VmImageHotPatchInformation, // 19H1
    VmPhysicalContiguityInformation, // MEMORY_PHYSICAL_CONTIGUITY_INFORMATION // 20H1 // (requires SeLockMemoryPrivilege)
    VmVirtualMachinePrepopulateInformation,
    VmRemoveFromWorkingSetInformation, // MEMORY_REMOVE_WORKING_SET_INFORMATION
    MaxVmInfoClass
} VIRTUAL_MEMORY_INFORMATION_CLASS, *PVIRTUAL_MEMORY_INFORMATION_CLASS;

#if !defined(_KERNEL_MODE)

typedef struct _MEMORY_RANGE_ENTRY
{
    PVOID VirtualAddress;
    SIZE_T NumberOfBytes;
} MEMORY_RANGE_ENTRY, *PMEMORY_RANGE_ENTRY;

#define VM_PREFETCH_TO_WORKING_SET 0x1 // since 24H4

typedef struct _MEMORY_PREFETCH_INFORMATION
{
    ULONG Flags;
} MEMORY_PREFETCH_INFORMATION, *PMEMORY_PREFETCH_INFORMATION;

// VmPagePriorityInformation
#define MEMORY_PAGE_PRIORITY_LOWEST          0
#define MEMORY_PAGE_PRIORITY_VERY_LOW        1
#define MEMORY_PAGE_PRIORITY_LOW             2
#define MEMORY_PAGE_PRIORITY_MEDIUM          3
#define MEMORY_PAGE_PRIORITY_BELOW_NORMAL    4
#define MEMORY_PAGE_PRIORITY_NORMAL          5
#define MEMORY_PAGE_PRIORITY_HIGH            6
#define MEMORY_PAGE_PRIORITY_REALTIME        7

typedef struct _MEMORY_PAGE_PRIORITY_INFORMATION
{
    ULONG PagePriority;
} MEMORY_PAGE_PRIORITY_INFORMATION, *PMEMORY_PAGE_PRIORITY_INFORMATION;

// VmCfgCallTargetInformation
typedef struct _CFG_CALL_TARGET_LIST_INFORMATION
{
    ULONG NumberOfEntries;
    ULONG Reserved;
    PULONG NumberOfEntriesProcessed;
    PCFG_CALL_TARGET_INFO CallTargetInfo;
    PVOID Section; // since REDSTONE5
    ULONGLONG FileOffset;
} CFG_CALL_TARGET_LIST_INFORMATION, *PCFG_CALL_TARGET_LIST_INFORMATION;

// rev
typedef struct _MEMORY_PAGE_DIRTY_STATE_INFORMATION
{
    ULONG Flags;
} MEMORY_PAGE_DIRTY_STATE_INFORMATION, *PMEMORY_PAGE_DIRTY_STATE_INFORMATION;

// rev
typedef struct _MEMORY_REMOVE_WORKING_SET_INFORMATION
{
    ULONG Flags;
} MEMORY_REMOVE_WORKING_SET_INFORMATION, *PMEMORY_REMOVE_WORKING_SET_INFORMATION;

#endif

#if (NTDDI_VERSION >= NTDDI_WIN8)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationVirtualMemory(
    _In_ HANDLE ProcessHandle,
    _In_ VIRTUAL_MEMORY_INFORMATION_CLASS VmInformationClass,
    _In_ SIZE_T NumberOfEntries,
    _In_reads_(NumberOfEntries) PMEMORY_RANGE_ENTRY VirtualAddresses,
    _In_reads_bytes_(VmInformationLength) PVOID VmInformation,
    _In_ ULONG VmInformationLength);
#endif

#pragma endregion

#pragma region Partition

typedef enum _PARTITION_INFORMATION_CLASS
{
    SystemMemoryPartitionInformation, // q: MEMORY_PARTITION_CONFIGURATION_INFORMATION
    SystemMemoryPartitionMoveMemory, // s: MEMORY_PARTITION_TRANSFER_INFORMATION
    SystemMemoryPartitionAddPagefile, // s: MEMORY_PARTITION_PAGEFILE_INFORMATION
    SystemMemoryPartitionCombineMemory, // q; s: MEMORY_PARTITION_PAGE_COMBINE_INFORMATION
    SystemMemoryPartitionInitialAddMemory, // q; s: MEMORY_PARTITION_INITIAL_ADD_INFORMATION
    SystemMemoryPartitionGetMemoryEvents, // MEMORY_PARTITION_MEMORY_EVENTS_INFORMATION // since REDSTONE2
    SystemMemoryPartitionSetAttributes,
    SystemMemoryPartitionNodeInformation,
    SystemMemoryPartitionCreateLargePages,
    SystemMemoryPartitionDedicatedMemoryInformation,
    SystemMemoryPartitionOpenDedicatedMemory, // 10
    SystemMemoryPartitionMemoryChargeAttributes,
    SystemMemoryPartitionClearAttributes,
    SystemMemoryPartitionSetMemoryThresholds, // since WIN11
    SystemMemoryPartitionMemoryListCommand, // since 24H2
    SystemMemoryPartitionMax
} PARTITION_INFORMATION_CLASS, *PPARTITION_INFORMATION_CLASS;

// private
typedef struct _MEMORY_PARTITION_CONFIGURATION_INFORMATION
{
    ULONG Flags;
    ULONG NumaNode;
    ULONG Channel;
    ULONG NumberOfNumaNodes;
    SIZE_T ResidentAvailablePages;
    SIZE_T CommittedPages;
    SIZE_T CommitLimit;
    SIZE_T PeakCommitment;
    SIZE_T TotalNumberOfPages;
    SIZE_T AvailablePages;
    SIZE_T ZeroPages;
    SIZE_T FreePages;
    SIZE_T StandbyPages;
    SIZE_T StandbyPageCountByPriority[8]; // since REDSTONE2
    SIZE_T RepurposedPagesByPriority[8];
    SIZE_T MaximumCommitLimit;
    SIZE_T Reserved; // DonatedPagesToPartitions
    ULONG PartitionId; // since REDSTONE3
} MEMORY_PARTITION_CONFIGURATION_INFORMATION, *PMEMORY_PARTITION_CONFIGURATION_INFORMATION;

// private
typedef struct _MEMORY_PARTITION_TRANSFER_INFORMATION
{
    SIZE_T NumberOfPages;
    ULONG NumaNode;
    ULONG Flags;
} MEMORY_PARTITION_TRANSFER_INFORMATION, *PMEMORY_PARTITION_TRANSFER_INFORMATION;

// private
typedef struct _MEMORY_PARTITION_PAGEFILE_INFORMATION
{
    UNICODE_STRING PageFileName;
    LARGE_INTEGER MinimumSize;
    LARGE_INTEGER MaximumSize;
    ULONG Flags;
} MEMORY_PARTITION_PAGEFILE_INFORMATION, *PMEMORY_PARTITION_PAGEFILE_INFORMATION;

// private
typedef struct _MEMORY_PARTITION_PAGE_COMBINE_INFORMATION
{
    HANDLE StopHandle;
    ULONG Flags;
    SIZE_T TotalNumberOfPages;
} MEMORY_PARTITION_PAGE_COMBINE_INFORMATION, *PMEMORY_PARTITION_PAGE_COMBINE_INFORMATION;

// private
typedef struct _MEMORY_PARTITION_PAGE_RANGE
{
    ULONG_PTR StartPage;
    SIZE_T NumberOfPages;
} MEMORY_PARTITION_PAGE_RANGE, *PMEMORY_PARTITION_PAGE_RANGE;

// private
typedef struct _MEMORY_PARTITION_INITIAL_ADD_INFORMATION
{
    ULONG Flags;
    ULONG NumberOfRanges;
    ULONG_PTR NumberOfPagesAdded;
    MEMORY_PARTITION_PAGE_RANGE PartitionRanges[1];
} MEMORY_PARTITION_INITIAL_ADD_INFORMATION, *PMEMORY_PARTITION_INITIAL_ADD_INFORMATION;

// private
typedef struct _MEMORY_PARTITION_MEMORY_EVENTS_INFORMATION
{
    union
    {
        struct
        {
            ULONG CommitEvents : 1;
            ULONG Spare : 31;
        };
        ULONG AllFlags;
    } Flags;

    ULONG HandleAttributes;
    ACCESS_MASK DesiredAccess;
    HANDLE LowCommitCondition; // \KernelObjects\LowCommitCondition
    HANDLE HighCommitCondition; // \KernelObjects\HighCommitCondition
    HANDLE MaximumCommitCondition; // \KernelObjects\MaximumCommitCondition
} MEMORY_PARTITION_MEMORY_EVENTS_INFORMATION, *PMEMORY_PARTITION_MEMORY_EVENTS_INFORMATION;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtManagePartition(
    _In_ HANDLE TargetHandle,
    _In_opt_ HANDLE SourceHandle,
    _In_ PARTITION_INFORMATION_CLASS PartitionInformationClass,
    _Inout_updates_bytes_(PartitionInformationLength) PVOID PartitionInformation,
    _In_ ULONG PartitionInformationLength);

#pragma endregion

EXTERN_C_END
