#pragma once

#include "../MinDef.h"
#include "Process/EnvironmentBlock.h"

EXTERN_C_START

/* ntddk.h */
NTSYSAPI
VOID
NTAPI
RtlGetCallersAddress(
    _Out_ PVOID* CallersAddress,
    _Out_ PVOID* CallersCaller);

/* phnt */

NTSYSAPI
VOID
NTAPI
RtlPushFrame(
    _In_ PTEB_ACTIVE_FRAME Frame);

NTSYSAPI
VOID
NTAPI
RtlPopFrame(
    _In_ PTEB_ACTIVE_FRAME Frame);

NTSYSAPI
PTEB_ACTIVE_FRAME
NTAPI
RtlGetFrame(VOID);

/* ntddk.h & phnt */

#define RTL_WALK_USER_MODE_STACK 0x00000001
#define RTL_WALK_KERNEL_STACK 0x00000002
#define RTL_WALK_USER_KERNEL_STACK 0x00000003
#define RTL_WALK_VALID_FLAGS 0x00000006

#define RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT 8

NTSYSAPI
ULONG
NTAPI
RtlWalkFrameChain(
    _Out_writes_(Count - (Flags >> RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT)) PVOID *Callers,
    _In_ ULONG Count,
    _In_ ULONG Flags);

EXTERN_C_END
