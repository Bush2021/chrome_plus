/*
 * This demo shows delay hook, user32.dll!EqualRect will be hooked automatically when user32.dll loaded.
 * Implemented by DLL Notification mechanism, introduced in NT6.
 *
 * Run "Demo.exe -Run DelayHook".
 *
 * See also https://github.com/KNSoft/KNSoft.SlimDetours/tree/main/Docs/TechWiki/Implement%20Delay%20Hook
 */

#include "Demo.h"

#if _WIN32_WINNT >= _WIN32_WINNT_WIN6

#pragma comment(lib, "KNSoft.NDK.WinAPI.lib") // For Ldr(Register/Unregister)DllNotification imports

static HRESULT g_hrDelayAttach = E_FAIL;
static FN_EqualRect* g_pfnOrgEqualRect = NULL;

static
_Function_class_(LDR_DLL_NOTIFICATION_FUNCTION)
VOID
CALLBACK
DllLoadCallback(
    _In_ ULONG NotificationReason,
    _In_ PCLDR_DLL_NOTIFICATION_DATA NotificationData,
    _In_opt_ PVOID Context)
{
    NTSTATUS Status;

    if (NotificationReason == LDR_DLL_NOTIFICATION_REASON_LOADED &&
        RtlCompareUnicodeString((PUNICODE_STRING)NotificationData->Loaded.BaseDllName, &g_usUser32, TRUE) != 0)
    {
        return;
    }

    Status = LdrGetProcedureAddress(NotificationData->Loaded.DllBase, &g_asEqualRect, 0, (PVOID*)&g_pfnEqualRect);
    if (NT_SUCCESS(Status))
    {
        g_pfnOrgEqualRect = g_pfnEqualRect;
        g_hrDelayAttach = SlimDetoursInlineHook(TRUE, (PVOID)&g_pfnEqualRect, Hooked_EqualRect);
    } else
    {
        UnitTest_FormatMessage("LdrGetProcedureAddress failed with 0x%08lX in DllLoadCallback\n", Status);
        g_hrDelayAttach = HRESULT_FROM_NT(Status);
    }
}

TEST_FUNC(DelayHook)
{
    NTSTATUS Status;
    PVOID Cookie;
    PVOID hUser32;
    RECT rc1 = { 0 }, rc2 = { 0 };

    /* Make sure user32.dll is not loaded yet */
    Status = LdrGetDllHandle(NULL, NULL, &g_usUser32, &hUser32);
    if (NT_SUCCESS(Status))
    {
        TEST_SKIP("user32.dll is loaded, test cannot continue\n");
        return;
    } else if (Status != STATUS_DLL_NOT_FOUND)
    {
        TEST_SKIP("LdrGetDllHandle failed with 0x%08lX\n", Status);
        return;
    }

    /* Register DLL load callback */
    Status = LdrRegisterDllNotification(0, DllLoadCallback, NULL, &Cookie);
    if (!NT_SUCCESS(Status))
    {
        TEST_SKIP("LdrRegisterDllNotification failed with 0x%08lX\n", Status);
        return;
    }

    /* Load user32.dll now */
    Status = LdrLoadDll(NULL, NULL, &g_usUser32, &hUser32);
    if (!NT_SUCCESS(Status))
    {
        TEST_SKIP("LdrLoadDll failed with 0x%08lX\n", Status);
        goto _Exit;
    }

    /* Delay attach callback should be called and EqualRect is hooked successfully */
    TEST_OK(SUCCEEDED(g_hrDelayAttach));
    TEST_OK(g_pfnOrgEqualRect(&rc1, &rc2) == TRUE);
    TEST_OK(g_lEqualRectCount == 1);

    LdrUnloadDll(hUser32);
_Exit:
    LdrUnregisterDllNotification(Cookie);
}

#endif /* _WIN32_WINNT >= _WIN32_WINNT_WIN6 */
