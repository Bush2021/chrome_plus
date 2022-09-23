#ifndef FAST_SEARCH_H_
#define FAST_SEARCH_H_

#include <stdint.h>

static const uint8_t *ForceSearch(const uint8_t *s, int n, const uint8_t *p)
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

static const uint8_t *SundaySearch(const uint8_t *s, int n, const uint8_t *p, int m)
{
    int i, j;

    size_t skip[256];

    for (i = 0; i < 256; i++)
    {
        skip[i] = m + 1;
    }

    for (i = 0; i < m; i++)
    {
        skip[p[i]] = m - i;
    }

    i = 0;
    while (i <= n - m)
    {
        j = 0;
        while (s[i + j] == p[j])
        {
            j++;
            if (j >= m)
            {
                return s + i;
            }
        }

        i += skip[s[i + m]];
    }

    return NULL;
}

const uint8_t *FastSearch(const uint8_t *s, int n, const uint8_t *p, int m)
{
    if (!s || !p || n < m)
        return NULL;

    if (m == 0)
    {
        return s;
    }
    else if (m == 1)
    {
        return ForceSearch(s, n, p);
    }

    return SundaySearch(s, n, p, m);
}

#endif // FAST_SEARCH_H_
