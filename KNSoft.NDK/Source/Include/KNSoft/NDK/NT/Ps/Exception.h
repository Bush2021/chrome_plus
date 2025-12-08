#pragma once

#include "../MinDef.h"

EXTERN_C_START

#pragma region Exception

NTSYSCALLAPI
NTSTATUS
NTAPI
NtContinue(
    _In_ PCONTEXT ContextRecord,
    _In_ BOOLEAN TestAlert);

#if (NTDDI_VERSION >= NTDDI_WINTHRESHOLD)

typedef enum _KCONTINUE_TYPE
{
    KCONTINUE_UNWIND,
    KCONTINUE_RESUME,
    KCONTINUE_LONGJUMP,
    KCONTINUE_SET,
    KCONTINUE_LAST,
} KCONTINUE_TYPE;

#define KCONTINUE_FLAG_TEST_ALERT 0x00000001 // wbenny
#define KCONTINUE_FLAG_DELIVER_APC 0x00000002 // wbenny

typedef struct _KCONTINUE_ARGUMENT
{
    KCONTINUE_TYPE ContinueType;
    ULONG ContinueFlags;
    ULONGLONG Reserved[2];
} KCONTINUE_ARGUMENT, *PKCONTINUE_ARGUMENT;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtContinueEx(
    _In_ PCONTEXT ContextRecord,
    _In_ PVOID ContinueArgument // "PKCONTINUE_ARGUMENT ContinueArgument" and "BOOLEAN TestAlert" are valid
);

//FORCEINLINE
//NTSTATUS
//NtContinue(
//    _In_ PCONTEXT ContextRecord,
//    _In_ BOOLEAN TestAlert
//    )
//{
//    return NtContinueEx(ContextRecord, (PCONTINUE_ARGUMENT)TestAlert);
//}

#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtRaiseException(
    _In_ PEXCEPTION_RECORD ExceptionRecord,
    _In_ PCONTEXT ContextRecord,
    _In_ BOOLEAN FirstChance);

#pragma endregion phnt

EXTERN_C_END
