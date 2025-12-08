| **English (en-US)** | [简体中文 (zh-CN)](./README.zh-CN.md) |
| --- | --- |

&nbsp;

# Implement Delay Hook

## What's "delay hook" and the benefits it brings?

The usual way to hook a function in a DLL has to load the corresponding DLL into the process space and locate its address (for example, use [`LoadLibraryW`](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibraryw) + [`GetProcAddress`](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress)) at first.

For the hooks designed for a specific program, usually theirs target function will be called sooner or later, and the DLL is also required by the process, so loading the corresponding DLL early is fair enough. But for hooks are injected into different processes (especially global hooks), they don't know this DLL is required by each process or not, so they usually still load the DLL into each process space and hook the function, even if the process itself doesn't want this DLL.

Imagine a global hook with many dependencies trying to hook functions in various system DLLs, and then bring all the DLLs involved into all processes for loading and initialization, which the cost is enormous.

"Delay hook" is a good solution to this problem. That is, the hook is set immediately if the target DLL is already loaded, otherwise set the hook when the target DLL is loaded into the process.

## Technical solution

Obviously, the key to implementing "delay hook" is to acquire DLL load notification in the first place. "[DLL Load Notification](https://learn.microsoft.com/en-us/windows/win32/devnotes/dll-load-notification)" mechanism is introduced since NT6, this is what we need.

See [LdrRegisterDllNotification](https://learn.microsoft.com/en-us/windows/win32/devnotes/ldrregisterdllnotification) function, DLL load (and unload) notification will be sent to the callback registered by it, and the memory space mapped from the DLL will be available at that point while we can set hook.

Although Microsoft Learning prompts that related APIs may be changed or removed, their usage has not changed, just changed the held lock from `LdrpLoaderLock` to a dedicated `LdrpDllNotificationLock` since NT6.1. However, please keep the callback as simple as possible.

> [!TIP]
> If you want to know the internal implementation of "[DLL Load Notification](https://learn.microsoft.com/en-us/windows/win32/devnotes/dll-load-notification)" on Windows, see [ReactOS PR #6795](https://github.com/reactos/reactos/pull/6795) which I contributed to [ReactOS](https://github.com/reactos/reactos). Don't see [the WINE implementation](https://gitlab.winehq.org/wine/wine/-/commit/4c13e1765f559b322d8c071b2e23add914981db7), as it has mistakes as of this writing, for example, its `LdrUnregisterDllNotification` removes the node without checking it is in the list or not.

[Demo: DelayHook](../../../Source/Demo/DelayHook.c) shows the implementation of hooking functions within a DLL during its loading process by using this mechanism.

<br>
<hr>

This work is licensed under [Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)](http://creativecommons.org/licenses/by-nc-sa/4.0/).  
<br>
**[Ratin](https://github.com/RatinCN) &lt;[<ratin@knsoft.org>](mailto:ratin@knsoft.org)&gt;**  
*China national certified senior system architect*  
*[ReactOS](https://github.com/reactos/reactos) contributor*
