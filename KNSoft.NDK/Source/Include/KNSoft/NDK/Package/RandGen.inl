/*
 * KNSoft.NDK RandGen.inl package, licensed under the MIT license.
 * Copyright (c) KNSoft.org (https://github.com/KNSoft). All rights reserved.
 *
 * Provide native implementation of random numbers generating.
 *
 * Rand_[SW/HW](16/32/64/SizeT/Buffer):
 *   SW/HW: Generate by software or hardware, if not specified, try hardware before software.
 *   16/32/64/SizeT: 16-bit, 32-bit, 64-bit, or size_t random number.
 *   Buffer: Fill buffer with random numbers.
 */

#pragma once

#include "../NDK.h"

/* Generate software random numbers by calling RtlRandomEx [0..MAXLONG-1], use the low 31 bits only */

static ULONG g_ulRandSeed = 0;

__forceinline
unsigned __int32
Rand_SW32(void)
{
    return ((RtlRandomEx(&g_ulRandSeed) & 0xFFFF) << 16) | (RtlRandomEx(&g_ulRandSeed) & 0xFFFF);
}

__forceinline
unsigned __int64
Rand_SW64(VOID)
{
    ULONGLONG p;

    p = (RtlRandomEx(&g_ulRandSeed) & 0xFFFFFFULL) << 40;
    p |= (RtlRandomEx(&g_ulRandSeed) & 0xFFFFFFULL) << 16;
    p |= RtlRandomEx(&g_ulRandSeed) & 0xFFFFULL;
    return p;
}

__forceinline
unsigned __int16
Rand_SW16(void)
{
    return (unsigned __int16)RtlRandomEx(&g_ulRandSeed);
}

/* Generate hardware random numbers by calling RDRAND on x64/x86 */
#if (defined(_M_X64) && !defined(_M_ARM64EC)) || defined(_M_IX86)
#define _RAND_HAS_RDRAND_ 1
#else
#define _RAND_HAS_RDRAND_ 0
#endif

#if _RAND_HAS_RDRAND_

/* Try 1000000 times to patch a CPU bug like Microsoft SymCrypt */

__forceinline
_Success_(return != FALSE)
LOGICAL
Rand_RdRand32(
    _Out_ unsigned __int32* Random)
{
    for (unsigned int i = 0; i < 1000000; i++)
    {
        if (_rdrand32_step(Random) != 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

__forceinline
_Success_(return != FALSE)
LOGICAL
Rand_RdRand64(
    _Out_ unsigned __int64* Random)
{
    for (unsigned int i = 0; i < 1000000; i++)
    {
        if (
#if _WIN64
            _rdrand64_step(Random) != 0
#else
            _rdrand32_step((unsigned int*)&Random) != 0 &&
            _rdrand32_step((unsigned int*)Add2Ptr(&Random, sizeof(unsigned int))) != 0
#endif
            )
        {
            return TRUE;
        }
    }
    return FALSE;
}

__forceinline
_Success_(return != FALSE)
LOGICAL
Rand_RdRand16(
    _Out_ unsigned __int16* Random)
{
    for (unsigned int i = 0; i < 1000000; i++)
    {
        if (_rdrand16_step(Random) != 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

#endif /* _RAND_HAS_RDRAND_ */

static int g_iFallbackToSW = -1;

__forceinline
LOGICAL
_Rand_FallbackToSW(VOID)
{
    if (g_iFallbackToSW != -1)
    {
        return !!g_iFallbackToSW;
    }

    /*
     * RDRAND is not emulated on WoA yet,
     * and SharedUserData->NativeProcessorArchitecture is avaliable since NT 6.2.
     */
    if (SharedUserData->NtMajorVersion < 10)
    {
        /* WoA since NT 10 */
        g_iFallbackToSW = FALSE;
    } else if (NtReadTebPVOID(WowTebOffset) == NULL)
    {
        /* Not in WOW */
        g_iFallbackToSW = FALSE;
    } else if (SharedUserData->NativeProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
               SharedUserData->NativeProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
    {
        /* RDRAND is supported natively */
        g_iFallbackToSW = FALSE;
    } else
    {
        g_iFallbackToSW = TRUE;
    }
    return !!g_iFallbackToSW;
}

__forceinline
_Success_(return != FALSE)
LOGICAL
Rand_HW32(
    _Out_ unsigned __int32* Random)
{
#if _RAND_HAS_RDRAND_
    if (_Rand_FallbackToSW())
    {
#endif
        *Random = Rand_SW32();
        return TRUE;
#if _RAND_HAS_RDRAND_
    } else
    {
        return Rand_RdRand32(Random);
    }
#endif
}

__forceinline
_Success_(return != FALSE)
LOGICAL
Rand_HW64(
    _Out_ unsigned __int64* Random)
{
#if _RAND_HAS_RDRAND_
    if (_Rand_FallbackToSW())
    {
#endif
        *Random = Rand_SW64();
        return TRUE;
#if _RAND_HAS_RDRAND_
    } else
    {
        return Rand_RdRand64(Random);
    }
#endif
}

__forceinline
_Success_(return != FALSE)
LOGICAL
Rand_HW16(
    _Out_ unsigned __int16* Random)
{
#if _RAND_HAS_RDRAND_
    if (_Rand_FallbackToSW())
    {
#endif
        *Random = Rand_SW16();
        return TRUE;
#if _RAND_HAS_RDRAND_
    } else
    {
        return Rand_RdRand16(Random);
    }
#endif
}

__forceinline
size_t
Rand_SWSizeT(VOID)
{
    return
#if defined(_WIN64)
        Rand_SW64();
#else
        Rand_SW32();
#endif
}

__forceinline
_Success_(return != FALSE)
LOGICAL
Rand_HWSizeT(
    _Out_ size_t* Random)
{
    return
#if defined(_WIN64)
        Rand_HW64(Random);
#else
        Rand_HW32(Random);
#endif
}

__forceinline
unsigned __int64
Rand_64(VOID)
{
    unsigned __int64 p;
    return Rand_HW64(&p) ? p : Rand_SW64();
}

__forceinline
unsigned __int32
Rand_32(VOID)
{
    unsigned int p;
    return Rand_HW32(&p) ? p : Rand_SW32();
}

__forceinline
unsigned __int16
Rand_16(VOID)
{
    unsigned __int16 p;
    return Rand_HW16(&p) ? p : Rand_SW16();
}

__forceinline
size_t
Rand_SizeT(VOID)
{
    size_t p;
    return Rand_HWSizeT(&p) ? p : Rand_SWSizeT();
}

__inline
void
Rand_SWBuffer(
    _Out_writes_bytes_(RandomBufferLength) void* RandomBuffer,
    _In_ unsigned int RandomBufferLength)
{
    unsigned char* pEnd = (unsigned char*)Add2Ptr(RandomBuffer, RandomBufferLength);
    unsigned char* p = (unsigned char*)RandomBuffer;
    unsigned char* q;
    unsigned __int64 s;

    while (p <= pEnd - sizeof(unsigned __int64))
    {
        *(unsigned __int64*)p = Rand_SW64();
        p += sizeof(unsigned __int64);
    }
    if (p != pEnd)
    {
        s = Rand_SW64();
        q = (unsigned char*)&s;
        do
        {
            *p++ = *q++;
        } while (p < pEnd);
    }
}

__inline
_Success_(return != FALSE)
LOGICAL
Rand_HWBuffer(
    _Out_writes_bytes_(RandomBufferLength) void* RandomBuffer,
    _In_ unsigned int RandomBufferLength)
{
    unsigned char* pEnd = (unsigned char*)Add2Ptr(RandomBuffer, RandomBufferLength);
    unsigned char* p = (unsigned char*)RandomBuffer;
    unsigned char* q;
    size_t s;

    while (p <= pEnd - sizeof(size_t))
    {
        if (!Rand_HWSizeT((size_t*)p))
        {
            return FALSE;
        }
        p += sizeof(size_t);
    }
    if (p != pEnd)
    {
        if (!Rand_HWSizeT(&s))
        {
            return FALSE;
        }
        q = (unsigned char*)&s;
        do
        {
            *p++ = *q++;
        } while (p < pEnd);
    }

    return TRUE;
}

__inline
void
Rand_Buffer(
    _Out_writes_bytes_(RandomBufferLength) void* RandomBuffer,
    _In_ unsigned int RandomBufferLength)
{
    if (!Rand_HWBuffer(RandomBuffer, RandomBufferLength))
    {
        Rand_SWBuffer(RandomBuffer, RandomBufferLength);
    }
}
