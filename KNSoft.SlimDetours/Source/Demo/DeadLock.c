/*
 * This demo shows the original Microsoft Detours may run into dead lock when update thread that operating CRT heap,
 * KNSoft.SlimDetours has fixed this problem by using a private heap.
 * 
 * Run "Demo.exe -Run DeadLock -Engine=MSDetours" in debugger,
 *   most probably breakpoint will be hitted and deadlock can be found in call stacks.
 * Run "Demo.exe -Run DeadLock -Engine=SlimDetours" will pass this test.
 * 
 * See also https://github.com/KNSoft/KNSoft.SlimDetours/tree/main/Docs/TechWiki/Avoid%20Deadlocking%20on%20The%20Heap%20When%20Updating%20Threads
 */

#include "Demo.h"

#define DEMO_WAIT_TIMEOUT 10000

static BOOL g_bStop = FALSE;
static DEMO_ENGINE_TYPE g_eEngineType = EngineInvalid;

/* This thread repeat malloc/free */
static
DWORD
WINAPI
HeapUserThread(
    LPVOID lpThreadParameter)
{
    PVOID p;
    DWORD ThreadId = GetCurrentThreadId();

    while (!g_bStop)
    {
        p = malloc(4);
        if (p != NULL)
        {
            free(p);
        }
    }

    UnitTest_FormatMessage("Heap user thread (%lu) exit\n", GetCurrentThreadId());
    return 0;
}

/* This thread repeat hook/unhook */
static
DWORD
WINAPI
SetHookThread(
    LPVOID lpThreadParameter)
{
    HRESULT hr = S_OK;
    BOOL EnableHook = TRUE;

    while (!g_bStop)
    {
        hr = HookTransactionBegin(g_eEngineType);
        if (FAILED(hr))
        {
            break;
        }
        if (g_eEngineType == EngineMicrosoftDetours)
        {
            hr = HRESULT_FROM_WIN32(DetourUpdateThread((HANDLE)lpThreadParameter));
            if (FAILED(hr))
            {
                break;
            }
        }
        hr = HookAttach(g_eEngineType, EnableHook, (PVOID*)&g_pfnEqualRect, Hooked_EqualRect);
        if (FAILED(hr))
        {
            HookTransactionAbort(g_eEngineType);
            break;
        }
        hr = HookTransactionCommit(g_eEngineType);
        if (FAILED(hr))
        {
            break;
        }

        EnableHook = !EnableHook;
    }

    UnitTest_FormatMessage("Set hook thread (%lu) exit with 0x%08lX\n", GetCurrentThreadId(), hr);
    return hr;
}

TEST_FUNC(DeadLock)
{
    NTSTATUS Status;
    HANDLE hThreads[2];
    DWORD dwRet;

    if (FAILED(GetEngineTypeFromArgs(TEST_PARAMETER_ARGC, TEST_PARAMETER_ARGV, &g_eEngineType)))
    {
        TEST_SKIP("Invalid engine type\n");
        return;
    }
    Status = LoadEqualRect();
    if (!NT_SUCCESS(Status))
    {
        TEST_SKIP("Load user32.dll!EqualRect failed with 0x%08lX\n", Status);
        return;
    }

    /* Create two threads, one thread calls malloc/free, another one calls detours */
    hThreads[0] = CreateThread(NULL, 0, HeapUserThread, NULL, CREATE_SUSPENDED, NULL);
    if (hThreads[0] == NULL)
    {
        TEST_SKIP("CreateThread failed with 0x%08lX\n", GetLastError());
        return;
    }
    hThreads[1] = CreateThread(NULL, 0, SetHookThread, (PVOID)hThreads[0], CREATE_SUSPENDED, NULL);
    if (hThreads[1] == NULL)
    {
        TEST_SKIP("CreateThread failed with 0x%08lX\n", GetLastError());
        g_bStop = TRUE;
        ResumeThread(hThreads[0]);
        WaitForSingleObject(hThreads[0], INFINITE);
        CloseHandle(hThreads[0]);
        return;
    }

    /* Run them together and wait */
    UnitTest_FormatMessage("Run threads and wait (up to %lums)...\n", DEMO_WAIT_TIMEOUT * 2);
    ResumeThread(hThreads[0]);
    ResumeThread(hThreads[1]);
    Sleep(DEMO_WAIT_TIMEOUT);
    g_bStop = TRUE;
    dwRet = WaitForMultipleObjects(ARRAYSIZE(hThreads), hThreads, TRUE, DEMO_WAIT_TIMEOUT);
    CloseHandle(hThreads[0]);
    CloseHandle(hThreads[1]);

    if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_OBJECT_0 + 1)
    {
        TEST_RESULT(Pass);
        return;
    } else if (dwRet == WAIT_TIMEOUT)
    {
        /* Deadlock very likely occurred! Break in debugger, check above two threads' call stack */
        if (IsDebuggerPresent())
        {
            __debugbreak();
        }
        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    } else if (dwRet == WAIT_FAILED)
    {
        dwRet = GetLastError();
    }
    TEST_SKIP("WaitForMultipleObjects failed with 0x%08lX\n", dwRet);
}
