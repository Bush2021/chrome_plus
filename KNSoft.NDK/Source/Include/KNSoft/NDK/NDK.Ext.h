/* KNSoft.NDK extensions */

#pragma once

#ifdef _KNSOFT_NDK_NO_EXTENSION
#pragma message("KNSoft.NDK: NDK.Ext.h is included but _KNSOFT_NDK_NO_EXTENSION is defineded.")
#endif

#include "NT/MinDef.h"

// Gets equality of two value after masked
#define IS_EQUAL_MASKED(val1, val2, mask) (!(((val1) ^ (val2)) & (mask)))
// Sets or removes a flag from a combination value
#define COMBINE_FLAGS(val, uflag, bEnable) ((bEnable) ? ((val) | (uflag)) : ((val) & ~(uflag)))
// Test combined flags
#define TEST_FLAGS(val, flags) (((val) & (flags)) == (flags))

#define MAKEDWORD2(ll, lh, hl, hh) (((DWORD)MAKEWORD(hl, hh) << 16) & (DWORD)MAKEWORD(ll, lh))

#define IPV4_LE(v1, v2, v3, v4) MAKEDWORD2(v1, v2, v3, v4)
#define IPV4_BE(v1, v2, v3, v4) MAKEDWORD2(v4, v3, v2, v1)

#define A8R8G8B8(r, g, b, a) MAKEDWORD2(b, g, r, a)
#define X8R8G8B8(r, g, b) MAKEDWORD2(b, g, r, 0)
#define A8B8G8R8(r, g, b, a) MAKEDWORD2(r, g, b, a)
#define X8B8G8R8(r, g, b) MAKEDWORD2(r, g, b, 0)

#pragma region Size in bytes

#define BYTE_BIT 8UL

#define _1KB (1024ULL)
#define _1MB (1024ULL * _1KB)
#define _1GB (1024ULL * _1MB)
#define _1TB (1024ULL * _1GB)
#define _1PB (1024ULL * _1TB)
#define _1EB (1024ULL * _1PB)

#define _KB(x) ((x) * _1KB)
#define _MB(x) ((x) * _1MB)
#define _GB(x) ((x) * _1GB)
#define _TB(x) ((x) * _1TB)
#define _PB(x) ((x) * _1PB)
#define _EB(x) ((x) * _1EB)

#define _2KB    _KB(2)
#define _4KB    _KB(4)
#define _8KB    _KB(8)
#define _16KB   _KB(16)
#define _32KB   _KB(32)
#define _64KB   _KB(64)
#define _128KB  _KB(128)
#define _256KB  _KB(256)
#define _512KB  _KB(512)
#define _1024KB _KB(1024)

#define _20KB    _KB(20)
#define _40KB    _KB(40)
#define _80KB    _KB(80)
#define _160KB   _KB(160)
#define _320KB   _KB(320)
#define _640KB   _KB(640)

#define _2MB    _MB(2)
#define _4MB    _MB(4)
#define _8MB    _MB(8)
#define _16MB   _MB(16)
#define _32MB   _MB(32)
#define _64MB   _MB(64)
#define _128MB  _MB(128)
#define _256MB  _MB(256)
#define _512MB  _MB(512)
#define _1024MB _MB(1024)

#define _20MB    _MB(20)
#define _40MB    _MB(40)
#define _80MB    _MB(80)
#define _160MB   _MB(160)
#define _320MB   _MB(320)
#define _640MB   _MB(640)

#define _2GB    _GB(2)
#define _4GB    _GB(4)
#define _8GB    _GB(8)
#define _16GB   _GB(16)
#define _32GB   _GB(32)
#define _64GB   _GB(64)
#define _128GB  _GB(128)
#define _256GB  _GB(256)
#define _512GB  _GB(512)
#define _1024GB _GB(1024)

#define _20GB    _GB(20)
#define _40GB    _GB(40)
#define _80GB    _GB(80)
#define _160GB   _GB(160)
#define _320GB   _GB(320)
#define _640GB   _GB(640)

#define _2TB    _TB(2)
#define _4TB    _TB(4)
#define _8TB    _TB(8)
#define _16TB   _TB(16)
#define _32TB   _TB(32)
#define _64TB   _TB(64)
#define _128TB  _TB(128)
#define _256TB  _TB(256)
#define _512TB  _TB(512)
#define _1024TB _TB(1024)

#define _20TB    _TB(20)
#define _40TB    _TB(40)
#define _80TB    _TB(80)
#define _160TB   _TB(160)
#define _320TB   _TB(320)
#define _640TB   _TB(640)

#define _2PB    _PB(2)
#define _4PB    _PB(4)
#define _8PB    _PB(8)
#define _16PB   _PB(16)
#define _32PB   _PB(32)
#define _64PB   _PB(64)
#define _128PB  _PB(128)
#define _256PB  _PB(256)
#define _512PB  _PB(512)
#define _1024PB _PB(1024)

#define _20PB    _PB(20)
#define _40PB    _PB(40)
#define _80PB    _PB(80)
#define _160PB   _PB(160)
#define _320PB   _PB(320)
#define _640PB   _PB(640)

#define _2EB    _EB(2)
#define _4EB    _EB(4)
#define _8EB    _EB(8)
#define _16EB   _EB(16)

_STATIC_ASSERT(_1024KB == _1MB &&
               _1024MB == _1GB &&
               _1024GB == _1TB &&
               _1024TB == _1PB &&
               _1024PB == _1EB &&
               _16EB - 1 == MAXULONGLONG);

#pragma endregion

#pragma region Limitations

#define MAX_CLASSNAME_CCH       256
#define MAX_CIDENTIFIERNAME_CCH 247
#define MAX_ATOM_CCH            255
#define MAX_REG_KEYNAME_CCH     255
#define MAX_REG_VALUENAME_CCH   16383
#define POINTER_CCH             (sizeof(PVOID) * 2 + 1)
#define HEX_RGB_CCH             8 // #RRGGBB

#pragma endregion

#pragma region String

#define _STR_LEN(quote) (ARRAYSIZE(quote) - 1)
#define _STR_SIZE(quote) (sizeof(quote) - sizeof(quote[0]))

#define ASCII_CASE_MASK 0b100000
#define UNICODE_EOL ((DWORD)0x000A000D)
#define ANSI_EOL ((WORD)0x0A0D)

#pragma endregion

#pragma region Any-size array

#define ANYSIZE_STRUCT_SIZE(structure, field, size) UFIELD_OFFSET(structure, field[size])

#define DEFINE_ANYSIZE_STRUCT(varName, baseType, arrayType, arraySize) struct {\
    baseType BaseType;\
    arrayType Array[(arraySize) - 1];\
} varName

#pragma endregion

#pragma region MSVC and WinSDK

#if _WIN64
#define IS_WIN64 TRUE
#else
#define IS_WIN64 FALSE
#endif

#define __A2U8(quote) u8##quote
#define _A2U8(quote) __A2U8(quote)

#define __A2W(quote) L##quote
#define _A2W(quote) __A2W(quote)

/* _CRT_LINKER_SYMBOL_PREFIX and _CRT_LINKER_FORCE_INCLUDE */

#if defined(_M_IX86)
#define MSVC_LINKER_SYMBOL_PREFIX "_"
#else
#define MSVC_LINKER_SYMBOL_PREFIX ""
#endif

#define MSVC_LINKER_FORCE_INCLUDE(name) __pragma(comment(linker, "/include:" MSVC_LINKER_SYMBOL_PREFIX #name))

/*
 * Initializer support
 * See also:
 *   https://devblogs.microsoft.com/cppblog/new-compiler-warnings-for-dynamic-initialization/
 *   https://learn.microsoft.com/en-us/cpp/c-runtime-library/crt-initialization
 */

// Section 'section-name' is reserved for C++ dynamic initialization.
#pragma warning(disable: 5247 5248)
#pragma section(".CRT$XCTNDK", long, read)
#pragma section(".CRT$XCVNDK", long, read)
#pragma warning(default: 5247 5248)

typedef void (__cdecl* _PVFV)(void);

// Will be called BEFORE any compiler generated C++ dynamic initializer
#define MSVC_PRE_CPP_USER_INITIALIZER(x)\
void __cdecl x(void);\
EXTERN_C __declspec(allocate(".CRT$XCTNDK")) _PVFV const _KNSoft_NDK_PreCppUserInitializer_##x = &x;\
MSVC_LINKER_FORCE_INCLUDE(_KNSoft_NDK_PreCppUserInitializer_##x)\
void __cdecl x(void)

// Will be called AFTER any compiler generated C++ dynamic initializer
#define MSVC_POST_CPP_USER_INITIALIZER(x)\
void __cdecl x(void);\
EXTERN_C __declspec(allocate(".CRT$XCVNDK")) _PVFV const _KNSoft_NDK_PostCppUserInitializer_##x = &x;\
MSVC_LINKER_FORCE_INCLUDE(_KNSoft_NDK_PostCppUserInitializer_##x)\
void __cdecl x(void)

#pragma endregion
