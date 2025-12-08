#pragma once

#include "../../MinDef.h"

EXTERN_C_START

/* phnt */

typedef struct _CONTEXT_CHUNK
{
    LONG Offset; // Offset may be negative.
    ULONG Length;
} CONTEXT_CHUNK, *PCONTEXT_CHUNK;

typedef struct _CONTEXT_EX
{
    CONTEXT_CHUNK All;
    CONTEXT_CHUNK Legacy;
    CONTEXT_CHUNK XState;
    CONTEXT_CHUNK KernelCet;
} CONTEXT_EX, *PCONTEXT_EX;

#if defined(_AMD64_) || defined(_ARM64_) || defined(_ARM64EC_)
#define CONTEXT_ALIGN 0x10
#else
#define CONTEXT_ALIGN 0x8
#endif

#if defined(_AMD64_)
#define CONTEXT_FRAME_LENGTH 0x4D0
#define CONTEXT_EX_PADDING   0x10
#elif defined(_ARM64_) || defined(_ARM64EC_)
#define CONTEXT_FRAME_LENGTH 0x390
#define CONTEXT_EX_PADDING   0x10
#elif defined(_M_ARM)
#define CONTEXT_FRAME_LENGTH 0x1a0
#define CONTEXT_EX_PADDING   0x8
#else
#define CONTEXT_FRAME_LENGTH 0x2CC
#define CONTEXT_EX_PADDING   0x4
#endif

#define CONTEXT_ALIGNMENT(Size, Align) \
    (((ULONG_PTR)(Size) + (Align) - 1) & ~((Align) - 1))

#define CONTEXT_EX_LENGTH \
    CONTEXT_ALIGNMENT(sizeof(CONTEXT_EX), CONTEXT_ALIGN)

_STATIC_ASSERT(CONTEXT_FRAME_LENGTH == sizeof(CONTEXT));
_STATIC_ASSERT(CONTEXT_EX_LENGTH == 0x20);

#define RTL_CONTEXT_EX_OFFSET(ContextEx, Chunk) ((ContextEx)->Chunk.Offset)
#define RTL_CONTEXT_EX_LENGTH(ContextEx, Chunk) ((ContextEx)->Chunk.Length)
#define RTL_CONTEXT_EX_CHUNK(Base, Layout, Chunk) ((PVOID)((PUCHAR)(Base) + RTL_CONTEXT_EX_OFFSET(Layout, Chunk)))
#define RTL_CONTEXT_OFFSET(Context, Chunk) RTL_CONTEXT_EX_OFFSET((PCONTEXT_EX)((Context) + 1), Chunk)
#define RTL_CONTEXT_LENGTH(Context, Chunk) RTL_CONTEXT_EX_LENGTH((PCONTEXT_EX)((Context) + 1), Chunk)
#define RTL_CONTEXT_CHUNK(Context, Chunk) RTL_CONTEXT_EX_CHUNK((PCONTEXT_EX)((Context) + 1), (PCONTEXT_EX)((Context) + 1), Chunk)

#if defined(_M_AMD64)
// returns constant 0xf0e0d0c0a0908070 (dmex)
NTSYSAPI
ULONG64
NTAPI
RtlInitializeContext(
    _Reserved_ HANDLE Reserved,
    _Out_ PCONTEXT Context,
    _In_opt_ PVOID Parameter,
    _In_opt_ PVOID InitialPc,
    _In_opt_ PVOID InitialSp
);
#else
// returns status of NtWriteVirtualMemory (dmex)
NTSYSAPI
NTSTATUS
NTAPI
RtlInitializeContext(
    _In_ HANDLE ProcessHandle,
    _Out_ PCONTEXT Context,
    _In_opt_ PVOID Parameter,
    _In_opt_ PVOID InitialPc,
    _In_opt_ PVOID InitialSp
);
#endif

NTSYSAPI
NTSTATUS
NTAPI
RtlInitializeExtendedContext(
    _Out_ PCONTEXT Context,
    _In_ ULONG ContextFlags,
    _Out_ PCONTEXT_EX* ContextEx);

NTSYSAPI
NTSTATUS
NTAPI
RtlInitializeExtendedContext2(
    _Out_ PCONTEXT Context,
    _In_ ULONG ContextFlags,
    _Out_ PCONTEXT_EX* ContextEx,
    _In_ ULONG64 EnabledExtendedFeatures // RtlGetEnabledExtendedFeatures(-1)
);

NTSYSAPI
NTSTATUS
NTAPI
RtlCopyContext(
    _Inout_ PCONTEXT Context,
    _In_ ULONG ContextFlags,
    _Out_ PCONTEXT Source);

NTSYSAPI
NTSTATUS
NTAPI
RtlCopyExtendedContext(
    _Out_ PCONTEXT_EX Destination,
    _In_ ULONG ContextFlags,
    _In_ PCONTEXT_EX Source);

NTSYSAPI
NTSTATUS
NTAPI
RtlGetExtendedContextLength(
    _In_ ULONG ContextFlags,
    _Out_ PULONG ContextLength);

NTSYSAPI
NTSTATUS
NTAPI
RtlGetExtendedContextLength2(
    _In_ ULONG ContextFlags,
    _Out_ PULONG ContextLength,
    _In_ ULONG64 EnabledExtendedFeatures // RtlGetEnabledExtendedFeatures(-1)
);

NTSYSAPI
ULONG64
NTAPI
RtlGetExtendedFeaturesMask(
    _In_ PCONTEXT_EX ContextEx);

NTSYSAPI
PVOID
NTAPI
RtlLocateExtendedFeature(
    _In_ PCONTEXT_EX ContextEx,
    _In_ ULONG FeatureId,
    _Out_opt_ PULONG Length);

NTSYSAPI
PVOID
NTAPI
RtlLocateExtendedFeature2(
    _In_ PCONTEXT_EX ContextEx,
    _In_ ULONG FeatureId,
    _In_ XSTATE_CONFIGURATION XState,
    _Out_opt_ PULONG Length);

NTSYSAPI
PCONTEXT
NTAPI
RtlLocateLegacyContext(
    _In_ PCONTEXT_EX ContextEx,
    _Out_opt_ PULONG Length);

NTSYSAPI
VOID
NTAPI
RtlSetExtendedFeaturesMask(
    _In_ PCONTEXT_EX ContextEx,
    _In_ ULONG64 FeatureMask);

EXTERN_C_END
