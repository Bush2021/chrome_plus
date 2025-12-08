| [English (en-US)](./README.md) | **简体中文 (zh-CN)** |
| --- | --- |

&nbsp;

# 分配Trampoline时避免占用系统保留区域

## Windows为系统DLL保留的区域

挂钩库分配Trampoline时一般优先从挂钩目标函数附近寻找可用的内存空间，如此挂钩系统API时十分可能占用系统DLL使用的区域，导致本应加载到该位置的系统DLL加载到别地并额外进行重定位操作，甚至引起`STATUS_ILLEGAL_DLL_RELOCATION (0xC0000269)`异常（比如kernel32.dll、user32.dll受影响时）。

Windows自NT6起引入ASLR，随之为系统DLL在用户模式下明确地预留了一段区域，使得同一个系统DLL在不同进程中都能映射到这片保留区域的同一位置，加载一次后即可复用该次重定位信息避免后续加载再次进行重定位操作。

这个机制在《Windows Internals 7th Part1》第五章《Memory management》的“Image randomization”小节有详细说明，此处不再赘述，我参考该书并经过分析`ntoskrnl.exe!MiInitializeRelocations`得到的确切保留范围是：  
32位进程：[0x50000000 ... 0x78000000)，共640MB  
64位进程：[0x00007FF7FFFF0000 ... 0x00007FFFFFFF0000)，共32GB

即使没有ASLR，也可以考虑从顶部保留一定大小的区域。

## 其它挂钩库的做法

[Detours](https://github.com/microsoft/Detours)作为微软官方的挂钩库，已考虑到系统保留区域不能给Trampoline使用这一点，它硬编码[0x70000000 ... 0x80000000]地址范围进行规避：
```C
//////////////////////////////////////////////////////////////////////////////
//
// Region reserved for system DLLs, which cannot be used for trampolines.
//
static PVOID    s_pSystemRegionLowerBound   = (PVOID)(ULONG_PTR)0x70000000;
static PVOID    s_pSystemRegionUpperBound   = (PVOID)(ULONG_PTR)0x80000000;
```

此范围仅适用于NT5，64位NT5中ntdll.dll、kernel32.dll、user32.dll也仍在此范围内。

同样注意到此问题的[jdu2600](https://github.com/jdu2600)为[Detours](https://github.com/microsoft/Detours)开了一个非官方的PR [microsoft/Detours PR #307](https://github.com/microsoft/Detours/pull/307) 想更新这个范围以适配最新的Windows。

[MinHook](https://github.com/TsudaKageyu/minhook)与[mhook](https://github.com/martona/mhook)都是熟知的Windows API挂钩库，遗憾的是它们似乎都没有考虑到这个问题。

## SlimDetours的实现

32位系统ASLR的预留范围大小仅640MB，直接规避即可。而对于64位系统则复杂一些，ASLR的预留范围有32GB，太大而不可能全部规避。结合ASLR的排布规则和Trampoline的选址需求，视`Ntdll.dll`之后1GB范围为要规避的保留范围是合理的，这个考虑与上面提到的PR一致。要注意这个范围可能被分成两块，例如以下场排布场景：

| 地址 | 加载顺序 | 系统DLL名称 | 大小 |
| :---: | :---: | :---: | :---: |
| （顶）<br>0x00007FFFFFFF0000<br>...<br>0x00007FFFFF9E0000 | #3 | B.dll | 6,208KB |
| 0x00007FFFFF9E0000<br>...<br>0x00007FFFFF820000 | #4 | C.dll | 1,792KB |
| ... | ... | ... |
| 0x00007FF800690000<br>...<br>0x00007FF800480000 | #1 | Ntdll.dll | 2,112KB |
| 0x00007FF800480000<br>...<br>0x00007FF7FFFF0000<br>（底） | #2 | A.dll | 4,672KB |

`Ntdll.dll`被ASLR随机加载到保留范围内较低的内存地址，后续DLL随后排布触底时，将切换到保留范围顶部继续排布，在这个情况下“`Ntdll.dll`之后的1GB范围”便是2块不连续的区域。

[SlimDetours](https://github.com/KNSoft/KNSoft.SlimDetours)的具体实现与规避范围均有别于上述PR，为不同NT版本进行了更周到的考虑，比如在NT6.0及NT6.1中ASLR可以被注册表`HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Memory Management`项`MoveImages`键设置关闭。还调用`NtQuerySystemInformation`以获得比硬编码更确切的用户地址空间范围，协助约束Trampoline的选址，参考[KNSoft.SlimDetours/Source/KNSoft.SlimDetours/Memory.c于main · KNSoft/KNSoft.SlimDetours](../../../Source/KNSoft.SlimDetours/Memory.c)。

<br>
<hr>

本作品采用 [知识共享署名-非商业性使用-相同方式共享 4.0 国际许可协议 (CC BY-NC-SA 4.0)](http://creativecommons.org/licenses/by-nc-sa/4.0/) 进行许可。  
<br>
**[Ratin](https://github.com/RatinCN) &lt;[<ratin@knsoft.org>](mailto:ratin@knsoft.org)&gt;**  
*中国国家认证系统架构设计师*  
*[ReactOS](https://github.com/reactos/reactos)贡献者*
