#pragma once

#include <KNSoft/NDK/NDK.h>
#include <KNSoft/NDK/Package/UnitTest.h>
#include <KNSoft/NDK/Package/StrSafe.inl>

#include "../KNSoft.SlimDetours/SlimDetours.h"
#include "../Microsoft.Detours/src/detours.h"

EXTERN_C_START

typedef
BOOL
WINAPI
FN_EqualRect(
    _In_ CONST RECT *lprc1,
    _In_ CONST RECT *lprc2);

typedef enum _DEMO_ENGINE_TYPE
{
    EngineInvalid,
    EngineSlimDetours,
    EngineMicrosoftDetours
} DEMO_ENGINE_TYPE, *PDEMO_ENGINE_TYPE;

EXTERN_C FN_EqualRect* g_pfnEqualRect;
EXTERN_C UNICODE_STRING g_usUser32;
EXTERN_C ANSI_STRING g_asEqualRect;
EXTERN_C LONG volatile g_lEqualRectCount;
EXTERN_C LONG volatile g_lEqualRectRefCount;

NTSTATUS LoadEqualRect(VOID);

BOOL
WINAPI
Hooked_EqualRect(
    _In_ CONST RECT *lprc1,
    _In_ CONST RECT *lprc2);

HRESULT GetEngineTypeFromArgs(
    _In_ INT ArgC,
    _In_reads_(ArgC) _Pre_z_ PCWSTR* ArgV,
    _Out_ PDEMO_ENGINE_TYPE EngineType);

HRESULT HookTransactionBegin(
    _In_ DEMO_ENGINE_TYPE EngineType);

HRESULT HookTransactionAbort(
    _In_ DEMO_ENGINE_TYPE EngineType);

HRESULT HookTransactionCommit(
    _In_ DEMO_ENGINE_TYPE EngineType);

HRESULT HookAttach(
    _In_ DEMO_ENGINE_TYPE EngineType,
    _In_ BOOL Enable,
    _Inout_ PVOID* ppPointer,
    _In_ PVOID pDetour);

EXTERN_C_END
