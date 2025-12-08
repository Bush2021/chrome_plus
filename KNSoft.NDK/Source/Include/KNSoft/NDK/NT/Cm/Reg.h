#pragma once

#include "../MinDef.h"

EXTERN_C_START

/* phnt */

#pragma region Initialize

//
// Boot condition flags (NtInitializeRegistry)
//

#define REG_INIT_BOOT_SM 0x0000
#define REG_INIT_BOOT_SETUP 0x0001
#define REG_INIT_BOOT_ACCEPTED_BASE 0x0002
#define REG_INIT_BOOT_ACCEPTED_MAX (REG_INIT_BOOT_ACCEPTED_BASE + 999)

/**
 * Initializes the registry.
 * 
 * @param BootCondition Condition for the boot.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtInitializeRegistry(
    _In_ USHORT BootCondition);

#pragma endregion

#define REG_MAX_KEY_VALUE_NAME_LENGTH 32767
#define REG_MAX_KEY_NAME_LENGTH 512

typedef enum _CM_EXTENDED_PARAMETER_TYPE
{
  CmExtendedParameterInvalidType,
  CmExtendedParameterTrustClassKey,
  CmExtendedParameterEvent,
  CmExtendedParameterFileAccessToken,
  CmExtendedParameterMax,
} CM_EXTENDED_PARAMETER_TYPE;

#define CM_EXTENDED_PARAMETER_TYPE_BITS 8

typedef struct DECLSPEC_ALIGN(8) _CM_EXTENDED_PARAMETER
{
    struct
    {
        ULONG64 Type : CM_EXTENDED_PARAMETER_TYPE_BITS;
        ULONG64 Reserved : 64 - CM_EXTENDED_PARAMETER_TYPE_BITS;
    };

    union
    {
        ULONG64 ULong64;
        PVOID Pointer;
        SIZE_T Size;
        HANDLE Handle;
        ULONG ULong;
        ACCESS_MASK AccessMask;
    };
} CM_EXTENDED_PARAMETER, *PCM_EXTENDED_PARAMETER;

/**
 * Creates a new registry key routine or opens an existing one.
 * 
 * @param[out] KeyHandle A pointer to a handle that receives the key handle.
 * @param[in] DesiredAccess The access mask that specifies the desired access rights.
 * @param[in] ObjectAttributes A pointer to an OBJECT_ATTRIBUTES structure that specifies the object attributes.
 * @param[in] TitleIndex Reserved.
 * @param[in, optional] Class A pointer to a UNICODE_STRING structure that specifies the class of the key.
 * @param[in] CreateOptions The options to use when creating the key.
 * @param[out, optional] Disposition A pointer to a variable that receives the disposition value.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateKey(
    _Out_ PHANDLE KeyHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _Reserved_ ULONG TitleIndex,
    _In_opt_ PUNICODE_STRING Class,
    _In_ ULONG CreateOptions,
    _Out_opt_ PULONG Disposition
    );

/**
 * Creates a new registry key or opens an existing one, and it associates the key with a transaction.
 * 
 * @param[out] KeyHandle A pointer to a handle that receives the key handle.
 * @param[in] DesiredAccess The access mask that specifies the desired access rights.
 * @param[in] ObjectAttributes A pointer to an OBJECT_ATTRIBUTES structure that specifies the object attributes.
 * @param[in] TitleIndex Reserved.
 * @param[in, optional] Class A pointer to a UNICODE_STRING structure that specifies the class of the key.
 * @param[in] CreateOptions The options to use when creating the key.
 * @param[in] TransactionHandle A handle to the transaction.
 * @param[out, optional] Disposition A pointer to a variable that receives the disposition value.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateKeyTransacted(
    _Out_ PHANDLE KeyHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _Reserved_ ULONG TitleIndex,
    _In_opt_ PUNICODE_STRING Class,
    _In_ ULONG CreateOptions,
    _In_ HANDLE TransactionHandle,
    _Out_opt_ PULONG Disposition
    );

/**
 * Opens an existing registry key.
 * 
 * @param[out] KeyHandle A pointer to a handle that receives the key handle.
 * @param[in] DesiredAccess The access mask that specifies the desired access rights.
 * @param[in] ObjectAttributes A pointer to an OBJECT_ATTRIBUTES structure that specifies the object attributes.
 * @return NTSTATUS Successful or errant status.
 * @remarks NtOpenKey ignores the security information in the ObjectAttributes structure.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenKey(
    _Out_ PHANDLE KeyHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes
    );

/**
 * Opens an existing registry key and associates the key with a transaction.
 * 
 * @param[out] KeyHandle A pointer to a handle that receives the key handle.
 * @param[in] DesiredAccess The access mask that specifies the desired access rights.
 * @param[in] ObjectAttributes A pointer to an OBJECT_ATTRIBUTES structure that specifies the object attributes.
 * @param[in] TransactionHandle A handle to the transaction.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenKeyTransacted(
    _Out_ PHANDLE KeyHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE TransactionHandle
    );

/**
 * Opens an existing registry key with extended options.
 * 
 * @param[out] KeyHandle A pointer to a handle that receives the key handle.
 * @param[in] DesiredAccess The access mask that specifies the desired access rights.
 * @param[in] ObjectAttributes A pointer to an OBJECT_ATTRIBUTES structure that specifies the object attributes.
 * @param[in] OpenOptions The options to use when opening the key.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenKeyEx(
    _Out_ PHANDLE KeyHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ ULONG OpenOptions
    );

/**
 * Opens an existing registry key in a transaction with extended options.
 * 
 * @param[out] KeyHandle A pointer to a handle that receives the key handle.
 * @param[in] DesiredAccess The access mask that specifies the desired access rights.
 * @param[in] ObjectAttributes A pointer to an OBJECT_ATTRIBUTES structure that specifies the object attributes.
 * @param[in] OpenOptions The options to use when opening the key.
 * @param[in] TransactionHandle A handle to the transaction.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenKeyTransactedEx(
    _Out_ PHANDLE KeyHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ ULONG OpenOptions,
    _In_ HANDLE TransactionHandle
    );

/**
 * Deletes a registry key.
 * 
 * @param[in] KeyHandle A handle to the key to be deleted.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteKey(
    _In_ HANDLE KeyHandle
    );

/**
 * Renames a registry key.
 * 
 * @param[in] KeyHandle A handle to the key to be renamed.
 * @param[in] NewName A pointer to a UNICODE_STRING structure that specifies the new name of the key.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtRenameKey(
    _In_ HANDLE KeyHandle,
    _In_ PUNICODE_STRING NewName
    );

/**
 * Deletes a value from a registry key.
 * 
 * @param[in] KeyHandle A handle to the key that contains the value to be deleted.
 * @param[in] ValueName A pointer to a UNICODE_STRING structure that specifies the name of the value to be deleted.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteValueKey(
    _In_ HANDLE KeyHandle,
    _In_ PUNICODE_STRING ValueName
    );

/**
 * Flushes the changes to a registry key.
 * 
 * @param[in] KeyHandle A handle to the key to be flushed.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtFlushKey(
    _In_ HANDLE KeyHandle
    );

/**
 * Compacts the specified registry keys.
 * 
 * @param[in] Count The number of keys to be compacted.
 * @param[in] KeyArray An array of handles to the keys to be compacted.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCompactKeys(
    _In_ ULONG Count,
    _In_reads_(Count) HANDLE KeyArray[]
    );

/**
 * Compresses a registry key.
 * 
 * @param[in] KeyHandle A handle to the key to be compressed.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCompressKey(
    _In_ HANDLE KeyHandle
    );

/**
 * Loads a registry key from a file.
 * 
 * @param[in] TargetKey A pointer to an OBJECT_ATTRIBUTES structure that specifies the target key.
 * @param[in] SourceFile A pointer to an OBJECT_ATTRIBUTES structure that specifies the source file.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtLoadKey(
    _In_ POBJECT_ATTRIBUTES TargetKey,
    _In_ POBJECT_ATTRIBUTES SourceFile
    );

/**
 * Loads a registry key from a file with additional options.
 * 
 * @param[in] TargetKey A pointer to an OBJECT_ATTRIBUTES structure that specifies the target key.
 * @param[in] SourceFile A pointer to an OBJECT_ATTRIBUTES structure that specifies the source file.
 * @param[in] Flags The options to use when loading the key.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtLoadKey2(
    _In_ POBJECT_ATTRIBUTES TargetKey,
    _In_ POBJECT_ATTRIBUTES SourceFile,
    _In_ ULONG Flags
    );

/**
 * Loads a registry key from a file with extended options.
 * 
 * @param[in] TargetKey A pointer to an OBJECT_ATTRIBUTES structure that specifies the target key.
 * @param[in] SourceFile A pointer to an OBJECT_ATTRIBUTES structure that specifies the source file.
 * @param[in] Flags The options to use when loading the key.
 * @param[in, optional] TrustClassKey A handle to the trust class key.
 * @param[in, optional] Event A handle to an event.
 * @param[in, optional] DesiredAccess The access mask that specifies the desired access rights.
 * @param[out, optional] RootHandle A pointer to a handle that receives the root handle.
 * @param[in, reserved] Reserved Reserved.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtLoadKeyEx(
    _In_ POBJECT_ATTRIBUTES TargetKey,
    _In_ POBJECT_ATTRIBUTES SourceFile,
    _In_ ULONG Flags,
    _In_opt_ HANDLE TrustClassKey,
    _In_opt_ HANDLE Event,
    _In_opt_ ACCESS_MASK DesiredAccess,
    _Out_opt_ PHANDLE RootHandle,
    _Reserved_ PVOID Reserved // previously PIO_STATUS_BLOCK
    );

// rev by tyranid
#if (NTDDI_VERSION >= NTDDI_WIN10_MN)
/**
 * Loads a registry key from a file with extended parameters.
 * 
 * @param[in] TargetKey A pointer to an OBJECT_ATTRIBUTES structure that specifies the target key.
 * @param[in] SourceFile A pointer to an OBJECT_ATTRIBUTES structure that specifies the source file.
 * @param[in] Flags The options to use when loading the key.
 * @param[in] ExtendedParameters A pointer to an array of extended parameters.
 * @param[in] ExtendedParameterCount The number of extended parameters.
 * @param[in, optional] DesiredAccess The access mask that specifies the desired access rights.
 * @param[out, optional] RootHandle A pointer to a handle that receives the root handle.
 * @param[in, reserved] Reserved Reserved.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtLoadKey3(
    _In_ POBJECT_ATTRIBUTES TargetKey,
    _In_ POBJECT_ATTRIBUTES SourceFile,
    _In_ ULONG Flags,
    _In_reads_(ExtendedParameterCount) PCM_EXTENDED_PARAMETER ExtendedParameters,
    _In_ ULONG ExtendedParameterCount,
    _In_opt_ ACCESS_MASK DesiredAccess,
    _Out_opt_ PHANDLE RootHandle,
    _Reserved_ PVOID Reserved
    );
#endif

/**
 * Replaces a registry key.
 * 
 * @param[in] NewFile A pointer to an OBJECT_ATTRIBUTES structure that specifies the new file.
 * @param[in] TargetHandle A handle to the target key.
 * @param[in] OldFile A pointer to an OBJECT_ATTRIBUTES structure that specifies the old file.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtReplaceKey(
    _In_ POBJECT_ATTRIBUTES NewFile,
    _In_ HANDLE TargetHandle,
    _In_ POBJECT_ATTRIBUTES OldFile
    );

/**
 * Saves the specified registry key to a file.
 * 
 * @param KeyHandle Handle to the registry key.
 * @param FileHandle Handle to the file where the key will be saved.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSaveKey(
    _In_ HANDLE KeyHandle,
    _In_ HANDLE FileHandle
    );

/**
 * Saves the specified registry key to a file with a specified format.
 * 
 * @param KeyHandle Handle to the registry key.
 * @param FileHandle Handle to the file where the key will be saved.
 * @param Format Format in which the key will be saved.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSaveKeyEx(
    _In_ HANDLE KeyHandle,
    _In_ HANDLE FileHandle,
    _In_ ULONG Format
    );

/**
 * Merges two registry keys and saves the result to a file.
 * 
 * @param HighPrecedenceKeyHandle Handle to the high precedence registry key.
 * @param LowPrecedenceKeyHandle Handle to the low precedence registry key.
 * @param FileHandle Handle to the file where the merged key will be saved.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSaveMergedKeys(
    _In_ HANDLE HighPrecedenceKeyHandle,
    _In_ HANDLE LowPrecedenceKeyHandle,
    _In_ HANDLE FileHandle
    );

/**
 * Restores a registry key from a file.
 * 
 * @param KeyHandle Handle to the registry key.
 * @param FileHandle Handle to the file from which the key will be restored.
 * @param Flags Flags for the restore operation.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtRestoreKey(
    _In_ HANDLE KeyHandle,
    _In_ HANDLE FileHandle,
    _In_ ULONG Flags
    );

/**
 * Unloads a registry key.
 * 
 * @param TargetKey Pointer to the object attributes of the target key.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnloadKey(
    _In_ POBJECT_ATTRIBUTES TargetKey
    );

/**
 * Unloads a registry key with additional flags.
 * 
 * @param TargetKey Pointer to the object attributes of the target key.
 * @param Flags Flags for the unload operation.
 * @return NTSTATUS Successful or errant status.
 * @remarks Valid flags are REG_FORCE_UNLOAD and REG_UNLOAD_LEGAL_FLAGS.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnloadKey2(
    _In_ POBJECT_ATTRIBUTES TargetKey,
    _In_ ULONG Flags
    );

/**
 * Unloads a registry key and optionally signals an event.
 * 
 * @param TargetKey Pointer to the object attributes of the target key.
 * @param Event Optional handle to an event to be signaled.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnloadKeyEx(
    _In_ POBJECT_ATTRIBUTES TargetKey,
    _In_opt_ HANDLE Event
    );

/**
 * Notifies of changes to a registry key.
 * 
 * @param KeyHandle Handle to the registry key.
 * @param Event Optional handle to an event to be signaled.
 * @param ApcRoutine Optional APC routine to be called.
 * @param ApcContext Optional context for the APC routine.
 * @param IoStatusBlock Pointer to an IO status block.
 * @param CompletionFilter Filter for the types of changes to notify.
 * @param WatchTree Whether to watch the entire tree.
 * @param Buffer Optional buffer for change data.
 * @param BufferSize Size of the buffer.
 * @param Asynchronous Whether the operation is asynchronous.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtNotifyChangeKey(
    _In_ HANDLE KeyHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG CompletionFilter,
    _In_ BOOLEAN WatchTree,
    _Out_writes_bytes_opt_(BufferSize) PVOID Buffer,
    _In_ ULONG BufferSize,
    _In_ BOOLEAN Asynchronous
    );

/**
 * Requests notification when a registry key or any of its subkeys changes.
 * 
 * @param MasterKeyHandle A handle to an open key. The handle must be opened with the KEY_NOTIFY access right.
 * @param Count The number of subkeys under the key specified by the MasterKeyHandle parameter.
 * @param SubordinateObjects Pointer to an array of OBJECT_ATTRIBUTES structures, one for each subkey. This array can contain one OBJECT_ATTRIBUTES structure.
 * @param Event A handle to an event created by the caller. If Event is not NULL, the caller waits until the operation succeeds, at which time the event is signaled.
 * @param ApcRoutine A pointer to an asynchronous procedure call (APC) function supplied by the caller. If ApcRoutine is not NULL, the specified APC function executes after the operation completes.
 * @param ApcContext A pointer to a context supplied by the caller for its APC function. This value is passed to the APC function when it is executed. The Asynchronous parameter must be TRUE. If ApcContext is specified, the Event parameter must be NULL.
 * @param IoStatusBlock A pointer to an IO_STATUS_BLOCK structure that contains the final status and information about the operation. For successful calls that return data, the number of bytes written to the Buffer parameter is supplied in the Information member of the IO_STATUS_BLOCK structure.
 * @param CompletionFilter A bitmap of operations that trigger notification. This parameter can be one or more of the following flags. REG_NOTIFY_CHANGE_NAME, REG_NOTIFY_CHANGE_ATTRIBUTES, REG_NOTIFY_CHANGE_LAST_SET, REG_NOTIFY_CHANGE_SECURITY.
 * @param WatchTree If this parameter is TRUE, the caller is notified about changes to all subkeys of the specified key. If this parameter is FALSE, the caller is notified only about changes to the specified key.
 * @param Buffer Reserved for system use. This parameter must be NULL.
 * @param BufferSize Reserved for system use. This parameter must be zero.
 * @param Asynchronous Whether the operation is asynchronous.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtNotifyChangeMultipleKeys(
    _In_ HANDLE MasterKeyHandle,
    _In_opt_ ULONG Count,
    _In_reads_opt_(Count) OBJECT_ATTRIBUTES SubordinateObjects[],
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG CompletionFilter,
    _In_ BOOLEAN WatchTree,
    _Out_writes_bytes_opt_(BufferSize) PVOID Buffer,
    _In_ ULONG BufferSize,
    _In_ BOOLEAN Asynchronous
    );

/**
 * Queries the number of open subkeys of a registry key.
 * 
 * @param TargetKey Pointer to the object attributes of the target key.
 * @param HandleCount Pointer to a variable to receive the handle count.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryOpenSubKeys(
    _In_ POBJECT_ATTRIBUTES TargetKey,
    _Out_ PULONG HandleCount
    );

typedef struct _KEY_PID_ARRAY
{
    HANDLE ProcessId;
    UNICODE_STRING KeyName;
} KEY_PID_ARRAY, *PKEY_PID_ARRAY;

typedef struct _KEY_OPEN_SUBKEYS_INFORMATION
{
    ULONG Count;
    _Field_size_(Count) KEY_PID_ARRAY KeyArray[ANYSIZE_ARRAY];
} KEY_OPEN_SUBKEYS_INFORMATION, *PKEY_OPEN_SUBKEYS_INFORMATION;

/**
 * Queries the open subkeys of a registry key with additional information.
 * 
 * @param TargetKey Pointer to the object attributes of the target key.
 * @param BufferLength Length of the buffer.
 * @param Buffer Optional buffer to receive the subkey information.
 * @param RequiredSize Pointer to a variable to receive the required size.
 * @return NTSTATUS Successful or errant status.
 * @remarks Returns an array of KEY_OPEN_SUBKEYS_INFORMATION structures.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryOpenSubKeysEx(
    _In_ POBJECT_ATTRIBUTES TargetKey,
    _In_ ULONG BufferLength,
    _Out_writes_bytes_opt_(BufferLength) PVOID Buffer,
    _Out_ PULONG RequiredSize
    );

/**
 * Locks the registry key and prevents changes from being written to disk.
 * 
 * @param KeyHandle Handle to the registry key.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtLockRegistryKey(
    _In_ HANDLE KeyHandle
    );

/**
 * Locks the product activation keys.
 * 
 * @param pPrivateVer Optional pointer to a private version variable.
 * @param pSafeMode Optional pointer to a safe mode variable.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtLockProductActivationKeys(
    _Inout_opt_ ULONG *pPrivateVer,
    _Out_opt_ ULONG *pSafeMode
    );

/**
 * Freezes the registry and prevents changes from being flushed to disk.
 * 
 * @param TimeOutInSeconds Timeout in seconds.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtFreezeRegistry(
    _In_ ULONG TimeOutInSeconds
    );

/**
 * Thaws the registry and enables flushing changes to disk.
 * 
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtThawRegistry(
    VOID
    );

#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)

/**
 * Creates a registry transaction.
 * 
 * @param RegistryTransactionHandle Pointer to a variable to receive the handle.
 * @param DesiredAccess Desired access mask.
 * @param ObjAttributes Optional pointer to object attributes.
 * @param CreateOptions Reserved for future use.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateRegistryTransaction(
    _Out_ PHANDLE RegistryTransactionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjAttributes,
    _Reserved_ ULONG CreateOptions);

/**
 * Opens a registry transaction.
 * 
 * @param RegistryTransactionHandle Pointer to a variable to receive the handle.
 * @param DesiredAccess Desired access mask.
 * @param ObjAttributes Pointer to object attributes.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenRegistryTransaction(
    _Out_ PHANDLE RegistryTransactionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjAttributes);

/**
 * Commits a registry transaction.
 * 
 * @param RegistryTransactionHandle Handle to the registry transaction.
 * @param Flags Reserved for future use.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCommitRegistryTransaction(
    _In_ HANDLE RegistryTransactionHandle,
    _Reserved_ ULONG Flags);

/**
 * Rolls back a registry transaction.
 * 
 * @param RegistryTransactionHandle Handle to the registry transaction.
 * @param Flags Reserved for future use.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtRollbackRegistryTransaction(
    _In_ HANDLE RegistryTransactionHandle,
    _Reserved_ ULONG Flags);

#endif

EXTERN_C_END
