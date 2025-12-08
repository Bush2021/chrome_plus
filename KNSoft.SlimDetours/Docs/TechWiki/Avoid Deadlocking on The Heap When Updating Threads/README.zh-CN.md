| [English (en-US)](./README.md) | **简体中文 (zh-CN)** |
| --- | --- |

&nbsp;

# 更新线程时避免堆死锁

## 为什么Detours更新线程时可能死锁？

原版[Detours](https://github.com/microsoft/Detours)使用了CRT堆（通过`new/delete`），更新线程时如果挂起了另一个也使用此堆且正持有堆锁的线程，[Detours](https://github.com/microsoft/Detours)再访问此堆就会发生死锁。

[Raymond Chen](https://devblogs.microsoft.com/oldnewthing/author/oldnewthing)在[博客“The Old New Thing”](https://devblogs.microsoft.com/oldnewthing/)的文章[《Are there alternatives to _lock and _unlock in Visual Studio 2015?》](https://devblogs.microsoft.com/oldnewthing/20170125-00/?p=95255)中详细讨论的挂起线程时出现CRT堆死锁问题正是同一个场景，也提到了[Detours](https://github.com/microsoft/Detours)，这里引用其原文不再赘述：
> Furthermore, you would be best served to take the heap lock (Heap­Lock) before suspending the thread, because the Detours library will allocate memory during thread suspension.  
> 此外，最好在挂起线程前占有堆锁（Heap­Lock），因为Detours库将在线程挂起期间分配内存。

## Detours死锁的演示

[SlimDetours](https://github.com/KNSoft/KNSoft.SlimDetours)提供了[示例：DeadLock](../../../Source/Demo/DeadLock.c)演示[Detours](https://github.com/microsoft/Detours)死锁的发生与在[SlimDetours](https://github.com/KNSoft/KNSoft.SlimDetours)中得到解决。

其中一个线程（`HeapUserThread`）不断调用`malloc/free`（等效于`new/delete`）：
```C
 while (!g_bStop)
 {
     p = malloc(4);
     if (p != NULL)
     {
         free(p);
     }
 }
```

另一个线程（`SetHookThread`）不断使用[Detours](https://github.com/microsoft/Detours)或[SlimDetours](https://github.com/KNSoft/KNSoft.SlimDetours)挂钩和脱钩：
```C
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
```
> [!NOTE]
> [SlimDetours](https://github.com/KNSoft/KNSoft.SlimDetours)会自动更新线程（参考[🔗 技术Wiki：应用内联钩子时自动更新线程](https://github.com/KNSoft/KNSoft.SlimDetours/blob/main/Docs/TechWiki/Update%20Threads%20Automatically%20When%20Applying%20Inline%20Hooks/README.zh-CN.md)），所以不存在[`DetourUpdateThread`](https://github.com/microsoft/Detours/wiki/DetourUpdateThread)这样的函数。

同时执行这2个线程10秒，然后发送停止信号（`g_bStop = TRUE;`）后再次等待10秒，如果超时则大概率发生死锁，将触发断点，可以在调试器中观察这2个线程的调用栈进行确认。例如指定使用[Detours](https://github.com/microsoft/Detours)运行此示例`"Demo.exe -Run DeadLock -Engine=MSDetours"`，以下调用栈可见堆死锁：
```C
Worker Thread	Demo.exe!HeapUserThread	Demo.exe!heap_alloc_dbg_internal
    [External Code]
    Demo.exe!heap_alloc_dbg_internal(const unsigned __int64 size, const int block_use, const char * const file_name, const int line_number) Line 359
    Demo.exe!heap_alloc_dbg(const unsigned __int64 size, const int block_use, const char * const file_name, const int line_number) Line 450
    Demo.exe!_malloc_dbg(unsigned __int64 size, int block_use, const char * file_name, int line_number) Line 496
    Demo.exe!malloc(unsigned __int64 size) Line 27
    Demo.exe!HeapUserThread(void * lpThreadParameter) Line 29
    [External Code]

Worker Thread	Demo.exe!SetHookThread	Demo.exe!__acrt_lock
    [External Code]
    Demo.exe!__acrt_lock(__acrt_lock_id _Lock) Line 55
    Demo.exe!heap_alloc_dbg_internal(const unsigned __int64 size, const int block_use, const char * const file_name, const int line_number) Line 309
    Demo.exe!heap_alloc_dbg(const unsigned __int64 size, const int block_use, const char * const file_name, const int line_number) Line 450
    Demo.exe!_malloc_dbg(unsigned __int64 size, int block_use, const char * file_name, int line_number) Line 496
    Demo.exe!malloc(unsigned __int64 size) Line 27
    [External Code]
    Demo.exe!DetourDetach(void * * ppPointer, void * pDetour) Line 2392
    Demo.exe!HookAttach(_DEMO_ENGINE_TYPE EngineType, int Enable, void * * ppPointer, void * pDetour) Line 140
    Demo.exe!SetHookThread(void * lpThreadParameter) Line 65
    [External Code]
```
使用[SlimDetours](https://github.com/KNSoft/KNSoft.SlimDetours)运行此示例`"Demo.exe -Run DeadLock -Engine=SlimDetours"`则能顺利通过。

## 其它挂钩库如何避免这个问题？

[mhook](https://github.com/martona/mhook)使用[`Virtual­Alloc`](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc)分配内存页代替[`Heap­Alloc`](https://learn.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heapalloc)分配堆内存，是上文末尾提到的一个解决方案。

[MinHook](https://github.com/TsudaKageyu/minhook)与[SlimDetours](https://github.com/KNSoft/KNSoft.SlimDetours)都新创建了一个私有堆供内部使用，避免此问题的同时也节约了内存使用：
```C
_detour_memory_heap = RtlCreateHeap(HEAP_NO_SERIALIZE | HEAP_GROWABLE, NULL, 0, 0, NULL, NULL);
```
> [!NOTE]
> [Detours](https://github.com/microsoft/Detours)已有事务机制，所以此堆无需序列化访问。

[MinHook](https://github.com/TsudaKageyu/minhook)在其初始化函数`MH_Initialize`中创建，而[SlimDetours](https://github.com/KNSoft/KNSoft.SlimDetours)在首个被调用的内存分配函数中进行一次初始化时创建，故没有也不需要单独的初始化函数。

<br>
<hr>

本作品采用 [知识共享署名-非商业性使用-相同方式共享 4.0 国际许可协议 (CC BY-NC-SA 4.0)](http://creativecommons.org/licenses/by-nc-sa/4.0/) 进行许可。  
<br>
**[Ratin](https://github.com/RatinCN) &lt;[<ratin@knsoft.org>](mailto:ratin@knsoft.org)&gt;**  
*中国国家认证系统架构设计师*  
*[ReactOS](https://github.com/reactos/reactos)贡献者*
