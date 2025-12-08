#pragma once

#include "../MinDef.h"

#include <minwinbase.h>

EXTERN_C_START

/* phnt */

typedef enum _MMLISTS
{
    ZeroedPageList = 0,
    FreePageList = 1,
    StandbyPageList = 2,
    ModifiedPageList = 3,
    ModifiedNoWritePageList = 4,
    BadPageList = 5,
    ActiveAndValid = 6,
    TransitionPage = 7
} MMLISTS, *PMMLISTS;

typedef enum _MMPFNUSE
{
    ProcessPrivatePage,
    MemoryMappedFilePage,
    PageFileMappedPage,
    PageTablePage,
    PagedPoolPage,
    NonPagedPoolPage,
    SystemPTEPage,
    SessionPrivatePage,
    MetafilePage,
    AWEPage,
    DriverLockedPage,
    KernelStackPage
} MMPFNUSE, *PMMPFNUSE;

// private
typedef struct _MEMORY_FRAME_INFORMATION
{
    ULONGLONG UseDescription : 4; // MMPFNUSE_*
    ULONGLONG ListDescription : 3; // MMPFNLIST_*
    ULONGLONG Cold : 1; // 19H1
    ULONGLONG Pinned : 1; // 1 - pinned, 0 - not pinned
    ULONGLONG DontUse : 48; // *_INFORMATION overlay
    ULONGLONG Priority : 3;
    ULONGLONG NonTradeable : 1;
    ULONGLONG Reserved : 3;
} MEMORY_FRAME_INFORMATION, *PMEMORY_FRAME_INFORMATION;

// private
typedef struct _FILEOFFSET_INFORMATION
{
    ULONGLONG DontUse : 9; // MEMORY_FRAME_INFORMATION overlay
    ULONGLONG Offset : 48; // mapped files
    ULONGLONG Reserved : 7;
} FILEOFFSET_INFORMATION, *PFILEOFFSET_INFORMATION;

// private
typedef struct _PAGEDIR_INFORMATION
{
    ULONGLONG DontUse : 9; // MEMORY_FRAME_INFORMATION overlay
    ULONGLONG PageDirectoryBase : 48; // private pages
    ULONGLONG Reserved : 7;
} PAGEDIR_INFORMATION, *PPAGEDIR_INFORMATION;

// private
typedef struct _UNIQUE_PROCESS_INFORMATION
{
    ULONGLONG DontUse : 9; // MEMORY_FRAME_INFORMATION overlay
    ULONGLONG UniqueProcessKey : 48; // ProcessId
    ULONGLONG Reserved : 7;
} UNIQUE_PROCESS_INFORMATION, *PUNIQUE_PROCESS_INFORMATION;

// private
typedef struct _MMPFN_IDENTITY
{
    union
    {
        MEMORY_FRAME_INFORMATION e1; // all
        FILEOFFSET_INFORMATION e2; // mapped files
        PAGEDIR_INFORMATION e3; // private pages
        UNIQUE_PROCESS_INFORMATION e4; // owning process
    } u1;
    ULONG_PTR PageFrameIndex; // all
    union
    {
        struct
        {
            ULONG_PTR Image : 1;
            ULONG_PTR Mismatch : 1;
        } e1;
        struct
        {
            ULONG_PTR CombinedPage;
        } e2;
        ULONG_PTR FileObject; // mapped files
        ULONG_PTR UniqueFileObjectKey;
        ULONG_PTR ProtoPteAddress;
        ULONG_PTR VirtualAddress;  // everything else
    } u2;
} MMPFN_IDENTITY, *PMMPFN_IDENTITY;

typedef struct _MMPFN_MEMSNAP_INFORMATION
{
    ULONG_PTR InitialPageFrameIndex;
    ULONG_PTR Count;
} MMPFN_MEMSNAP_INFORMATION, *PMMPFN_MEMSNAP_INFORMATION;

// Flags directly correspond to KPROCESS.Flags, of type KEXECUTE_OPTIONS (named bitfields available).
// Flags adjust OS behavior for 32-bit processes only. They are effectively ignored for ARM64 and x64 processes.
// [nt!Mi]canGrantExecute = KF_GLOBAL_32BIT_EXECUTE || MEM_EXECUTE_OPTION_ENABLE || (!KF_GLOBAL_32BIT_NOEXECUTE && !MEM_EXECUTE_OPTION_DISABLE)
#define MEM_EXECUTE_OPTION_DISABLE 0x1      // respect the NX bit: DEP on, only run code from executable pages
#define MEM_EXECUTE_OPTION_ENABLE 0x2       // ignore the NX bit: DEP off, enable executing most of ro/rw memory; trumps over the _DISABLE option
#define MEM_EXECUTE_OPTION_DISABLE_THUNK_EMULATION 0x4  // do not emulate NX code sequences which look like ATL thunks
#define MEM_EXECUTE_OPTION_PERMANENT 0x8                // changing any MEM_EXECUTE_* option for the process is not allowed [anymore]
#define MEM_EXECUTE_OPTION_EXECUTE_DISPATCH_ENABLE 0x10 // allow non-executable exception handlers (ntdll!RtlIsValidHandler)
#define MEM_EXECUTE_OPTION_IMAGE_DISPATCH_ENABLE 0x20   // allow non-MEM_IMAGE exception handlers (ntdll!RtlIsValidHandler)
#define MEM_EXECUTE_OPTION_DISABLE_EXCEPTION_CHAIN_VALIDATION 0x40  // don't invoke ntdll!RtlpIsValidExceptionChain to check SEH chain
#define MEM_EXECUTE_OPTION_VALID_FLAGS 0x7f

#pragma region Virtual memory

#if !defined(_KERNEL_MODE)

/**
 * The NtAllocateVirtualMemory routine reserves, commits, or both, a region of pages within the user-mode virtual address space of a specified process.
 *
 * @param ProcessHandle A handle for the process for which the mapping should be done.
 * @param BaseAddress A pointer to a variable that will receive the base address of the allocated region of pages. If the initial value is not zero, the region is allocated at the specified virtual address.
 * @param ZeroBits The number of high-order address bits that must be zero in the base address of the section view. This value must be less than 21 and the initial value of BaseAddress must be zero.
 * @param RegionSize A pointer to a variable that will receive the actual size, in bytes, of the allocated region of pages.
 * @param AllocationType A bitmask containing flags that specify the type of allocation to be performed.
 * @param PageProtection A bitmask containing page protection flags that specify the protection desired for the committed region of pages.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwallocatevirtualmemory
 */
_Must_inspect_result_
_When_(return == 0, __drv_allocatesMem(mem))
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAllocateVirtualMemory(
    _In_ HANDLE ProcessHandle,
    _Inout_ _At_(*BaseAddress, _Readable_bytes_(*RegionSize) _Writable_bytes_(*RegionSize) _Post_readable_byte_size_(*RegionSize)) PVOID * BaseAddress,
    _In_ ULONG_PTR ZeroBits,
    _Inout_ PSIZE_T RegionSize,
    _In_ ULONG AllocationType,
    _In_ ULONG PageProtection);

#if (NTDDI_VERSION >= NTDDI_WIN10_RS5)
/**
 * The NtAllocateVirtualMemoryEx routine reserves, commits, or both, a region of pages within the user-mode virtual address space of a specified process.
 *
 * @param ProcessHandle A handle for the process for which the mapping should be done.
 * @param BaseAddress A pointer to a variable that will receive the base address of the allocated region of pages. If the initial value is not zero, the region is allocated at the specified virtual address.
 * @param RegionSize A pointer to a variable that will receive the actual size, in bytes, of the allocated region of pages.
 * @param AllocationType A bitmask containing flags that specify the type of allocation to be performed.
 * @param PageProtection A bitmask containing page protection flags that specify the protection desired for the committed region of pages.
 * @param ExtendedParameters An optional pointer to one or more extended parameters of type MEM_EXTENDED_PARAMETER.
 * @param ExtendedParameterCount Specifies the number of elements in the ExtendedParameters array.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwallocatevirtualmemory
 */
_Must_inspect_result_
_When_(return == 0, __drv_allocatesMem(Mem))
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAllocateVirtualMemoryEx(
    _In_ HANDLE ProcessHandle,
    _Inout_ _At_(*BaseAddress, _Readable_bytes_(*RegionSize) _Writable_bytes_(*RegionSize) _Post_readable_byte_size_(*RegionSize)) PVOID * BaseAddress,
    _Inout_ PSIZE_T RegionSize,
    _In_ ULONG AllocationType,
    _In_ ULONG PageProtection,
    _Inout_updates_opt_(ExtendedParameterCount) PMEM_EXTENDED_PARAMETER ExtendedParameters,
    _In_ ULONG ExtendedParameterCount);
#endif

/**
 * Frees virtual memory allocated for a process.
 *
 * @param ProcessHandle A handle to the process whose virtual memory is to be freed.
 * @param BaseAddress A pointer to the base address of the region of pages to be freed.
 * @param RegionSize A pointer to a variable that specifies the size of the region of memory to be freed.
 * @param FreeType The type of free operation. This parameter can be MEM_DECOMMIT or MEM_RELEASE.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtFreeVirtualMemory(
    _In_ HANDLE ProcessHandle,
    _Inout_ __drv_freesMem(Mem) PVOID* BaseAddress,
    _Inout_ PSIZE_T RegionSize,
    _In_ ULONG FreeType);

/**
 * Reads virtual memory from a process.
 *
 * @param ProcessHandle A handle to the process whose memory is to be read.
 * @param BaseAddress A pointer to the base address in the specified process from which to read.
 * @param Buffer A pointer to a buffer that receives the contents from the address space of the specified process.
 * @param NumberOfBytesToRead The number of bytes to be read from the specified process.
 * @param NumberOfBytesRead A pointer to a variable that receives the number of bytes transferred into the specified buffer.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtReadVirtualMemory(
    _In_ HANDLE ProcessHandle,
    _In_ PVOID BaseAddress,
    _Out_writes_bytes_to_(NumberOfBytesToRead, *NumberOfBytesRead) PVOID Buffer,
    _In_ SIZE_T NumberOfBytesToRead,
    _Out_opt_ PSIZE_T NumberOfBytesRead);

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)
/**
 * Reads virtual memory from a process with extended options.
 *
 * @param ProcessHandle A handle to the process whose memory is to be read.
 * @param BaseAddress A pointer to the base address in the specified process from which to read.
 * @param Buffer A pointer to a buffer that receives the contents from the address space of the specified process.
 * @param NumberOfBytesToRead The number of bytes to be read from the specified process.
 * @param NumberOfBytesRead A pointer to a variable that receives the number of bytes transferred into the specified buffer.
 * @param Flags Additional flags for the read operation.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtReadVirtualMemoryEx(
    _In_ HANDLE ProcessHandle,
    _In_ PVOID BaseAddress,
    _Out_writes_bytes_to_(NumberOfBytesToRead, *NumberOfBytesRead) PVOID Buffer,
    _In_ SIZE_T NumberOfBytesToRead,
    _Out_opt_ PSIZE_T NumberOfBytesRead,
    _In_ ULONG Flags);
#endif

/**
 * Writes virtual memory to a process.
 *
 * @param ProcessHandle A handle to the process whose memory is to be written.
 * @param BaseAddress A pointer to the base address in the specified process to which to write.
 * @param Buffer A pointer to the buffer that contains the data to be written to the address space of the specified process.
 * @param NumberOfBytesToWrite The number of bytes to be written to the specified process.
 * @param NumberOfBytesWritten A pointer to a variable that receives the number of bytes transferred into the specified buffer.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtWriteVirtualMemory(
    _In_ HANDLE ProcessHandle,
    _In_ PVOID BaseAddress,
    _In_reads_bytes_(NumberOfBytesToWrite) PVOID Buffer,
    _In_ SIZE_T NumberOfBytesToWrite,
    _Out_opt_ PSIZE_T NumberOfBytesWritten);

/**
 * Changes the protection on a region of virtual memory.
 *
 * @param ProcessHandle A handle to the process whose memory protection is to be changed.
 * @param BaseAddress A pointer to the base address of the region of pages whose access protection attributes are to be changed.
 * @param RegionSize A pointer to a variable that specifies the size of the region whose access protection attributes are to be changed.
 * @param NewProtection The memory protection option. This parameter can be one of the memory protection constants.
 * @param OldProtection A pointer to a variable that receives the previous access protection of the first page in the specified region of pages.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtProtectVirtualMemory(
    _In_ HANDLE ProcessHandle,
    _Inout_ PVOID* BaseAddress,
    _Inout_ PSIZE_T RegionSize,
    _In_ ULONG NewProtection,
    _Out_ PULONG OldProtection);

/**
 * Flushes the instruction cache for a specified process.
 *
 * @param ProcessHandle A handle to the process whose instruction cache is to be flushed.
 * @param BaseAddress A pointer to the base address of the region of memory to be flushed.
 * @param RegionSize A pointer to a variable that specifies the size of the region to be flushed.
 * @param IoStatus A pointer to an IO_STATUS_BLOCK structure that receives the status of the flush operation.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtFlushVirtualMemory(
    _In_ HANDLE ProcessHandle,
    _Inout_ PVOID* BaseAddress,
    _Inout_ PSIZE_T RegionSize,
    _Out_ PIO_STATUS_BLOCK IoStatus);

#endif

#if !defined(_KERNEL_MODE)

#define MAP_PROCESS 1 // Process WorkingSet
#define MAP_SYSTEM 2 // Physical Memory // (requires SeLockMemoryPrivilege)

/**
 * Locks the specified region of the process's virtual address space into physical memory, ensuring that subsequent access to the region will not incur a page fault.
 *
 * @param ProcessHandle A handle to the process whose virtual address space is to be locked.
 * @param BaseAddress A pointer to the base address of the region of pages to be locked.
 * @param RegionSize The size of the region to be locked, in bytes. The size is rounded up to the nearest multiple of PAGE_SIZE.
 * @param MapType A bitmask containing one or more flags that specify the type of operations to be performed.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtuallock
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtLockVirtualMemory(
    _In_ HANDLE ProcessHandle,
    _Inout_ PVOID* BaseAddress,
    _Inout_ PSIZE_T RegionSize,
    _In_ ULONG MapType);

/**
 * Unlocks a specified range of pages in the virtual address space of a process, enabling the system to swap the pages out to the paging file if necessary.
 *
 * @param ProcessHandle A handle to the process whose virtual address space is to be unlocked.
 * @param BaseAddress A pointer to the base address of the region of pages to be unlocked.
 * @param RegionSize The size of the region to be unlocked, in bytes. The size is rounded up to the nearest multiple of PAGE_SIZE.
 * @param MapType A bitmask containing one or more flags that specify the type of operations to be performed.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualunlock
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnlockVirtualMemory(
    _In_ HANDLE ProcessHandle,
    _Inout_ PVOID* BaseAddress,
    _Inout_ PSIZE_T RegionSize,
    _In_ ULONG MapType);

#endif

#pragma endregion

#pragma region Section

#define SEC_DRIVER_IMAGE 0x00100000 // rev

#if !defined(_KERNEL_MODE)
typedef enum _SECTION_INHERIT
{
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT;
#endif

#if !defined(_KERNEL_MODE)

/**
 * The NtCreateSection routine creates a section object.
 *
 * @param SectionHandle Pointer to a variable that receives a handle to the section object.
 * @param DesiredAccess The access mask that specifies the requested access to the section object.
 * @param ObjectAttributes Pointer to the base virtual address of the view to unmap. This value can be any virtual address within the view.
 * @param MaximumSize The maximum size, in bytes, of the section. The actual size when backed by the paging file, or the maximum the file can be extended or mapped when backed by an ordinary file.
 * @param SectionPageProtection Specifies the protection to place on each page in the section.
 * @param AllocationAttributes A bitmask of SEC_XXX flags that determines the allocation attributes of the section.
 * @param FileHandle Optionally specifies a handle for an open file object. If the value of FileHandle is NULL, the section is backed by the paging file. Otherwise, the section is backed by the specified file.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwcreatesection
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateSection(
    _Out_ PHANDLE SectionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PLARGE_INTEGER MaximumSize,
    _In_ ULONG SectionPageProtection,
    _In_ ULONG AllocationAttributes,
    _In_opt_ HANDLE FileHandle);

#if (NTDDI_VERSION >= NTDDI_WIN10_RS5)
/**
 * The NtCreateSectionEx routine creates a section object.
 *
 * @param SectionHandle Pointer to a variable that receives a handle to the section object.
 * @param DesiredAccess The access mask that specifies the requested access to the section object.
 * @param ObjectAttributes Pointer to the base virtual address of the view to unmap. This value can be any virtual address within the view.
 * @param MaximumSize The maximum size, in bytes, of the section. The actual size when backed by the paging file, or the maximum the file can be extended or mapped when backed by an ordinary file.
 * @param SectionPageProtection Specifies the protection to place on each page in the section.
 * @param AllocationAttributes A bitmask of SEC_XXX flags that determines the allocation attributes of the section.
 * @param FileHandle Optionally specifies a handle for an open file object. If the value of FileHandle is NULL, the section is backed by the paging file. Otherwise, the section is backed by the specified file.
 * @param ExtendedParameters An optional pointer to one or more extended parameters of type MEM_EXTENDED_PARAMETER.
 * @param ExtendedParameterCount Specifies the number of elements in the ExtendedParameters array.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwcreatesection
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateSectionEx(
    _Out_ PHANDLE SectionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PLARGE_INTEGER MaximumSize,
    _In_ ULONG SectionPageProtection,
    _In_ ULONG AllocationAttributes,
    _In_opt_ HANDLE FileHandle,
    _Inout_updates_opt_(ExtendedParameterCount) PMEM_EXTENDED_PARAMETER ExtendedParameters,
    _In_ ULONG ExtendedParameterCount);
#endif

/**
 * The NtOpenSection routine opens a handle for an existing section object.
 *
 * @param SectionHandle Handle to a process object that was previously passed to NtMapViewOfSection.
 * @param DesiredAccess The access mask that specifies the requested access to the section object.
 * @param ObjectAttributes Pointer to an OBJECT_ATTRIBUTES structure that specifies the object name and other attributes.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwopensection
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenSection(
    _Out_ PHANDLE SectionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes);

/**
 * Maps a view of a section into the virtual address space of a subject process.
 *
 * @param SectionHandle A handle to an existing section object.
 * @param ProcessHandle A handle to the object that represents the process that the view should be mapped into. The handle must have been opened with PROCESS_VM_OPERATION access.
 * @param BaseAddress A pointer to a variable that receives the base address of the view. If the value is not NULL, the view is allocated starting at the specified virtual address rounded down to the next 64-kilobyte address boundary.
 * @param ZeroBits The number of high-order address bits that must be zero in the base address of the section view. The value of this parameter must be less than 21 and is used only if BaseAddress is NULL.
 * @param CommitSize Specifies the size, in bytes, of the initially committed region of the view. CommitSize is meaningful only for page-file backed sections and is rounded up to the nearest multiple of PAGE_SIZE.
 * @param SectionOffset A pointer to a variable that receives the offset, in bytes, from the beginning of the section to the view. 
 * @param ViewSize A pointer to a variable that specifies the size of the view in bytes. If the initial value is zero, NtMapViewOfSection maps a view of the section that starts at SectionOffset and continues to the end of the section. 
 * @param InheritDisposition A value that specifies how the view is to be shared with child processes. 
 * @param AllocationType Specifies the type of allocation to be performed for the specified region of pages. The valid flags are MEM_RESERVE, MEM_TOP_DOWN, MEM_LARGE_PAGES, MEM_DIFFERENT_IMAGE_BASE_OK and MEM_REPLACE_PLACEHOLDER. Although MEM_COMMIT is not allowed, it is implied unless MEM_RESERVE is specified. 
 * @param PageProtection Specifies the page protection to be applied to the mapped view. Not used with SEC_IMAGE, must be set to PAGE_READONLY for SEC_IMAGE_NO_EXECUTE. For non-image sections, the value must be compatible with the section's page protection from NtCreateSection.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwmapviewofsection
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtMapViewOfSection(
    _In_ HANDLE SectionHandle,
    _In_ HANDLE ProcessHandle,
    _Inout_ _At_(*BaseAddress, _Readable_bytes_(*ViewSize) _Writable_bytes_(*ViewSize) _Post_readable_byte_size_(*ViewSize)) PVOID * BaseAddress,
    _In_ ULONG_PTR ZeroBits,
    _In_ SIZE_T CommitSize,
    _Inout_opt_ PLARGE_INTEGER SectionOffset,
    _Inout_ PSIZE_T ViewSize,
    _In_ SECTION_INHERIT InheritDisposition,
    _In_ ULONG AllocationType,
    _In_ ULONG PageProtection);

#if (NTDDI_VERSION >= NTDDI_WIN10_RS5)
/**
 * Maps a view of a section into the virtual address space of a subject process.
 *
 * @param SectionHandle A handle to an existing section object.
 * @param ProcessHandle A handle to the object that represents the process that the view should be mapped into. The handle must have been opened with PROCESS_VM_OPERATION access.
 * @param BaseAddress A pointer to a variable that receives the base address of the view. If the value is not NULL, the view is allocated starting at the specified virtual address rounded down to the next 64-kilobyte address boundary.
 * @param SectionOffset A pointer to a variable that receives the offset, in bytes, from the beginning of the section to the view.
 * @param ViewSize A pointer to a variable that specifies the size of the view in bytes. If the initial value is zero, NtMapViewOfSection maps a view of the section that starts at SectionOffset and continues to the end of the section.
 * @param AllocationType Specifies the type of allocation to be performed for the specified region of pages. The valid flags are MEM_RESERVE, MEM_TOP_DOWN, MEM_LARGE_PAGES, MEM_DIFFERENT_IMAGE_BASE_OK and MEM_REPLACE_PLACEHOLDER. Although MEM_COMMIT is not allowed, it is implied unless MEM_RESERVE is specified.
 * @param PageProtection Specifies the page protection to be applied to the mapped view. Not used with SEC_IMAGE, must be set to PAGE_READONLY for SEC_IMAGE_NO_EXECUTE. For non-image sections, the value must be compatible with the section's page protection from NtCreateSection.
 * @param ExtendedParameters An optional pointer to one or more extended parameters of type MEM_EXTENDED_PARAMETER.
 * @param ExtendedParameterCount Specifies the number of elements in the ExtendedParameters array.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwmapviewofsectionex
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtMapViewOfSectionEx(
    _In_ HANDLE SectionHandle,
    _In_ HANDLE ProcessHandle,
    _Inout_ _At_(*BaseAddress, _Readable_bytes_(*ViewSize) _Writable_bytes_(*ViewSize) _Post_readable_byte_size_(*ViewSize)) PVOID * BaseAddress,
    _Inout_opt_ PLARGE_INTEGER SectionOffset,
    _Inout_ PSIZE_T ViewSize,
    _In_ ULONG AllocationType,
    _In_ ULONG PageProtection,
    _Inout_updates_opt_(ExtendedParameterCount) PMEM_EXTENDED_PARAMETER ExtendedParameters,
    _In_ ULONG ExtendedParameterCount);
#endif

/**
 * The NtUnmapViewOfSection routine unmaps a view of a section from the virtual address space of a subject process.
 *
 * @param ProcessHandle Handle to a process object that was previously passed to NtMapViewOfSection.
 * @param BaseAddress Pointer to the base virtual address of the view to unmap. This value can be any virtual address within the view.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwunmapviewofsection
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnmapViewOfSection(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID BaseAddress);

#if (NTDDI_VERSION >= NTDDI_WIN8)
/**
 * The NtUnmapViewOfSectionEx routine unmaps a view of a section from the virtual address space of a subject process.
 *
 * @param ProcessHandle Handle to a process object that was previously passed to NtMapViewOfSection.
 * @param BaseAddress Pointer to the base virtual address of the view to unmap. This value can be any virtual address within the view.
 * @param Flags Additional flags for the unmap operation.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwunmapviewofsection
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnmapViewOfSectionEx(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID BaseAddress,
    _In_ ULONG Flags);
#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtExtendSection(
    _In_ HANDLE SectionHandle,
    _Inout_ PLARGE_INTEGER NewSectionSize);

/**
 * Determines whether two mapped files are the same.
 *
 * \param File1MappedAsAnImage A pointer to the base address of the first file mapped as an image.
 * \param File2MappedAsFile A pointer to the base address of the second file mapped as a file.
 * \return NTSTATUS Returns STATUS_SUCCESS if the files are the same; otherwise, an appropriate NTSTATUS error code.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAreMappedFilesTheSame(
    _In_ PVOID File1MappedAsAnImage,
    _In_ PVOID File2MappedAsFile);

#endif

#pragma endregion

#pragma region Partitions

#if !defined(_KERNEL_MODE)

#if (NTDDI_VERSION >= NTDDI_WIN10)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreatePartition(
    _In_opt_ HANDLE ParentPartitionHandle,
    _Out_ PHANDLE PartitionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenPartition(
    _Out_ PHANDLE PartitionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes);

#endif

#endif

#pragma endregion

#pragma region User physical pages

#if !defined(_KERNEL_MODE)

/**
 * Maps previously allocated physical memory pages at a specified address in an Address Windowing Extensions (AWE) region.
 *
 * @param VirtualAddress A pointer to the starting address of the region of memory to remap. The value of VirtualAddress must be within the address range that the VirtualAlloc function returns when the Address Windowing Extensions (AWE) region is allocated.
 * @param NumberOfPages The size of the physical memory and virtual address space for which to establish translations, in pages.
 * @param UserPfnArray A pointer to an array of physical page frame numbers.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-mapuserphysicalpages
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtMapUserPhysicalPages(
    _In_ PVOID VirtualAddress,
    _In_ SIZE_T NumberOfPages,
    _In_reads_opt_(NumberOfPages) PULONG_PTR UserPfnArray);

/**
 * Maps previously allocated physical memory pages at a specified address in an Address Windowing Extensions (AWE) region.
 *
 * @param VirtualAddresses A pointer to an array of starting addresses of the regions of memory to remap. The value of VirtualAddress must be within the address range that the VirtualAlloc function returns when the Address Windowing Extensions (AWE) region is allocated.
 * @param NumberOfPages The size of the physical memory and virtual address space for which to establish translations, in pages.
 * @param UserPfnArray A pointer to an array of values that indicates how each corresponding page in VirtualAddresses should be treated. A 0 (zero) indicates the entry should be unmapped, and any nonzero value should be mapped.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-mapuserphysicalpagesscatter
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtMapUserPhysicalPagesScatter(
    _In_reads_(NumberOfPages) PVOID* VirtualAddresses,
    _In_ SIZE_T NumberOfPages,
    _In_reads_opt_(NumberOfPages) PULONG_PTR UserPfnArray);

/**
 * Allocates physical memory pages to be mapped and unmapped within any Address Windowing Extensions (AWE) region of a specified process.
 *
 * @param ProcessHandle A handle to the process whose physical memory pages are to be allocated within the virtual address space of this process.
 * @param NumberOfPages The size of the physical memory to allocate, in pages.
 * @param UserPfnArray A pointer to an array to store the page frame numbers of the allocated memory. Do not attempt to modify this buffer. It contains operating system data, and corruption could be catastrophic. The information in the buffer is not useful to an application.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-allocateuserphysicalpages
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAllocateUserPhysicalPages(
    _In_ HANDLE ProcessHandle,
    _Inout_ PSIZE_T NumberOfPages,
    _Out_writes_(*NumberOfPages) PULONG_PTR UserPfnArray);

#if (NTDDI_VERSION >= NTDDI_WIN10)
/**
 * Allocates physical memory pages to be mapped and unmapped within any Address Windowing Extensions (AWE) region of a specified process, with extended parameters.
 *
 * @param ProcessHandle A handle to the process whose physical memory pages are to be allocated within the virtual address space of this process.
 * @param NumberOfPages The size of the physical memory to allocate, in pages.
 * @param UserPfnArray A pointer to an array to store the page frame numbers of the allocated memory. Do not attempt to modify this buffer. It contains operating system data, and corruption could be catastrophic. The information in the buffer is not useful to an application.
 * @param ExtendedParameters Pointer to an array of MEM_EXTENDED_PARAMETER structures.
 * @param ExtendedParameterCount The number of MEM_EXTENDED_PARAMETER in the ExtendedParameters array.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-allocateuserphysicalpages
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAllocateUserPhysicalPagesEx(
    _In_ HANDLE ProcessHandle,
    _Inout_ PULONG_PTR NumberOfPages,
    _Out_writes_(*NumberOfPages) PULONG_PTR UserPfnArray,
    _Inout_updates_opt_(ExtendedParameterCount) PMEM_EXTENDED_PARAMETER ExtendedParameters,
    _In_ ULONG ExtendedParameterCount
);
#endif

/**
 * Frees physical memory pages that are allocated previously by using NtAllocateUserPhysicalPages.
 *
 * @param ProcessHandle A handle to the process. The function frees memory within the virtual address space of this process.
 * @param NumberOfPages The size of the physical memory to free, in pages. On return, if the function fails, this parameter indicates the number of pages that are freed.
 * @param UserPfnArray A pointer to an array of page frame numbers of the allocated memory to be freed.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-freeuserphysicalpages
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtFreeUserPhysicalPages(
    _In_ HANDLE ProcessHandle,
    _Inout_ PULONG_PTR NumberOfPages,
    _In_reads_(*NumberOfPages) PULONG_PTR UserPfnArray);

#endif

#pragma endregion

// Misc.

#if !defined(_KERNEL_MODE)

/**
 * Retrieves the addresses of the pages that are written to in a region of virtual memory.
 *
 * @param ProcessHandle A handle to the process whose watch information is to be queried.
 * @param Flags Additional flags for the operation. To reset the write-tracking state, set this parameter to WRITE_WATCH_FLAG_RESET. Otherwise, set this parameter to zero.
 * @param BaseAddress The base address of the memory region for which to retrieve write-tracking information. This address must a region that is allocated using MEM_WRITE_WATCH.
 * @param RegionSize The size of the memory region for which to retrieve write-tracking information, in bytes.
 * @param UserAddressArray A pointer to a buffer that receives an array of page addresses that have been written to since the region has been allocated or the write-tracking state has been reset.
 * @param EntriesInUserAddressArray On input, this variable indicates the size of the UserAddressArray array. On output, the variable receives the number of page addresses that are returned in the array.
 * @param Granularity A pointer to a variable that receives the page size, in bytes.
 * @return NTSTATUS Successful or errant status.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-getwritewatch
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtGetWriteWatch(
    _In_ HANDLE ProcessHandle,
    _In_ ULONG Flags,
    _In_ PVOID BaseAddress,
    _In_ SIZE_T RegionSize,
    _Out_writes_(*EntriesInUserAddressArray) PVOID* UserAddressArray,
    _Inout_ PULONG_PTR EntriesInUserAddressArray,
    _Out_ PULONG Granularity);

/**
 * Resets the write-tracking state for a region of virtual memory.
 *
 * @param ProcessHandle A handle to the process whose watch information is to be reset.
 * @param BaseAddress A pointer to the base address of the memory region for which to reset the write-tracking state.
 * @param RegionSize The size of the memory region for which to reset the write-tracking information, in bytes.
 * @return NTSTATUS Successful or errant status.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-resetwritewatch
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtResetWriteWatch(
    _In_ HANDLE ProcessHandle,
    _In_ PVOID BaseAddress,
    _In_ SIZE_T RegionSize);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreatePagingFile(
    _In_ PUNICODE_STRING PageFileName,
    _In_ PLARGE_INTEGER MinimumSize,
    _In_ PLARGE_INTEGER MaximumSize,
    _In_ ULONG Priority);

/**
 * Flushes the instruction cache for the specified process.
 *
 * @param ProcessHandle A handle to the process whose instruction cache is to be flushed.
 * @param BaseAddress A pointer to the base address of the memory region to be flushed. This parameter can be NULL.
 * @param RegionSize The size of the memory region to be flushed, in bytes.
 * @return NTSTATUS Successful or errant status.
 * @remarks Applications should call NtFlushInstructionCache if they generate or modify code in memory. The CPU cannot detect the change, and may execute the old code it cached.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-flushinstructioncache
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtFlushInstructionCache(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID BaseAddress,
    _In_ SIZE_T RegionSize);

/**
 * The NtFlushWriteBuffer routine flushes the write queue of the current processor that is running a thread of the current process.
 *
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtFlushWriteBuffer(VOID);

/**
 * The NtFlushProcessWriteBuffers routine flushes the write queue of each processor that is running a thread of the current process.
 *
 * @return NTSTATUS Successful or errant status.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-flushprocesswritebuffers
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtFlushProcessWriteBuffers(VOID);

#endif

#pragma region Enclave

#if (NTDDI_VERSION >= NTDDI_WIN10)

/**
 * Creates a new uninitialized enclave. An enclave is an isolated region of code and data within the address space for an application.
 * Only code that runs within the enclave can access data within the same enclave.
 *
 * @param ProcessHandle A handle to the process for which you want to create an enclave.
 * @param BaseAddress The preferred base address of the enclave. Specify NULL to have the operating system assign the base address.
 * @param ZeroBits The number of high-order address bits that must be zero in the base address of the section view. This value must be less than 21 and the initial value of BaseAddress must be zero.
 * @param Size The size of the enclave that you want to create, including the size of the code that you will load into the enclave, in bytes. VBS enclaves must be a multiple of 2 MB in size.
 * SGX enclaves must be a power of 2 in size and must have their base aligned to the same power of 2 as the size, with a minimum alignment of 2 MB. As an example, if the enclave is 128 MB, then its base must be aligned to a 128 MB boundary.
 * @param InitialCommitment The amount of memory to commit for the enclave, in bytes.
 * @param EnclaveType The architecture type of the enclave that you want to create. To verify that an enclave type is supported, call IsEnclaveTypeSupported.
 * @param EnclaveInformation A pointer to the architecture-specific information to use to create the enclave.
 * @param EnclaveInformationLength The length of the structure that the EnclaveInformation parameter points to, in bytes.
 * For the ENCLAVE_TYPE_SGX and ENCLAVE_TYPE_SGX2 enclave types, this value must be 4096. For the ENCLAVE_TYPE_VBS enclave type, this value must be sizeof(ENCLAVE_CREATE_INFO_VBS), which is 36 bytes.
 * @param EnclaveError An optional pointer to a variable that receives an enclave error code that is architecture-specific.
 * @return NTSTATUS Successful or errant status.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/enclaveapi/nf-enclaveapi-createenclave
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateEnclave(
    _In_ HANDLE ProcessHandle,
    _Inout_ PVOID* BaseAddress,
    _In_ ULONG_PTR ZeroBits,
    _In_ SIZE_T Size,
    _In_ SIZE_T InitialCommitment,
    _In_ ULONG EnclaveType,
    _In_reads_bytes_(EnclaveInformationLength) PVOID EnclaveInformation,
    _In_ ULONG EnclaveInformationLength,
    _Out_opt_ PULONG EnclaveError);

/**
 * Loads data into an uninitialized enclave that you created by calling NtCreateEnclave.
 *
 * @param ProcessHandle A handle to the process for which the enclave was created.
 * @param BaseAddress The address in the enclave where you want to load the data.
 * @param Buffer A pointer to the data the you want to load into the enclave.
 * @param BufferSize The size of the data that you want to load into the enclave, in bytes. This value must be a whole-number multiple of the page size.
 * @param Protect The memory protection to use for the pages that you want to add to the enclave.
 * @param PageInformation A pointer to information that describes the pages that you want to add to the enclave.
 * @param PageInformationLength The length of the structure that the PageInformation parameter points to, in bytes.
 * @param NumberOfBytesWritten A pointer to a variable that receives the number of bytes that NtLoadEnclaveData copied into the enclave.
 * @param EnclaveError An optional pointer to a variable that receives an enclave error code that is architecture-specific. 
 * @return NTSTATUS Successful or errant status.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/enclaveapi/nf-enclaveapi-loadenclavedata
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtLoadEnclaveData(
    _In_ HANDLE ProcessHandle,
    _In_ PVOID BaseAddress,
    _In_reads_bytes_(BufferSize) PVOID Buffer,
    _In_ SIZE_T BufferSize,
    _In_ ULONG Protect,
    _In_reads_bytes_(PageInformationLength) PVOID PageInformation,
    _In_ ULONG PageInformationLength,
    _Out_opt_ PSIZE_T NumberOfBytesWritten,
    _Out_opt_ PULONG EnclaveError);

/**
 * Initializes an enclave that you created and loaded with data.
 *
 * @param ProcessHandle A handle to the process for which the enclave was created.
 * @param BaseAddress Any address within the enclave.
 * @param EnclaveInformation A pointer to architecture-specific information to use to initialize the enclave.
 * @param EnclaveInformationLength The length of the structure that the EnclaveInformation parameter points to, in bytes.
 * @param EnclaveError An optional pointer to a variable that receives an enclave error code that is architecture-specific.
 * @return NTSTATUS Successful or errant status.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/enclaveapi/nf-enclaveapi-initializeenclave
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtInitializeEnclave(
    _In_ HANDLE ProcessHandle,
    _In_ PVOID BaseAddress,
    _In_reads_bytes_(EnclaveInformationLength) PVOID EnclaveInformation,
    _In_ ULONG EnclaveInformationLength,
    _Out_opt_ PULONG EnclaveError);

#define TERMINATE_ENCLAVE_FLAG_NO_WAIT    0x00000001ul
#define TERMINATE_ENCLAVE_FLAG_WAIT_ERROR 0x00000004ul // STATUS_PENDING -> STATUS_ENCLAVE_NOT_TERMINATED
#define TERMINATE_ENCLAVE_VALID_FLAGS     0x00000005ul

/**
 * Ends the execution of the threads that are running within an enclave.
 *
 * @param BaseAddress The base address of the enclave in which to end the execution of the threads.
 * @param Flags Additional flags for the termination operation.
 * @return NTSTATUS Successful or errant status.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/enclaveapi/nf-enclaveapi-terminateenclave
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtTerminateEnclave(
    _In_ PVOID BaseAddress,
    _In_ ULONG Flags // TERMINATE_ENCLAVE_FLAG_*
);

#if !defined(_KERNEL_MODE)

#define ENCLAVE_CALL_VALID_FLAGS  0x00000001ul
#define ENCLAVE_CALL_FLAG_NO_WAIT 0x00000001ul

/**
 * Calls a function within an enclave. NtCallEnclave can also be called within an enclave to call a function outside of the enclave.
 *
 * @param Routine The address of the function that you want to call.
 * @param Reserved Reserved for dispatch (RtlEnclaveCallDispatch)
 * @param Flags Additional flags for the call operation.
 * @param RoutineParamReturn The parameter than you want to pass to the function and return value of the function.
 * @return NTSTATUS Successful or errant status.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/enclaveapi/nf-enclaveapi-callenclave
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCallEnclave(
    _In_ PENCLAVE_ROUTINE Routine,
    _In_ PVOID Reserved,              // SelectVsmEnclaveByNumber > 0 // reserved for dispatch (RtlEnclaveCallDispatch)
    _In_ ULONG Flags,                 // ENCLAVE_CALL_FLAG_*
    _Inout_ PVOID* RoutineParamReturn // input routine parameter, output routine return value
);
#endif

#endif

#pragma endregion

EXTERN_C_END
