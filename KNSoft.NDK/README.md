| **English (en-US)** | [简体中文 (zh-CN)](https://github.com/KNSoft/KNSoft.NDK/blob/main/README.zh-CN.md) |
| --- | --- |

&nbsp;

# KNSoft.NDK

[![NuGet Downloads](https://img.shields.io/nuget/dt/KNSoft.NDK)](https://www.nuget.org/packages/KNSoft.NDK) [![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/KNSoft/KNSoft.NDK/Build_Publish.yml)](https://github.com/KNSoft/KNSoft.NDK/actions/workflows/Build_Publish.yml) ![PR Welcome](https://img.shields.io/badge/PR-welcome-0688CB.svg) [![GitHub License](https://img.shields.io/github/license/KNSoft/KNSoft.NDK)](https://github.com/KNSoft/KNSoft.NDK/blob/main/LICENSE)

[KNSoft.NDK](https://github.com/KNSoft/KNSoft.NDK) provides native C/C++ definitions and import libraries for Windows NT development.

## Feature
- Undocumented API declarations and type definitions, e.g. `Nt/Zw*`, `Ldr*`, `PEB`, `TEB`, `LDR_*`, ...
- Import library addendum for Windows DLL exports, e.g. `KERNEL32.dll!CreateProcessInternalW`, `ntdll.dll!LdrRegisterDllNotification`, ...
- Addendum to Windows SDK
- Extension macros and definitions, e.g. `NtCurrentPeb()`, `PEB(64/32)`, `TEB(64/32)`, ...
- Function packages implemented by native method, e.g. unit test framework, safe string functions, command-line parsing, random numbers generation, ...

## Usage

[![NuGet Downloads](https://img.shields.io/nuget/dt/KNSoft.NDK)](https://www.nuget.org/packages/KNSoft.NDK)

### TL;DR

Include [NDK.h](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/Include/KNSoft/NDK/NDK.h) instead of (or **BEFORE**) `Windows.h` will do.
```C
#include <KNSoft/NDK/NDK.h>
```

NuGet package [KNSoft.NDK](https://www.nuget.org/packages/KNSoft.NDK) includes all the headers and import libraries.

### Details

The following features **are NOT enabled by default**, reference corresponding headers and libraries on demand:

- Ntdll Hash API (`(A_SHA/MD4/MD5)(Init/Update/Final)`)
  - [Ntdll.Hash.h](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/Include/KNSoft/NDK/Win32/API/Ntdll.Hash.h)
  - KNSoft.NDK.Ntdll.Hash.lib (Generated from [KNSoft.NDK.Ntdll.Hash.xml](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/KNSoft.NDK/WinAPI/KNSoft.NDK.Ntdll.Hash.xml))
- Ntdll CRT
  - KNSoft.NDK.Ntdll.CRT.lib (Generated from [KNSoft.NDK.Ntdll.CRT.xml](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/KNSoft.NDK/WinAPI/KNSoft.NDK.Ntdll.CRT.xml))
- Windows API import library addendum
  - KNSoft.NDK.WinAPI.lib (Generated from [KNSoft.NDK.WinAPI.xml](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/KNSoft.NDK/WinAPI/KNSoft.NDK.WinAPI.xml))
- Unit Test Framework
  - [UnitTest.h](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/Include/KNSoft/NDK/Package/UnitTest.h)
  - [UnitTest.inl](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/Include/KNSoft/NDK/Package/UnitTest.inl)
- Safe string functions (different from `strsafe.h` in Windows SDK)
  - [StrSafe.inl](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/Include/KNSoft/NDK/Package/StrSafe.inl)
- Command-line parsing functions
  - [ArgParse.inl](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/Include/KNSoft/NDK/Package/ArgParse.inl)
- Random numbers generation functions
  - [RandGen.inl](https://github.com/KNSoft/KNSoft.NDK/blob/main/Source/Include/KNSoft/NDK/Package/RandGen.inl)

The following features **are enabled by default**, can be excluded by defining corresponding macro:

| Macro | Exclude feature |
| ---- | ---- |
| _KNSOFT_NDK_NO_EXTENSION | Addendum or extension macros and definitions |

## Compatibility

![PR Welcome](https://img.shields.io/badge/PR-welcome-0688CB.svg) [![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/KNSoft/KNSoft.NDK/Build_Publish.yml)](https://github.com/KNSoft/KNSoft.NDK/actions/workflows/Build_Publish.yml)

[KNSoft.NDK](https://github.com/KNSoft/KNSoft.NDK) always follow up the latest Windows NT and specifications, targets to x86, x64, ARM64, and ARM64EC platforms.

**Only support the latest MSVC generation tools and SDKs.**

> [!CAUTION]
> In beta stage, should be used with caution.

## License

[![GitHub License](https://img.shields.io/github/license/KNSoft/KNSoft.NDK)](https://github.com/KNSoft/KNSoft.NDK/blob/main/LICENSE)

[KNSoft.NDK](https://github.com/KNSoft/KNSoft.NDK) is licensed under the [MIT](https://github.com/KNSoft/KNSoft.NDK/blob/main/LICENSE) license.

The content from the following public project(s):
- phnt (from [winsiderss/systeminformer](https://github.com/winsiderss/systeminformer/tree/master/phnt)) - [MIT](https://github.com/winsiderss/phnt/blob/master/LICENSE)

And public sources were lawfully used:
- Microsoft WDK/DDK/SDK
- Microsoft Public Symbolic Data
- Microsoft Learning
- Microsoft Windows Protocols
- Windows Internals
- [Microsoft.Windows.SDK.CRTSource](https://www.nuget.org/packages/Microsoft.Windows.SDK.CRTSource)

And [KNSoft/Precomp4C](https://github.com/KNSoft/Precomp4C) to generate DLL import libraries.
