#pragma once

#include "../MinDef.h"

EXTERN_C_START

typedef enum _FILE_INFORMATION_CLASS
{
    FileDirectoryInformation = 1,                   // q: FILE_DIRECTORY_INFORMATION (requires FILE_LIST_DIRECTORY) (NtQueryDirectoryFile[Ex])
    FileFullDirectoryInformation,                   // q: FILE_FULL_DIR_INFORMATION (requires FILE_LIST_DIRECTORY) (NtQueryDirectoryFile[Ex])
    FileBothDirectoryInformation,                   // q: FILE_BOTH_DIR_INFORMATION (requires FILE_LIST_DIRECTORY) (NtQueryDirectoryFile[Ex])
    FileBasicInformation,                           // qs: FILE_BASIC_INFORMATION (q: requires FILE_READ_ATTRIBUTES; s: requires FILE_WRITE_ATTRIBUTES)
    FileStandardInformation,                        // q: FILE_STANDARD_INFORMATION, FILE_STANDARD_INFORMATION_EX
    FileInternalInformation,                        // q: FILE_INTERNAL_INFORMATION
    FileEaInformation,                              // q: FILE_EA_INFORMATION (requires FILE_READ_EA)
    FileAccessInformation,                          // q: FILE_ACCESS_INFORMATION
    FileNameInformation,                            // q: FILE_NAME_INFORMATION
    FileRenameInformation,                          // s: FILE_RENAME_INFORMATION (requires DELETE) // 10
    FileLinkInformation,                            // s: FILE_LINK_INFORMATION
    FileNamesInformation,                           // q: FILE_NAMES_INFORMATION (requires FILE_LIST_DIRECTORY) (NtQueryDirectoryFile[Ex])
    FileDispositionInformation,                     // s: FILE_DISPOSITION_INFORMATION (requires DELETE)
    FilePositionInformation,                        // qs: FILE_POSITION_INFORMATION (q: requires FILE_READ_ATTRIBUTES; s: requires FILE_WRITE_ATTRIBUTES)
    FileFullEaInformation,                          // q: FILE_FULL_EA_INFORMATION (requires FILE_READ_EA)
    FileModeInformation,                            // qs: FILE_MODE_INFORMATION (q: requires FILE_READ_ATTRIBUTES; s: requires FILE_WRITE_ATTRIBUTES)
    FileAlignmentInformation,                       // q: FILE_ALIGNMENT_INFORMATION
    FileAllInformation,                             // q: FILE_ALL_INFORMATION
    FileAllocationInformation,                      // s: FILE_ALLOCATION_INFORMATION (requires FILE_WRITE_DATA)
    FileEndOfFileInformation,                       // s: FILE_END_OF_FILE_INFORMATION (requires FILE_WRITE_DATA) // 20
    FileAlternateNameInformation,                   // q: FILE_NAME_INFORMATION
    FileStreamInformation,                          // q: FILE_STREAM_INFORMATION
    FilePipeInformation,                            // qs: FILE_PIPE_INFORMATION (q: requires FILE_READ_ATTRIBUTES; s: requires FILE_WRITE_ATTRIBUTES)
    FilePipeLocalInformation,                       // q: FILE_PIPE_LOCAL_INFORMATION
    FilePipeRemoteInformation,                      // qs: FILE_PIPE_REMOTE_INFORMATION (q: requires FILE_READ_ATTRIBUTES; s: requires FILE_WRITE_ATTRIBUTES)
    FileMailslotQueryInformation,                   // q: FILE_MAILSLOT_QUERY_INFORMATION
    FileMailslotSetInformation,                     // s: FILE_MAILSLOT_SET_INFORMATION
    FileCompressionInformation,                     // q: FILE_COMPRESSION_INFORMATION
    FileObjectIdInformation,                        // q: FILE_OBJECTID_INFORMATION (requires FILE_LIST_DIRECTORY) (NtQueryDirectoryFile[Ex])
    FileCompletionInformation,                      // s: FILE_COMPLETION_INFORMATION // 30
    FileMoveClusterInformation,                     // s: FILE_MOVE_CLUSTER_INFORMATION (requires FILE_WRITE_DATA)
    FileQuotaInformation,                           // q: FILE_QUOTA_INFORMATION (requires FILE_LIST_DIRECTORY) (NtQueryDirectoryFile[Ex])
    FileReparsePointInformation,                    // q: FILE_REPARSE_POINT_INFORMATION (requires FILE_LIST_DIRECTORY) (NtQueryDirectoryFile[Ex])
    FileNetworkOpenInformation,                     // q: FILE_NETWORK_OPEN_INFORMATION
    FileAttributeTagInformation,                    // q: FILE_ATTRIBUTE_TAG_INFORMATION
    FileTrackingInformation,                        // s: FILE_TRACKING_INFORMATION (requires FILE_WRITE_DATA)
    FileIdBothDirectoryInformation,                 // q: FILE_ID_BOTH_DIR_INFORMATION (requires FILE_LIST_DIRECTORY) (NtQueryDirectoryFile[Ex])
    FileIdFullDirectoryInformation,                 // q: FILE_ID_FULL_DIR_INFORMATION (requires FILE_LIST_DIRECTORY) (NtQueryDirectoryFile[Ex])
    FileValidDataLengthInformation,                 // s: FILE_VALID_DATA_LENGTH_INFORMATION (requires FILE_WRITE_DATA and/or SeManageVolumePrivilege)
    FileShortNameInformation,                       // s: FILE_NAME_INFORMATION (requires DELETE) // 40
    FileIoCompletionNotificationInformation,        // qs: FILE_IO_COMPLETION_NOTIFICATION_INFORMATION (q: requires FILE_READ_ATTRIBUTES; s: requires FILE_WRITE_ATTRIBUTES) // since VISTA
    FileIoStatusBlockRangeInformation,              // s: FILE_IOSTATUSBLOCK_RANGE_INFORMATION (requires SeLockMemoryPrivilege)
    FileIoPriorityHintInformation,                  // qs: FILE_IO_PRIORITY_HINT_INFORMATION, FILE_IO_PRIORITY_HINT_INFORMATION_EX (q: requires FILE_READ_DATA)
    FileSfioReserveInformation,                     // qs: FILE_SFIO_RESERVE_INFORMATION (q: requires FILE_READ_DATA)
    FileSfioVolumeInformation,                      // q: FILE_SFIO_VOLUME_INFORMATION
    FileHardLinkInformation,                        // q: FILE_LINKS_INFORMATION
    FileProcessIdsUsingFileInformation,             // q: FILE_PROCESS_IDS_USING_FILE_INFORMATION
    FileNormalizedNameInformation,                  // q: FILE_NAME_INFORMATION
    FileNetworkPhysicalNameInformation,             // q: FILE_NETWORK_PHYSICAL_NAME_INFORMATION
    FileIdGlobalTxDirectoryInformation,             // q: FILE_ID_GLOBAL_TX_DIR_INFORMATION (requires FILE_LIST_DIRECTORY) (NtQueryDirectoryFile[Ex]) // since WIN7 // 50
    FileIsRemoteDeviceInformation,                  // q: FILE_IS_REMOTE_DEVICE_INFORMATION
    FileUnusedInformation,                          // q: 
    FileNumaNodeInformation,                        // q: FILE_NUMA_NODE_INFORMATION
    FileStandardLinkInformation,                    // q: FILE_STANDARD_LINK_INFORMATION
    FileRemoteProtocolInformation,                  // q: FILE_REMOTE_PROTOCOL_INFORMATION
    FileRenameInformationBypassAccessCheck,         // s: FILE_RENAME_INFORMATION // (kernel-mode only) // since WIN8
    FileLinkInformationBypassAccessCheck,           // s: FILE_LINK_INFORMATION // (kernel-mode only)
    FileVolumeNameInformation,                      // q: FILE_VOLUME_NAME_INFORMATION
    FileIdInformation,                              // q: FILE_ID_INFORMATION
    FileIdExtdDirectoryInformation,                 // q: FILE_ID_EXTD_DIR_INFORMATION (requires FILE_LIST_DIRECTORY) (NtQueryDirectoryFile[Ex]) // 60
    FileReplaceCompletionInformation,               // s: FILE_COMPLETION_INFORMATION // since WINBLUE
    FileHardLinkFullIdInformation,                  // q: FILE_LINK_ENTRY_FULL_ID_INFORMATION // FILE_LINKS_FULL_ID_INFORMATION
    FileIdExtdBothDirectoryInformation,             // q: FILE_ID_EXTD_BOTH_DIR_INFORMATION (requires FILE_LIST_DIRECTORY) (NtQueryDirectoryFile[Ex]) // since THRESHOLD
    FileDispositionInformationEx,                   // s: FILE_DISPOSITION_INFO_EX (requires DELETE) // since REDSTONE
    FileRenameInformationEx,                        // s: FILE_RENAME_INFORMATION_EX
    FileRenameInformationExBypassAccessCheck,       // s: FILE_RENAME_INFORMATION_EX // (kernel-mode only)
    FileDesiredStorageClassInformation,             // qs: FILE_DESIRED_STORAGE_CLASS_INFORMATION // since REDSTONE2
    FileStatInformation,                            // q: FILE_STAT_INFORMATION
    FileMemoryPartitionInformation,                 // s: FILE_MEMORY_PARTITION_INFORMATION // since REDSTONE3
    FileStatLxInformation,                          // q: FILE_STAT_LX_INFORMATION (requires FILE_READ_ATTRIBUTES and FILE_READ_EA) // since REDSTONE4 // 70
    FileCaseSensitiveInformation,                   // qs: FILE_CASE_SENSITIVE_INFORMATION
    FileLinkInformationEx,                          // s: FILE_LINK_INFORMATION_EX // since REDSTONE5
    FileLinkInformationExBypassAccessCheck,         // s: FILE_LINK_INFORMATION_EX // (kernel-mode only)
    FileStorageReserveIdInformation,                // qs: FILE_STORAGE_RESERVE_ID_INFORMATION
    FileCaseSensitiveInformationForceAccessCheck,   // qs: FILE_CASE_SENSITIVE_INFORMATION
    FileKnownFolderInformation,                     // qs: FILE_KNOWN_FOLDER_INFORMATION // since WIN11
    FileStatBasicInformation,                       // qs: FILE_STAT_BASIC_INFORMATION // since 23H2
    FileId64ExtdDirectoryInformation,               // q: FILE_ID_64_EXTD_DIR_INFORMATION
    FileId64ExtdBothDirectoryInformation,           // q: FILE_ID_64_EXTD_BOTH_DIR_INFORMATION
    FileIdAllExtdDirectoryInformation,              // q: FILE_ID_ALL_EXTD_DIR_INFORMATION
    FileIdAllExtdBothDirectoryInformation,          // q: FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION
    FileStreamReservationInformation,               // q: FILE_STREAM_RESERVATION_INFORMATION // since 24H2
    FileMupProviderInfo,                            // qs: MUP_PROVIDER_INFORMATION
    FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

// NtQueryInformationFile/NtSetInformationFile types

/**
 * The FILE_BASIC_INFORMATION structure contains timestamps and basic attributes of a file.
 * @li If you specify a value of zero for any of the XxxTime members, the file system keeps a file's current value for that time.
 * @li If you specify a value of -1 for any of the XxxTime members, time stamp updates are disabled for I/O operations performed on the file handle.
 * @li If you specify a value of -2 for any of the XxxTime members, time stamp updates are enabled for I/O operations performed on the file handle.
 * @remarks To set the members of this structure, the caller must have FILE_WRITE_ATTRIBUTES access to the file.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_basic_information
 */
typedef struct _FILE_BASIC_INFORMATION
{
    LARGE_INTEGER CreationTime;         // Specifies the time that the file was created.
    LARGE_INTEGER LastAccessTime;       // Specifies the time that the file was last accessed.
    LARGE_INTEGER LastWriteTime;        // Specifies the time that the file was last written to.
    LARGE_INTEGER ChangeTime;           // Specifies the last time the file was changed.
    ULONG FileAttributes;               // Specifies one or more FILE_ATTRIBUTE_XXX flags.
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

/**
 * The FILE_STANDARD_INFORMATION structure contains standard information of a file.
 * @remarks EndOfFile specifies the byte offset to the end of the file.
 * Because this value is zero-based, it actually refers to the first free byte in the file; that is, it is the offset to the byte immediately following the last valid byte in the file.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_standard_information
 */
typedef struct _FILE_STANDARD_INFORMATION
{
    LARGE_INTEGER AllocationSize;       // The file allocation size in bytes. Usually, this value is a multiple of the sector or cluster size of the underlying physical device.
    LARGE_INTEGER EndOfFile;            // The end of file location as a byte offset.
    ULONG NumberOfLinks;                // The number of hard links to the file.
    BOOLEAN DeletePending;              // The delete pending status. TRUE indicates that a file deletion has been requested.
    BOOLEAN Directory;                  // The file directory status. TRUE indicates the file object represents a directory.
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;

#if (NTDDI_VERSION >= NTDDI_WIN10)
/**
 * The FILE_STANDARD_INFORMATION_EX structure is used as an argument to routines that query or set file information
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_standard_information_ex
 */
typedef struct _FILE_STANDARD_INFORMATION_EX
{
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG NumberOfLinks;
    BOOLEAN DeletePending;
    BOOLEAN Directory;
    BOOLEAN AlternateStream;
    BOOLEAN MetadataAttribute;
} FILE_STANDARD_INFORMATION_EX, *PFILE_STANDARD_INFORMATION_EX;
#endif

/**
 * The FILE_INTERNAL_INFORMATION structure is used to query for the file system's 8-byte file reference number for a file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_internal_information
 * \remark The IndexNumber member is the same as the FileId member of the FILE_ID_BOTH_DIR_INFORMATION and FILE_ID_FULL_DIR_INFORMATION structures.
 */
typedef struct _FILE_INTERNAL_INFORMATION
{
    LARGE_INTEGER IndexNumber;
} FILE_INTERNAL_INFORMATION, *PFILE_INTERNAL_INFORMATION;

/**
 * The FILE_EA_INFORMATION structure is used to query for the size of the extended attributes (EA) for a file.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_ea_information
 */
typedef struct _FILE_EA_INFORMATION
{
    ULONG EaSize;
} FILE_EA_INFORMATION, *PFILE_EA_INFORMATION;

/**
 * The FILE_ACCESS_INFORMATION structure is used to query for or set the access rights of a file.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_access_information
 */
typedef struct _FILE_ACCESS_INFORMATION
{
    ACCESS_MASK AccessFlags;
} FILE_ACCESS_INFORMATION, *PFILE_ACCESS_INFORMATION;

/**
 * The FILE_POSITION_INFORMATION structure is used as an argument to routines that query or set file information.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_position_information
 */
typedef struct _FILE_POSITION_INFORMATION
{
    LARGE_INTEGER CurrentByteOffset;
} FILE_POSITION_INFORMATION, *PFILE_POSITION_INFORMATION;

/**
 * The FILE_MODE_INFORMATION structure is used to query or set the access mode of a file.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_mode_information
 */
typedef struct _FILE_MODE_INFORMATION
{
    ULONG Mode;
} FILE_MODE_INFORMATION, *PFILE_MODE_INFORMATION;

/**
 * The FILE_ALIGNMENT_INFORMATION structure is used to query or set the buffer alignment required by the underlying device.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_file_alignment_information
 */
typedef struct _FILE_ALIGNMENT_INFORMATION
{
    ULONG AlignmentRequirement;
} FILE_ALIGNMENT_INFORMATION, *PFILE_ALIGNMENT_INFORMATION;

/**
 * The FILE_NAME_INFORMATION structure is used to query or set the file name and/or new short name for a file.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_file_name_information
 */
typedef struct _FILE_NAME_INFORMATION
{
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

/**
 * The FILE_ALL_INFORMATION structure is used as a container for several FILE_XXX_INFORMATION structures.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_all_information
 */
typedef struct _FILE_ALL_INFORMATION
{
    FILE_BASIC_INFORMATION BasicInformation;
    FILE_STANDARD_INFORMATION StandardInformation;
    FILE_INTERNAL_INFORMATION InternalInformation;
    FILE_EA_INFORMATION EaInformation;
    FILE_ACCESS_INFORMATION AccessInformation;
    FILE_POSITION_INFORMATION PositionInformation;
    FILE_MODE_INFORMATION ModeInformation;
    FILE_ALIGNMENT_INFORMATION AlignmentInformation;
    FILE_NAME_INFORMATION NameInformation;
} FILE_ALL_INFORMATION, *PFILE_ALL_INFORMATION;

/**
 * The FILE_NETWORK_OPEN_INFORMATION structure is used to query for information that is commonly needed when a file is opened across a network.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_network_open_information
 */
typedef struct _FILE_NETWORK_OPEN_INFORMATION
{
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG FileAttributes;
} FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;

/**
 * The FILE_ATTRIBUTE_TAG_INFORMATION structure is used to query for attribute and reparse tag information for a file.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_file_attribute_tag_information
 */
typedef struct _FILE_ATTRIBUTE_TAG_INFORMATION
{
    ULONG FileAttributes;
    ULONG ReparseTag;
} FILE_ATTRIBUTE_TAG_INFORMATION, *PFILE_ATTRIBUTE_TAG_INFORMATION;

/**
 * The FILE_ALLOCATION_INFORMATION structure is used to set the allocation size for a file.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_allocation_information
 */
typedef struct _FILE_ALLOCATION_INFORMATION
{
    LARGE_INTEGER AllocationSize;
} FILE_ALLOCATION_INFORMATION, *PFILE_ALLOCATION_INFORMATION;

/**
 * The FILE_COMPRESSION_INFORMATION structure describes the state of a compressed data buffer.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_compression_information
 */
typedef struct _FILE_COMPRESSION_INFORMATION
{
    LARGE_INTEGER CompressedFileSize;
    USHORT CompressionFormat;
    UCHAR CompressionUnitShift;
    UCHAR ChunkShift;
    UCHAR ClusterShift;
    UCHAR Reserved[3];
} FILE_COMPRESSION_INFORMATION, *PFILE_COMPRESSION_INFORMATION;

/**
 * The FILE_DISPOSITION_INFORMATION structure is used to mark a file for deletion.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_file_disposition_information
 */
#pragma push_macro("DeleteFile")
#ifdef DeleteFile
#undef DeleteFile
#endif
typedef struct _FILE_DISPOSITION_INFORMATION
{
    BOOLEAN DeleteFile;
} FILE_DISPOSITION_INFORMATION, *PFILE_DISPOSITION_INFORMATION;
#pragma pop_macro("DeleteFile")

/**
 * The FILE_END_OF_FILE_INFORMATION structure is used to set end-of-file information for a file.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_file_end_of_file_information
 */
typedef struct _FILE_END_OF_FILE_INFORMATION
{
    LARGE_INTEGER EndOfFile;
} FILE_END_OF_FILE_INFORMATION, *PFILE_END_OF_FILE_INFORMATION;

//#if (NTDDI_VERSION >= NTDDI_WIN10_RS5)
#define FLAGS_END_OF_FILE_INFO_EX_EXTEND_PAGING             0x00000001
#define FLAGS_END_OF_FILE_INFO_EX_NO_EXTRA_PAGING_EXTEND    0x00000002
#define FLAGS_END_OF_FILE_INFO_EX_TIME_CONSTRAINED          0x00000004
#define FLAGS_DELAY_REASONS_LOG_FILE_FULL                   0x00000001
#define FLAGS_DELAY_REASONS_BITMAP_SCANNED                  0x00000002

typedef struct _FILE_END_OF_FILE_INFORMATION_EX
{
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER PagingFileSizeInMM;
    LARGE_INTEGER PagingFileMaxSize;
    ULONG Flags;
} FILE_END_OF_FILE_INFORMATION_EX, *PFILE_END_OF_FILE_INFORMATION_EX;
//#endif

/**
 * The FILE_VALID_DATA_LENGTH_INFORMATION structure is used to set the valid data length information for a file.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_file_valid_data_length_information
 */
typedef struct _FILE_VALID_DATA_LENGTH_INFORMATION
{
    LARGE_INTEGER ValidDataLength;
} FILE_VALID_DATA_LENGTH_INFORMATION, *PFILE_VALID_DATA_LENGTH_INFORMATION;

#define FILE_LINK_REPLACE_IF_EXISTS 0x00000001 // since RS5
#define FILE_LINK_POSIX_SEMANTICS   0x00000002

#define FILE_LINK_SUPPRESS_STORAGE_RESERVE_INHERITANCE  0x00000008
#define FILE_LINK_NO_INCREASE_AVAILABLE_SPACE           0x00000010
#define FILE_LINK_NO_DECREASE_AVAILABLE_SPACE           0x00000020
#define FILE_LINK_PRESERVE_AVAILABLE_SPACE              0x00000030
#define FILE_LINK_IGNORE_READONLY_ATTRIBUTE             0x00000040
#define FILE_LINK_FORCE_RESIZE_TARGET_SR                0x00000080 // since 19H1
#define FILE_LINK_FORCE_RESIZE_SOURCE_SR                0x00000100
#define FILE_LINK_FORCE_RESIZE_SR                       0x00000180

/**
 * The FILE_LINK_INFORMATION structure is used to create an NTFS hard link to an existing file.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_link_information
 */
typedef struct _FILE_LINK_INFORMATION
{
    BOOLEAN ReplaceIfExists;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_LINK_INFORMATION, *PFILE_LINK_INFORMATION;

typedef struct _FILE_LINK_INFORMATION_EX
{
    ULONG Flags;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_LINK_INFORMATION_EX, *PFILE_LINK_INFORMATION_EX;

typedef struct _FILE_MOVE_CLUSTER_INFORMATION
{
    ULONG ClusterCount;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_MOVE_CLUSTER_INFORMATION, *PFILE_MOVE_CLUSTER_INFORMATION;

/**
 * The FILE_RENAME_INFORMATION structure is used to rename a file.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_rename_information
 */
typedef struct _FILE_RENAME_INFORMATION
{
    BOOLEAN ReplaceIfExists;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_RENAME_INFORMATION, *PFILE_RENAME_INFORMATION;

#define FILE_RENAME_REPLACE_IF_EXISTS                       0x00000001 // since REDSTONE
#define FILE_RENAME_POSIX_SEMANTICS                         0x00000002
#define FILE_RENAME_SUPPRESS_PIN_STATE_INHERITANCE          0x00000004 // since REDSTONE3
#define FILE_RENAME_SUPPRESS_STORAGE_RESERVE_INHERITANCE    0x00000008 // since REDSTONE5
#define FILE_RENAME_NO_INCREASE_AVAILABLE_SPACE             0x00000010
#define FILE_RENAME_NO_DECREASE_AVAILABLE_SPACE             0x00000020
#define FILE_RENAME_PRESERVE_AVAILABLE_SPACE                0x00000030
#define FILE_RENAME_IGNORE_READONLY_ATTRIBUTE               0x00000040
#define FILE_RENAME_FORCE_RESIZE_TARGET_SR                  0x00000080 // since 19H1
#define FILE_RENAME_FORCE_RESIZE_SOURCE_SR                  0x00000100
#define FILE_RENAME_FORCE_RESIZE_SR                         0x00000180

/**
 * The FILE_RENAME_INFORMATION_EX structure is used to rename a file.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_rename_information
 */
typedef struct _FILE_RENAME_INFORMATION_EX
{
    ULONG Flags;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_RENAME_INFORMATION_EX, *PFILE_RENAME_INFORMATION_EX;

/**
 * The FILE_STREAM_INFORMATION structure contains information about a file stream.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_stream_information
 */
typedef struct _FILE_STREAM_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG StreamNameLength;
    LARGE_INTEGER StreamSize;
    LARGE_INTEGER StreamAllocationSize;
    _Field_size_bytes_(StreamNameLength) WCHAR StreamName[1];
} FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;

typedef struct _FILE_TRACKING_INFORMATION
{
    HANDLE DestinationFile;
    ULONG ObjectInformationLength;
    _Field_size_bytes_(ObjectInformationLength) CHAR ObjectInformation[1];
} FILE_TRACKING_INFORMATION, *PFILE_TRACKING_INFORMATION;

/**
 * The FILE_COMPLETION_INFORMATION structure contains the port handle and key for an I/O completion port created for a file handle.
 *
 * @remarks he FILE_COMPLETION_INFORMATION structure is used to replace the completion information for a port handle set in Port.
 * Completion information is replaced with the ZwSetInformationFile routine with the FileInformationClass parameter set to FileReplaceCompletionInformation.
 * The Port and Key members of FILE_COMPLETION_INFORMATION are set to their new values. To remove an existing completion port for a file handle, Port is set to NULL.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_completion_information
 */
typedef struct _FILE_COMPLETION_INFORMATION
{
    HANDLE Port;
    PVOID Key;
} FILE_COMPLETION_INFORMATION, *PFILE_COMPLETION_INFORMATION;

/**
 * The FILE_PIPE_INFORMATION structure contains information about a named pipe that is not specific to the local or the remote end of the pipe.
 *
 * @remarks If ReadMode is set to FILE_PIPE_BYTE_STREAM_MODE, any attempt to change it must fail with a STATUS_INVALID_PARAMETER error code.
 * When CompletionMode is set to FILE_PIPE_QUEUE_OPERATION, if the pipe is connected to, read to, or written from,
 * the operation is not completed until there is data to read, all data is written, or a client is connected.
 * When CompletionMode is set to FILE_PIPE_COMPLETE_OPERATION, if the pipe is being connected to, read to, or written from, the operation is completed immediately.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_pipe_information
 */
typedef struct _FILE_PIPE_INFORMATION
{
    ULONG ReadMode;
    ULONG CompletionMode;
} FILE_PIPE_INFORMATION, *PFILE_PIPE_INFORMATION;

/**
 * The FILE_PIPE_LOCAL_INFORMATION structure contains information about the local end of a named pipe.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_pipe_local_information
 */
typedef struct _FILE_PIPE_LOCAL_INFORMATION
{
    ULONG NamedPipeType;
    ULONG NamedPipeConfiguration;
    ULONG MaximumInstances;
    ULONG CurrentInstances;
    ULONG InboundQuota;
    ULONG ReadDataAvailable;
    ULONG OutboundQuota;
    ULONG WriteQuotaAvailable;
    ULONG NamedPipeState;
    ULONG NamedPipeEnd;
} FILE_PIPE_LOCAL_INFORMATION, *PFILE_PIPE_LOCAL_INFORMATION;

/**
 * The FILE_PIPE_REMOTE_INFORMATION structure contains information about the remote end of a named pipe.
 *
 * @remarks Remote information is not available for local pipes or for the server end of a remote pipe.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_pipe_remote_information
 */
typedef struct _FILE_PIPE_REMOTE_INFORMATION
{
    LARGE_INTEGER CollectDataTime;
    ULONG MaximumCollectionCount;
} FILE_PIPE_REMOTE_INFORMATION, *PFILE_PIPE_REMOTE_INFORMATION;

/**
 * The FILE_MAILSLOT_QUERY_INFORMATION structure contains information about a mailslot.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_mailslot_query_information
 */
typedef struct _FILE_MAILSLOT_QUERY_INFORMATION
{
    ULONG MaximumMessageSize;
    ULONG MailslotQuota;
    ULONG NextMessageSize;
    ULONG MessagesAvailable;
    LARGE_INTEGER ReadTimeout;
} FILE_MAILSLOT_QUERY_INFORMATION, *PFILE_MAILSLOT_QUERY_INFORMATION;

/**
 * The FILE_MAILSLOT_SET_INFORMATION structure is used to set a value on a mailslot.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_mailslot_set_information
 */
typedef struct _FILE_MAILSLOT_SET_INFORMATION
{
    PLARGE_INTEGER ReadTimeout;
} FILE_MAILSLOT_SET_INFORMATION, *PFILE_MAILSLOT_SET_INFORMATION;

/**
 * The FILE_REPARSE_POINT_INFORMATION structure contains information about a reparse point.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_reparse_point_information
 */
typedef struct _FILE_REPARSE_POINT_INFORMATION
{
    LONGLONG FileReference;
    ULONG Tag;
} FILE_REPARSE_POINT_INFORMATION, *PFILE_REPARSE_POINT_INFORMATION;

typedef struct _FILE_LINK_ENTRY_INFORMATION
{
    ULONG NextEntryOffset;
    LONGLONG ParentFileId; // LARGE_INTEGER
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_LINK_ENTRY_INFORMATION, *PFILE_LINK_ENTRY_INFORMATION;

typedef struct _FILE_LINKS_INFORMATION
{
    ULONG BytesNeeded;
    ULONG EntriesReturned;
    FILE_LINK_ENTRY_INFORMATION Entry;
} FILE_LINKS_INFORMATION, *PFILE_LINKS_INFORMATION;

/**
 * The FILE_NETWORK_PHYSICAL_NAME_INFORMATION structure contains information about the network physical name of a file.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_network_physical_name_information
 */
typedef struct _FILE_NETWORK_PHYSICAL_NAME_INFORMATION
{
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_NETWORK_PHYSICAL_NAME_INFORMATION, *PFILE_NETWORK_PHYSICAL_NAME_INFORMATION;

/**
 * The FILE_STANDARD_LINK_INFORMATION structure contains standard information about a file link.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-file_standard_link_information
 */
typedef struct _FILE_STANDARD_LINK_INFORMATION
{
    ULONG NumberOfAccessibleLinks;
    ULONG TotalNumberOfLinks;
    BOOLEAN DeletePending;
    BOOLEAN Directory;
} FILE_STANDARD_LINK_INFORMATION, *PFILE_STANDARD_LINK_INFORMATION;

typedef struct _FILE_SFIO_RESERVE_INFORMATION
{
    ULONG RequestsPerPeriod;
    ULONG Period;
    BOOLEAN RetryFailures;
    BOOLEAN Discardable;
    ULONG RequestSize;
    ULONG NumOutstandingRequests;
} FILE_SFIO_RESERVE_INFORMATION, *PFILE_SFIO_RESERVE_INFORMATION;

typedef struct _FILE_SFIO_VOLUME_INFORMATION
{
    ULONG MaximumRequestsPerPeriod;
    ULONG MinimumPeriod;
    ULONG MinimumTransferSize;
} FILE_SFIO_VOLUME_INFORMATION, *PFILE_SFIO_VOLUME_INFORMATION;

typedef enum _IO_PRIORITY_HINT
{
    IoPriorityVeryLow = 0, // Defragging, content indexing and other background I/Os.
    IoPriorityLow, // Prefetching for applications.
    IoPriorityNormal, // Normal I/Os.
    IoPriorityHigh, // Used by filesystems for checkpoint I/O.
    IoPriorityCritical, // Used by memory manager. Not available for applications.
    MaxIoPriorityTypes
} IO_PRIORITY_HINT;

/**
 * The FILE_IO_PRIORITY_HINT_INFORMATION structure is used to query and set the default IRP priority hint for requests on the specified file handle.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_io_priority_hint_information
 */
typedef struct DECLSPEC_ALIGN(8) _FILE_IO_PRIORITY_HINT_INFORMATION
{
    IO_PRIORITY_HINT PriorityHint;
} FILE_IO_PRIORITY_HINT_INFORMATION, *PFILE_IO_PRIORITY_HINT_INFORMATION;

/**
 * The FILE_IO_PRIORITY_HINT_INFORMATION_EX structure is used to query and set the default IRP priority hint for requests on the specified file handle.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_io_priority_hint_information
 */
typedef struct _FILE_IO_PRIORITY_HINT_INFORMATION_EX
{
    IO_PRIORITY_HINT PriorityHint;
    BOOLEAN BoostOutstanding;
} FILE_IO_PRIORITY_HINT_INFORMATION_EX, *PFILE_IO_PRIORITY_HINT_INFORMATION_EX;

#define FILE_SKIP_COMPLETION_PORT_ON_SUCCESS 0x1
#define FILE_SKIP_SET_EVENT_ON_HANDLE 0x2
#define FILE_SKIP_SET_USER_EVENT_ON_FAST_IO 0x4

typedef struct _FILE_IO_COMPLETION_NOTIFICATION_INFORMATION
{
    ULONG Flags;
} FILE_IO_COMPLETION_NOTIFICATION_INFORMATION, *PFILE_IO_COMPLETION_NOTIFICATION_INFORMATION;

typedef struct _FILE_PROCESS_IDS_USING_FILE_INFORMATION
{
    ULONG NumberOfProcessIdsInList;
    _Field_size_(NumberOfProcessIdsInList) HANDLE ProcessIdList[ANYSIZE_ARRAY];
} FILE_PROCESS_IDS_USING_FILE_INFORMATION, *PFILE_PROCESS_IDS_USING_FILE_INFORMATION;

/**
 * The FILE_IS_REMOTE_DEVICE_INFORMATION structure indicates whether the file system that contains the file is a remote file system.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_is_remote_device_information
 */
typedef struct _FILE_IS_REMOTE_DEVICE_INFORMATION
{
    BOOLEAN IsRemote; // A value that indicates whether the file system that contains the file is a remote file system.
} FILE_IS_REMOTE_DEVICE_INFORMATION, *PFILE_IS_REMOTE_DEVICE_INFORMATION;

typedef struct _FILE_NUMA_NODE_INFORMATION
{
    USHORT NodeNumber;
} FILE_NUMA_NODE_INFORMATION, *PFILE_NUMA_NODE_INFORMATION;

typedef struct _FILE_IOSTATUSBLOCK_RANGE_INFORMATION
{
    PUCHAR IoStatusBlockRange;
    ULONG Length;
} FILE_IOSTATUSBLOCK_RANGE_INFORMATION, *PFILE_IOSTATUSBLOCK_RANGE_INFORMATION;

// Win32 FILE_REMOTE_PROTOCOL_INFO
typedef struct _FILE_REMOTE_PROTOCOL_INFORMATION
{
    // Structure Version
    USHORT StructureVersion;     // 1 for Win7, 2 for Win8 SMB3, 3 for Blue SMB3, 4 for RS5
    USHORT StructureSize;        // sizeof(FILE_REMOTE_PROTOCOL_INFORMATION)

    ULONG Protocol;             // Protocol (WNNC_NET_*) defined in winnetwk.h or ntifs.h.

    // Protocol Version & Type
    USHORT ProtocolMajorVersion;
    USHORT ProtocolMinorVersion;
    USHORT ProtocolRevision;

    USHORT Reserved;

    // Protocol-Generic Information
    ULONG Flags;

    struct
    {
        ULONG Reserved[8];
    } GenericReserved;

    // Protocol specific information

#if (NTDDI_VERSION < NTDDI_WIN8)
    struct
    {
        ULONG Reserved[16];
    } ProtocolSpecificReserved;
#endif

#if (NTDDI_VERSION >= NTDDI_WIN8)
    union
    {
        struct
        {
            struct
            {
                ULONG Capabilities;
            } Server;
            struct
            {
                ULONG Capabilities;
#if (NTDDI_VERSION >= NTDDI_21H1)
                ULONG ShareFlags;
#else
                ULONG CachingFlags;
#endif
#if (NTDDI_VERSION >= NTDDI_WIN10_RS5)
                UCHAR ShareType;
                UCHAR Reserved0[3];
                ULONG Reserved1;
#endif
            } Share;
        } Smb2;
        ULONG Reserved[16];
    } ProtocolSpecific;
#endif
} FILE_REMOTE_PROTOCOL_INFORMATION, *PFILE_REMOTE_PROTOCOL_INFORMATION;

#define CHECKSUM_ENFORCEMENT_OFF 0x00000001

typedef struct _FILE_INTEGRITY_STREAM_INFORMATION
{
    USHORT ChecksumAlgorithm;
    UCHAR ChecksumChunkShift;
    UCHAR ClusterShift;
    ULONG Flags;
} FILE_INTEGRITY_STREAM_INFORMATION, *PFILE_INTEGRITY_STREAM_INFORMATION;

typedef struct _FILE_VOLUME_NAME_INFORMATION
{
    ULONG DeviceNameLength;
    _Field_size_bytes_(DeviceNameLength) WCHAR DeviceName[1];
} FILE_VOLUME_NAME_INFORMATION, *PFILE_VOLUME_NAME_INFORMATION;

#ifndef FILE_INVALID_FILE_ID
#define FILE_INVALID_FILE_ID ((LONGLONG)-1LL)
#endif

#define FILE_ID_IS_INVALID(FID) ((FID).QuadPart == FILE_INVALID_FILE_ID)

#define FILE_ID_128_IS_INVALID(FID128) \
    (((FID128).Identifier[0] == (UCHAR)-1) && \
    ((FID128).Identifier[1] == (UCHAR)-1) && \
    ((FID128).Identifier[2] == (UCHAR)-1) && \
    ((FID128).Identifier[3] == (UCHAR)-1) && \
    ((FID128).Identifier[4] == (UCHAR)-1) && \
    ((FID128).Identifier[5] == (UCHAR)-1) && \
    ((FID128).Identifier[6] == (UCHAR)-1) && \
    ((FID128).Identifier[7] == (UCHAR)-1) && \
    ((FID128).Identifier[8] == (UCHAR)-1) && \
    ((FID128).Identifier[9] == (UCHAR)-1) && \
    ((FID128).Identifier[10] == (UCHAR)-1) && \
    ((FID128).Identifier[11] == (UCHAR)-1) && \
    ((FID128).Identifier[12] == (UCHAR)-1) && \
    ((FID128).Identifier[13] == (UCHAR)-1) && \
    ((FID128).Identifier[14] == (UCHAR)-1) && \
    ((FID128).Identifier[15] == (UCHAR)-1))

#define MAKE_INVALID_FILE_ID_128(FID128) { \
    ((FID128).Identifier[0] = (UCHAR)-1); \
    ((FID128).Identifier[1] = (UCHAR)-1); \
    ((FID128).Identifier[2] = (UCHAR)-1); \
    ((FID128).Identifier[3] = (UCHAR)-1); \
    ((FID128).Identifier[4] = (UCHAR)-1); \
    ((FID128).Identifier[5] = (UCHAR)-1); \
    ((FID128).Identifier[6] = (UCHAR)-1); \
    ((FID128).Identifier[7] = (UCHAR)-1); \
    ((FID128).Identifier[8] = (UCHAR)-1); \
    ((FID128).Identifier[9] = (UCHAR)-1); \
    ((FID128).Identifier[10] = (UCHAR)-1); \
    ((FID128).Identifier[11] = (UCHAR)-1); \
    ((FID128).Identifier[12] = (UCHAR)-1); \
    ((FID128).Identifier[13] = (UCHAR)-1); \
    ((FID128).Identifier[14] = (UCHAR)-1); \
    ((FID128).Identifier[15] = (UCHAR)-1); \
}

/**
 * The FILE_ID_INFORMATION structure is used to query file identification information.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-file_id_information
 */
typedef struct _FILE_ID_INFORMATION
{
    ULONGLONG VolumeSerialNumber;
    FILE_ID_128 FileId;
} FILE_ID_INFORMATION, *PFILE_ID_INFORMATION;

/**
 * The FILE_ID_EXTD_DIR_INFORMATION structure is used to query 128-bit file reference number information for the files in a directory.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-file_id_extd_dir_information
 */
typedef struct _FILE_ID_EXTD_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    ULONG ReparsePointTag;
    FILE_ID_128 FileId;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_EXTD_DIR_INFORMATION, *PFILE_ID_EXTD_DIR_INFORMATION;

#define FileIdExtdDirectoryInformationDefinition {                  \
    FileIdExtdDirectoryInformation,                                 \
    FIELD_OFFSET(FILE_ID_EXTD_DIR_INFORMATION, NextEntryOffset),    \
    FIELD_OFFSET(FILE_ID_EXTD_DIR_INFORMATION, FileName),           \
    FIELD_OFFSET(FILE_ID_EXTD_DIR_INFORMATION, FileNameLength)      \
}

typedef struct _FILE_LINK_ENTRY_FULL_ID_INFORMATION
{
    ULONG NextEntryOffset;
    FILE_ID_128 ParentFileId;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_LINK_ENTRY_FULL_ID_INFORMATION, *PFILE_LINK_ENTRY_FULL_ID_INFORMATION;

typedef struct _FILE_LINKS_FULL_ID_INFORMATION
{
    ULONG BytesNeeded;
    ULONG EntriesReturned;
    FILE_LINK_ENTRY_FULL_ID_INFORMATION Entry;
} FILE_LINKS_FULL_ID_INFORMATION, *PFILE_LINKS_FULL_ID_INFORMATION;

typedef struct _FILE_ID_EXTD_BOTH_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    ULONG ReparsePointTag;
    FILE_ID_128 FileId;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_EXTD_BOTH_DIR_INFORMATION, *PFILE_ID_EXTD_BOTH_DIR_INFORMATION;

#define FileIdExtdBothDirectoryInformationDefinition {                  \
    FileIdExtdBothDirectoryInformation,                                 \
    FIELD_OFFSET(FILE_ID_EXTD_BOTH_DIR_INFORMATION, NextEntryOffset),   \
    FIELD_OFFSET(FILE_ID_EXTD_BOTH_DIR_INFORMATION, FileName),          \
    FIELD_OFFSET(FILE_ID_EXTD_BOTH_DIR_INFORMATION, FileNameLength)     \
}

typedef struct _FILE_ID_64_EXTD_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    ULONG ReparsePointTag;
    LARGE_INTEGER FileId;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_64_EXTD_DIR_INFORMATION, *PFILE_ID_64_EXTD_DIR_INFORMATION;

#define FileId64ExtdDirectoryInformationDefinition {                    \
    FileId64ExtdDirectoryInformation,                                   \
    FIELD_OFFSET(FILE_ID_64_EXTD_DIR_INFORMATION, NextEntryOffset),     \
    FIELD_OFFSET(FILE_ID_64_EXTD_DIR_INFORMATION, FileName),            \
    FIELD_OFFSET(FILE_ID_64_EXTD_DIR_INFORMATION, FileNameLength)       \
}

typedef struct _FILE_ID_64_EXTD_BOTH_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    ULONG ReparsePointTag;
    LARGE_INTEGER FileId;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_64_EXTD_BOTH_DIR_INFORMATION, *PFILE_ID_64_EXTD_BOTH_DIR_INFORMATION;

#define FileId64ExtdBothDirectoryInformationDefinition {                    \
    FileId64ExtdBothDirectoryInformation,                                   \
    FIELD_OFFSET(FILE_ID_64_EXTD_BOTH_DIR_INFORMATION, NextEntryOffset),    \
    FIELD_OFFSET(FILE_ID_64_EXTD_BOTH_DIR_INFORMATION, FileName),           \
    FIELD_OFFSET(FILE_ID_64_EXTD_BOTH_DIR_INFORMATION, FileNameLength)      \
}

typedef struct _FILE_ID_ALL_EXTD_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    ULONG ReparsePointTag;
    LARGE_INTEGER FileId;
    FILE_ID_128 FileId128;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_ALL_EXTD_DIR_INFORMATION, *PFILE_ID_ALL_EXTD_DIR_INFORMATION;

#define FileIdAllExtdDirectoryInformationDefinition {                    \
    FileIdAllExtdDirectoryInformation,                                   \
    FIELD_OFFSET(FILE_ID_ALL_EXTD_DIR_INFORMATION, NextEntryOffset),     \
    FIELD_OFFSET(FILE_ID_ALL_EXTD_DIR_INFORMATION, FileName),            \
    FIELD_OFFSET(FILE_ID_ALL_EXTD_DIR_INFORMATION, FileNameLength)       \
}

typedef struct _FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    ULONG ReparsePointTag;
    LARGE_INTEGER FileId;
    FILE_ID_128 FileId128;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION, *PFILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION;

#define FileIdAllExtdBothDirectoryInformationDefinition {                    \
    FileIdAllExtdBothDirectoryInformation,                                   \
    FIELD_OFFSET(FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION, NextEntryOffset),    \
    FIELD_OFFSET(FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION, FileName),           \
    FIELD_OFFSET(FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION, FileNameLength)      \
}

#if !defined(NTDDI_WIN11_GE) || (NTDDI_VERSION < NTDDI_WIN11_GE)
typedef struct _FILE_STAT_INFORMATION
{
    LARGE_INTEGER FileId;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG FileAttributes;
    ULONG ReparseTag;
    ULONG NumberOfLinks;
    ACCESS_MASK EffectiveAccess;
} FILE_STAT_INFORMATION, *PFILE_STAT_INFORMATION;

typedef struct _FILE_STAT_BASIC_INFORMATION
{
    LARGE_INTEGER FileId;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG FileAttributes;
    ULONG ReparseTag;
    ULONG NumberOfLinks;
    ULONG DeviceType;
    ULONG DeviceCharacteristics;
    ULONG Reserved;
    LARGE_INTEGER VolumeSerialNumber;
    FILE_ID_128 FileId128;
} FILE_STAT_BASIC_INFORMATION, *PFILE_STAT_BASIC_INFORMATION;
#endif

/**
 * The FILE_MEMORY_PARTITION_INFORMATION structure is used to query information about a memory partition.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_memory_partition_information
 */
typedef struct _FILE_MEMORY_PARTITION_INFORMATION
{
    HANDLE OwnerPartitionHandle;
    union
    {
        struct
        {
            UCHAR NoCrossPartitionAccess;
            UCHAR Spare[3];
        };
        ULONG AllFlags;
    } Flags;
} FILE_MEMORY_PARTITION_INFORMATION, *PFILE_MEMORY_PARTITION_INFORMATION;

// LxFlags
#define LX_FILE_METADATA_HAS_UID 0x1
#define LX_FILE_METADATA_HAS_GID 0x2
#define LX_FILE_METADATA_HAS_MODE 0x4
#define LX_FILE_METADATA_HAS_DEVICE_ID 0x8
#define LX_FILE_CASE_SENSITIVE_DIR 0x10

#if !defined(NTDDI_WIN11_GE) || (NTDDI_VERSION < NTDDI_WIN11_GE)
typedef struct _FILE_STAT_LX_INFORMATION
{
    LARGE_INTEGER FileId;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG FileAttributes;
    ULONG ReparseTag;
    ULONG NumberOfLinks;
    ACCESS_MASK EffectiveAccess;
    ULONG LxFlags;
    ULONG LxUid;
    ULONG LxGid;
    ULONG LxMode;
    ULONG LxDeviceIdMajor;
    ULONG LxDeviceIdMinor;
} FILE_STAT_LX_INFORMATION, *PFILE_STAT_LX_INFORMATION;
#endif

typedef struct _FILE_STORAGE_RESERVE_ID_INFORMATION
{
    STORAGE_RESERVE_ID StorageReserveId;
} FILE_STORAGE_RESERVE_ID_INFORMATION, *PFILE_STORAGE_RESERVE_ID_INFORMATION;

#define FILE_CS_FLAG_CASE_SENSITIVE_DIR     0x00000001

#if !defined(NTDDI_WIN11_GE) || (NTDDI_VERSION < NTDDI_WIN11_GE)
typedef struct _FILE_CASE_SENSITIVE_INFORMATION
{
    ULONG Flags;
} FILE_CASE_SENSITIVE_INFORMATION, *PFILE_CASE_SENSITIVE_INFORMATION;
#endif

typedef enum _FILE_KNOWN_FOLDER_TYPE
{
    KnownFolderNone = 0,
    KnownFolderDesktop,
    KnownFolderDocuments,
    KnownFolderDownloads,
    KnownFolderMusic,
    KnownFolderPictures,
    KnownFolderVideos,
    KnownFolderOther,
    KnownFolderMax
} FILE_KNOWN_FOLDER_TYPE;

typedef struct _FILE_KNOWN_FOLDER_INFORMATION
{
    FILE_KNOWN_FOLDER_TYPE Type;
} FILE_KNOWN_FOLDER_INFORMATION, *PFILE_KNOWN_FOLDER_INFORMATION;

// private
typedef struct _FILE_STREAM_RESERVATION_INFORMATION
{
    ULONG_PTR TrackedReservation;
    ULONG_PTR EnforcedReservation;
} FILE_STREAM_RESERVATION_INFORMATION, *PFILE_STREAM_RESERVATION_INFORMATION;

// private
typedef struct _MUP_PROVIDER_INFORMATION
{
    ULONG Level;
    PVOID Buffer;
    PULONG BufferSize;
} MUP_PROVIDER_INFORMATION, *PMUP_PROVIDER_INFORMATION;

// NtQueryDirectoryFile types

typedef struct _FILE_INFORMATION_DEFINITION
{
    FILE_INFORMATION_CLASS Class;
    ULONG NextEntryOffset;
    ULONG FileNameOffset;
    ULONG FileNameLengthOffset;
} FILE_INFORMATION_DEFINITION, *PFILE_INFORMATION_DEFINITION;

typedef struct _FILE_DIRECTORY_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;

#define FileDirectoryInformationDefinition {                    \
    FileDirectoryInformation,                                   \
    FIELD_OFFSET(FILE_DIRECTORY_INFORMATION, NextEntryOffset),  \
    FIELD_OFFSET(FILE_DIRECTORY_INFORMATION, FileName),         \
    FIELD_OFFSET(FILE_DIRECTORY_INFORMATION, FileNameLength)    \
}

typedef struct _FILE_FULL_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_FULL_DIR_INFORMATION, *PFILE_FULL_DIR_INFORMATION;

#define FileFullDirectoryInformationDefinition {                \
    FileFullDirectoryInformation,                               \
    FIELD_OFFSET(FILE_FULL_DIR_INFORMATION, NextEntryOffset),   \
    FIELD_OFFSET(FILE_FULL_DIR_INFORMATION, FileName),          \
    FIELD_OFFSET(FILE_FULL_DIR_INFORMATION, FileNameLength)     \
}

typedef struct _FILE_ID_FULL_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    LARGE_INTEGER FileId;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_FULL_DIR_INFORMATION, *PFILE_ID_FULL_DIR_INFORMATION;

#define FileIdFullDirectoryInformationDefinition {              \
    FileIdFullDirectoryInformation,                             \
    FIELD_OFFSET(FILE_ID_FULL_DIR_INFORMATION, NextEntryOffset),\
    FIELD_OFFSET(FILE_ID_FULL_DIR_INFORMATION, FileName),       \
    FIELD_OFFSET(FILE_ID_FULL_DIR_INFORMATION, FileNameLength)  \
}

typedef struct _FILE_BOTH_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;

#define FileBothDirectoryInformationDefinition {                \
    FileBothDirectoryInformation,                               \
    FIELD_OFFSET(FILE_BOTH_DIR_INFORMATION, NextEntryOffset),   \
    FIELD_OFFSET(FILE_BOTH_DIR_INFORMATION, FileName),          \
    FIELD_OFFSET(FILE_BOTH_DIR_INFORMATION, FileNameLength)     \
}

typedef struct _FILE_ID_BOTH_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    LARGE_INTEGER FileId;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_BOTH_DIR_INFORMATION, *PFILE_ID_BOTH_DIR_INFORMATION;

#define FileIdBothDirectoryInformationDefinition {              \
    FileIdBothDirectoryInformation,                             \
    FIELD_OFFSET(FILE_ID_BOTH_DIR_INFORMATION, NextEntryOffset),\
    FIELD_OFFSET(FILE_ID_BOTH_DIR_INFORMATION, FileName),       \
    FIELD_OFFSET(FILE_ID_BOTH_DIR_INFORMATION, FileNameLength)  \
}

typedef struct _FILE_NAMES_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_NAMES_INFORMATION, *PFILE_NAMES_INFORMATION;

#define FileNamesInformationDefinition {                    \
    FileNamesInformation,                                   \
    FIELD_OFFSET(FILE_NAMES_INFORMATION, NextEntryOffset),  \
    FIELD_OFFSET(FILE_NAMES_INFORMATION, FileName),         \
    FIELD_OFFSET(FILE_NAMES_INFORMATION, FileNameLength)    \
}

typedef struct _FILE_ID_GLOBAL_TX_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    LARGE_INTEGER FileId;
    GUID LockingTransactionId;
    ULONG TxInfoFlags;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_GLOBAL_TX_DIR_INFORMATION, *PFILE_ID_GLOBAL_TX_DIR_INFORMATION;

#define FILE_ID_GLOBAL_TX_DIR_INFO_FLAG_WRITELOCKED 0x00000001
#define FILE_ID_GLOBAL_TX_DIR_INFO_FLAG_VISIBLE_TO_TX 0x00000002
#define FILE_ID_GLOBAL_TX_DIR_INFO_FLAG_VISIBLE_OUTSIDE_TX 0x00000004

#define FileIdGlobalTxDirectoryInformationDefinition {                  \
    FileIdGlobalTxDirectoryInformation,                                 \
    FIELD_OFFSET(FILE_ID_GLOBAL_TX_DIR_INFORMATION, NextEntryOffset),   \
    FIELD_OFFSET(FILE_ID_GLOBAL_TX_DIR_INFORMATION, FileName),          \
    FIELD_OFFSET(FILE_ID_GLOBAL_TX_DIR_INFORMATION, FileNameLength)     \
}

typedef struct _FILE_OBJECTID_INFORMATION
{
    ULONGLONG FileReference;
    UCHAR ObjectId[16]; // GUID
    union
    {
        struct
        {
            UCHAR BirthVolumeId[16];
            UCHAR BirthObjectId[16];
            UCHAR DomainId[16];
        };
        UCHAR ExtendedInfo[48];
    };
} FILE_OBJECTID_INFORMATION, *PFILE_OBJECTID_INFORMATION;

typedef struct _FILE_DIRECTORY_NEXT_INFORMATION
{
    ULONG NextEntryOffset;
} FILE_DIRECTORY_NEXT_INFORMATION, *PFILE_DIRECTORY_NEXT_INFORMATION;

// NtQueryEaFile/NtSetEaFile types

typedef struct _FILE_FULL_EA_INFORMATION
{
    ULONG NextEntryOffset;
    UCHAR Flags;
    UCHAR EaNameLength;
    USHORT EaValueLength;
    _Field_size_bytes_(EaNameLength) CHAR EaName[1];
    // ...
    // UCHAR EaValue[1]
} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;

typedef struct _FILE_GET_EA_INFORMATION
{
    ULONG NextEntryOffset;
    UCHAR EaNameLength;
    _Field_size_bytes_(EaNameLength) CHAR EaName[1];
} FILE_GET_EA_INFORMATION, *PFILE_GET_EA_INFORMATION;

// NtQueryQuotaInformationFile/NtSetQuotaInformationFile types

typedef struct _FILE_GET_QUOTA_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG SidLength;
    _Field_size_bytes_(SidLength) SID Sid;
} FILE_GET_QUOTA_INFORMATION, *PFILE_GET_QUOTA_INFORMATION;

typedef struct _FILE_QUOTA_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG SidLength;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER QuotaUsed;
    LARGE_INTEGER QuotaThreshold;
    LARGE_INTEGER QuotaLimit;
    _Field_size_bytes_(SidLength) SID Sid;
} FILE_QUOTA_INFORMATION, *PFILE_QUOTA_INFORMATION;

/* Supported Features, ntifs.h */

//
//  To better guarantee backwards compatibility for selective new file system
//  functionality, this new functionality will be disabled until all mini
//  file system filters as well as legacy file system filters explicitly opt-in
//  to this new functionality.  This is controlled by a new registry key in
//  the filters service definition called "SupportedFeatures".
//
//  File System filters need to update their .INF files to set state that the
//  given functionality is now supported.  Even if a filter can't actually
//  support the given operations they should mark in the .INF that it is
//  supported and modify their filter to fail the operations they don't support.
//
//  Following are the currently defined flag definitions for new functionality
//  and the indicator as to which version
//

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)

#define SUPPORTED_FS_FEATURES_OFFLOAD_READ    0x00000001
#define SUPPORTED_FS_FEATURES_OFFLOAD_WRITE   0x00000002

#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)

#define SUPPORTED_FS_FEATURES_QUERY_OPEN      0x00000004

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10_CO)

#define SUPPORTED_FS_FEATURES_BYPASS_IO       0x00000008

#define SUPPORTED_FS_FEATURES_VALID_MASK      (SUPPORTED_FS_FEATURES_OFFLOAD_READ |\
                                               SUPPORTED_FS_FEATURES_OFFLOAD_WRITE |\
                                               SUPPORTED_FS_FEATURES_QUERY_OPEN |\
                                               SUPPORTED_FS_FEATURES_BYPASS_IO)

#else /*_WIN32_WINNT >= _WIN32_WINNT_WIN10_CO*/


#define SUPPORTED_FS_FEATURES_VALID_MASK      (SUPPORTED_FS_FEATURES_OFFLOAD_READ |\
                                               SUPPORTED_FS_FEATURES_OFFLOAD_WRITE |\
                                               SUPPORTED_FS_FEATURES_QUERY_OPEN)

#endif /*_WIN32_WINNT >= _WIN32_WINNT_WIN10_CO*/

#else  /*(NTDDI_VERSION >= NTDDI_WIN10_RS2)*/

#define SUPPORTED_FS_FEATURES_VALID_MASK      (SUPPORTED_FS_FEATURES_OFFLOAD_READ |\
                                               SUPPORTED_FS_FEATURES_OFFLOAD_WRITE)

#endif /*(NTDDI_VERSION >= NTDDI_WIN10_RS2) */
#endif /*_WIN32_WINNT >= _WIN32_WINNT_WIN8 */

typedef enum _FSINFOCLASS
{
    FileFsVolumeInformation = 1,            // q: FILE_FS_VOLUME_INFORMATION
    FileFsLabelInformation,                 // s: FILE_FS_LABEL_INFORMATION // SeManageVolumePrivilege
    FileFsSizeInformation,                  // q: FILE_FS_SIZE_INFORMATION
    FileFsDeviceInformation,                // q: FILE_FS_DEVICE_INFORMATION
    FileFsAttributeInformation,             // q: FILE_FS_ATTRIBUTE_INFORMATION
    FileFsControlInformation,               // qs: FILE_FS_CONTROL_INFORMATION // SeManageVolumePrivilege
    FileFsFullSizeInformation,              // q: FILE_FS_FULL_SIZE_INFORMATION
    FileFsObjectIdInformation,              // qs: FILE_FS_OBJECTID_INFORMATION // SeRestorePrivilege
    FileFsDriverPathInformation,            // q: FILE_FS_DRIVER_PATH_INFORMATION
    FileFsVolumeFlagsInformation,           // qs: FILE_FS_VOLUME_FLAGS_INFORMATION // SeManageVolumePrivilege // 10
    FileFsSectorSizeInformation,            // q: FILE_FS_SECTOR_SIZE_INFORMATION // since WIN8
    FileFsDataCopyInformation,              // q: FILE_FS_DATA_COPY_INFORMATION
    FileFsMetadataSizeInformation,          // q: FILE_FS_METADATA_SIZE_INFORMATION // since THRESHOLD
    FileFsFullSizeInformationEx,            // q: FILE_FS_FULL_SIZE_INFORMATION_EX // since REDSTONE5
    FileFsGuidInformation,                  // q: FILE_FS_GUID_INFORMATION // since 23H2
    FileFsMaximumInformation
} FSINFOCLASS, *PFSINFOCLASS;
typedef enum _FSINFOCLASS FS_INFORMATION_CLASS;

// NtQueryVolumeInformation/NtSetVolumeInformation types

typedef struct _FILE_FS_VOLUME_INFORMATION
{
    LARGE_INTEGER VolumeCreationTime;
    ULONG VolumeSerialNumber;
    ULONG VolumeLabelLength;
    BOOLEAN SupportsObjects;
    _Field_size_bytes_(VolumeLabelLength) WCHAR VolumeLabel[1];
} FILE_FS_VOLUME_INFORMATION, *PFILE_FS_VOLUME_INFORMATION;

typedef struct _FILE_FS_LABEL_INFORMATION
{
    ULONG VolumeLabelLength;
    _Field_size_bytes_(VolumeLabelLength) WCHAR VolumeLabel[1];
} FILE_FS_LABEL_INFORMATION, *PFILE_FS_LABEL_INFORMATION;

typedef struct _FILE_FS_SIZE_INFORMATION
{
    LARGE_INTEGER TotalAllocationUnits;
    LARGE_INTEGER AvailableAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_SIZE_INFORMATION, *PFILE_FS_SIZE_INFORMATION;

// FileSystemControlFlags
#define FILE_VC_QUOTA_NONE 0x00000000
#define FILE_VC_QUOTA_TRACK 0x00000001
#define FILE_VC_QUOTA_ENFORCE 0x00000002
#define FILE_VC_QUOTA_MASK 0x00000003
#define FILE_VC_CONTENT_INDEX_DISABLED 0x00000008
#define FILE_VC_LOG_QUOTA_THRESHOLD 0x00000010
#define FILE_VC_LOG_QUOTA_LIMIT 0x00000020
#define FILE_VC_LOG_VOLUME_THRESHOLD 0x00000040
#define FILE_VC_LOG_VOLUME_LIMIT 0x00000080
#define FILE_VC_QUOTAS_INCOMPLETE 0x00000100
#define FILE_VC_QUOTAS_REBUILDING 0x00000200
#define FILE_VC_VALID_MASK 0x000003ff

typedef struct _FILE_FS_CONTROL_INFORMATION
{
    LARGE_INTEGER FreeSpaceStartFiltering;
    LARGE_INTEGER FreeSpaceThreshold;
    LARGE_INTEGER FreeSpaceStopFiltering;
    LARGE_INTEGER DefaultQuotaThreshold;
    LARGE_INTEGER DefaultQuotaLimit;
    ULONG FileSystemControlFlags; // FILE_VC_*
} FILE_FS_CONTROL_INFORMATION, *PFILE_FS_CONTROL_INFORMATION;

typedef struct _FILE_FS_FULL_SIZE_INFORMATION
{
    LARGE_INTEGER TotalAllocationUnits;
    LARGE_INTEGER CallerAvailableAllocationUnits;
    LARGE_INTEGER ActualAvailableAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_FULL_SIZE_INFORMATION, *PFILE_FS_FULL_SIZE_INFORMATION;

typedef struct _FILE_FS_OBJECTID_INFORMATION
{
    UCHAR ObjectId[16];
    union
    {
        struct
        {
            UCHAR BirthVolumeId[16];
            UCHAR BirthObjectId[16];
            UCHAR DomainId[16];
        };
        UCHAR ExtendedInfo[48];
    };
} FILE_FS_OBJECTID_INFORMATION, *PFILE_FS_OBJECTID_INFORMATION;

typedef struct _FILE_FS_DEVICE_INFORMATION
{
    DEVICE_TYPE DeviceType;
    ULONG Characteristics;
} FILE_FS_DEVICE_INFORMATION, *PFILE_FS_DEVICE_INFORMATION;

typedef struct _FILE_FS_ATTRIBUTE_INFORMATION
{
    ULONG FileSystemAttributes;
    LONG MaximumComponentNameLength;
    ULONG FileSystemNameLength;
    _Field_size_bytes_(FileSystemNameLength) WCHAR FileSystemName[1];
} FILE_FS_ATTRIBUTE_INFORMATION, *PFILE_FS_ATTRIBUTE_INFORMATION;

typedef struct _FILE_FS_DRIVER_PATH_INFORMATION
{
    BOOLEAN DriverInPath;
    ULONG DriverNameLength;
    _Field_size_bytes_(DriverNameLength) WCHAR DriverName[1];
} FILE_FS_DRIVER_PATH_INFORMATION, *PFILE_FS_DRIVER_PATH_INFORMATION;

typedef struct _FILE_FS_VOLUME_FLAGS_INFORMATION
{
    ULONG Flags;
} FILE_FS_VOLUME_FLAGS_INFORMATION, *PFILE_FS_VOLUME_FLAGS_INFORMATION;

#define SSINFO_FLAGS_ALIGNED_DEVICE 0x00000001
#define SSINFO_FLAGS_PARTITION_ALIGNED_ON_DEVICE 0x00000002
#define SSINFO_FLAGS_NO_SEEK_PENALTY 0x00000004
#define SSINFO_FLAGS_TRIM_ENABLED 0x00000008
#define SSINFO_FLAGS_BYTE_ADDRESSABLE 0x00000010 // since REDSTONE

// If set for Sector and Partition fields, alignment is not known.
#define SSINFO_OFFSET_UNKNOWN 0xffffffff

typedef struct _FILE_FS_SECTOR_SIZE_INFORMATION
{
    ULONG LogicalBytesPerSector;
    ULONG PhysicalBytesPerSectorForAtomicity;
    ULONG PhysicalBytesPerSectorForPerformance;
    ULONG FileSystemEffectivePhysicalBytesPerSectorForAtomicity;
    ULONG Flags; // SSINFO_FLAGS_*
    ULONG ByteOffsetForSectorAlignment;
    ULONG ByteOffsetForPartitionAlignment;
} FILE_FS_SECTOR_SIZE_INFORMATION, *PFILE_FS_SECTOR_SIZE_INFORMATION;

typedef struct _FILE_FS_DATA_COPY_INFORMATION
{
    ULONG NumberOfCopies;
} FILE_FS_DATA_COPY_INFORMATION, *PFILE_FS_DATA_COPY_INFORMATION;

typedef struct _FILE_FS_METADATA_SIZE_INFORMATION
{
    LARGE_INTEGER TotalMetadataAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_METADATA_SIZE_INFORMATION, *PFILE_FS_METADATA_SIZE_INFORMATION;

typedef struct _FILE_FS_FULL_SIZE_INFORMATION_EX
{
    ULONGLONG ActualTotalAllocationUnits;
    ULONGLONG ActualAvailableAllocationUnits;
    ULONGLONG ActualPoolUnavailableAllocationUnits;
    ULONGLONG CallerTotalAllocationUnits;
    ULONGLONG CallerAvailableAllocationUnits;
    ULONGLONG CallerPoolUnavailableAllocationUnits;
    ULONGLONG UsedAllocationUnits;
    ULONGLONG TotalReservedAllocationUnits;
    ULONGLONG VolumeStorageReserveAllocationUnits;
    ULONGLONG AvailableCommittedAllocationUnits;
    ULONGLONG PoolAvailableAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_FULL_SIZE_INFORMATION_EX, *PFILE_FS_FULL_SIZE_INFORMATION_EX;

typedef struct _FILE_FS_GUID_INFORMATION
{
    GUID FsGuid;
} FILE_FS_GUID_INFORMATION, *PFILE_FS_GUID_INFORMATION;

/**
 * The NtQueryInformationFile routine returns various kinds of information about a file object.
 *
 * \param FileHandle A handle to the file object representing the file or directory.
 * \param IoStatusBlock A pointer to an IO_STATUS_BLOCK structure that receives the final completion status, and the number of bytes written to the buffer pointed to by FileInformation.
 * \param FileInformation Pointer to a caller-allocated buffer into which the routine writes the requested information about the file object.
 * \param Length The size, in bytes, of the buffer pointed to by FileInformation.
 * \param FileInformationClass Specifies the type of information to be returned about the file, in the buffer that FileInformation points to.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntqueryinformationfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass);

#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)
/**
 * The NtQueryInformationByName routine returns various kinds of information about a file object by file name.
 *
 * \param ObjectAttributes Pointer to an OBJECT_ATTRIBUTES structure that contains the file's attributes, including file name.
 * \param IoStatusBlock A pointer to an IO_STATUS_BLOCK structure that receives the final completion status, and the number of bytes written to the buffer pointed to by FileInformation.
 * \param FileInformation Pointer to a caller-allocated buffer into which the routine writes the requested information about the file object.
 * \param Length The size, in bytes, of the buffer pointed to by FileInformation.
 * \param FileInformationClass Specifies the type of information to be returned about the file, in the buffer that FileInformation points to.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntqueryinformationbyname
 * \remarks NtQueryInformationByName queries and returns the requested information without opening the actual file,
 * making it more efficient than NtQueryInformationFile, which requires a file open and subsequent file close.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationByName(
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass);
#endif

/**
 * The NtSetInformationFile routine changes various kinds of information about a file object.
 *
 * \param FileHandle A handle to the file object representing the file or directory.
 * \param IoStatusBlock A pointer to an IO_STATUS_BLOCK structure that receives the final completion status, and the number of bytes written to the buffer pointed to by FileInformation.
 * \param FileInformation Pointer to a buffer that contains the information to set for the file.
 * \param Length The size, in bytes, of the buffer pointed to by FileInformation.
 * \param FileInformationClass The type of information, supplied in the buffer pointed to by FileInformation, to set for the file.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntsetinformationfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_reads_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass);

/**
 * The NtQueryAttributesFile function retrieves basic attributes for the specified file.
 *
 * @param ObjectAttributes A pointer to an OBJECT_ATTRIBUTES structure that supplies the attributes to be used for the file object.
 * @param FileInformation A pointer to a FILE_BASIC_INFORMATION structure to receive the returned file attribute information.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows/win32/devnotes/ntqueryattributesfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryAttributesFile(
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _Out_ PFILE_BASIC_INFORMATION FileInformation);

/**
 * The NtQueryFullAttributesFile function retrieves network open information for the specified file.
 *
 * @param ObjectAttributes A pointer to an OBJECT_ATTRIBUTES structure that supplies the attributes to be used for the file object.
 * @param FileInformation A pointer to a FILE_NETWORK_OPEN_INFORMATION structure that receives the returned file attributes information.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwqueryfullattributesfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryFullAttributesFile(
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _Out_ PFILE_NETWORK_OPEN_INFORMATION FileInformation);

/**
 * The NtQueryDirectoryFile routine returns various kinds of information about files in the directory specified by a given file handle.
 *
 * \param[in] FileHandle A handle for the file object that represents the directory for which information is being requested.
 * \param[in] Event An optional handle for a caller-created event. The event is set to the Signaled state the requested operation is completed.
 * \param[in] ApcRoutine An address of an optional, caller-supplied APC routine to be called when the requested operation completes.
 * \param[in] ApcContext An address of an optional, caller-supplied APC or I/O completion object associated with the file object.
 * \param[out] IoStatusBlock A pointer to an IO_STATUS_BLOCK structure that receives the final completion status, and the number of bytes written to the buffer pointed to by FileInformation.
 * \param[out] FileInformation A pointer to a buffer that receives the desired information about the file.
 * \param[in] Length The size, in bytes, of the buffer pointed to by FileInformation.
 * \param[in] FileInformationClass The type of information to be returned about files in the directory.
 * \param[in] ReturnSingleEntry Set to TRUE if only a single entry should be returned, FALSE otherwise.
 * \param[in] FileName An optional pointer to a Unicode string search expression containing the name of a file (or multiple files, if wildcards are used) within the directory.
 * \param[in] RestartScan Set to TRUE if the scan is to start at the first entry in the directory. Set to FALSE if resuming the scan from a previous call.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntquerydirectoryfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryDirectoryFile(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass,
    _In_ BOOLEAN ReturnSingleEntry,
    _In_opt_ PUNICODE_STRING FileName,
    _In_ BOOLEAN RestartScan);

#if (NTDDI_VERSION >= NTDDI_WIN10_RS3)

// QueryFlags values for NtQueryDirectoryFileEx
/**
 * The scan will start at the first entry in the directory. If this flag is not set, the scan will resume from where the last query ended.
 */
#define FILE_QUERY_RESTART_SCAN 0x00000001
/**
 * Normally the return buffer is packed with as many matching directory entries that fit.
 * If this flag is set, the file system will return only one directory entry at a time.
 * This does make the operation less efficient.
 */
#define FILE_QUERY_RETURN_SINGLE_ENTRY 0x00000002
/**
 * The scan should start at a specified indexed position in the directory.
 * This flag can only be set if you generate your own IRP_MJ_DIRECTORY_CONTROL IRP; the index is specified in the IRP.
 * How the position is specified varies from file system to file system.
 */
#define FILE_QUERY_INDEX_SPECIFIED 0x00000004
/**
 * Any file system filters that perform directory virtualization or just-in-time expansion should simply pass the request
 * through to the file system and return entries that are currently on disk. Not all file systems support this flag.
 */
#define FILE_QUERY_RETURN_ON_DISK_ENTRIES_ONLY 0x00000008

#if (NTDDI_VERSION >= NTDDI_WIN10_RS5)
/**
 * File systems maintain per-FileObject directory cursor information. When multiple threads do queries using the same FileObject,
 * access to the per-FileObject structure is single threaded to prevent corruption of the cursor state.
 * This flag tells the file system to not update per-FileObject cursor state information thus allowing multiple threads 
 * to query in parallel using the same handle. It behaves as if SL_RESTART_SCAN is specified on each call. If a wild card pattern
 * is given on the next call, the operation will not pick up where the last query ended. 
 * This allows for true asynchronous directory query support. Not all file systems support this flag.
 */
#define FILE_QUERY_NO_CURSOR_UPDATE 0x00000010
#endif

/**
 * The NtQueryDirectoryFileEx routine returns various kinds of information about files in the directory specified by a given file handle.
 *
 * \param[in] FileHandle A handle for the file object that represents the directory for which information is being requested.
 * \param[in] Event An optional handle for a caller-created event. The event is set to the Signaled state the requested operation is completed.
 * \param[in] ApcRoutine An address of an optional, caller-supplied APC routine to be called when the requested operation completes.
 * \param[in] ApcContext An address of an optional, caller-supplied APC or I/O completion object associated with the file object.
 * \param[out] IoStatusBlock A pointer to an IO_STATUS_BLOCK structure that receives the final completion status, and the number of bytes written to the buffer pointed to by FileInformation.
 * \param[out] FileInformation A pointer to a buffer that receives the desired information about the file.
 * \param[in] Length The size, in bytes, of the buffer pointed to by FileInformation.
 * \param[in] FileInformationClass The type of information to be returned about files in the directory.
 * \param[in] QueryFlags One or more of the flags contained in SL_QUERY_DIRECTORY_MASK
 * \param[in] FileName An optional pointer to a Unicode string search expression containing the name of a file (or multiple files, if wildcards are used) within the directory.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntquerydirectoryfileex
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryDirectoryFileEx(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass,
    _In_ ULONG QueryFlags, // FILE_QUERY_XXX
    _In_opt_ PUNICODE_STRING FileName);

#endif /* (NTDDI_VERSION >= NTDDI_WIN10_RS3) */

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryEaFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length,
    _In_ BOOLEAN ReturnSingleEntry,
    _In_reads_bytes_opt_(EaListLength) PVOID EaList,
    _In_ ULONG EaListLength,
    _In_opt_ PULONG EaIndex,
    _In_ BOOLEAN RestartScan);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetEaFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_reads_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryQuotaInformationFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length,
    _In_ BOOLEAN ReturnSingleEntry,
    _In_reads_bytes_opt_(SidListLength) PVOID SidList,
    _In_ ULONG SidListLength,
    _In_opt_ PSID StartSid,
    _In_ BOOLEAN RestartScan);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetQuotaInformationFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_reads_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length);

/**
 * The NtQueryVolumeInformationFile routine retrieves information about the volume associated with a given file, directory, storage device, or volume.
 *
 * \param[in] FileHandle A handle to the file, directory, storage device, or volume for which volume information is being requested.
 * \param[out] IoStatusBlock A pointer to an IO_STATUS_BLOCK structure that receives the final completion status, and the number of bytes written to the buffer pointed to by FsInformation. 
 * \param[out] FsInformation A pointer to a caller-allocated buffer that receives the desired information about the volume.
 * \param[in] Length The size, in bytes, of the buffer pointed to by FsInformation.
 * \param[in] FsInformationClass The type of information to be returned about the volume.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntqueryvolumeinformationfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryVolumeInformationFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID FsInformation,
    _In_ ULONG Length,
    _In_ FSINFOCLASS FsInformationClass);

/**
 * The NtSetVolumeInformationFile routine modifies information about the volume associated with a given file, directory, storage device, or volume.
 *
 * \param[in] FileHandle A handle to the file, directory, storage device, or volume for which volume information is being requested.
 * \param[out] IoStatusBlock A pointer to an IO_STATUS_BLOCK structure that receives the final completion status, and the number of bytes written to the buffer pointed to by FsInformation.
 * \param[in] FsInformation A pointer to a caller-allocated buffer containing the volume information to be modified.
 * \param[in] Length The size, in bytes, of the buffer pointed to by FsInformation.
 * \param[in] FsInformationClass The type of information to set about the volume.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwsetvolumeinformationfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetVolumeInformationFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_reads_bytes_(Length) PVOID FsInformation,
    _In_ ULONG Length,
    _In_ FSINFOCLASS FsInformationClass);

/* NtNotifyChangeDirectoryFile information */

#define FILE_NOTIFY_CHANGE_FILE_NAME    0x00000001   // winnt
#define FILE_NOTIFY_CHANGE_DIR_NAME     0x00000002   // winnt
#define FILE_NOTIFY_CHANGE_NAME         0x00000003
#define FILE_NOTIFY_CHANGE_ATTRIBUTES   0x00000004   // winnt
#define FILE_NOTIFY_CHANGE_SIZE         0x00000008   // winnt
#define FILE_NOTIFY_CHANGE_LAST_WRITE   0x00000010   // winnt
#define FILE_NOTIFY_CHANGE_LAST_ACCESS  0x00000020   // winnt
#define FILE_NOTIFY_CHANGE_CREATION     0x00000040   // winnt
#define FILE_NOTIFY_CHANGE_EA           0x00000080
#define FILE_NOTIFY_CHANGE_SECURITY     0x00000100   // winnt
#define FILE_NOTIFY_CHANGE_STREAM_NAME  0x00000200
#define FILE_NOTIFY_CHANGE_STREAM_SIZE  0x00000400
#define FILE_NOTIFY_CHANGE_STREAM_WRITE 0x00000800
#define FILE_NOTIFY_VALID_MASK          0x00000fff

#define FILE_ACTION_ADDED                   0x00000001   // winnt
#define FILE_ACTION_REMOVED                 0x00000002   // winnt
#define FILE_ACTION_MODIFIED                0x00000003   // winnt
#define FILE_ACTION_RENAMED_OLD_NAME        0x00000004   // winnt
#define FILE_ACTION_RENAMED_NEW_NAME        0x00000005   // winnt
#define FILE_ACTION_ADDED_STREAM            0x00000006
#define FILE_ACTION_REMOVED_STREAM          0x00000007
#define FILE_ACTION_MODIFIED_STREAM         0x00000008
#define FILE_ACTION_REMOVED_BY_DELETE       0x00000009
#define FILE_ACTION_ID_NOT_TUNNELLED        0x0000000A
#define FILE_ACTION_TUNNELLED_ID_COLLISION  0x0000000B

NTSYSCALLAPI
NTSTATUS
NTAPI
NtNotifyChangeDirectoryFile(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID Buffer, // FILE_NOTIFY_INFORMATION
    _In_ ULONG Length,
    _In_ ULONG CompletionFilter,
    _In_ BOOLEAN WatchTree);

// private
typedef enum _DIRECTORY_NOTIFY_INFORMATION_CLASS
{
    DirectoryNotifyInformation = 1, // FILE_NOTIFY_INFORMATION
    DirectoryNotifyExtendedInformation, // FILE_NOTIFY_EXTENDED_INFORMATION
    DirectoryNotifyFullInformation, // FILE_NOTIFY_FULL_INFORMATION // since 22H2
    DirectoryNotifyMaximumInformation
} DIRECTORY_NOTIFY_INFORMATION_CLASS, *PDIRECTORY_NOTIFY_INFORMATION_CLASS;

#if !defined(NTDDI_WIN10_RS5) || (NTDDI_VERSION < NTDDI_WIN10_RS5)
typedef struct _FILE_NOTIFY_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG Action;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_NOTIFY_INFORMATION, *PFILE_NOTIFY_INFORMATION;

typedef struct _FILE_NOTIFY_EXTENDED_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG Action;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastModificationTime;
    LARGE_INTEGER LastChangeTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER AllocatedLength;
    LARGE_INTEGER FileSize;
    ULONG FileAttributes;
    union
    {
        ULONG ReparsePointTag;
        ULONG EaSize;
    };
    LARGE_INTEGER FileId;
    LARGE_INTEGER ParentFileId;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_NOTIFY_EXTENDED_INFORMATION, *PFILE_NOTIFY_EXTENDED_INFORMATION;
#endif

#define FILE_NAME_FLAG_HARDLINK      0    // not part of a name pair
#define FILE_NAME_FLAG_NTFS          0x01 // NTFS name in a name pair
#define FILE_NAME_FLAG_DOS           0x02 // DOS name in a name pair
#define FILE_NAME_FLAG_BOTH          0x03 // NTFS+DOS combined name
#define FILE_NAME_FLAGS_UNSPECIFIED  0x80 // not specified by file system (do not combine with other flags)

#if !defined(NTDDI_WIN10_NI) || (NTDDI_VERSION < NTDDI_WIN10_NI)
typedef struct _FILE_NOTIFY_FULL_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG Action;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastModificationTime;
    LARGE_INTEGER LastChangeTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER AllocatedLength;
    LARGE_INTEGER FileSize;
    ULONG FileAttributes;
    union
    {
        ULONG ReparsePointTag;
        ULONG EaSize;
    };
    LARGE_INTEGER FileId;
    LARGE_INTEGER ParentFileId;
    USHORT FileNameLength;
    BYTE FileNameFlags;
    BYTE Reserved;
    WCHAR FileName[1];
} FILE_NOTIFY_FULL_INFORMATION, *PFILE_NOTIFY_FULL_INFORMATION;
#endif

#if (NTDDI_VERSION >= NTDDI_WIN10_RS3)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtNotifyChangeDirectoryFileEx(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length,
    _In_ ULONG CompletionFilter,
    _In_ BOOLEAN WatchTree,
    _In_opt_ DIRECTORY_NOTIFY_INFORMATION_CLASS DirectoryNotifyInformationClass);
#endif

EXTERN_C_END
