| **English (en-US)** | [简体中文 (zh-CN)](https://github.com/KNSoft/KNSoft.SlimDetours/blob/main/README.zh-CN.md) |
| --- | --- |

&nbsp;

# KNSoft.SlimDetours

[![NuGet Downloads](https://img.shields.io/nuget/dt/KNSoft.SlimDetours)](https://www.nuget.org/packages/KNSoft.SlimDetours) [![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/KNSoft/KNSoft.SlimDetours/msbuild.yml)](https://github.com/KNSoft/KNSoft.SlimDetours/actions/workflows/msbuild.yml) ![PR Welcome](https://img.shields.io/badge/PR-welcome-0688CB.svg) [![GitHub License](https://img.shields.io/github/license/KNSoft/KNSoft.SlimDetours)](https://github.com/KNSoft/KNSoft.SlimDetours/blob/main/LICENSE)

[SlimDetours](https://github.com/KNSoft/KNSoft.SlimDetours) is an improved Windows API hooking library base on [Microsoft Detours](https://github.com/microsoft/Detours).

Compared to the original [Detours](https://github.com/microsoft/Detours), the advantages are:

- Improved
  - **Automatically update threads when set hooks** [🔗 TechWiki: Update Threads Automatically When Applying Inline Hooks](https://github.com/KNSoft/KNSoft.SlimDetours/blob/main/Docs/TechWiki/Update%20Threads%20Automatically%20When%20Applying%20Inline%20Hooks/README.md)
  - **Avoid deadlocks when updating threads** [🔗 TechWiki: Avoid Deadlocking on The Heap When Updating Threads](https://github.com/KNSoft/KNSoft.SlimDetours/blob/main/Docs/TechWiki/Avoid%20Deadlocking%20on%20The%20Heap%20When%20Updating%20Threads/README.md)
  - Avoid occupying system reserved memory region [🔗 TechWiki: Avoid Occupying System Reserved Region When Allocating Trampoline](https://github.com/KNSoft/KNSoft.SlimDetours/blob/main/Docs/TechWiki/Avoid%20Occupying%20System%20Reserved%20Region%20When%20Allocating%20Trampoline/README.md)
  - Other bug fixes and code improvements
- Lite
  - **Depends on `Ntdll.dll` only**
  - Retain API hooking functions only
  - Remove support for ARM (ARM32), IA64
  - Smaller binary size

And here is a [Todo List](https://github.com/KNSoft/KNSoft.SlimDetours/milestones?with_issues=no).

## Usage

[![NuGet Downloads](https://img.shields.io/nuget/dt/KNSoft.SlimDetours)](https://www.nuget.org/packages/KNSoft.SlimDetours)

### TL;DR

[KNSoft.SlimDetours package](https://www.nuget.org/packages/KNSoft.SlimDetours) is out-of-the-box, contains both of [KNSoft.SlimDetours](https://github.com/KNSoft/KNSoft.SlimDetours) and the latest [Microsoft Detours](https://github.com/microsoft/Detours), include corresponding header ([SlimDetours.h](https://github.com/KNSoft/KNSoft.SlimDetours/blob/main/Source/KNSoft.SlimDetours/SlimDetours.h) or [detours.h](https://github.com/KNSoft/KNSoft.SlimDetours/blob/main/Source/Microsoft.Detours/src/detours.h)) and compiled static library to use them.

```C
/* KNSoft.SlimDetours */
#include <KNSoft/SlimDetours/SlimDetours.h>
#pragma comment(lib, "KNSoft.SlimDetours.lib")

/* Microsoft Detours */
#include <KNSoft/SlimDetours/detours.h>
#pragma comment(lib, "Microsoft.Detours.lib")
```

If your project configuration name contains neither "Release" nor "Debug", [MSBuild sheet](https://github.com/KNSoft/KNSoft.SlimDetours/blob/main/Source/KNSoft.SlimDetours.targets) in NuGet package cannot determinate automatically the last level directory name ("Release" or "Debug") of library path should be used, add it manually is required, for example:
```C
#if DBG
#pragma comment(lib, "Debug/KNSoft.SlimDetours.lib")
#else
#pragma comment(lib, "Release/KNSoft.SlimDetours.lib")
#endif
```

The usage has been simplified, e.g. the hook only needs one line:
```C
SlimDetoursInlineHook(TRUE, (PVOID*)&g_pfnXxx, Hooked_Xxx);  // Hook
...
SlimDetoursInlineHook(FALSE, (PVOID*)&g_pfnXxx, Hooked_Xxx); // Unhook
```
For more simplified API see [InlineHook.c](https://github.com/KNSoft/KNSoft.SlimDetours/blob/main/Source/KNSoft.SlimDetours/InlineHook.c).

### Details

The original [Microsoft Detours](https://github.com/microsoft/Detours) style functions are also retained, but with a few differences:

- Function name begin with `"SlimDetours"`
- Most of return values are [`HRESULT`](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-erref/0642cb2f-2075-4469-918c-4441e69c548a) that wraps [`NTSTATUS`](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-erref/87fba13e-bf06-450e-83b1-9241dc81e781) by [`HRESULT_FROM_NT`](https://learn.microsoft.com/en-us/windows/win32/api/winerror/nf-winerror-hresult_from_nt) macro, use macros like [`SUCCEEDED`](https://learn.microsoft.com/en-us/windows/win32/api/winerror/nf-winerror-succeeded) to check them.
- [Threads are updated automatically](https://github.com/KNSoft/KNSoft.SlimDetours/blob/main/Docs/TechWiki/Update%20Threads%20Automatically%20When%20Applying%20Inline%20Hooks/README.md), [`DetourUpdateThread`](https://github.com/microsoft/Detours/wiki/DetourUpdateThread) has been omitted.
```C
hr = SlimDetoursTransactionBegin();
if (FAILED(hr))
{
    return hr;
}
hr = SlimDetoursAttach((PVOID*)&g_pfnXxx, Hooked_Xxx);
if (FAILED(hr))
{
    SlimDetoursTransactionAbort();
    return hr;
}
return SlimDetoursTransactionCommit();
```

## Compatibility

Project building: support for the latest MSVC generation tools and SDKs is mainly considered. The code in this project is backwards compatible with the MSVC generation tool and GCC, but it depends on the NDK it depends on, see also [SlimDetours.NDK.inl](./Source/KNSoft.SlimDetours/SlimDetours.NDK.inl). Can be built with [ReactOS](https://github.com/reactos/reactos). The minimum target platform is NT6 by default, specifying the `_WIN32_WINNT` macro in compiling-time to build binaries that target to lower NT versions.

Artifact integration: widely compatible with MSVC generation tools (support for VS2015 is known), and different compilation configurations (e.g., `/MD`, `/MT`).

Runtime environment: NT5 or above OS, x86/x64/ARM64/ARM64EC target platforms.

> [!CAUTION]
> In beta stage, should be used with caution. Some APIs may be altered frequently, please keep an eye out for the release notes.

## License

[![GitHub License](https://img.shields.io/github/license/KNSoft/KNSoft.SlimDetours)](https://github.com/KNSoft/KNSoft.SlimDetours/blob/main/LICENSE)

KNSoft.SlimDetours is licensed under the [MIT](https://github.com/KNSoft/KNSoft.SlimDetours/blob/main/LICENSE) license.

Source is based on [Microsoft Detours](https://github.com/microsoft/Detours) which is licensed under the [MIT](https://github.com/microsoft/Detours/blob/main/LICENSE) license.

Also uses [KNSoft.NDK](https://github.com/KNSoft/KNSoft.NDK) to access low-level Windows NT APIs and its Unit Test Framework.
