#include "Demo.h"

#include <KNSoft/NDK/Package/UnitTest.inl>

TEST_DECL_FUNC(COMHook);
TEST_DECL_FUNC(DeadLock);
TEST_DECL_FUNC(TwiceSimpleHook);
TEST_DECL_FUNC(Instruction);
#if _WIN32_WINNT >= _WIN32_WINNT_WIN6
TEST_DECL_FUNC(DelayHook);
#endif

CONST UNITTEST_ENTRY UnitTestList[] = {
    TEST_DECL_ENTRY(COMHook),
    TEST_DECL_ENTRY(DeadLock),
    TEST_DECL_ENTRY(TwiceSimpleHook),
    TEST_DECL_ENTRY(Instruction),
#if _WIN32_WINNT >= _WIN32_WINNT_WIN6
    TEST_DECL_ENTRY(DelayHook),
#endif
    { 0 }
};

#define DEMO_SWITCH_ENGINE L"-Engine="

FN_EqualRect* g_pfnEqualRect = NULL;
UNICODE_STRING g_usUser32 = RTL_CONSTANT_STRING(L"user32.dll");
ANSI_STRING g_asEqualRect = RTL_CONSTANT_STRING("EqualRect");
LONG volatile g_lEqualRectCount = 0;
LONG volatile g_lEqualRectRefCount = 0;

int _cdecl wmain(
    _In_ int argc,
    _In_reads_(argc) _Pre_z_ wchar_t** argv)
{
    return UnitTest_Main(argc, argv);
}

HRESULT GetEngineTypeFromArgs(
    _In_ INT ArgC,
    _In_reads_(ArgC) _Pre_z_ PCWSTR* ArgV,
    _Out_ PDEMO_ENGINE_TYPE EngineType)
{
    INT i;
    PCWSTR pszEngineType;

    for (i = 0; i < ArgC; i++)
    {
        if (_wcsnicmp(ArgV[i], DEMO_SWITCH_ENGINE, _STR_LEN(DEMO_SWITCH_ENGINE)) == 0)
        {
            pszEngineType = ArgV[i] + _STR_LEN(DEMO_SWITCH_ENGINE);
            if (_wcsicmp(pszEngineType, L"SlimDetours") == 0)
            {
                *EngineType = EngineSlimDetours;
                return S_OK;
            } else if (_wcsicmp(pszEngineType, L"MSDetours") == 0)
            {
                *EngineType = EngineMicrosoftDetours;
                return S_OK;
            }
            break;
        }
    }

    return E_INVALIDARG;
}

NTSTATUS LoadEqualRect(VOID)
{
    NTSTATUS Status;
    PVOID hUser32;

    Status = LdrLoadDll(NULL, NULL, &g_usUser32, &hUser32);
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }
    return LdrGetProcedureAddress(hUser32, &g_asEqualRect, 0, (PVOID*)&g_pfnEqualRect);
}

BOOL
WINAPI
Hooked_EqualRect(
    _In_ CONST RECT *lprc1,
    _In_ CONST RECT *lprc2)
{
    BOOL Ret;

    _InterlockedIncrement(&g_lEqualRectCount);
    UnitTest_FormatMessage("Hooked EqualRect enter: lprc1 = (%ld, %ld, %ld, %ld), lprc2 = (%ld, %ld, %ld, %ld)\n",
                           lprc1->top,
                           lprc1->right,
                           lprc1->bottom,
                           lprc1->left,
                           lprc2->top,
                           lprc2->right,
                           lprc2->bottom,
                           lprc2->left);
    _InterlockedIncrement(&g_lEqualRectRefCount);
    Ret = g_pfnEqualRect(lprc1, lprc2);
    _InterlockedDecrement(&g_lEqualRectRefCount);
    UnitTest_FormatMessage("Hooked EqualRect leave with return value: %ld\n", Ret);
    return Ret;
}

HRESULT HookTransactionBegin(
    _In_ DEMO_ENGINE_TYPE EngineType)
{
    if (EngineType == EngineSlimDetours)
    {
        return SlimDetoursTransactionBegin();
    } else if (EngineType == EngineMicrosoftDetours)
    {
        return HRESULT_FROM_WIN32(DetourTransactionBegin());
    }

    return E_NOTIMPL;
}

HRESULT HookTransactionAbort(
    _In_ DEMO_ENGINE_TYPE EngineType)
{
    if (EngineType == EngineSlimDetours)
    {
        return SlimDetoursTransactionAbort();
    } else if (EngineType == EngineMicrosoftDetours)
    {
        return HRESULT_FROM_WIN32(DetourTransactionAbort());
    }

    return E_NOTIMPL;
}

HRESULT HookTransactionCommit(
    _In_ DEMO_ENGINE_TYPE EngineType)
{
    if (EngineType == EngineSlimDetours)
    {
        return SlimDetoursTransactionCommit();
    } else if (EngineType == EngineMicrosoftDetours)
    {
        return HRESULT_FROM_WIN32(DetourTransactionCommit());
    }

    return E_NOTIMPL;
}

HRESULT HookAttach(
    _In_ DEMO_ENGINE_TYPE EngineType,
    _In_ BOOL Enable,
    _Inout_ PVOID* ppPointer,
    _In_ PVOID pDetour)
{
    if (EngineType == EngineSlimDetours)
    {
        return Enable ? SlimDetoursAttach(ppPointer, pDetour) : SlimDetoursDetach(ppPointer, pDetour);
    } else if (EngineType == EngineMicrosoftDetours)
    {
        return HRESULT_FROM_WIN32(Enable ? DetourAttach(ppPointer, pDetour) : DetourDetach(ppPointer, pDetour));
    }

    return E_NOTIMPL;
}
