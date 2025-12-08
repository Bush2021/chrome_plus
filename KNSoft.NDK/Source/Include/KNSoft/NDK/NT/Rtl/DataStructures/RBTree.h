#pragma once

#include "../../MinDef.h"

EXTERN_C_START

/* phnt */

typedef struct _RTL_RB_TREE
{
    PRTL_BALANCED_NODE Root;
    PRTL_BALANCED_NODE Min;
} RTL_RB_TREE, *PRTL_RB_TREE;

#if (NTDDI_VERSION >= NTDDI_WIN8)

NTSYSAPI
BOOLEAN
NTAPI
RtlRbInsertNodeEx(
    _In_ PRTL_RB_TREE Tree,
    _In_opt_ PRTL_BALANCED_NODE Parent,
    _In_ BOOLEAN Right,
    _Out_ PRTL_BALANCED_NODE Node);

NTSYSAPI
BOOLEAN
NTAPI
RtlRbRemoveNode(
    _In_ PRTL_RB_TREE Tree,
    _In_ PRTL_BALANCED_NODE Node);

#endif

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)

// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlCompareExchangePointerMapping( 
    _In_ PRTL_BALANCED_NODE Node1,
    _In_ PRTL_BALANCED_NODE Node2,
    _Out_ PRTL_BALANCED_NODE *Node3,
    _Out_ PRTL_BALANCED_NODE *Node4);

// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlQueryPointerMapping(
    _In_ PRTL_RB_TREE Tree,
    _Inout_ PRTL_BALANCED_NODE Children);

// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlRemovePointerMapping(
    _In_ PRTL_RB_TREE Tree,
    _Inout_ PRTL_BALANCED_NODE Children);

#endif

EXTERN_C_END
