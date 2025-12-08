#pragma once

#include "../../NT/MinDef.h"

EXTERN_C_START

NTSYSAPI
BOOL 
NTAPI 
ApiSetQueryApiSetPresence(
    _In_ PCUNICODE_STRING Namespace,
    _Out_ PBOOLEAN Present);

NTSYSAPI
BOOL 
NTAPI 
ApiSetQueryApiSetPresenceEx(
    _In_ PCUNICODE_STRING Namespace,
    _Out_ PBOOLEAN IsInSchema,
    _Out_ PBOOLEAN Present);

typedef
_Function_class_(SWITCH_BACK_PROCEDURE)
PVOID
NTAPI
SWITCH_BACK_PROCEDURE(
    _In_opt_ PVOID Context);
typedef SWITCH_BACK_PROCEDURE *PSWITCH_BACK_PROCEDURE;

NTSYSAPI
PSWITCH_BACK_PROCEDURE
NTAPI
SbSelectProcedure(
    ULONG Signature,        // 0xABABABAB
    ULONG Unknown,          // 0? 1?
    PVOID ScenarioTable,
    ULONG ScenarioIndex);

NTSYSAPI
PVOID
NTAPI
SbExecuteProcedure(
    ULONG Signature,        // 0xABABABAB
    ULONG Unknown,          // 0? 1?
    PVOID ScenarioTable,
    ULONG ScenarioIndex,
    PVOID Context);

EXTERN_C_END
