#pragma once

#include "../MinDef.h"
#include "../Io/Info.h"
#include "../Ex/Wnf.h"
#include "../Ps/Basic.h"

#include <minwinbase.h>

EXTERN_C_START

/* phnt */

#pragma region Thread Profiling

// rev
/**
 * The RtlEnableThreadProfiling routine enables thread profiling on the specified thread.
 *
 * \param ThreadHandle The handle to the thread on which you want to enable profiling. This must be the current thread.
 * \param Flags To receive thread profiling data such as context switch count, set this parameter to THREAD_PROFILING_FLAG_DISPATCH; otherwise, set to 0.
 * \param HardwareCounters To receive hardware performance counter data, set this parameter to a bitmask that identifies the hardware counters to collect.
 * \param PerformanceDataHandle An opaque handle that you use when calling the RtlReadThreadProfilingData and RtlDisableThreadProfiling functions.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-enablethreadprofiling
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlEnableThreadProfiling(
    _In_ HANDLE ThreadHandle,
    _In_ ULONG Flags,
    _In_ ULONG64 HardwareCounters,
    _Out_ PVOID* PerformanceDataHandle);

// rev
/**
 * The RtlDisableThreadProfiling routine disables thread profiling.
 *
 * \param PerformanceDataHandle The handle that the RtlEnableThreadProfiling function returned.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-querythreadprofiling
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlDisableThreadProfiling(
    _In_ PVOID PerformanceDataHandle);

// rev
/**
 * The RtlQueryThreadProfiling routine determines whether thread profiling is enabled for the specified thread.
 *
 * \param ThreadHandle The handle to the thread on which you want to enable profiling. This must be the current thread.
 * \param Enabled Is TRUE if thread profiling is enabled for the specified thread; otherwise, FALSE.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-querythreadprofiling
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlQueryThreadProfiling(
    _In_ HANDLE ThreadHandle,
    _Out_ PBOOLEAN Enabled);

// rev
/**
 * The RtlReadThreadProfilingData routine reads the specified profiling data associated with the thread.
 *
 * \param PerformanceDataHandle The handle that the RtlEnableThreadProfiling function returned.
 * \param Flags One or more flags set when you called the RtlEnableThreadProfiling function that specify the counter data to read.
 * \param PerformanceData A PERFORMANCE_DATA structure that contains the thread profiling and hardware counter data.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-readthreadprofilingdata
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlReadThreadProfilingData(
    _In_ HANDLE PerformanceDataHandle,
    _In_ ULONG Flags,
    _Out_ PPERFORMANCE_DATA PerformanceData);

#pragma endregion

#pragma region Timer

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateTimerQueue(
    _Out_ PHANDLE TimerQueueHandle);

// WAITORTIMERCALLBACKFUNC
typedef _Function_class_(RTL_TIMER_CALLBACK)
VOID NTAPI RTL_TIMER_CALLBACK(
    _In_ PVOID Parameter,
    _In_ BOOLEAN TimerOrWaitFired
    );
typedef RTL_TIMER_CALLBACK *PRTL_TIMER_CALLBACK;

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateTimer(
    _In_ HANDLE TimerQueueHandle,
    _Out_ PHANDLE Handle,
    _In_ PRTL_TIMER_CALLBACK Function,
    _In_opt_ PVOID Context,
    _In_ ULONG DueTime,
    _In_ ULONG Period,
    _In_ ULONG Flags);

NTSYSAPI
NTSTATUS
NTAPI
RtlSetTimer(
    _In_ HANDLE TimerQueueHandle,
    _Out_ PHANDLE Handle,
    _In_ PRTL_TIMER_CALLBACK Function,
    _In_opt_ PVOID Context,
    _In_ ULONG DueTime,
    _In_ ULONG Period,
    _In_ ULONG Flags);

NTSYSAPI
NTSTATUS
NTAPI
RtlUpdateTimer(
    _In_ HANDLE TimerQueueHandle,
    _In_ HANDLE TimerHandle,
    _In_ ULONG DueTime,
    _In_ ULONG Period);

#define RTL_TIMER_DELETE_WAIT_FOR_COMPLETION ((HANDLE)(LONG_PTR)-1)

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteTimer(
    _In_ HANDLE TimerQueueHandle,
    _In_ HANDLE TimerToCancel,
    _In_opt_ HANDLE Event // optional: RTL_TIMER_DELETE_WAIT_FOR_COMPLETION
);

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteTimerQueue(
    _In_ HANDLE TimerQueueHandle);

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteTimerQueueEx(
    _In_ HANDLE TimerQueueHandle,
    _In_opt_ HANDLE Event);

#pragma endregion

#pragma region QPC

// rev
/**
 * The RtlQueryPerformanceCounter routine retrieves the current value of the performance counter, which is a high resolution (<1us) time stamp that can be used for time-interval measurements.
 *
 * \param PerformanceCounter A pointer to a variable that receives the current performance-counter value, in counts.
 * \return Returns TRUE if the function succeeds, otherwise FALSE. On systems that run Windows XP or later, the function will always succeed and will thus never return zero.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter
 */
NTSYSAPI
LOGICAL
NTAPI
RtlQueryPerformanceCounter(
    _Out_ PLARGE_INTEGER PerformanceCounter);

// rev
/**
 * The RtlQueryPerformanceFrequency routine retrieves the frequency of the performance counter. The frequency of the performance counter is fixed at system boot and is consistent across all processors.
 * Therefore, the frequency need only be queried upon application initialization, and the result can be cached.
 *
 * \param PerformanceFrequency A pointer to a variable that receives the current performance-counter frequency, in counts per second. 
 * \return Returns TRUE if the function succeeds, otherwise FALSE. On systems that run Windows XP or later, the function will always succeed and will thus never return zero.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancefrequency
 */
NTSYSAPI
LOGICAL
NTAPI
RtlQueryPerformanceFrequency(
    _Out_ PLARGE_INTEGER PerformanceFrequency);

#pragma endregion

#pragma region Transactions

NTSYSAPI
HANDLE
NTAPI
RtlGetCurrentTransaction(
    _In_opt_ PCWSTR ExistingFileName,
    _In_opt_ PCWSTR NewFileName);

NTSYSAPI
LOGICAL
NTAPI
RtlSetCurrentTransaction(
    _In_opt_ HANDLE TransactionHandle);

#pragma endregion

#pragma region Pointer Encode/Decode

/**
 * The RtlEncodePointer routine encodes the specified pointer. Encoded pointers can be used to provide another layer of protection for pointer values.
 *
 * \param Ptr The system pointer to be encoded.
 * \return The function returns the encoded pointer.
 * \sa https://learn.microsoft.com/en-us/previous-versions/bb432254(v=vs.85)
 */
NTSYSAPI
_Ret_maybenull_
PVOID
NTAPI
RtlEncodePointer(
    _In_opt_ PVOID Ptr);

/**
 * The RtlDecodePointer routine decodes a pointer that was previously encoded with RtlEncodePointer.
 *
 * \param Ptr The system pointer to be decoded.
 * \return The function returns the decoded pointer.
 * \sa https://learn.microsoft.com/en-us/previous-versions/bb432242(v=vs.85)
 */
NTSYSAPI
_Ret_maybenull_
PVOID
NTAPI
RtlDecodePointer(
    _In_opt_ PVOID Ptr);

/**
 * The RtlEncodeSystemPointer routine encodes the specified pointer with a system-specific value.
 * Encoded pointers can be used to provide another layer of protection for pointer values.
 *
 * \param Ptr The system pointer to be encoded.
 * \return The function returns the encoded pointer.
 * \sa https://learn.microsoft.com/en-us/previous-versions/bb432255(v=vs.85)
 */
NTSYSAPI
_Ret_maybenull_
PVOID
NTAPI
RtlEncodeSystemPointer(
    _In_opt_ PVOID Ptr);

/**
 * The RtlDecodeSystemPointer routine decodes a pointer that was previously encoded with RtlEncodeSystemPointer.
 *
 * \param Ptr The pointer to be decoded.
 * \return The function returns the decoded pointer.
 * \sa https://learn.microsoft.com/en-us/previous-versions/bb432243(v=vs.85)
 */
NTSYSAPI
_Ret_maybenull_
PVOID
NTAPI
RtlDecodeSystemPointer(
    _In_opt_ PVOID Ptr);

#if (NTDDI_VERSION >= NTDDI_WIN10)

// rev
/**
 * The RtlEncodeRemotePointer routine encodes the specified pointer of the specified process.
 * Encoded pointers can be used to provide another layer of protection for pointer values.
 *
 * \param ProcessHandle Handle to the remote process that owns the pointer.
 * \param Pointer The pointer to be encoded.
 * \param EncodedPointer The encoded pointer.
 * \return HRESULT Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/previous-versions/dn877135(v=vs.85)
 */
NTSYSAPI
HRESULT
NTAPI
RtlEncodeRemotePointer(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID Pointer,
    _Out_ PVOID* EncodedPointer);

// rev
/**
 * The RtlDecodeRemotePointer routine decodes a pointer in a specified process that was previously 
 * encoded with RtlEncodePointer or RtlEncodeRemotePointer.
 *
 * \param ProcessHandle Handle to the remote process that owns the pointer.
 * \param Pointer The pointer to be decoded.
 * \param DecodedPointer The decoded pointer.
 * \return HRESULT Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/previous-versions/dn877133(v=vs.85)
 */
NTSYSAPI
HRESULT
NTAPI
RtlDecodeRemotePointer(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID Pointer,
    _Out_ PVOID* DecodedPointer);

#endif

#pragma endregion

#pragma region Session

NTSYSAPI
ULONG
NTAPI
RtlGetCurrentServiceSessionId(VOID);

NTSYSAPI
ULONG
NTAPI
RtlGetActiveConsoleId(VOID);

#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)
NTSYSAPI
LONGLONG
NTAPI
RtlGetConsoleSessionForegroundProcessId(VOID);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)
NTSYSAPI
NTSTATUS
NTAPI
RtlGetSessionProperties(
    _In_ ULONG SessionId,
    _Out_ PULONG SharedUserSessionId);
#endif

#pragma endregion

#pragma region Lock/Unlock TEB/Stack/Module Section

NTSYSAPI
NTSTATUS
NTAPI
RtlLockCurrentThread(VOID);

NTSYSAPI
NTSTATUS
NTAPI
RtlUnlockCurrentThread(VOID);

NTSYSAPI
NTSTATUS
NTAPI
RtlLockModuleSection(
    _In_ PVOID Address);

NTSYSAPI
NTSTATUS
NTAPI
RtlUnlockModuleSection(
    _In_ PVOID Address);

#pragma endregion

#pragma region Place Holder

#if (NTDDI_VERSION >= NTDDI_WIN10_RS3)

NTSYSAPI
BOOLEAN
NTAPI
RtlIsCloudFilesPlaceholder(
    _In_ ULONG FileAttributes,
    _In_ ULONG ReparseTag);

NTSYSAPI
BOOLEAN
NTAPI
RtlIsPartialPlaceholder(
    _In_ ULONG FileAttributes,
    _In_ ULONG ReparseTag);

NTSYSAPI
NTSTATUS
NTAPI
RtlIsPartialPlaceholderFileHandle(
    _In_ HANDLE FileHandle,
    _Out_ PBOOLEAN IsPartialPlaceholder);

NTSYSAPI
NTSTATUS
NTAPI
RtlIsPartialPlaceholderFileInfo(
    _In_ PVOID InfoBuffer,
    _In_ FILE_INFORMATION_CLASS InfoClass,
    _Out_ PBOOLEAN IsPartialPlaceholder);

#undef PHCM_MAX
#define PHCM_APPLICATION_DEFAULT ((CHAR)0)
#define PHCM_DISGUISE_PLACEHOLDERS ((CHAR)1)
#define PHCM_EXPOSE_PLACEHOLDERS ((CHAR)2)
#define PHCM_MAX ((CHAR)2)

#define PHCM_ERROR_INVALID_PARAMETER ((CHAR)-1)
#define PHCM_ERROR_NO_TEB ((CHAR)-2)

NTSYSAPI
CHAR
NTAPI
RtlQueryThreadPlaceholderCompatibilityMode(VOID);

NTSYSAPI
CHAR
NTAPI
RtlSetThreadPlaceholderCompatibilityMode(
    _In_ CHAR Mode);

#endif

#undef PHCM_MAX
#define PHCM_DISGUISE_FULL_PLACEHOLDERS ((CHAR)3)
#define PHCM_MAX ((CHAR)3)
#define PHCM_ERROR_NO_PEB ((CHAR)-3)

#if (NTDDI_VERSION >= NTDDI_WIN10_RS4)

NTSYSAPI
CHAR
NTAPI
RtlQueryProcessPlaceholderCompatibilityMode(VOID);

NTSYSAPI
CHAR
NTAPI
RtlSetProcessPlaceholderCompatibilityMode(
    _In_ CHAR Mode);

#endif

#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)
NTSYSAPI
BOOLEAN
NTAPI
RtlIsNonEmptyDirectoryReparsePointAllowed(
    _In_ ULONG ReparseTag);
#endif

#pragma endregion

#pragma region AppX

#if (NTDDI_VERSION >= NTDDI_WIN8)
NTSYSAPI
NTSTATUS
NTAPI
RtlAppxIsFileOwnedByTrustedInstaller(
    _In_ HANDLE FileHandle,
    _Out_ PBOOLEAN IsFileOwnedByTrustedInstaller);
#endif

// Windows Internals book
#define PSM_ACTIVATION_TOKEN_PACKAGED_APPLICATION       0x00000001UL // AppX package format
#define PSM_ACTIVATION_TOKEN_SHARED_ENTITY              0x00000002UL // Shared token, multiple binaries in the same package
#define PSM_ACTIVATION_TOKEN_FULL_TRUST                 0x00000004UL // Trusted (Centennial), converted Win32 application
#define PSM_ACTIVATION_TOKEN_NATIVE_SERVICE             0x00000008UL // Packaged service created by SCM
//#define PSM_ACTIVATION_TOKEN_DEVELOPMENT_APP          0x00000010UL
#define PSM_ACTIVATION_TOKEN_MULTIPLE_INSTANCES_ALLOWED 0x00000010UL
#define PSM_ACTIVATION_TOKEN_BREAKAWAY_INHIBITED        0x00000020UL // Cannot create non-packaged child processes
#define PSM_ACTIVATION_TOKEN_RUNTIME_BROKER             0x00000040UL // rev
#define PSM_ACTIVATION_TOKEN_UNIVERSAL_CONSOLE          0x00000200UL // rev
#define PSM_ACTIVATION_TOKEN_WIN32ALACARTE_PROCESS      0x00010000UL // rev

// PackageOrigin appmodel.h
//#define PackageOrigin_Unknown           0
//#define PackageOrigin_Unsigned          1
//#define PackageOrigin_Inbox             2
//#define PackageOrigin_Store             3
//#define PackageOrigin_DeveloperUnsigned 4
//#define PackageOrigin_DeveloperSigned   5
//#define PackageOrigin_LineOfBusiness    6

#define PSMP_MINIMUM_SYSAPP_CLAIM_VALUES 2
#define PSMP_MAXIMUM_SYSAPP_CLAIM_VALUES 4

typedef struct _PS_PKG_CLAIM
{
    ULONG Flags;  // PSM_ACTIVATION_TOKEN_*
    ULONG Origin; // PackageOrigin
} PS_PKG_CLAIM, *PPS_PKG_CLAIM;

// private // WIN://BGKD
typedef enum _PSM_ACTIVATE_BACKGROUND_TYPE
{
  PsmActNotBackground = 0,
  PsmActMixedHost = 1,
  PsmActPureHost = 2,
  PsmActSystemHost = 3,
  PsmActInvalidType = 4,
} PSM_ACTIVATE_BACKGROUND_TYPE;

#if (NTDDI_VERSION >= NTDDI_WIN10)
NTSYSAPI
NTSTATUS
NTAPI
RtlQueryPackageClaims(
    _In_ HANDLE TokenHandle,
    _Out_writes_bytes_to_opt_(*PackageSize, *PackageSize) PWSTR PackageFullName,
    _Inout_opt_ PSIZE_T PackageSize,
    _Out_writes_bytes_to_opt_(*AppIdSize, *AppIdSize) PWSTR AppId,
    _Inout_opt_ PSIZE_T AppIdSize,
    _Out_opt_ PGUID DynamicId,
    _Out_opt_ PPS_PKG_CLAIM PkgClaim,
    _Out_opt_ PULONG64 AttributesPresent);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN8)
NTSYSAPI
NTSTATUS
NTAPI
RtlQueryPackageIdentity(
    _In_ HANDLE TokenHandle,
    _Out_writes_bytes_to_(*PackageSize, *PackageSize) PWSTR PackageFullName,
    _Inout_ PSIZE_T PackageSize,
    _Out_writes_bytes_to_opt_(*AppIdSize, *AppIdSize) PWSTR AppId,
    _Inout_opt_ PSIZE_T AppIdSize,
    _Out_opt_ PBOOLEAN Packaged);
#endif

#if (NTDDI_VERSION >= NTDDI_WINBLUE)
NTSYSAPI
NTSTATUS
NTAPI
RtlQueryPackageIdentityEx(
    _In_ HANDLE TokenHandle,
    _Out_writes_bytes_to_(*PackageSize, *PackageSize) PWSTR PackageFullName,
    _Inout_ PSIZE_T PackageSize,
    _Out_writes_bytes_to_opt_(*AppIdSize, *AppIdSize) PWSTR AppId,
    _Inout_opt_ PSIZE_T AppIdSize,
    _Out_opt_ PGUID DynamicId,
    _Out_opt_ PULONG64 Flags);
#endif

#pragma endregion

#pragma region Wnf

#if (NTDDI_VERSION >= NTDDI_WIN10)

#define WNF_STATE_KEY 0x41C64E6DA3BC0074

_Must_inspect_result_
NTSYSAPI
BOOLEAN
NTAPI
RtlEqualWnfChangeStamps(
    _In_ WNF_CHANGE_STAMP ChangeStamp1,
    _In_ WNF_CHANGE_STAMP ChangeStamp2
);

_Always_(_Post_satisfies_(return == STATUS_NO_MEMORY || return == STATUS_RETRY || return == STATUS_SUCCESS))
typedef _Function_class_(WNF_USER_CALLBACK)
NTSTATUS NTAPI WNF_USER_CALLBACK(
    _In_ WNF_STATE_NAME StateName,
    _In_ WNF_CHANGE_STAMP ChangeStamp,
    _In_opt_ PWNF_TYPE_ID TypeId,
    _In_opt_ PVOID CallbackContext,
    _In_reads_bytes_opt_(Length) const VOID* Buffer,
    _In_ ULONG Length
);
typedef WNF_USER_CALLBACK *PWNF_USER_CALLBACK;

NTSYSAPI
NTSTATUS
NTAPI
RtlQueryWnfStateData(
    _Out_ PWNF_CHANGE_STAMP ChangeStamp,
    _In_ WNF_STATE_NAME StateName,
    _In_ PWNF_USER_CALLBACK Callback,
    _In_opt_ PVOID CallbackContext,
    _In_opt_ PWNF_TYPE_ID TypeId
);

NTSYSAPI
NTSTATUS
NTAPI
RtlPublishWnfStateData(
    _In_ WNF_STATE_NAME StateName,
    _In_opt_ PCWNF_TYPE_ID TypeId,
    _In_reads_bytes_opt_(Length) const VOID * Buffer,
    _In_opt_ ULONG Length,
    _In_opt_ const VOID * ExplicitScope
);

typedef struct WNF_USER_SUBSCRIPTION *PWNF_USER_SUBSCRIPTION;

#define WNF_CREATE_SERIALIZATION_GROUP_FLAG 0x00000001L

NTSYSAPI
NTSTATUS
NTAPI
RtlSubscribeWnfStateChangeNotification(
    _Out_ PWNF_USER_SUBSCRIPTION* SubscriptionHandle,
    _In_ WNF_STATE_NAME StateName,
    _In_ WNF_CHANGE_STAMP ChangeStamp,
    _In_ PWNF_USER_CALLBACK Callback,
    _In_opt_ PVOID CallbackContext,
    _In_opt_ PCWNF_TYPE_ID TypeId,
    _In_opt_ ULONG SerializationGroup,
    _In_ ULONG Flags
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUnsubscribeWnfStateChangeNotification(
    _In_ PWNF_USER_CALLBACK Callback
);

NTSYSAPI
NTSTATUS
NTAPI
RtlWnfDllUnloadCallback(
    _In_ PVOID DllBase
);

#endif

#pragma endregion

#pragma region Unload Event Trace

#define RTL_UNLOAD_EVENT_TRACE_NUMBER 64

// private
/**
 * The RTL_UNLOAD_EVENT_TRACE structure contains information about modules unloaded by the current process.
 *
 * \sa https://learn.microsoft.com/en-us/windows/win32/devnotes/rtlgetunloadeventtrace
 */
typedef struct _RTL_UNLOAD_EVENT_TRACE
{
    PVOID BaseAddress;   // Base address of dll
    SIZE_T SizeOfImage;  // Size of image
    ULONG Sequence;      // Sequence number for this event
    ULONG TimeDateStamp; // Time and date of image
    ULONG CheckSum;      // Image checksum
    WCHAR ImageName[32]; // Image name
    ULONG Version[2];
} RTL_UNLOAD_EVENT_TRACE, *PRTL_UNLOAD_EVENT_TRACE;

typedef struct _RTL_UNLOAD_EVENT_TRACE64
{
    VOID* POINTER_64 BaseAddress;
    ULONGLONG SizeOfImage;
    ULONG Sequence;
    ULONG TimeDateStamp;
    ULONG CheckSum;
    WCHAR ImageName[32];
    ULONG Version[2];
} RTL_UNLOAD_EVENT_TRACE64, *PRTL_UNLOAD_EVENT_TRACE64;

typedef struct _RTL_UNLOAD_EVENT_TRACE32
{
    VOID* POINTER_32 BaseAddress;
    ULONG SizeOfImage;
    ULONG Sequence;
    ULONG TimeDateStamp;
    ULONG CheckSum;
    WCHAR ImageName[32];
    ULONG Version[2];
} RTL_UNLOAD_EVENT_TRACE32, *PRTL_UNLOAD_EVENT_TRACE32;

/**
 * The RtlGetUnloadEventTrace routine enables the dump code to get the unloaded module information from Ntdll.dll for storage in the minidump.
 *
 * \return A pointer to an array of unload events.
 * \sa https://learn.microsoft.com/en-us/windows/win32/devnotes/rtlgetunloadeventtrace
 */
NTSYSAPI
PRTL_UNLOAD_EVENT_TRACE
NTAPI
RtlGetUnloadEventTrace(VOID);

/**
 * The RtlGetUnloadEventTraceEx routine retrieves the size and location of the dynamically unloaded module list for the current process.
 *
 * \param ElementSize A pointer to a variable that contains the size of an element in the list.
 * \param ElementCount A pointer to a variable that contains the number of elements in the list.
 * \param EventTrace A pointer to an array of RTL_UNLOAD_EVENT_TRACE structures.
 * \return A pointer to an array of unload events.
 * \sa https://learn.microsoft.com/en-us/windows/win32/devnotes/rtlgetunloadeventtraceex
 */
NTSYSAPI
PRTL_UNLOAD_EVENT_TRACE
NTAPI
RtlGetUnloadEventTraceEx(
    _Out_ PULONG * ElementSize,
    _Out_ PULONG * ElementCount,
    _Out_ PVOID * EventTrace // works across all processes
);

#pragma endregion

#pragma region State Isolation

typedef enum _STATE_LOCATION_TYPE
{
    LocationTypeRegistry,
    LocationTypeFileSystem,
    LocationTypeMaximum
} STATE_LOCATION_TYPE, *PSTATE_LOCATION_TYPE;

#if (NTDDI_VERSION >= NTDDI_WIN10_RS3)

NTSYSAPI
BOOLEAN
NTAPI
RtlIsStateSeparationEnabled(VOID);

NTSYSAPI
NTSTATUS
NTAPI
RtlGetPersistedStateLocation(
    _In_ PCWSTR SourceID,
    _In_opt_ PCWSTR CustomValue,
    _In_opt_ PCWSTR DefaultPath,
    _In_ STATE_LOCATION_TYPE StateLocationType,
    _Out_writes_bytes_to_opt_(BufferLengthIn, *BufferLengthOut) PWCHAR TargetPath,
    _In_ ULONG BufferLengthIn,
    _Out_opt_ PULONG BufferLengthOut);

#endif

#pragma region Property Store

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)

NTSYSAPI
NTSTATUS
NTAPI
RtlQueryPropertyStore(
    _In_ ULONG_PTR Key,
    _Out_ PULONG_PTR Context);

NTSYSAPI
NTSTATUS
NTAPI
RtlRemovePropertyStore(
    _In_ ULONG_PTR Key,
    _Out_ PULONG_PTR Context);

NTSYSAPI
NTSTATUS
NTAPI
RtlCompareExchangePropertyStore(
    _In_ ULONG_PTR Key,
    _In_ PULONG_PTR Comperand,
    _In_opt_ PULONG_PTR Exchange,
    _Out_ PULONG_PTR Context);

#endif

#pragma endregion

#pragma region Thread Pool (Old)

NTSYSAPI
NTSTATUS
NTAPI
RtlRegisterWait(
    _Out_ PHANDLE WaitHandle,
    _In_ HANDLE Handle,
    _In_ WAITORTIMERCALLBACKFUNC Function,
    _In_opt_ PVOID Context,
    _In_ ULONG Milliseconds,
    _In_ ULONG Flags);

NTSYSAPI
NTSTATUS
NTAPI
RtlDeregisterWait(
    _In_ HANDLE WaitHandle);

#define RTL_WAITER_DEREGISTER_WAIT_FOR_COMPLETION ((HANDLE)(LONG_PTR)-1)

/**
 * Releases all resources used by a wait object.
 *
 * @param WaitHandle The access mask that specifies the granted access rights.
 * @param CompletionEvent Optional completion event for wait callback completion.
 * @remarks RTL_WAITER_DEREGISTER_WAIT_FOR_COMPLETION: blocking wait for wait callback completion.
 * NULL: non-blocking wait for wait callback completion.
 * EventHandle: caller wait for wait callback completion.
 * @return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
RtlDeregisterWaitEx(
    _In_ HANDLE WaitHandle,
    _In_opt_ HANDLE CompletionEvent // optional: RTL_WAITER_DEREGISTER_WAIT_FOR_COMPLETION
);

// WORKERCALLBACKFUNC
typedef _Function_class_(RTL_WORK_CALLBACK)
VOID NTAPI RTL_WORK_CALLBACK(
    _In_ PVOID ThreadParameter
    );
typedef RTL_WORK_CALLBACK* PRTL_WORK_CALLBACK;

NTSYSAPI
NTSTATUS
NTAPI
RtlQueueWorkItem(
    _In_ PRTL_WORK_CALLBACK Function,
    _In_opt_ PVOID Context,
    _In_ ULONG Flags);

// APC_CALLBACK_FUNCTION
typedef _Function_class_(RTL_OVERLAPPED_COMPLETION_ROUTINE)
VOID NTAPI RTL_OVERLAPPED_COMPLETION_ROUTINE(
    _In_ NTSTATUS StatusCode,
    _In_ PVOID Context1,
    _In_ PVOID Context2
    );
typedef RTL_OVERLAPPED_COMPLETION_ROUTINE* PRTL_OVERLAPPED_COMPLETION_ROUTINE;

NTSYSAPI
NTSTATUS
NTAPI
RtlSetIoCompletionCallback(
    _In_ HANDLE FileHandle,
    _In_ PRTL_OVERLAPPED_COMPLETION_ROUTINE Function,
    _In_ ULONG Flags);

typedef
_Function_class_(RTL_START_POOL_THREAD)
NTSTATUS
NTAPI
RTL_START_POOL_THREAD(
    _In_ PUSER_THREAD_START_ROUTINE Function,
    _In_ PVOID Parameter,
    _Out_ PHANDLE ThreadHandle);
typedef RTL_START_POOL_THREAD *PRTL_START_POOL_THREAD;

typedef
_Function_class_(RTL_EXIT_POOL_THREAD)
NTSTATUS
NTAPI
RTL_EXIT_POOL_THREAD(
    _In_ NTSTATUS ExitStatus);
typedef RTL_EXIT_POOL_THREAD *PRTL_EXIT_POOL_THREAD;

NTSYSAPI
NTSTATUS
NTAPI
RtlSetThreadPoolStartFunc(
    _In_ PRTL_START_POOL_THREAD StartPoolThread,
    _In_ PRTL_EXIT_POOL_THREAD ExitPoolThread);

#pragma endregion

#define RTL_IMPORT_TABLE_HASH_REVISION 1

NTSYSAPI
NTSTATUS
NTAPI
RtlComputeImportTableHash(
    _In_ HANDLE FileHandle,
    _Out_writes_bytes_(16) PUCHAR Hash,
    _In_ ULONG ImportTableHashRevision);

/**
 * The RtlComputeCrc32 routine computes the CRC32 checksum for a buffer, allowing for incremental computation by providing a partial CRC value.
 *
 * \param PartialCrc The initial CRC32 value. Use 0 for a new computation, or the result of a previous call to continue CRC calculation over additional data.
 * \param Buffer Pointer to the buffer containing the data to compute the CRC32 for.
 * \param Length The length, in bytes, of the buffer.
 * \return The computed CRC32 value.
 */
NTSYSAPI
ULONG32
NTAPI
RtlComputeCrc32(
    _In_ ULONG32 PartialCrc,
    _In_ PVOID Buffer,
    _In_ ULONG Length);

#if (NTDDI_VERSION >= NTDDI_WIN10_MN)
NTSYSAPI
ULONG_PTR
NTAPI
RtlGetReturnAddressHijackTarget(VOID);
#endif

EXTERN_C_END
