| **English (en-US)** | [简体中文 (zh-CN)](./README.zh-CN.md) |
| --- | --- |

&nbsp;

# Avoid Occupying System Reserved Region When Allocating Trampoline

## Windows reserved region for system DLLs

Hooking libraries usually preferred to find available memory space near the hook target function when allocating trampoline, so it's very likely to occupy the area used by the system DLL, causing the system DLL that should be loaded to that location to be loaded to another place and perform additional relocation operations, it can even cause a `STATUS_ILLEGAL_DLL_RELOCATION (0xC0000269)` exception (for example, when kernel32.dll or user32.dll is affected).

Windows introduced ASLR since NT6, a region has been reserved for system DLLs explicitly, so that the same system DLL can be mapped at the same location in the reserved region inside different processes, the relocation information can be reused after being loaded once to avoid the relocation operation again on subsequent loading.

This mechanism is introduced in detail in the "Image randomization" section of Chapter 5 "Memory management" in "Windows Internals 7th Part 1", here will not go into details, the exact reserved region that I have obtained by referring to the book and analyzing `ntoskrnl.exe!MiInitializeRelocations` is:  
32-bit process：[0x50000000 ... 0x78000000), a total of 640MB  
64-bit process：[0x00007FF7FFFF0000 ... 0x00007FFFFFFF0000), a total of 32GB

Even without ASLR, consider keeping a certain size area from the top.

## Other hooking libraries' practices

[Detours](https://github.com/microsoft/Detours) as Microsoft's official hooking library, has taken into account that the system reserved region cannot be used for trampolines, it hardcodes the [0x70000000 ... 0x80000000] address range to circumvent:
```C
//////////////////////////////////////////////////////////////////////////////
//
// Region reserved for system DLLs, which cannot be used for trampolines.
//
static PVOID    s_pSystemRegionLowerBound   = (PVOID)(ULONG_PTR)0x70000000;
static PVOID    s_pSystemRegionUpperBound   = (PVOID)(ULONG_PTR)0x80000000;
```

This range only applies to NT5, ntdll.dll, kernel32.dll, and user32.dll remain within this range in 64-bit NT5.

[jdu2600](https://github.com/jdu2600) is also aware of this issue and has opened an unofficial PR [microsoft/Detours PR #307](https://github.com/microsoft/Detours/pull/307) for [Detours](https://github.com/microsoft/Detours) wants to update this range to adapt the latest Windows.

[MinHook](https://github.com/TsudaKageyu/minhook) and [mhook](https://github.com/martona/mhook) are both well-known Windows API hooking libraries, but unfortunately they don't seem to take this issue into account.

## SlimDetours implementation

ASLR only reserves a range of 640MB in size for 32-bit systems, which can be directly circumvented. For 64-bit systems, it's a bit more complicated, ASLR reserves a range of of 32GB in size, which is too large to circumvent completely. Consider the ASLR layout rules and the location requirements of trampolines, treat 1GB range after `Ntdll.dll` as a reserved range to be circumvented is make sense, this consideration is consistent with the PR mentioned above. Note that this range may be split into two blocks, for example, the following layout:

| Address | Load Order | System DLL Name | Size |
| :---: | :---: | :---: | :---: |
| （Top）<br>0x00007FFFFFFF0000<br>...<br>0x00007FFFFF9E0000 | #3 | B.dll | 6,208KB |
| 0x00007FFFFF9E0000<br>...<br>0x00007FFFFF820000 | #4 | C.dll | 1,792KB |
| ... | ... | ... |
| 0x00007FF800690000<br>...<br>0x00007FF800480000 | #1 | Ntdll.dll | 2,112KB |
| 0x00007FF800480000<br>...<br>0x00007FF7FFFF0000<br>（Bottom） | #2 | A.dll | 4,672KB |

`Ntdll.dll` is randomly loaded by ASLR to a memory address lower in the reserved range, and when the subsequent DLL layout bottoms out, it will wrap to the top of the reserved range and continue to be arranged, in which case the "1GB range after `Ntdll.dll`" is 2 discontinuous regions.

[SlimDetours](https://github.com/KNSoft/KNSoft.SlimDetours)' implementation details and circumvention range are different from the above PR, more thoughtful consideration has been given to different NT versions, e.g. in NT6.0 and NT6.1 ASLR can be turned off by the `MoveImages` value under `HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Memory Management` key in the registry. And calls `NtQuerySystemInformation` to obtain a more accurate user address space range than hardcoded to help constrain the location of trampolines, see also [KNSoft.SlimDetours/Source/KNSoft.SlimDetours/Memory.c at main · KNSoft/KNSoft.SlimDetours](../../../Source/KNSoft.SlimDetours/Memory.c).

<br>
<hr>

This work is licensed under [Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)](http://creativecommons.org/licenses/by-nc-sa/4.0/).  
<br>
**[Ratin](https://github.com/RatinCN) &lt;[<ratin@knsoft.org>](mailto:ratin@knsoft.org)&gt;**  
*China national certified senior system architect*  
*[ReactOS](https://github.com/reactos/reactos) contributor*
