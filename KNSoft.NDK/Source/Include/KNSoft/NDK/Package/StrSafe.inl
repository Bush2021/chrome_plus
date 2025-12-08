/*
 * KNSoft.NDK StrSafe.inl package, licensed under the MIT license.
 * Copyright (c) KNSoft.org (https://github.com/KNSoft). All rights reserved.
 *
 * Make C string routines a bit more safe and easy.
 * Similar to strsafe.h in Windows SDK but different.
 *
 * Usually, the return value is the number of characters written, not including null-terminator, so:
 *     Return == 0: No data or an error occurred (e.g. the buffer too small).
 *     Return < BufferCount: Success, returns the number of characters or bytes written.
 * Only if `BufferCount == 0`:
 *     Return value is the required size of Buffer, including null-terminator, or 0 if no data or an error occurred.
 * The output buffer is always null-terminated: `Buffer[return] == '\0'`, and won't be truncated.
 *
 * Define `_NO_CRT_STDIO_INLINE` to use `legacy_stdio_definitions.lib`.
 */

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#pragma region String PrintF

_Success_(return > 0)
__forceinline
unsigned int
__cdecl
StrSafe_CchVPrintfW(
    _Out_writes_(BufferCount) _Always_(_Post_z_) wchar_t* Buffer,
    _In_ size_t BufferCount,
    _In_z_ _Printf_format_string_ const wchar_t* Format,
    va_list ArgList)
{
#pragma warning(disable: 4996)
    int i = _vsnwprintf(Buffer, BufferCount, Format, ArgList);
#pragma warning(default: 4996)
    if (i > 0 && i == BufferCount)
    {
        i--;
    } else if (i < 0)
    {
        i = 0;
    } else
    {
        return BufferCount > 0 ? i : i + 1;
    }
    Buffer[i] = L'\0';
    return i;
}

_Success_(return > 0)
__forceinline
unsigned int
__cdecl
StrSafe_CchVPrintfA(
    _Out_writes_(BufferCount) _Always_(_Post_z_) char* Buffer,
    _In_ size_t BufferCount,
    _In_z_ _Printf_format_string_ const char* Format,
    va_list ArgList)
{
#pragma warning(disable: 4996)
    int i = _vsnprintf(Buffer, BufferCount, Format, ArgList);
#pragma warning(default: 4996)
    if (i > 0 && i == BufferCount)
    {
        i--;
    } else if (i < 0)
    {
        i = 0;
    } else
    {
        return BufferCount > 0 ? i : i + 1;
    }
    Buffer[i] = '\0';
    return (unsigned int)i;
}

_Success_(return > 0)
__forceinline
unsigned int
__cdecl
StrSafe_CchPrintfW(
    _Out_writes_(BufferCount) _Always_(_Post_z_) wchar_t* Buffer,
    _In_ size_t BufferCount,
    _In_z_ _Printf_format_string_ const wchar_t* Format,
    ...)
{
    va_list ArgList;
    unsigned int i;

    va_start(ArgList, Format);
    i = StrSafe_CchVPrintfW(Buffer, BufferCount, Format, ArgList);
    va_end(ArgList);

    return (unsigned int)i;
}

_Success_(return > 0)
__forceinline
unsigned int
__cdecl
StrSafe_CchPrintfA(
    _Out_writes_(BufferCount) _Always_(_Post_z_) char* Buffer,
    _In_ size_t BufferCount,
    _In_z_ _Printf_format_string_ const char* Format,
    ...)
{
    va_list ArgList;
    unsigned int i;

    va_start(ArgList, Format);
    i = StrSafe_CchVPrintfA(Buffer, BufferCount, Format, ArgList);
    va_end(ArgList);

    return i;
}

#pragma endregion StrSafe_Cch[V]Printf(W/A)

#pragma region String Copy

_Success_(return > 0)
__forceinline
unsigned int
__cdecl
StrSafe_CchCopyW(
    _Out_writes_(BufferCount) _Always_(_Post_z_) wchar_t* Buffer,
    _In_range_(>, 0) unsigned int BufferCount,
    _In_z_ const wchar_t* Source)
{
    unsigned int i;

    for (i = 0; i < BufferCount; i++)
    {
        if ((Buffer[i] = Source[i]) == L'\0')
        {
            return i;
        }
    }
    Buffer[0] = L'\0';
    return 0;
}

_Success_(return > 0)
__forceinline
unsigned int
__cdecl
StrSafe_CchCopyA(
    _Out_writes_(BufferCount) _Always_(_Post_z_) char* Buffer,
    _In_range_(>, 0) unsigned int BufferCount,
    _In_z_ const char* Source)
{
    unsigned int i;

    for (i = 0; i < BufferCount; i++)
    {
        if ((Buffer[i] = Source[i]) == '\0')
        {
            return i;
        }
    }
    Buffer[0] = '\0';
    return 0;
}

#pragma endregion StrSafe_CchCopy(W/A)
