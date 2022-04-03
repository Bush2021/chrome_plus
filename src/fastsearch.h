#ifndef FAST_SEARCH_H_
#define FAST_SEARCH_H_

#include <stdint.h>

static const uint8_t* ForceSearch(const uint8_t* s, int n, const uint8_t* p)
{
    int i;
    for (i = 0; i < n; i++)
    {
        if (*(s + i) == *p)
        {
            return s + i;
        }
    }

    return NULL;
}

static const uint8_t* BNDMSearch(const uint8_t* s, int n, const uint8_t* p, int m)
{
    int i, j;

    uint32_t maskv[256] = {0};
    for (i = 0; i < m; ++i)
    {
        maskv[p[i]] |= 1 << (m - 1 - i);
    }

    for (i = 0; i <= n - m; i += j)
    {
        uint32_t mask = maskv[s[i + m - 1]];
        for (j = m; mask;)
        {
            if (!--j) return s + i;
            mask = (mask << 1) & maskv[s[i + j - 1]];
        }
    }

    return NULL;
}

#include <emmintrin.h>

#define INTBITS (sizeof(int)*8)
static inline void setbit(void *v, int p)          // p in 0..255
{
    ((int *)v)[p / INTBITS] |= 1 << (p & (INTBITS - 1));
}

static inline __m128i xm_shl_001(__m128i x)
{
    return _mm_or_si128(_mm_slli_epi64(x, 1), _mm_srli_epi64(_mm_slli_si128(x, 8), 63));
}

static const uint8_t* SSEBNDMSearch(const uint8_t* s, int n, const uint8_t* p, int m)
{
    uint8_t *tgt = (uint8_t *) s, *pat = (uint8_t *) p;
    uint8_t *tgtend = tgt + n - m;

    int i, skip;
    __m128i mask, maskv[256], zero = {};
    int8_t used[256] = { };

    for (i = 0; i < m; ++i)
    {
        if (!used[pat[i]])
            used[pat[i]] = 1, maskv[pat[i]] = zero;
        setbit(&maskv[pat[i]], sizeof(mask) * 8 - 1 - i);
    }

    for (; tgt <= tgtend; tgt += skip)
    {
        i = skip = m;
        if (!used[tgt[--i]])
            continue;
        mask = maskv[tgt[i]];   // gteed not zero.
        do
        {
            if (0 > (int16_t) _mm_movemask_epi8(mask) && !(skip = i))
                return tgt;
            if (!used[tgt[--i]])
                break;
            mask = _mm_and_si128(xm_shl_001(mask), maskv[tgt[i]]);
        }
        while (0xFFFF != _mm_movemask_epi8(_mm_cmpeq_epi8(mask, zero)));
    }

    return NULL;
}

static const uint8_t* SundaySearch(const uint8_t* s, int n, const uint8_t* p, int m)
{
    int i;

    size_t skip[256];

    for ( i = 0; i < 256; i++ )
    {
        *(skip + i) = m + 1;
    }

    for ( i = 0; i < m; i++ )
    {
        *(skip + *(p + i)) = m - i;
    }

    while ( s < s + (n - m + 1) )
    {
        for ( i = 0; *(s + i) == *(p + i) ; )
        {
            i++;
            if (i >= m )
            {
                return s;
            }
        }

        s += *(skip + *(s + m));
    }

    return NULL;
}

const uint8_t* FastSearch(const uint8_t* s, int n, const uint8_t* p, int m)
{
    if ( !s || !p || n < m )
        return NULL;

    if (m == 0)
    {
        return s;
    }
    else if (m == 1)
    {
        return ForceSearch(s, n, p);
    }
    else if (m <= 32)
    {
        return BNDMSearch(s, n, p, m);
    }
    else if (m <= 128)
    {
        return SSEBNDMSearch(s, n, p, m);
    }

    return SundaySearch(s, n, p, m);
}

#endif // FAST_SEARCH_H_
