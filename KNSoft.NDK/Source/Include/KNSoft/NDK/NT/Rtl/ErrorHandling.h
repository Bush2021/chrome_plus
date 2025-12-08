#pragma once

#include "../MinDef.h"

EXTERN_C_START

#pragma region Fast Fail

/**
 * The RtlFailFast routine brings down the caller immediately in the event that critical corruption has been detected. No exception handlers are invoked.
 * 
 * \param Code A FAST_FAIL_<description> symbolic constant from winnt.h or wdm.h that indicates the reason for process termination.
 * \return None. There is no return from this routine.
 * \remarks The routine is shared with user mode and kernel mode. In user mode, the process is terminated, whereas in kernel mode, a KERNEL_SECURITY_CHECK_FAILURE bug check is raised.
 */
DECLSPEC_NORETURN
FORCEINLINE
VOID
RtlFailFast(
    _In_ ULONG Code)
{
    __fastfail(Code);
}

#pragma endregion wdm.h

#pragma region Assert

NTSYSAPI
__analysis_noreturn
VOID
NTAPI
RtlAssert(
    _In_ PVOID VoidFailedAssertion,
    _In_ PVOID VoidFileName,
    _In_ ULONG LineNumber,
    _In_opt_ PSTR MutableMessage);

#if DBG

#define ASSERT( exp ) \
    ((!(exp)) ? \
        (RtlAssert( (PVOID)#exp, (PVOID)__FILE__, __LINE__, NULL ),FALSE) : \
        TRUE)
#define ASSERTMSG( msg, exp ) \
    ((!(exp)) ? \
        (RtlAssert( (PVOID)#exp, (PVOID)__FILE__, __LINE__, (PSTR)msg ),FALSE) : \
        TRUE)
#define RTL_SOFT_ASSERT(_exp) \
    ((!(_exp)) ? \
        (DbgPrint("%s(%d): Soft assertion failed\n   Expression: %s\n", __FILE__, __LINE__, #_exp),FALSE) : \
        TRUE)
#define RTL_SOFT_ASSERTMSG(_msg, _exp) \
    ((!(_exp)) ? \
        (DbgPrint("%s(%d): Soft assertion failed\n   Expression: %s\n   Message: %s\n", __FILE__, __LINE__, #_exp, (_msg)),FALSE) : \
        TRUE)
#define RTL_VERIFY         ASSERT
#define RTL_VERIFYMSG      ASSERTMSG
#define RTL_SOFT_VERIFY    RTL_SOFT_ASSERT
#define RTL_SOFT_VERIFYMSG RTL_SOFT_ASSERTMSG

#else

#define ASSERT(exp)         ((void) 0)
#define ASSERTMSG(msg, exp) ((void) 0)
#define RTL_SOFT_ASSERT(_exp)           ((void) 0)
#define RTL_SOFT_ASSERTMSG(_msg, _exp)  ((void) 0)
#define RTL_VERIFY(exp)         ((exp) ? TRUE : FALSE)
#define RTL_VERIFYMSG(msg, exp) ((exp) ? TRUE : FALSE)
#define RTL_SOFT_VERIFY(_exp)           ((_exp) ? TRUE : FALSE)
#define RTL_SOFT_VERIFYMSG(msg, _exp)   ((_exp) ? TRUE : FALSE)

#endif // DBG

#pragma endregion wdm.h

#pragma region Exception

/* phnt */
NTSYSAPI
BOOLEAN
NTAPI
RtlDispatchException(
    _In_ PEXCEPTION_RECORD ExceptionRecord,
    _In_ PCONTEXT ContextRecord);

/* phnt */
_Analysis_noreturn_
NTSYSAPI
DECLSPEC_NORETURN
VOID
NTAPI
RtlRaiseStatus(
    _In_ NTSTATUS Status);

#if (NTDDI_VERSION >= NTDDI_WIN10_MN)

/* phnt */
NTSYSAPI
VOID
NTAPI
RtlRaiseExceptionForReturnAddressHijack(VOID);

/* phnt */
_Analysis_noreturn_
NTSYSAPI
DECLSPEC_NORETURN
VOID
NTAPI
RtlRaiseNoncontinuableException(
    _In_ PEXCEPTION_RECORD ExceptionRecord,
    _In_ PCONTEXT ContextRecord);

#endif

#pragma endregion

#pragma region Exception Handling

/**
 * Registers a vectored exception handler.
 * 
 * @param First If this parameter is TRUE, the handler is the first handler in the list.
 * @param Handler A pointer to the vectored exception handler to be called.
 * @return A handle to the vectored exception handler.
 * @see https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-addvectoredexceptionhandler
 */
NTSYSAPI
PVOID
NTAPI
RtlAddVectoredExceptionHandler(
    _In_ ULONG First,
    _In_ PVECTORED_EXCEPTION_HANDLER Handler);

/**
 * Removes a vectored exception handler.
 * 
 * @param Handle A handle to the vectored exception handler to remove.
 * @return The function returns 0 if the handler is removed, or -1 if the handler is not found.
 * @see https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-removevectoredexceptionhandler
 */
NTSYSAPI
ULONG
NTAPI
RtlRemoveVectoredExceptionHandler(
    _In_ PVOID Handle);

/**
 * Registers a vectored continue handler.
 * 
 * @param First If this parameter is TRUE, the handler is the first handler in the list.
 * @param Handler A pointer to the vectored exception handler to be called.
 * @return A handle to the vectored continue handler.
 * @see https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-addvectoredcontinuehandler
 */
NTSYSAPI
PVOID
NTAPI
RtlAddVectoredContinueHandler(
    _In_ ULONG First,
    _In_ PVECTORED_EXCEPTION_HANDLER Handler);

/**
 * Removes a vectored continue handler.
 * 
 * @param Handle A handle to the vectored continue handler to remove.
 * @return The function returns 0 if the handler is removed, or -1 if the handler is not found.
 * @see https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-removevectoredcontinuehandler
 */
NTSYSAPI
ULONG
NTAPI
RtlRemoveVectoredContinueHandler(
    _In_ PVOID Handle);

typedef
_Function_class_(RTLP_UNHANDLED_EXCEPTION_FILTER)
LONG
NTAPI
RTLP_UNHANDLED_EXCEPTION_FILTER(
    _In_ PEXCEPTION_POINTERS ExceptionInfo);
typedef RTLP_UNHANDLED_EXCEPTION_FILTER *PRTLP_UNHANDLED_EXCEPTION_FILTER;

NTSYSAPI
VOID
NTAPI
RtlSetUnhandledExceptionFilter(
    _In_opt_ PRTLP_UNHANDLED_EXCEPTION_FILTER UnhandledExceptionFilter);

// rev
NTSYSAPI
LONG
NTAPI
RtlUnhandledExceptionFilter(
    _In_ PEXCEPTION_POINTERS ExceptionPointers);

// rev
NTSYSAPI
LONG
NTAPI
RtlUnhandledExceptionFilter2(
    _In_ PEXCEPTION_POINTERS ExceptionPointers,
    _In_ ULONG Flags);

// rev
NTSYSAPI
LONG
NTAPI
RtlKnownExceptionFilter(
    _In_ PEXCEPTION_POINTERS ExceptionPointers);

#pragma endregion

#pragma region Win32 Error and NTSTATUS

_When_(Status < 0, _Out_range_(>, 0))
_When_(Status >= 0, _Out_range_(==, 0))
NTSYSAPI
ULONG
NTAPI
RtlNtStatusToDosErrorNoTeb(
    _In_ NTSTATUS Status);

_When_(Status < 0, _Out_range_(>, 0))
_When_(Status >= 0, _Out_range_(==, 0))
NTSYSAPI
ULONG
NTAPI
RtlNtStatusToDosError(
    _In_ NTSTATUS Status);

NTSYSAPI
NTSTATUS
NTAPI
RtlGetLastNtStatus(VOID);

_Check_return_
_Post_equals_last_error_
NTSYSAPI
ULONG
NTAPI
RtlGetLastWin32Error(VOID);

NTSYSAPI
VOID
NTAPI
RtlSetLastWin32Error(
    _In_ ULONG Win32Error);

NTSYSAPI
VOID
NTAPI
RtlRestoreLastWin32Error(
    _In_ ULONG Win32Error);

NTSYSAPI
VOID
NTAPI
RtlSetLastWin32ErrorAndNtStatusFromNtStatus(
    _In_ NTSTATUS Status);

/*
 * Error code conversion (NOT translation) Win32 Error/NTSTATUS/HRESULT 
 * HRESULT_FROM_WIN32 / NTSTATUS_FROM_WIN32 / HRESULT_FROM_NT
 */

#define RTL_ERRORMODE_FAILCRITICALERRORS 0x0010
#define RTL_ERRORMODE_NOGPFAULTERRORBOX 0x0020
#define RTL_ERRORMODE_NOOPENFILEERRORBOX 0x0040

NTSYSAPI
ULONG
NTAPI
RtlGetThreadErrorMode(VOID);

NTSYSAPI
NTSTATUS
NTAPI
RtlSetThreadErrorMode(
    _In_ ULONG NewMode,
    _Out_opt_ PULONG OldMode);

#pragma endregion phnt, with fixes by KNSoft.NDK

#pragma region WER

NTSYSAPI
NTSTATUS
NTAPI
RtlReportException(
    _In_ PEXCEPTION_RECORD ExceptionRecord,
    _In_ PCONTEXT ContextRecord,
    _In_ ULONG Flags);

#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)
NTSYSAPI
NTSTATUS
NTAPI
RtlReportExceptionEx(
    _In_ PEXCEPTION_RECORD ExceptionRecord,
    _In_ PCONTEXT ContextRecord,
    _In_ ULONG Flags,
    _In_ PLARGE_INTEGER Timeout);
#endif

NTSYSAPI
NTSTATUS
NTAPI
RtlWerpReportException(
    _In_ ULONG ProcessId,
    _In_ HANDLE CrashReportSharedMem,
    _In_ ULONG Flags,
    _Out_ PHANDLE CrashVerticalProcessHandle);

NTSYSAPI
NTSTATUS
NTAPI
RtlReportSilentProcessExit(
    _In_ HANDLE ProcessHandle,
    _In_ NTSTATUS ExitStatus);

#pragma endregion phnt

EXTERN_C_END
