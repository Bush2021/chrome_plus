#pragma once

#include "../../NDK.h"
#include "../../NT/NT.inl"

EXTERN_C_START

#pragma region Internal functions

__inline
ULONG
_Inline_BaseSetLastNTError(
    _In_ NTSTATUS Status)
{
    ULONG Error = _Inline_RtlNtStatusToDosError(Status);
    _Inline_RtlSetLastWin32Error(Error);
    return Error;
}

__inline
PVOID
_Inline_BasepMapModuleHandle(
    _In_opt_ HMODULE ModuleHandle,
    _In_ BOOLEAN AsResourceFile)
{
    if (ModuleHandle == NULL)
    {
        return NtCurrentPeb()->ImageBaseAddress;
    }
    return !LDR_IS_RESOURCE(ModuleHandle) || AsResourceFile ? ModuleHandle : NULL;
}

__inline
PLARGE_INTEGER
_Inline_BaseFormatTimeOut(
    PLARGE_INTEGER Timeout,
    _In_ ULONG Milliseconds)
{
    if (Milliseconds == INFINITE)
    {
        return NULL;
    } else
    {
        Timeout->QuadPart = Milliseconds * -10000LL;
        return Timeout;
    }
}

#pragma endregion

#pragma region Process Environment

__inline
DWORD
WINAPI
_Inline_GetCurrentProcessId(VOID)
{
    return (DWORD)(ULONG_PTR)NtCurrentProcessId();
}

__inline
DWORD
WINAPI
_Inline_GetCurrentThreadId(VOID)
{
    return (DWORD)(ULONG_PTR)NtCurrentThreadId();
}

__inline
HANDLE
WINAPI
_Inline_GetCurrentProcess(VOID)
{
    return NtCurrentProcess();
}

__inline
HANDLE
WINAPI
_Inline_GetCurrentThread(VOID)
{
    return NtCurrentThread();
}

__inline
_NullNull_terminated_
LPWCH
WINAPI
_Inline_GetEnvironmentStringsW(VOID)
{
    PWCHAR pEnv, p;
    SIZE_T sSize;

    _Inline_RtlAcquirePebLock();

    pEnv = (PWCHAR)NtCurrentPeb()->ProcessParameters->Environment;
    for (p = pEnv; *p != UNICODE_NULL; p += wcslen(p) + 1);
    sSize = (p - pEnv + 1) * sizeof(WCHAR);

    p = (PWCHAR)RtlAllocateHeap(RtlProcessHeap(), 0, sSize);
    if (p)
    {
        memcpy(p, pEnv, sSize);
    } else
    {
        _Inline_BaseSetLastNTError(STATUS_NO_MEMORY);
    }

    _Inline_RtlReleasePebLock();
    return p;
}

__inline
BOOL
WINAPI
_Inline_FreeEnvironmentStringsW(
    _In_ _Pre_ _NullNull_terminated_ LPWCH penv)
{
    return RtlFreeHeap(RtlProcessHeap(), 0, penv);
}

__inline
BOOL
WINAPI
_Inline_SetEnvironmentVariableW(
    _In_ LPCWSTR lpName,
    _In_opt_ LPCWSTR lpValue)
{
    NTSTATUS Status = RtlSetEnvironmentVar(NULL,
                                           lpName,
                                           wcslen(lpName),
                                           lpValue,
                                           lpValue != NULL ? wcslen(lpValue) : 0);
    if (NT_SUCCESS(Status))
    {
        return TRUE;
    }

    _Inline_BaseSetLastNTError(Status);
    return FALSE;
}

__inline
VOID
WINAPI
_Inline_GetStartupInfoW(
    _Out_ LPSTARTUPINFOW lpStartupInfo)
{
    PRTL_USER_PROCESS_PARAMETERS ProcParam;
    ULONG WindowFlags;

    ProcParam = NtCurrentPeb()->ProcessParameters;
    lpStartupInfo->cb = sizeof(*lpStartupInfo);
    lpStartupInfo->lpReserved = ProcParam->ShellInfo.Buffer;
    lpStartupInfo->lpDesktop = ProcParam->DesktopInfo.Buffer;
    lpStartupInfo->lpTitle = ProcParam->WindowTitle.Buffer;
    lpStartupInfo->dwX = ProcParam->StartingX;
    lpStartupInfo->dwY = ProcParam->StartingY;
    lpStartupInfo->dwXSize = ProcParam->CountX;
    lpStartupInfo->dwYSize = ProcParam->CountY;
    lpStartupInfo->dwXCountChars = ProcParam->CountCharsX;
    lpStartupInfo->dwYCountChars = ProcParam->CountCharsY;
    lpStartupInfo->dwFillAttribute = ProcParam->FillAttribute;
    WindowFlags = ProcParam->WindowFlags;
    lpStartupInfo->dwFlags = WindowFlags;
    lpStartupInfo->wShowWindow = (WORD)ProcParam->ShowWindowFlags;
    lpStartupInfo->cbReserved2 = ProcParam->RuntimeData.Length;
    lpStartupInfo->lpReserved2 = (LPBYTE)ProcParam->RuntimeData.Buffer;

    if (WindowFlags & (STARTF_USESTDHANDLES | STARTF_USEHOTKEY | STARTF_USEMONITOR))
    {
        lpStartupInfo->hStdInput = ProcParam->StandardInput;
        lpStartupInfo->hStdOutput = ProcParam->StandardOutput;
        lpStartupInfo->hStdError = ProcParam->StandardError;
    }
}

__inline
BOOL
WINAPI
_Inline_IsDebuggerPresent(VOID)
{
    return NtCurrentPeb()->BeingDebugged;
}

#pragma endregion

#pragma region Loader

__inline
_When_(lpModuleName == NULL, _Ret_notnull_)
_When_(lpModuleName != NULL, _Ret_maybenull_)
HMODULE
WINAPI
_Inline_GetModuleHandleW(
    _In_opt_ LPCWSTR lpModuleName)
{
    NTSTATUS Status;
    UNICODE_STRING DllName;
    PVOID DllHandle;

    if (lpModuleName == NULL)
    {
        return (HMODULE)NtCurrentPeb()->ImageBaseAddress;
    }

    _Inline_RtlInitUnicodeString(&DllName, lpModuleName);
    Status = _Inline_LdrGetDllHandle(NULL, NULL, &DllName, &DllHandle);
    if (NT_SUCCESS(Status))
    {
        return (HMODULE)DllHandle;
    }

    _Inline_BaseSetLastNTError(Status);
    return NULL;
}

__inline
BOOL
WINAPI
_Inline_GetModuleHandleExW(
    _In_ DWORD dwFlags,
    _In_opt_ LPCWSTR lpModuleName,
    _Out_ HMODULE* phModule)
{
    NTSTATUS Status;
    PVOID DllHandle;
    UNICODE_STRING DllName;

    if (phModule != NULL)
    {
        *phModule = NULL;
    } else
    {
        Status = STATUS_INVALID_PARAMETER_2;
        goto _Fail;
    }
    if ((dwFlags & ~(GET_MODULE_HANDLE_EX_FLAG_PIN |
                     GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT |
                     GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)) ||
        TEST_FLAGS(dwFlags, GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) ||
        (lpModuleName == NULL && (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)))
    {
        Status = STATUS_INVALID_PARAMETER_1;
        goto _Fail;
    }

    if (lpModuleName == NULL)
    {
        *phModule = (HMODULE)NtCurrentPeb()->ImageBaseAddress;
        return TRUE;
    }
    if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
    {
        if (RtlPcToFileHeader((PVOID)lpModuleName, &DllHandle) == NULL)
        {
            Status = STATUS_DLL_NOT_FOUND;
            goto _Fail;
        }
    } else
    {
        _Inline_RtlInitUnicodeString(&DllName, lpModuleName);
        Status = _Inline_LdrGetDllHandle(NULL, NULL, &DllName, &DllHandle);
        if (!NT_SUCCESS(Status))
        {
            goto _Fail;
        }
    }
    if (!(dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT))
    {
        Status = LdrAddRefDll((dwFlags & GET_MODULE_HANDLE_EX_FLAG_PIN) ? LDR_ADDREF_DLL_PIN : 0, DllHandle);
        if (!NT_SUCCESS(Status))
        {
            goto _Fail;
        }
    }

    *phModule = (HMODULE)DllHandle;
    return TRUE;

_Fail:
    _Inline_BaseSetLastNTError(Status);
    return FALSE;
}

__inline
_Success_(return != 0)
_Ret_range_(1, nSize)
DWORD
WINAPI
_Inline_GetModuleFileNameW(
    _In_opt_ HMODULE hModule,
    _Out_writes_to_(nSize, ((return < nSize) ? (return +1) : nSize)) LPWSTR lpFilename,
    _In_ DWORD nSize)
{
    PVOID DllBase;
    PUNICODE_STRING ImageName;
    BOOLEAN LoaderLocked;
    PVOID Cookie;
    DWORD dwCch;
    PWCH Path;

    DllBase = _Inline_BasepMapModuleHandle(hModule, FALSE);
    if (hModule != NULL)
    {
        if (LDR_IS_RESOURCE(hModule))
        {
            _Inline_BaseSetLastNTError(STATUS_DLL_NOT_FOUND);
            return 0;
        }
    } else
    {
        PRTL_PERTHREAD_CURDIR PerThreadCurdir = (PRTL_PERTHREAD_CURDIR)NtReadTeb(NtTib.SubSystemTib);
        if (PerThreadCurdir != NULL && PerThreadCurdir->ImageName != NULL)
        {
            ImageName = PerThreadCurdir->ImageName;
            LoaderLocked = FALSE;
            goto _CopyPath;
        }
    }

    PLDR_DATA_TABLE_ENTRY HeadEntry, Entry;
    HeadEntry = CONTAINING_RECORD(NtCurrentPeb()->Ldr->InLoadOrderModuleList.Flink,
                                  LDR_DATA_TABLE_ENTRY,
                                  InLoadOrderLinks);
    Entry = HeadEntry;
    LdrLockLoaderLock(LDR_LOCK_LOADER_LOCK_FLAG_RAISE_ON_ERRORS, NULL, &Cookie);
    LoaderLocked = TRUE;
    do
    {
        if (DllBase == Entry->DllBase)
        {
            ImageName = &Entry->FullDllName;
            goto _CopyPath;
        }
        Entry = CONTAINING_RECORD(Entry->InLoadOrderLinks.Flink, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
    } while (Entry != HeadEntry);
    _Inline_BaseSetLastNTError(STATUS_DLL_NOT_FOUND);
    dwCch = 0;
    goto _Exit;

_CopyPath:
    Path = ImageName->Buffer;
    dwCch = ImageName->Length / sizeof(WCHAR);
    if (dwCch + 1 > nSize)
    {
        dwCch = nSize - 1;
        _Inline_RtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
    } else
    {
        _Inline_RtlSetLastWin32Error(ERROR_SUCCESS);
    }
    if (nSize != 0)
    {
        memcpy(lpFilename, Path, dwCch * sizeof(WCHAR));
        lpFilename[dwCch] = UNICODE_NULL;
        return dwCch;
    } else
    {
        dwCch = 0;
    }
_Exit:
    if (LoaderLocked)
    {
        LdrUnlockLoaderLock(LDR_UNLOCK_LOADER_LOCK_FLAG_RAISE_ON_ERRORS, Cookie);
    }
    return dwCch;
}
#pragma endregion

#pragma region QPC and Time

__inline
BOOL
WINAPI
_Inline_QueryPerformanceCounter(
    _Out_ LARGE_INTEGER* lpPerformanceCount)
{
    RtlQueryPerformanceCounter(lpPerformanceCount);
    return TRUE;
}

__inline
BOOL
WINAPI
_Inline_QueryPerformanceFrequency(
    _Out_ LARGE_INTEGER* lpFrequency)
{
    _Inline_RtlQueryPerformanceFrequency(lpFrequency);
    return TRUE;
}

__inline
VOID
WINAPI
_Inline_GetSystemTimeAsFileTime(
    _Out_ LPFILETIME lpSystemTimeAsFileTime
)
{
#if _WIN64
    *(PULONGLONG)lpSystemTimeAsFileTime = *(PULONGLONG)&SharedUserData->SystemTime;
#else
    ULONG HighPart;
    while (TRUE)
    {
        HighPart = SharedUserData->SystemTime.High1Time;
        if (SharedUserData->SystemTime.High1Time == SharedUserData->SystemTime.High2Time)
        {
            break;
        }
        YieldProcessor();
    }
    lpSystemTimeAsFileTime->dwLowDateTime = SharedUserData->SystemTime.LowPart;
    lpSystemTimeAsFileTime->dwHighDateTime = HighPart;
#endif
}

#pragma endregion

#pragma region Pointer encode / decode

__inline
_Ret_maybenull_
PVOID
WINAPI
_Inline_EncodePointer(
    _In_opt_ PVOID Ptr)
{
    return RtlEncodePointer(Ptr);
}

__inline
_Ret_maybenull_
PVOID
WINAPI
_Inline_DecodePointer(
    _In_opt_ PVOID Ptr)
{
    return RtlDecodePointer(Ptr);
}

__inline
_Ret_maybenull_
PVOID
WINAPI
_Inline_EncodeSystemPointer(
    _In_opt_ PVOID Ptr)
{
    return RtlEncodeSystemPointer(Ptr);
}

__inline
_Ret_maybenull_
PVOID
WINAPI
_Inline_DecodeSystemPointer(
    _In_opt_ PVOID Ptr)
{
    return RtlDecodeSystemPointer(Ptr);
}

__inline
HRESULT
WINAPI
_Inline_EncodeRemotePointer(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID Ptr,
    _Out_ PVOID * EncodedPtr)
{
    return RtlEncodeRemotePointer(ProcessHandle, Ptr, EncodedPtr);
}

__inline
HRESULT
WINAPI
_Inline_DecodeRemotePointer(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID Ptr,
    _Out_ PVOID * DecodedPtr)
{
    return RtlDecodeRemotePointer(ProcessHandle, Ptr, DecodedPtr);
}

#pragma endregion

#pragma region Critical Section

__inline
VOID
WINAPI
_Inline_InitializeCriticalSection(
    _Out_ LPCRITICAL_SECTION lpCriticalSection)
{
    RtlInitializeCriticalSection(lpCriticalSection);
}

__inline
_Must_inspect_result_
BOOL
WINAPI
_Inline_InitializeCriticalSectionAndSpinCount(
    _Out_ LPCRITICAL_SECTION lpCriticalSection,
    _In_ DWORD dwSpinCount)
{
#if _KNSOFT_NDK_NTDDI_MIN >= NTDDI_VISTA
    RtlInitializeCriticalSectionAndSpinCount(lpCriticalSection, dwSpinCount);
    return TRUE;
#else
    NTSTATUS Status = RtlInitializeCriticalSectionAndSpinCount(lpCriticalSection, dwSpinCount);
    if (NT_SUCCESS(Status))
    {
        return TRUE;
    }

    _Inline_BaseSetLastNTError(Status);
    return FALSE;
#endif
}

__inline
BOOL
WINAPI
_Inline_InitializeCriticalSectionEx(
    _Out_ LPCRITICAL_SECTION lpCriticalSection,
    _In_ DWORD dwSpinCount,
    _In_ DWORD Flags)
{
    NTSTATUS Status = RtlInitializeCriticalSectionEx(lpCriticalSection, dwSpinCount, Flags);
    if (NT_SUCCESS(Status))
    {
        return TRUE;
    }

    _Inline_BaseSetLastNTError(Status);
    return FALSE;
}

__inline
DWORD
WINAPI
_Inline_SetCriticalSectionSpinCount(
    _Inout_ LPCRITICAL_SECTION lpCriticalSection,
    _In_ DWORD dwSpinCount)
{
    return RtlSetCriticalSectionSpinCount(lpCriticalSection, dwSpinCount);
}

__inline
BOOL
WINAPI
_Inline_TryEnterCriticalSection(
    _Inout_ LPCRITICAL_SECTION lpCriticalSection)
{
    return RtlTryEnterCriticalSection(lpCriticalSection);
}

__inline
VOID
WINAPI
_Inline_EnterCriticalSection(
    _Inout_ LPCRITICAL_SECTION lpCriticalSection)
{
    RtlEnterCriticalSection(lpCriticalSection);
}

__inline
VOID
WINAPI
_Inline_LeaveCriticalSection(
    _Inout_ LPCRITICAL_SECTION lpCriticalSection)
{
    _Analysis_assume_lock_held_(lpCriticalSection);
    RtlLeaveCriticalSection(lpCriticalSection);
}

__inline
VOID
WINAPI
_Inline_DeleteCriticalSection(
    _Inout_ LPCRITICAL_SECTION lpCriticalSection)
{
    RtlDeleteCriticalSection(lpCriticalSection);
}

#pragma endregion

#pragma region TLS / FLS

_Must_inspect_result_
__inline
DWORD
WINAPI
_Inline_TlsAlloc(VOID)
{
    PPEB Peb;
    ULONG Index;
    PVOID* Slots;

    Peb = NtCurrentPeb();
    _Inline_RtlAcquirePebLock();
    while (TRUE)
    {
        Index = RtlFindClearBitsAndSet(Peb->TlsBitmap, 1, 0);
        if (Index != -1)
        {
            _Inline_RtlReleasePebLock();
            NtWriteTeb(TlsSlots[Index], NULL);
            return Index;
        }
        Slots = NtReadTeb(TlsExpansionSlots);
        if (Slots != NULL)
        {
            break;
        }
        _Inline_RtlReleasePebLock();
        Slots = (PVOID*)RtlAllocateHeap(RtlProcessHeap(), HEAP_ZERO_MEMORY, TLS_EXPANSION_SLOTS * sizeof(PVOID));
        if (Slots == NULL)
        {
            goto _Fail;
        }
        NtWriteTeb(TlsExpansionSlots, Slots);
        _Inline_RtlAcquirePebLock();
    }
    Index = RtlFindClearBitsAndSet(Peb->TlsExpansionBitmap, 1, 0);
    _Inline_RtlReleasePebLock();
    if (Index != -1)
    {
        Slots[Index] = NULL;
        return Index + TLS_MINIMUM_AVAILABLE;
    }

_Fail:
    _Inline_BaseSetLastNTError(STATUS_NO_MEMORY);
    return TLS_OUT_OF_INDEXES;
}

__inline
BOOL
WINAPI
_Inline_TlsFree(
    _In_ DWORD dwTlsIndex)
{
    PPEB Peb;
    ULONG Index;
    PRTL_BITMAP Bitmap;

    Peb = NtCurrentPeb();
    Index = dwTlsIndex;
    if (dwTlsIndex >= TLS_MINIMUM_AVAILABLE)
    {
        Index = dwTlsIndex - TLS_MINIMUM_AVAILABLE;
        if (Index >= TLS_EXPANSION_SLOTS)
        {
            goto _Fail;
        }
        Bitmap = Peb->TlsExpansionBitmap;
    } else
    {
        Bitmap = Peb->TlsBitmap;
    }
    _Inline_RtlAcquirePebLock();
    if (RtlAreBitsSet(Bitmap, Index, 1) &&
        NT_SUCCESS(NtSetInformationThread(NtCurrentThread(), ThreadZeroTlsCell, &dwTlsIndex, sizeof(dwTlsIndex))))
    {
        RtlClearBits(Bitmap, Index, 1);
        _Inline_RtlReleasePebLock();
        return TRUE;
    }
    _Inline_RtlReleasePebLock();

_Fail:
    _Inline_BaseSetLastNTError(STATUS_INVALID_PARAMETER);
    return FALSE;
}

__inline
BOOL
WINAPI
_Inline_TlsSetValue(
    _In_ DWORD dwTlsIndex,
    _In_opt_ LPVOID lpTlsValue)
{
    if (dwTlsIndex < TLS_MINIMUM_AVAILABLE)
    {
        NtWriteTeb(TlsSlots[dwTlsIndex], lpTlsValue);
    } else if (dwTlsIndex < TLS_MINIMUM_AVAILABLE + TLS_EXPANSION_SLOTS)
    {
        PVOID* Slots = NtReadTeb(TlsExpansionSlots);
        if (Slots == NULL)
        {
            Slots = (PVOID*)RtlAllocateHeap(RtlProcessHeap(), HEAP_ZERO_MEMORY, TLS_EXPANSION_SLOTS * sizeof(PVOID));
            if (Slots == NULL)
            {
                _Inline_BaseSetLastNTError(STATUS_NO_MEMORY);
                return FALSE;
            }
        }
        Slots[dwTlsIndex - TLS_MINIMUM_AVAILABLE] = lpTlsValue;
    } else
    {
        _Inline_BaseSetLastNTError(STATUS_INVALID_PARAMETER);
        return FALSE;
    }
    return TRUE;
}

__inline
LPVOID
WINAPI
_Inline_TlsGetValue(
    _In_ DWORD dwTlsIndex)
{
    PVOID Value;

    if (dwTlsIndex < TLS_MINIMUM_AVAILABLE)
    {
        Value = NtReadCurrentTebPVOID(FIELD_OFFSET(TEB, TlsSlots) + dwTlsIndex * sizeof(PVOID));
    } else if (dwTlsIndex < TLS_MINIMUM_AVAILABLE + TLS_EXPANSION_SLOTS)
    {
        PVOID* Slots = NtReadTeb(TlsExpansionSlots);
        Value = Slots != NULL ? Slots[dwTlsIndex - TLS_MINIMUM_AVAILABLE] : NULL;
    } else
    {
        _Inline_BaseSetLastNTError(STATUS_INVALID_PARAMETER);
        return NULL;
    }
    if (NtReadTeb(LastErrorValue) != ERROR_SUCCESS)
    {
        NtWriteTeb(LastErrorValue, ERROR_SUCCESS);
    }
    return Value;
}

__inline
LPVOID
WINAPI
_Inline_TlsGetValue2(
    _In_ DWORD dwTlsIndex)
{
    if (dwTlsIndex < TLS_MINIMUM_AVAILABLE)
    {
        return NtReadCurrentTebPVOID(FIELD_OFFSET(TEB, TlsSlots) + dwTlsIndex * sizeof(PVOID));
    }
    dwTlsIndex -= TLS_MINIMUM_AVAILABLE;
    if (dwTlsIndex < TLS_EXPANSION_SLOTS)
    {
        PVOID* Slots = NtReadTeb(TlsExpansionSlots);
        if (Slots)
        {
            return Slots[dwTlsIndex];
        }
    }

    return NULL;
}

__inline
BOOL
WINAPI
_Inline_IsThreadAFiber(VOID)
{
    return NtCurrentTeb()->HasFiberData;
}

__inline
DWORD
WINAPI
_Inline_FlsAlloc(
    _In_opt_ PFLS_CALLBACK_FUNCTION lpCallback)
{
    NTSTATUS Status;
    ULONG Index;

    Status = RtlFlsAlloc(lpCallback, &Index);
    if (!NT_SUCCESS(Status))
    {
        _Inline_BaseSetLastNTError(Status);
        return FLS_OUT_OF_INDEXES;
    }
    return Index;
}

__inline
BOOL
WINAPI
_Inline_FlsFree(
    _In_ DWORD dwFlsIndex)
{
    NTSTATUS Status = RtlFlsFree(dwFlsIndex);
    if (!NT_SUCCESS(Status))
    {
        _Inline_BaseSetLastNTError(Status);
        return FALSE;
    }
    return TRUE;
}

__inline
BOOL
WINAPI
_Inline_FlsSetValue(
    _In_ DWORD dwFlsIndex,
    _In_opt_ PVOID lpFlsData)
{
    NTSTATUS Status;

    Status = RtlFlsSetValue(dwFlsIndex, lpFlsData);
    if (NT_SUCCESS(Status))
    {
        return TRUE;
    };
    _Inline_BaseSetLastNTError(Status);
    return FALSE;
}

__inline
PVOID
WINAPI
_Inline_FlsGetValue(
    _In_ DWORD dwFlsIndex)
{
    NTSTATUS Status;
    PVOID Value;

    Status = RtlFlsGetValue(dwFlsIndex, &Value);
    if (NT_SUCCESS(Status))
    {
        NtWriteTeb(LastErrorValue, ERROR_SUCCESS);
        return Value;
    }
    if (Status == STATUS_MEMORY_NOT_ALLOCATED)
    {
        Status = STATUS_INVALID_PARAMETER;
    }
    _Inline_BaseSetLastNTError(Status);
    return NULL;
}

__inline
PVOID
WINAPI
_Inline_FlsGetValue2(
    _In_ DWORD dwFlsIndex)
{
    return RtlFlsGetValue2(dwFlsIndex);
}

#pragma endregion

#pragma region Heap

__inline
HANDLE
WINAPI
_Inline_GetProcessHeap(VOID)
{
    return RtlProcessHeap();
}

__inline
_Ret_maybenull_
_Post_writable_byte_size_(dwBytes)
DECLSPEC_ALLOCATOR
LPVOID
WINAPI
_Inline_HeapAlloc(
    _In_ HANDLE hHeap,
    _In_ DWORD dwFlags,
    _In_ SIZE_T dwBytes)
{
    return RtlAllocateHeap(hHeap, dwFlags, dwBytes);
}

__inline
_Success_(return != 0)
_Ret_maybenull_
_Post_writable_byte_size_(dwBytes)
DECLSPEC_ALLOCATOR
LPVOID
WINAPI
_Inline_HeapReAlloc(
    _Inout_ HANDLE hHeap,
    _In_ DWORD dwFlags,
    _Frees_ptr_opt_ LPVOID lpMem,
    _In_ SIZE_T dwBytes)
{
    return RtlReAllocateHeap(hHeap, dwFlags, lpMem, dwBytes);
}

__inline
_Success_(return != FALSE)
BOOL
WINAPI
_Inline_HeapFree(
    _Inout_ HANDLE hHeap,
    _In_ DWORD dwFlags,
    __drv_freesMem(Mem) _Frees_ptr_opt_ LPVOID lpMem)
{
    return RtlFreeHeap(hHeap, dwFlags, lpMem);
}

__inline
SIZE_T
WINAPI
_Inline_HeapSize(
    _In_ HANDLE hHeap,
    _In_ DWORD dwFlags,
    _In_ LPCVOID lpMem)
{
    return RtlSizeHeap(hHeap, dwFlags, (PVOID)lpMem);
}

__inline
BOOL
WINAPI
_Inline_HeapQueryInformation(
    _In_opt_ HANDLE HeapHandle,
    _In_ HEAP_INFORMATION_CLASS HeapInformationClass,
    _Out_writes_bytes_to_opt_(HeapInformationLength, *ReturnLength) PVOID HeapInformation,
    _In_ SIZE_T HeapInformationLength,
    _Out_opt_ PSIZE_T ReturnLength)
{
    NTSTATUS Status = RtlQueryHeapInformation(HeapHandle,
                                              HeapInformationClass,
                                              HeapInformation,
                                              HeapInformationLength,
                                              ReturnLength);
    if (!NT_SUCCESS(Status))
    {
        _Inline_BaseSetLastNTError(Status);
        return FALSE;
    }
    return TRUE;
}

__inline
BOOL
WINAPI
_Inline_HeapValidate(
    _In_ HANDLE hHeap,
    _In_ DWORD dwFlags,
    _In_opt_ LPCVOID lpMem)
{
    return RtlValidateHeap(hHeap, dwFlags, (PVOID)lpMem);
}

__inline
SIZE_T
WINAPI
_Inline_HeapCompact(
    _In_ HANDLE hHeap,
    _In_ DWORD dwFlags)
{
    return RtlCompactHeap(hHeap, dwFlags);
}

#pragma endregion

#pragma region Console

__inline
HANDLE
WINAPI
_Inline_GetStdHandle(
    _In_ DWORD nStdHandle)
{
    HANDLE StdHandle;

    if (nStdHandle == STD_INPUT_HANDLE)
    {
        if (NtCurrentPeb()->ProcessParameters->WindowFlags & STARTF_USEHOTKEY)
        {
            return NULL;
        }
        StdHandle = NtCurrentPeb()->ProcessParameters->StandardInput;
    } else if (nStdHandle == STD_OUTPUT_HANDLE)
    {
        if (NtCurrentPeb()->ProcessParameters->WindowFlags & STARTF_USEMONITOR)
        {
            return NULL;
        }
        StdHandle = NtCurrentPeb()->ProcessParameters->StandardOutput;
    } else if (nStdHandle == STD_ERROR_HANDLE)
    {
        StdHandle = NtCurrentPeb()->ProcessParameters->StandardError;
    } else
    {
        StdHandle = INVALID_HANDLE_VALUE;
    }
    if (StdHandle == INVALID_HANDLE_VALUE)
    {
        _Inline_BaseSetLastNTError(STATUS_INVALID_HANDLE);
    }
    return StdHandle;
}

__inline
BOOL
WINAPI
_Inline_SetStdHandle(
    _In_ DWORD nStdHandle,
    _In_ HANDLE hHandle)
{
    if (nStdHandle == STD_INPUT_HANDLE)
    {
        NtCurrentPeb()->ProcessParameters->StandardInput = hHandle;
    } else if (nStdHandle == STD_OUTPUT_HANDLE)
    {
        NtCurrentPeb()->ProcessParameters->StandardOutput = hHandle;
    } else if (nStdHandle == STD_ERROR_HANDLE)
    {
        NtCurrentPeb()->ProcessParameters->StandardError = hHandle;
    } else
    {
        _Inline_BaseSetLastNTError(STATUS_INVALID_HANDLE);
        return FALSE;
    }

    return TRUE;
}

__inline
BOOL
WINAPI
_Inline_SetStdHandleEx(
    _In_ DWORD nStdHandle,
    _In_ HANDLE hHandle,
    _Out_opt_ PHANDLE phPrevValue)
{
    PHANDLE HandlePtr;

    if (phPrevValue != NULL)
    {
        *phPrevValue = NULL;
    }

    if (nStdHandle == STD_INPUT_HANDLE)
    {
        HandlePtr = &NtCurrentPeb()->ProcessParameters->StandardInput;
    } else if (nStdHandle == STD_OUTPUT_HANDLE)
    {
        HandlePtr = &NtCurrentPeb()->ProcessParameters->StandardOutput;
    } else if (nStdHandle == STD_ERROR_HANDLE)
    {
        HandlePtr = &NtCurrentPeb()->ProcessParameters->StandardError;
    } else
    {
        _Inline_BaseSetLastNTError(STATUS_INVALID_HANDLE);
        return FALSE;
    }

    if (phPrevValue != NULL)
    {
        *phPrevValue = *HandlePtr;
    }
    *HandlePtr = hHandle;
    return TRUE;
}

#pragma endregion

#pragma region I/O

__inline
BOOL
WINAPI
_Inline_WriteFile(
    _In_ HANDLE hFile,
    _In_reads_bytes_opt_(nNumberOfBytesToWrite) LPCVOID lpBuffer,
    _In_ DWORD nNumberOfBytesToWrite,
    _Out_opt_ LPDWORD lpNumberOfBytesWritten,
    _Inout_opt_ LPOVERLAPPED lpOverlapped)
{
    NTSTATUS Status;
    PVOID ApcContext;
    IO_STATUS_BLOCK IoStatusBlock = { 0 };
    DWORD StdHandle = (DWORD)(DWORD_PTR)hFile;

    if (lpNumberOfBytesWritten != NULL)
    {
        *lpNumberOfBytesWritten = 0;
    }
    if (StdHandle >= STD_ERROR_HANDLE)
    {
        if (StdHandle == STD_ERROR_HANDLE)
        {
            hFile = NtCurrentPeb()->ProcessParameters->StandardError;
        } else if (StdHandle == STD_OUTPUT_HANDLE)
        {
            hFile = NtCurrentPeb()->ProcessParameters->StandardOutput;
        } else if (StdHandle == STD_INPUT_HANDLE)
        {
            hFile = NtCurrentPeb()->ProcessParameters->StandardInput;
        }
    }

    if (lpOverlapped == NULL)
    {
        Status = NtWriteFile(hFile,
                             NULL,
                             NULL,
                             NULL,
                             &IoStatusBlock,
                             (PVOID)lpBuffer,
                             nNumberOfBytesToWrite,
                             NULL,
                             NULL);
        if (Status == STATUS_PENDING)
        {
            Status = NtWaitForSingleObject(hFile, FALSE, NULL);
            if (NT_SUCCESS(Status))
            {
                Status = IoStatusBlock.Status;
            }
        }
        if (NT_SUCCESS(Status))
        {
            if (lpNumberOfBytesWritten != NULL)
            {
                *lpNumberOfBytesWritten = (ULONG)IoStatusBlock.Information;
            }
            return TRUE;
        }
        if (NT_WARNING(Status) && lpNumberOfBytesWritten != NULL)
        {
            *lpNumberOfBytesWritten = (ULONG)IoStatusBlock.Information;
        }
    } else
    {
        lpOverlapped->Internal = STATUS_PENDING;
        ApcContext = (ULONG_PTR)lpOverlapped->hEvent & 1 ? NULL : lpOverlapped;

        /* False positive warnings, hFile and lpBuffer are assumed not NULL */
#pragma warning(disable: __WARNING_INVALID_PARAM_VALUE_1 __WARNING_INVALID_PARAM_VALUE_3)
        Status = NtWriteFile(hFile,
                             lpOverlapped->hEvent,
                             NULL,
                             ApcContext,
                             (PIO_STATUS_BLOCK)lpOverlapped,
                             (PVOID)lpBuffer,
                             nNumberOfBytesToWrite,
                             (PLARGE_INTEGER)&lpOverlapped->Offset,
                             NULL);
#pragma warning(disable: __WARNING_INVALID_PARAM_VALUE_1 __WARNING_INVALID_PARAM_VALUE_3)

        if (Status != STATUS_PENDING && !NT_ERROR(Status))
        {
            if (lpNumberOfBytesWritten != NULL)
            {
                *lpNumberOfBytesWritten = (ULONG)lpOverlapped->InternalHigh;
            }
            return TRUE;
        }
    }

    _Inline_BaseSetLastNTError(Status);
    return FALSE;
}

__inline
BOOL
WINAPI
_Inline_FlushFileBuffers(
    _In_ HANDLE hFile)
{
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;
    DWORD StdHandle = (DWORD)(DWORD_PTR)hFile;

    if (StdHandle == STD_ERROR_HANDLE)
    {
        hFile = NtCurrentPeb()->ProcessParameters->StandardError;
    } else if (StdHandle == STD_OUTPUT_HANDLE)
    {
        hFile = NtCurrentPeb()->ProcessParameters->StandardOutput;
    } else if (StdHandle == STD_INPUT_HANDLE)
    {
        hFile = NtCurrentPeb()->ProcessParameters->StandardInput;
    }

    Status = NtFlushBuffersFile(hFile, &IoStatusBlock);
    if (NT_SUCCESS(Status))
    {
        return TRUE;
    }
    _Inline_BaseSetLastNTError(Status);
    return FALSE;
}

/*
 * A successful path through the function doesn't set the _Out_ annotated parameter.
 * The original SAL annotation in Windows SDK has no _Success_ expression.
 */
#pragma warning(disable: 6101)
__inline
BOOL
WINAPI
_Inline_SetFilePointerEx(
    _In_ HANDLE hFile,
    _In_ LARGE_INTEGER liDistanceToMove,
    _Out_opt_ PLARGE_INTEGER lpNewFilePointer,
    _In_ DWORD dwMoveMethod)
{
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_POSITION_INFORMATION FilePosition;
    FILE_STANDARD_INFORMATION FileStandard;

    if (dwMoveMethod == FILE_BEGIN)
    {
        FilePosition.CurrentByteOffset.QuadPart = liDistanceToMove.QuadPart;
    } else if (dwMoveMethod == FILE_CURRENT)
    {
        Status = NtQueryInformationFile(hFile,
                                        &IoStatusBlock,
                                        &FilePosition,
                                        sizeof(FilePosition),
                                        FilePositionInformation);
        if (!NT_SUCCESS(Status))
        {
            _Inline_BaseSetLastNTError(Status);
            return FALSE;
        }
        FilePosition.CurrentByteOffset.QuadPart += liDistanceToMove.QuadPart;
    } else if (dwMoveMethod == FILE_END)
    {
        Status = NtQueryInformationFile(hFile,
                                        &IoStatusBlock,
                                        &FileStandard,
                                        sizeof(FileStandard),
                                        FileStandardInformation);
        if (!NT_SUCCESS(Status))
        {
            _Inline_BaseSetLastNTError(Status);
            return FALSE;
        }
        FilePosition.CurrentByteOffset.QuadPart = FileStandard.EndOfFile.QuadPart + liDistanceToMove.QuadPart;
    } else
    {
        _Inline_RtlSetLastWin32Error(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if (FilePosition.CurrentByteOffset.QuadPart < 0)
    {
        _Inline_RtlSetLastWin32Error(ERROR_NEGATIVE_SEEK);
        return FALSE;
    }
    Status = NtSetInformationFile(hFile,
                                  &IoStatusBlock,
                                  &FilePosition,
                                  sizeof(FilePosition),
                                  FilePositionInformation);
    if (!NT_SUCCESS(Status))
    {
        _Inline_BaseSetLastNTError(Status);
        return FALSE;
    }
    if (lpNewFilePointer != NULL)
    {
        *lpNewFilePointer = FilePosition.CurrentByteOffset;
    }

    return TRUE;
}
#pragma warning(default: 6101)

__inline
BOOL
WINAPI
_Inline_CloseHandle(
    _In_ _Post_ptr_invalid_ HANDLE hObject)
{
    NTSTATUS Status;
    DWORD StdHandle;
    HANDLE PrevStdHandle;

    /* Handle standard I/O handles */
    StdHandle = (DWORD)(DWORD_PTR)hObject;
    if (StdHandle >= STD_ERROR_HANDLE && StdHandle <= STD_INPUT_HANDLE)
    {
        /* SAL marked input handle cannot be NULL, but we need to do that for clearing standard handle */
#pragma warning(disable: __WARNING_INVALID_PARAM_VALUE_1)
        if (_Inline_SetStdHandleEx(StdHandle, NULL, &PrevStdHandle))
        {
            hObject = PrevStdHandle;
        }
#pragma warning(default: __WARNING_INVALID_PARAM_VALUE_1)
    }

    // FIXME: SbExecuteProcedure...

    /* hObject seems can be NULL when reach here... */
#pragma warning(disable: __WARNING_INVALID_PARAM_VALUE_3)
    Status = NtClose(hObject);
#pragma warning(default: __WARNING_INVALID_PARAM_VALUE_3)
    if (NT_SUCCESS(Status))
    {
        return TRUE;
    }
    _Inline_BaseSetLastNTError(Status);
    return FALSE;
}

#pragma endregion

#pragma region AVX

__inline
DWORD64
WINAPI
_Inline_GetEnabledXStateFeatures(VOID)
{
    ULONG64 u = _Inline_RtlGetEnabledExtendedFeatures(MAXULONGLONG);
    if (u)
    {
        return u;
    }

    /*
     * Geoff Chappell:
     * The PF_XMMI_INSTRUCTIONS_AVAILABLE feature is necessarily TRUE in x86 version 6.2 and higher,
     * and in all x64 versions.
     */
#if defined(_WIN64)
    return XSTATE_MASK_LEGACY;
#else
    return SharedUserData->ProcessorFeatures[PF_XMMI_INSTRUCTIONS_AVAILABLE] ?
        XSTATE_MASK_LEGACY :
        XSTATE_MASK_LEGACY_FLOATING_POINT;
#endif
}

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)
__inline
DWORD64
WINAPI
_Inline_GetThreadEnabledXStateFeatures(VOID)
{
    return _Inline_GetEnabledXStateFeatures() &
#if defined(_WIN64)
        NtReadTeb(ExtendedFeatureDisableMask)
#else
        ~XSTATE_MASK_AMX_TILE_DATA
#endif
        ;
}
#endif

#pragma endregion

__inline
_Check_return_
_Post_equals_last_error_
DWORD
WINAPI
_Inline_GetLastError(VOID)
{
    return _Inline_RtlGetLastWin32Error();
}

__inline
VOID
WINAPI
_Inline_SetLastError(
    _In_ DWORD dwErrCode)
{
    _Inline_RtlSetLastWin32Error(dwErrCode);
}

__inline
LCID
WINAPI
_Inline_GetThreadLocale(void)
{
    return NtReadTeb(CurrentLocale);
}

__inline
BOOL
WINAPI
_Inline_IsProcessorFeaturePresent(
    _In_ DWORD ProcessorFeature)
{
    return _Inline_RtlIsProcessorFeaturePresent(ProcessorFeature);
}

__inline
DECLSPEC_NORETURN
VOID
WINAPI
_Inline_ExitProcess(
    _In_ UINT uExitCode)
{
    RtlExitUserProcess(uExitCode);
}

__inline
BOOL
WINAPI
_Inline_TerminateProcess(
    _In_ HANDLE hProcess,
    _In_ UINT uExitCode)
{
    NTSTATUS Status;

    if (hProcess != NULL)
    {
        RtlReportSilentProcessExit(hProcess, uExitCode);
        Status = NtTerminateProcess(hProcess, uExitCode);
        if (NT_SUCCESS(Status))
        {
            return TRUE;
        }
        _Inline_BaseSetLastNTError(Status);
    } else
    {
        _Inline_RtlSetLastWin32Error(ERROR_INVALID_HANDLE);
    }
    return FALSE;
}

/* SList */

__inline
VOID
WINAPI
_Inline_InitializeSListHead(
    _Out_ PSLIST_HEADER ListHead)
{
    RtlInitializeSListHead(ListHead);
}

__inline
PSLIST_ENTRY
WINAPI
_Inline_InterlockedFlushSList(
    _Inout_ PSLIST_HEADER ListHead)
{
    return RtlInterlockedFlushSList(ListHead);
}

__inline
PSLIST_ENTRY
WINAPI
_Inline_InterlockedPopEntrySList(
    _Inout_ PSLIST_HEADER ListHead)
{
    return RtlInterlockedPopEntrySList(ListHead);
}

__inline
PSLIST_ENTRY
WINAPI
_Inline_InterlockedPushEntrySList(
    _Inout_ PSLIST_HEADER ListHead,
    _Inout_ __drv_aliasesMem PSLIST_ENTRY ListEntry)
{
    return RtlInterlockedPushEntrySList(ListHead, ListEntry);
}

__inline
PSLIST_ENTRY
WINAPI
_Inline_InterlockedPushListSList(
    _Inout_ PSLIST_HEADER ListHead,
    _Inout_ PSLIST_ENTRY List,
    _Inout_ PSLIST_ENTRY ListEnd,
    _In_ ULONG Count)
{
    return RtlInterlockedPushListSList(ListHead, List, ListEnd, Count);
}

__inline
PSLIST_ENTRY
WINAPI
_Inline_InterlockedPushListSListEx(
    _Inout_ PSLIST_HEADER ListHead,
    _Inout_ PSLIST_ENTRY List,
    _Inout_ PSLIST_ENTRY ListEnd,
    _In_ ULONG Count)
{
    return
#if _KNSOFT_NDK_NTDDI_MIN >= NTDDI_WIN8
        RtlInterlockedPushListSListEx
#else
        RtlInterlockedPushListSList
#endif
        (ListHead, List, ListEnd, Count);
}

__inline
USHORT
WINAPI
_Inline_QueryDepthSList(
    _In_ PSLIST_HEADER ListHead)
{
    return RtlQueryDepthSList(ListHead);
}

EXTERN_C_END
