| [English (en-US)](https://github.com/KNSoft/KNSoft.NDK/blob/main/README.md) | **简体中文 (zh-CN)** |
| --- | --- |

&nbsp;

# KNSoft.NDK

[![NuGet Downloads](https://img.shields.io/nuget/dt/KNSoft.NDK)](https://www.nuget.org/packages/KNSoft.NDK) [![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/KNSoft/KNSoft.NDK/Build_Publish.yml)](https://github.com/KNSoft/KNSoft.NDK/actions/workflows/Build_Publish.yml) ![PR Welcome](https://img.shields.io/badge/PR-welcome-0688CB.svg) [![GitHub License](https://img.shields.io/github/license/KNSoft/KNSoft.NDK)](https://github.com/KNSoft/KNSoft.NDK/blob/main/LICENSE)

[KNSoft.NDK](https://github.com/KNSoft/KNSoft.NDK)提供了原始C/C++定义和引入库供Windows NT开发。

## 功能
- 未公开的API声明和类型定义，例如：`Nt/Zw*`、`Ldr*`、`PEB`、`TEB`、`LDR_*`……
- Windows DLL导出项引入库补充，例如：`KERNEL32.dll!CreateProcessInternalW`、`ntdll.dll!LdrRegisterDllNotification`……
- Windows SDK的补充
- 扩展的宏和定义，例如：`NtCurrentPeb()`、`PEB(64/32)`、`TEB(64/32)`……
- 使用原生方式实现的功能包（如单元测试框架、安全字符串函数、命令行解析、随机数生成等）

## 用法

[![NuGet Downloads](https://img.shields.io/nuget/dt/KNSoft.NDK)](https://www.nuget.org/packages/KNSoft.NDK)

### 提要

包含[NDK.h](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/Include/KNSoft/NDK/NDK.h)代替（或先于）`Windows.h`即可。
```C
#include <KNSoft/NDK/NDK.h>
```

NuGet包[KNSoft.NDK](https://www.nuget.org/packages/KNSoft.NDK)包含所有头文件和引入库。

### 详细说明

下列功能**默认没有启用**，按需引用对应的头文件和库：

- Ntdll Hash API (`(A_SHA/MD4/MD5)(Init/Update/Final)`)
  - [Ntdll.Hash.h](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/Include/KNSoft/NDK/Win32/API/Ntdll.Hash.h)
  - KNSoft.NDK.Ntdll.Hash.lib (由[KNSoft.NDK.Ntdll.Hash.xml](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/KNSoft.NDK/WinAPI/KNSoft.NDK.Ntdll.Hash.xml)生成)
- Ntdll CRT
  - KNSoft.NDK.Ntdll.CRT.lib (由[KNSoft.NDK.Ntdll.CRT.xml](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/KNSoft.NDK/WinAPI/KNSoft.NDK.Ntdll.CRT.xml)生成)
- Windows API 引入库补充
  - KNSoft.NDK.WinAPI.lib (由[KNSoft.NDK.WinAPI.xml](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/KNSoft.NDK/WinAPI/KNSoft.NDK.WinAPI.xml)生成)
- 单元测试框架
  - [UnitTest.h](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/Include/KNSoft/NDK/Package/UnitTest.h)
  - [UnitTest.inl](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/Include/KNSoft/NDK/Package/UnitTest.inl)
- 安全字符串函数（与Windows SDK里的`strsafe.h`不同）
  - [StrSafe.inl](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/Include/KNSoft/NDK/Package/StrSafe.inl)
- 命令行解析函数
  - [ArgParse.inl](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/Include/KNSoft/NDK/Package/ArgParse.inl)
- 随机数生成函数
  - [RandGen.inl](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/Include/KNSoft/NDK/Package/RandGen.inl)

下列功能**默认启用**，定义对应宏可进行排除：

| 宏 | 排除功能 |
| ---- | ---- |
| _KNSOFT_NDK_NO_EXTENSION | 补充或扩展的宏和定义 |

## 兼容性

![PR Welcome](https://img.shields.io/badge/PR-welcome-0688CB.svg) [![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/KNSoft/KNSoft.NDK/Build_Publish.yml)](https://github.com/KNSoft/KNSoft.NDK/actions/workflows/Build_Publish.yml)

[KNSoft.NDK](https://github.com/KNSoft/KNSoft.NDK)始终跟进最新的Windows NT和规范，目标x86、x64、ARM64和ARM64EC平台。

**仅支持最新的MSVC生成工具和SDK。**

> [!CAUTION]
> 处于beta阶段，应小心使用。

## 协议

[![GitHub License](https://img.shields.io/github/license/KNSoft/KNSoft.NDK)](https://github.com/KNSoft/KNSoft.NDK/blob/main/LICENSE)

[KNSoft.NDK](https://github.com/KNSoft/KNSoft.NDK)根据[MIT](https://github.com/KNSoft/KNSoft.NDK/blob/main/LICENSE)协议进行许可。

合法使用了来自下列公开项目：
- phnt（来自[winsiderss/systeminformer](https://github.com/winsiderss/systeminformer/tree/master/phnt)）- [MIT](https://github.com/winsiderss/phnt/blob/master/LICENSE)

和公开源的内容：
- Microsoft WDK/DDK/SDK
- Microsoft Public Symbolic Data
- Microsoft Learning
- Microsoft Windows Protocols
- Windows Internals
- [Microsoft.Windows.SDK.CRTSource](https://www.nuget.org/packages/Microsoft.Windows.SDK.CRTSource)

以及[KNSoft/Precomp4C](https://github.com/KNSoft/Precomp4C)用于生成DLL导入库。
