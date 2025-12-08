| **English (en-US)** | [简体中文 (zh-CN)](./README.zh-CN.md) |
| --- | --- |

&nbsp;

# Update Threads Automatically When Applying Inline Hooks

## The necessity of updating threads when applying inline hooks

Inline hook has to modify the instructions at the beginning of the function to implement the jump, in order to cope with the possibility that a thread is running on the instruction to be modified, it's necessary to update the thread in this state to avoid executing an illegal combination of old and new instructions.

## Implementations on other hooking libraries

### Detours

[Detours](https://github.com/microsoft/Detours) provides [`DetourUpdateThread`](https://github.com/microsoft/Detours/wiki/DetourUpdateThread) function to update threads, but it needs the caller pass the handle of the thread that needs to be updated:
```C
LONG WINAPI DetourUpdateThread(_In_ HANDLE hThread);
```
In other words, the caller needs to traverse all threads in the process except itself and pass them to this function, which is complicated and inconvenient to use.

[Detours](https://github.com/microsoft/Detours) updates threads very precisely, it accurately adjusts the PC (Program Counter) in the thread context to the correct position by using [`GetThreadContext`](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getthreadcontext) and [`SetThreadContext`](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreadcontext), see [Detours/src/detours.cpp at 4b8c659f · microsoft/Detours](https://github.com/microsoft/Detours/blob/4b8c659f549b0ab21cf649377c7a84eb708f5e68/src/detours.cpp#L1840-L1906) for implementation.

But [Detours](https://github.com/microsoft/Detours) still has inadequacy in thread updating under x64, see also [PR #344: Improve thread program counter adjustment](https://github.com/microsoft/Detours/pull/344) submitted by me for this.

> [!TIP]
> While its official example "[Using Detours](https://github.com/microsoft/Detours/wiki/Using-Detours)" has code like `DetourUpdateThread(GetCurrentThread())`, such usage is pointless and invalid, and should be used to update all threads in the process except the current thread, see also: [`DetourUpdateThread`](https://github.com/microsoft/Detours/wiki/DetourUpdateThread). But even updating threads in the right way, it also brings a new risk, see [🔗 TechWiki: Avoid Deadlocking on The Heap When Updating Threads](https://github.com/KNSoft/KNSoft.SlimDetours/blob/main/Docs/TechWiki/Avoid%20Deadlocking%20on%20The%20Heap%20When%20Updating%20Threads/README.md).

### MinHook

[MinHook](https://github.com/TsudaKageyu/minhook) does a better job, it calls [CreateToolhelp32Snapshot](https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-createtoolhelp32snapshot) to obtain other threads and updates them automatically when set (or unset) hooks, then adjusts PC (Program Counter) in the thread context as accurately as [Detours](https://github.com/microsoft/Detours).

### mhook

[mhook](https://github.com/martona/mhook) calls [NtQuerySystemInformation](https://learn.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-ntquerysysteminformation) to obtain other threads and updates them automatically when set (or unset) hooks. But the way to update threads is more hacky, wait 100ms if the thread is exactly in the area where the instruction is about to be modified, try up to 3 times, see [mhook/mhook-lib/mhook.cpp at e58a58ca · martona/mhook](https://github.com/martona/mhook/blob/e58a58ca31dbe14f202b9b26315bff9f7a32598c/mhook-lib/mhook.cpp#L557-L631) for implementation:
```C
while (GetThreadContext(hThread, &ctx))
{
    ...
    if (nTries < 3)
    {
        // oops - we should try to get the instruction pointer out of here. 
        ODPRINTF((L"mhooks: SuspendOneThread: suspended thread %d - IP is at %p - IS COLLIDING WITH CODE", dwThreadId, pIp));
        ResumeThread(hThread);
        Sleep(100);
        SuspendThread(hThread);
        nTries++;
    }
    ...
}
```

## SlimDetours' implementation

[SlimDetours](https://github.com/KNSoft/KNSoft.SlimDetours) has two methods to obtain other threads, calls [NtQuerySystemInformation](https://learn.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-ntquerysysteminformation) when targeting NT5, but use `NtGetNextThread` when targeting NT6+ (default) to significantly improve performance and correctness guarantees.

Threads updating follows the [Detours](https://github.com/microsoft/Detours) with some fixes and improvements.

Key points:
1. Call `NtGetNextThread` to enumerate all threads of the current process
2. Call `NtSuspendThread` to suspend all threads except the current thread
3. Modify the instruction to implement the inline hook
4. Update the threads that were successfully suspended
5. Call `NtResumeThread` to resume the suspended threads

See [KNSoft.SlimDetours/Source/KNSoft.SlimDetours/Thread.c at main · KNSoft/KNSoft.SlimDetours](../../../Source/KNSoft.SlimDetours/Thread.c) for the full implementation.

<br>
<hr>

This work is licensed under [Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)](http://creativecommons.org/licenses/by-nc-sa/4.0/).  
<br>
**[Ratin](https://github.com/RatinCN) &lt;[<ratin@knsoft.org>](mailto:ratin@knsoft.org)&gt;**  
*China national certified senior system architect*  
*[ReactOS](https://github.com/reactos/reactos) contributor*
