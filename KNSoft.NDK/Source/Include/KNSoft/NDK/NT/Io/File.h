#pragma once

#include "../MinDef.h"

EXTERN_C_START

/* wdm.h */

//
// Define the create disposition values
//

#define FILE_SUPERSEDE                  0x00000000
#define FILE_OPEN                       0x00000001
#define FILE_CREATE                     0x00000002
#define FILE_OPEN_IF                    0x00000003
#define FILE_OVERWRITE                  0x00000004
#define FILE_OVERWRITE_IF               0x00000005
#define FILE_MAXIMUM_DISPOSITION        0x00000005

//
// Define the create/open option flags
//

#define FILE_DIRECTORY_FILE                     0x00000001
#define FILE_WRITE_THROUGH                      0x00000002
#define FILE_SEQUENTIAL_ONLY                    0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING          0x00000008

#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT            0x00000020
#define FILE_NON_DIRECTORY_FILE                 0x00000040
#define FILE_CREATE_TREE_CONNECTION             0x00000080

#define FILE_COMPLETE_IF_OPLOCKED               0x00000100
#define FILE_NO_EA_KNOWLEDGE                    0x00000200
#define FILE_OPEN_REMOTE_INSTANCE               0x00000400
#define FILE_RANDOM_ACCESS                      0x00000800

#define FILE_DELETE_ON_CLOSE                    0x00001000
#define FILE_OPEN_BY_FILE_ID                    0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT             0x00004000
#define FILE_NO_COMPRESSION                     0x00008000

#if (NTDDI_VERSION >= NTDDI_WIN7)
#define FILE_OPEN_REQUIRING_OPLOCK              0x00010000
#define FILE_DISALLOW_EXCLUSIVE                 0x00020000
#endif /* NTDDI_VERSION >= NTDDI_WIN7 */
#if (NTDDI_VERSION >= NTDDI_WIN8)
#define FILE_SESSION_AWARE                      0x00040000
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

//
//  CreateOptions flag to pass in call to CreateFile to allow the write through xro.sys
//

#define FILE_RESERVE_OPFILTER                   0x00100000
#define FILE_OPEN_REPARSE_POINT                 0x00200000
#define FILE_OPEN_NO_RECALL                     0x00400000
#define FILE_OPEN_FOR_FREE_SPACE_QUERY          0x00800000

#if (NTDDI_VERSION >= NTDDI_WIN10_RS5)

//
// Create options that go with FILE_CREATE_TREE_CONNECTION.
//

#define TREE_CONNECT_NO_CLIENT_BUFFERING          0x00000008  // matches with FILE_NO_INTERMEDIATE_BUFFERING
#define TREE_CONNECT_WRITE_THROUGH                0x00000002  // matches with FILE_WRITE_THROUGH
#endif  // NTDDI_WIN10_RS5

//
//  The FILE_VALID_OPTION_FLAGS mask cannot be expanded to include the
//  highest 8 bits of the DWORD because those are used to represent the
//  create disposition in the IO Request Packet when sending information
//  to the file system
//
#define FILE_VALID_OPTION_FLAGS                 0x00ffffff
#define FILE_VALID_PIPE_OPTION_FLAGS            0x00000032
#define FILE_VALID_MAILSLOT_OPTION_FLAGS        0x00000032
#define FILE_VALID_SET_FLAGS                    0x00000036

#if (NTDDI_VERSION >= NTDDI_WIN10_VB)

//
// While the highest 8 bits of the create options are reserved for the
// create disposition in the IRP, if a create option flag is processed
// prior to IRP creation, we can utilize these bits. These values are
// masked out of the open packet in IopCreateFile prior to the later
// call to IopParseDevice, where we create the IRP.
// Currently this includes the flag to interpet the EABuffer as an
// instance of EXTENDED_CREATE_INFORMATION.
//

#define FILE_CONTAINS_EXTENDED_CREATE_INFORMATION   0x10000000
#define FILE_VALID_EXTENDED_OPTION_FLAGS            0x10000000

//
//================= Extended Create Information ====================
//

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)

typedef struct _EXTENDED_CREATE_DUAL_OPLOCK_KEYS {

    //
    //  Parent oplock key.
    //  All-zero if not set.
    //

    GUID ParentOplockKey;

    //
    //  Target oplock key.
    //  All-zero if not set.
    //

    GUID TargetOplockKey;

} EXTENDED_CREATE_DUAL_OPLOCK_KEYS, *PEXTENDED_CREATE_DUAL_OPLOCK_KEYS;

#endif // (NTDDI_VERSION >= NTDDI_WIN11_ZN)

//
// This struct can be extended and new fields may be added to the end
// of the struct in the future.
//
typedef struct _EXTENDED_CREATE_INFORMATION {
    LONGLONG ExtendedCreateFlags;   // extended create flags
    PVOID EaBuffer;                 // EA buffer
    ULONG EaLength;                 // EA buffer length
#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)
    PEXTENDED_CREATE_DUAL_OPLOCK_KEYS DualOplockKeys;       // if not NULL, dual oplock keys
#endif
} EXTENDED_CREATE_INFORMATION, *PEXTENDED_CREATE_INFORMATION;

//
// 32-bit version of EXTENDED_CREATE_INFORMATION struct
//
typedef struct _EXTENDED_CREATE_INFORMATION_32 {
    LONGLONG ExtendedCreateFlags;   // extended create flags
    void* POINTER_32 EaBuffer;      // EA buffer
    ULONG EaLength;                 // EA buffer length
#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)
    EXTENDED_CREATE_DUAL_OPLOCK_KEYS* POINTER_32 DualOplockKeys;       // if not NULL, dual oplock keys
#endif
} EXTENDED_CREATE_INFORMATION_32, *PEXTENDED_CREATE_INFORMATION_32;

//
// Define extra create/open option flags. These are passed in through
// the defined ExtendedInformation struct in the EaBuffer.
//
#define EX_CREATE_FLAG_FILE_SOURCE_OPEN_FOR_COPY        0x00000001
#define EX_CREATE_FLAG_FILE_DEST_OPEN_FOR_COPY          0x00000002

#endif // (NTDDI_VERSION >= NTDDI_WIN10_VB)

//
// Define the I/O status information return values for NtCreateFile/NtOpenFile
//

#define FILE_SUPERSEDED                 0x00000000
#define FILE_OPENED                     0x00000001
#define FILE_CREATED                    0x00000002
#define FILE_OVERWRITTEN                0x00000003
#define FILE_EXISTS                     0x00000004
#define FILE_DOES_NOT_EXIST             0x00000005

//
// Define special ByteOffset parameters for read and write operations
//

#define FILE_WRITE_TO_END_OF_FILE       0xffffffff
#define FILE_USE_FILE_POINTER_POSITION  0xfffffffe

//
// Define alignment requirement values
//

#define FILE_BYTE_ALIGNMENT             0x00000000
#define FILE_WORD_ALIGNMENT             0x00000001
#define FILE_LONG_ALIGNMENT             0x00000003
#define FILE_QUAD_ALIGNMENT             0x00000007
#define FILE_OCTA_ALIGNMENT             0x0000000f
#define FILE_32_BYTE_ALIGNMENT          0x0000001f
#define FILE_64_BYTE_ALIGNMENT          0x0000003f
#define FILE_128_BYTE_ALIGNMENT         0x0000007f
#define FILE_256_BYTE_ALIGNMENT         0x000000ff
#define FILE_512_BYTE_ALIGNMENT         0x000001ff

//
// Define the maximum length of a filename string
//

#define MAXIMUM_FILENAME_LENGTH 256

//
// Define the various device characteristics flags
//

#define FILE_REMOVABLE_MEDIA                        0x00000001
#define FILE_READ_ONLY_DEVICE                       0x00000002
#define FILE_FLOPPY_DISKETTE                        0x00000004
#define FILE_WRITE_ONCE_MEDIA                       0x00000008
#define FILE_REMOTE_DEVICE                          0x00000010
#define FILE_DEVICE_IS_MOUNTED                      0x00000020
#define FILE_VIRTUAL_VOLUME                         0x00000040
#define FILE_AUTOGENERATED_DEVICE_NAME              0x00000080
#define FILE_DEVICE_SECURE_OPEN                     0x00000100
#define FILE_CHARACTERISTIC_PNP_DEVICE              0x00000800
#define FILE_CHARACTERISTIC_TS_DEVICE               0x00001000
#define FILE_CHARACTERISTIC_WEBDAV_DEVICE           0x00002000
#define FILE_CHARACTERISTIC_CSV                     0x00010000
#define FILE_DEVICE_ALLOW_APPCONTAINER_TRAVERSAL    0x00020000
#define FILE_PORTABLE_DEVICE                        0x00040000
#define FILE_REMOTE_DEVICE_VSMB                     0x00080000
#define FILE_DEVICE_REQUIRE_SECURITY_CHECK          0x00100000

/* ntifs.h */

//
// Define the flags for NtSet(Query)EaFile service structure entries
//

#define FILE_NEED_EA                    0x00000080

//
// Define EA type values
//

#define FILE_EA_TYPE_BINARY             0xfffe
#define FILE_EA_TYPE_ASCII              0xfffd
#define FILE_EA_TYPE_BITMAP             0xfffb
#define FILE_EA_TYPE_METAFILE           0xfffa
#define FILE_EA_TYPE_ICON               0xfff9
#define FILE_EA_TYPE_EA                 0xffee
#define FILE_EA_TYPE_MVMT               0xffdf
#define FILE_EA_TYPE_MVST               0xffde
#define FILE_EA_TYPE_ASN1               0xffdd
#define FILE_EA_TYPE_FAMILY_IDS         0xff01

/* phnt */
#define FILE_COPY_STRUCTURED_STORAGE        0x00000041
#define FILE_STRUCTURED_STORAGE             0x00000441

/**
 * The NtCreateFile routine creates a new file or directory, or opens an existing file, device, directory, or volume.
 *
 * \param[out] FileHandle Pointer to a variable that receives a handle to the pipe.
 * \param[in] DesiredAccess The requested access to the object.
 * \param[in] ObjectAttributes Pointer to an OBJECT_ATTRIBUTES structure that contains the object attributes, including pipe name.
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that receives the final completion status and information about the operation.
 * \param[in] AllocationSize The initial allocation size in bytes for the file. Specify a non-zero value to eliminate disk fragmentation, since the file system pre-allocates the file using a contiguous block.
 * \param[in] FileAttributes The file attributes. Explicitly specified attributes are applied only when the file is created, superseded, or, in some cases, overwritten.
 * \param[in] ShareAccess The type of share access that the caller would like to use in the file.
 * \param[in] CreateDisposition Specifies how the file should be handled when the file already exists.
 * \param[in] CreateOptions Specifies the options to be applied when creating or opening the file.
 * \param[in] EaBuffer Pointer to an EA buffer used to pass extended attributes.
 * \param[in] EaLength Length of the EA buffer.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/Winternl/nf-winternl-ntcreatefile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateFile(
    _Out_ PHANDLE FileHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_opt_ PLARGE_INTEGER AllocationSize,
    _In_ ULONG FileAttributes,
    _In_ ULONG ShareAccess,
    _In_ ULONG CreateDisposition,
    _In_ ULONG CreateOptions,
    _In_reads_bytes_opt_(EaLength) PVOID EaBuffer,
    _In_ ULONG EaLength);

/**
 * The NtOpenFile routine opens an existing file, device, directory, or volume, and returns a handle for the file object.
 *
 * \param[out] FileHandle Pointer to a variable that receives a handle to the file.
 * \param[in] DesiredAccess The requested access to the object. 
 * \param[in] ObjectAttributes Pointer to an OBJECT_ATTRIBUTES structure that contains the file's attributes, including file name.
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that receives the final completion status and information about the operation.
 * \param[in] ShareAccess Specifies the type of share access for the file.
 * \param[in] OpenOptions Specifies the options to apply when opening the file.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntopenfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenFile(
    _Out_ PHANDLE FileHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG ShareAccess,
    _In_ ULONG OpenOptions);

/**
 * The NtDeleteFile routine deletes the specified file.
 *
 * \param[in] ObjectAttributes Pointer to an OBJECT_ATTRIBUTES structure that contains the file's attributes, including file name.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwdeletefile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteFile(
    _In_ POBJECT_ATTRIBUTES ObjectAttributes);

/**
 * The NtFlushBuffersFile routine sends a flush request for the specified file to the file system.
 *
 * \param[in] FileHandle A handle to the file whose buffers will be flushed.
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that receives the final completion status and information about the operation.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwflushbuffersfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtFlushBuffersFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock);

//  Flag definitions for NtFlushBuffersFileEx
//
//  If none of the below flags are specified the following will occur for a
//  given file handle:
//      - Write any modified data for the given file from the Windows in-memory
//        cache.
//      - Commit all pending metadata changes for the given file from the
//        Windows in-memory cache.
//      - Send a SYNC command to the underlying storage device to commit all
//        written data in the devices cache to persistent storage.
//
//  If a volume handle is specified:
//      - Write all modified data for all files on the volume from the Windows
//        in-memory cache.
//      - Commit all pending metadata changes for all files on the volume from
//        the Windows in-memory cache.
//      - Send a SYNC command to the underlying storage device to commit all
//        written data in the devices cache to persistent storage.
//
//  This is equivalent to how NtFlushBuffersFile has always worked.
//

// If set, File data and metadata in the file cache will be written, and the 
// underlying storage is synchronized to flush its cache.
// Windows file systems supported: NTFS, ReFS, FAT, exFAT.
//
#define FLUSH_FLAGS_FILE_NORMAL 0x00000000
//  If set, this operation will write the data for the given file from the
//  Windows in-memory cache.  This will NOT commit any associated metadata
//  changes.  This will NOT send a SYNC to the storage device to flush its
//  cache.  Not supported on volume handles.
//
#define FLUSH_FLAGS_FILE_DATA_ONLY 0x00000001
//
//  If set, this operation will commit both the data and metadata changes for
//  the given file from the Windows in-memory cache.  This will NOT send a SYNC
//  to the storage device to flush its cache.  Not supported on volume handles.
//
#define FLUSH_FLAGS_NO_SYNC 0x00000002
//
//  If set, this operation will write the data for the given file from the
//  Windows in-memory cache.  It will also try to skip updating the timestamp
//  as much as possible.  This will send a SYNC to the storage device to flush its
//  cache.  Not supported on volume or directory handles.
//
#define FLUSH_FLAGS_FILE_DATA_SYNC_ONLY 0x00000004 // REDSTONE1
//
//  If set, this operation will write the data for the given file from the
//  Windows in-memory cache.  It will also try to skip updating the timestamp
//  as much as possible.  This will send a SYNC to the storage device to flush its
//  cache.  Not supported on volume or directory handles.
//
#define FLUSH_FLAGS_FLUSH_AND_PURGE 0x00000008 // 24H2

#if (NTDDI_VERSION >= NTDDI_WIN8)
/**
 * The NtFlushBuffersFileEx routine sends a flush request for a given file to the file system. An optional flush operation flag can be set to control how file data is written to storage.
 *
 * \param FileHandle A handle to the file object representing the file, directory, device or volume.
 * \param Flags Flush operation flags.
 * \param Parameters Pointer to a block with additional parameters. This parameter must currently be set to NULL.
 * \param ParametersSize The size, in bytes, of the block that Parameters point to. This parameter must currently be set to 0.
 * \param IoStatusBlock Address of the caller's I/O status block. This parameter is required and cannot be NULL.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntflushbuffersfileex
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtFlushBuffersFileEx(
    _In_ HANDLE FileHandle,
    _In_ ULONG Flags,
    _In_reads_bytes_(ParametersSize) PVOID Parameters,
    _In_ ULONG ParametersSize,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock);
#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCancelIoFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCancelIoFileEx(
    _In_ HANDLE FileHandle,
    _In_opt_ PIO_STATUS_BLOCK IoRequestToCancel,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCancelSynchronousIoFile(
    _In_ HANDLE ThreadHandle,
    _In_opt_ PIO_STATUS_BLOCK IoRequestToCancel,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock);

/**
 * The NtDeviceIoControlFile function sends a control code directly to a specified device driver, causing the corresponding driver to perform the specified operation.
 *
 * \param[in] FileHandle A handle to the file object representing the file or directory on which the specified action is to be performed.
 * \param[in] Event A handle for a caller-created event. This parameter is optional and can be NULL. It must be NULL if the caller will wait for the FileHandle to be set to the Signaled state.
 * \param[in] ApcRoutine Address of a caller-supplied APC routine to be called when the requested operation completes. This parameter is optional and can be NULL.
 * \param[in] ApcContext Pointer to a caller-determined context area. This parameter value is used as the APC context if the caller supplies an APC, or is used as the completion context if an I/O completion object has been associated with the file object.
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that receives the final completion status and information about the operation.
 * \param[in] IoControlCode IOCTL_XXX code that indicates which device I/O control operation is to be carried out on, usually by the underlying device driver.
 * \param[in] InputBuffer Pointer to a caller-allocated input buffer that contains device-specific information to be given to the target driver.
 * \param[in] InputBufferLength Size, in bytes, of the buffer at InputBuffer. This value is ignored if InputBuffer is NULL.
 * \param[out] OutputBuffer Pointer to a caller-allocated output buffer in which information is returned from the target driver.
 * \param[in] OutputBufferLength Size, in bytes, of the buffer at OutputBuffer. This value is ignored if OutputBuffer is NULL.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwdeviceiocontrolfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeviceIoControlFile(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG IoControlCode,
    _In_reads_bytes_opt_(InputBufferLength) PVOID InputBuffer,
    _In_ ULONG InputBufferLength,
    _Out_writes_bytes_opt_(OutputBufferLength) PVOID OutputBuffer,
    _In_ ULONG OutputBufferLength);

/**
 * The NtFsControlFile function sends a control code directly to a file system or filter driver, causing the corresponding driver to perform the specified action.
 *
 * \param[in] FileHandle A handle to the file object representing the file or directory on which the specified action is to be performed.
 * \param[in] Event A handle for a caller-created event. This parameter is optional and can be NULL. It must be NULL if the caller will wait for the FileHandle to be set to the Signaled state.
 * \param[in] ApcRoutine Address of a caller-supplied APC routine to be called when the requested operation completes. This parameter is optional and can be NULL.
 * \param[in] ApcContext Pointer to a caller-determined context area. This parameter value is used as the APC context if the caller supplies an APC, or is used as the completion context if an I/O completion object has been associated with the file object. 
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that receives the final completion status and information about the operation.
 * \param[in] FsControlCode FSCTL_XXX code that indicates which file system control operation is to be carried out.
 * \param[in] InputBuffer Pointer to a caller-allocated input buffer that contains device-specific information to be given to the target driver.
 * \param[in] InputBufferLength Size, in bytes, of the buffer at InputBuffer. This value is ignored if InputBuffer is NULL.
 * \param[out] OutputBuffer Pointer to a caller-allocated output buffer in which information is returned from the target driver.
 * \param[in] OutputBufferLength Size, in bytes, of the buffer at OutputBuffer. This value is ignored if OutputBuffer is NULL.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwfscontrolfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtFsControlFile(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG FsControlCode,
    _In_reads_bytes_opt_(InputBufferLength) PVOID InputBuffer,
    _In_ ULONG InputBufferLength,
    _Out_writes_bytes_opt_(OutputBufferLength) PVOID OutputBuffer,
    _In_ ULONG OutputBufferLength);

/**
 * The NtReadFile function reads data from an open file.
 *
 * \param[in] FileHandle A handle to the file object representing the file or directory on which the specified action is to be performed.
 * \param[in] Event A handle for a caller-created event. This parameter is optional and can be NULL. It must be NULL if the caller will wait for the FileHandle to be set to the Signaled state.
 * \param[in] ApcRoutine Address of a caller-supplied APC routine to be called when the requested operation completes. This parameter is optional and can be NULL.
 * \param[in] ApcContext Pointer to a caller-determined context area. This parameter value is used as the APC context if the caller supplies an APC, or is used as the completion context if an I/O completion object has been associated with the file object.
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that receives the final completion status and information about the operation.
 * \param[out] Buffer Pointer to a caller-allocated buffer that receives the data read from the file.
 * \param[in] Length The size, in bytes, of the buffer pointed to by Buffer.
 * \param[in] ByteOffset Pointer to a variable that specifies the starting byte offset in the file where the read operation will begin.
 * \param[in] Key Device and intermediate drivers should set this pointer to NULL.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwreadfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtReadFile(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length,
    _In_opt_ PLARGE_INTEGER ByteOffset,
    _In_opt_ PULONG Key);

/**
 * The NtWriteFile function writes data to an open file.
 *
 * \param[in] FileHandle A handle to the file object representing the file or directory on which the specified action is to be performed.
 * \param[in] Event A handle for a caller-created event. This parameter is optional and can be NULL. It must be NULL if the caller will wait for the FileHandle to be set to the Signaled state.
 * \param[in] ApcRoutine Address of a caller-supplied APC routine to be called when the requested operation completes. This parameter is optional and can be NULL.
 * \param[in] ApcContext Pointer to a caller-determined context area. This parameter value is used as the APC context if the caller supplies an APC, or is used as the completion context if an I/O completion object has been associated with the file object.
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that receives the final completion status and information about the operation.
 * \param[in] Buffer Pointer to a caller-allocated buffer that contains the data to write to the file.
 * \param[in] Length The size, in bytes, of the buffer pointed to by Buffer.
 * \param[in] ByteOffset Pointer to a variable that specifies the starting byte offset in the file for beginning the write operation.
 * \param[in] Key Device and intermediate drivers should set this pointer to NULL.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwwritefile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtWriteFile(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_reads_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length,
    _In_opt_ PLARGE_INTEGER ByteOffset,
    _In_opt_ PULONG Key);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtReadFileScatter(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ PFILE_SEGMENT_ELEMENT SegmentArray,
    _In_ ULONG Length,
    _In_opt_ PLARGE_INTEGER ByteOffset,
    _In_opt_ PULONG Key);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtWriteFileGather(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ PFILE_SEGMENT_ELEMENT SegmentArray,
    _In_ ULONG Length,
    _In_opt_ PLARGE_INTEGER ByteOffset,
    _In_opt_ PULONG Key);

#if (NTDDI_VERSION >= NTDDI_WIN10_VB)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCopyFileChunk(
    _In_ HANDLE SourceHandle,
    _In_ HANDLE DestHandle,
    _In_opt_ HANDLE Event,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG Length,
    _In_ PLARGE_INTEGER SourceOffset,
    _In_ PLARGE_INTEGER DestOffset,
    _In_opt_ PULONG SourceKey,
    _In_opt_ PULONG DestKey,
    _In_ ULONG Flags);

#if (NTDDI_VERSION >= NTDDI_WIN11_GA)

#define COPY_FILE_CHUNK_DUPLICATE_EXTENTS    0x00000001L
#define VALID_COPY_FILE_CHUNK_FLAGS          (COPY_FILE_CHUNK_DUPLICATE_EXTENTS)

#else // (NTDDI_VERSION < NTDDI_WIN11_GA)

#define VALID_COPY_FILE_CHUNK_FLAGS          0x00000000L

#endif // (NTDDI_VERSION >= NTDDI_WIN11_GA)

#endif // (NTDDI_VERSION >= NTDDI_WIN10_VB)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtLockFile(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ PLARGE_INTEGER ByteOffset,
    _In_ PLARGE_INTEGER Length,
    _In_ ULONG Key,
    _In_ BOOLEAN FailImmediately,
    _In_ BOOLEAN ExclusiveLock);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnlockFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ PLARGE_INTEGER ByteOffset,
    _In_ PLARGE_INTEGER Length,
    _In_ ULONG Key);

EXTERN_C_END
