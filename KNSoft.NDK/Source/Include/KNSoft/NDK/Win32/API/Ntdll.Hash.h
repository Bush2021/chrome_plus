#pragma once

#include "../../NT/MinDef.h"

EXTERN_C_START

typedef struct _A_SHA_CTX
{
    UCHAR Buffer[64];
    ULONG State[5];
    ULONG Count[2];
} A_SHA_CTX, *PA_SHA_CTX;

#define A_SHA_DIGEST_LEN 20

typedef struct _MD5_CTX
{
    ULONG Count[2];
    ULONG State[4];
    UCHAR Buffer[64];
    UCHAR Hash[16];
} MD5_CTX, *PMD5_CTX;

#define MD5_DIGEST_LEN 16

typedef struct _MD4_CTX
{
    ULONG State[4];
    ULONG Count[2];
    UCHAR Buffer[64];
    UCHAR Hash[16];
} MD4_CTX, *PMD4_CTX;

#define MD4_DIGEST_LEN 16

#ifndef RSA32API
#define RSA32API __stdcall
#endif

NTSYSAPI
VOID
RSA32API
A_SHAInit(
    _Out_ PA_SHA_CTX Context);

NTSYSAPI
VOID
RSA32API
A_SHAUpdate(
    _Inout_ PA_SHA_CTX Context,
    _In_reads_(BufferSize) PUCHAR Buffer,
    _In_ ULONG BufferSize);

NTSYSAPI
VOID
RSA32API
A_SHAFinal(
    _Inout_ PA_SHA_CTX Context,
    _Out_ PUCHAR Result);

NTSYSAPI
VOID
RSA32API
MD5Init(
    _Out_ PMD5_CTX Context);

NTSYSAPI
VOID
RSA32API
MD5Update(
    _Inout_ PMD5_CTX Context,
    _In_reads_(BufferSize) PUCHAR Buffer,
    _In_ ULONG BufferSize);

NTSYSAPI
VOID
RSA32API
MD5Final(
    _Inout_ PMD5_CTX Context);

NTSYSAPI
VOID
RSA32API
MD4Init(
    _Out_ PMD4_CTX Context);

NTSYSAPI
VOID
RSA32API
MD4Update(
    _Inout_ PMD4_CTX Context,
    _In_reads_(BufferSize) PUCHAR Buffer,
    _In_ ULONG BufferSize);

NTSYSAPI
VOID
RSA32API
MD4Final(
    _Inout_ PMD4_CTX Context);

EXTERN_C_END
