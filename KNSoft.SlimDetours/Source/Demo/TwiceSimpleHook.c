/*
 * This demo hooks user32.dll!EqualRect twice. The latter detour will be called before the former one.
 * 
 * Run "Demo.exe -Run TwiceSimpleHook".
 */

#include "Demo.h"

static LONG volatile g_lEqualRect = 0;
static FN_EqualRect* g_pfnEqualRect1 = NULL;
static FN_EqualRect* g_pfnEqualRect2 = NULL;

static
BOOL
WINAPI
Hooked_EqualRect1(
    _In_ CONST RECT *lprc1,
    _In_ CONST RECT *lprc2)
{
    /* The former detour will be called after the latter */
    _InterlockedCompareExchange(&g_lEqualRect, 2, 1);
    return g_pfnEqualRect1(lprc1, lprc2);
}

static
BOOL
WINAPI
Hooked_EqualRect2(
    _In_ CONST RECT *lprc1,
    _In_ CONST RECT *lprc2)
{
    /* The latter detour will be called before the former */
    _InterlockedCompareExchange(&g_lEqualRect, 1, 0);
    return g_pfnEqualRect2(lprc1, lprc2);
}

TEST_FUNC(TwiceSimpleHook)
{
    NTSTATUS Status;
    HRESULT hr;
    RECT rc = { 0 };

    Status = LoadEqualRect();
    if (!NT_SUCCESS(Status))
    {
        TEST_SKIP("Load user32.dll!EqualRect failed with 0x%08lX\n", Status);
        return;
    }

    g_pfnEqualRect1 = g_pfnEqualRect2 = g_pfnEqualRect;
    hr = SlimDetoursInlineHook(TRUE, (PVOID*)&g_pfnEqualRect1, Hooked_EqualRect1);
    if (FAILED(hr))
    {
        TEST_SKIP("1st SlimDetoursInlineHook failed with 0x%08lX\n", hr);
        return;
    }
    hr = SlimDetoursInlineHook(TRUE, (PVOID*)&g_pfnEqualRect2, Hooked_EqualRect2);
    if (FAILED(hr))
    {
        TEST_SKIP("2nd SlimDetoursInlineHook failed with 0x%08lX\n", hr);
        return;
    }

    TEST_OK(g_pfnEqualRect(&rc, &rc) != FALSE);
    TEST_OK(g_lEqualRect == 2);

    TEST_OK(SUCCEEDED(SlimDetoursInlineHook(FALSE, (PVOID*)&g_pfnEqualRect2, Hooked_EqualRect2)));
    TEST_OK(SUCCEEDED(SlimDetoursInlineHook(FALSE, (PVOID*)&g_pfnEqualRect1, Hooked_EqualRect1)));
}
