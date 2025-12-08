#pragma once

#include "../MinDef.h"

EXTERN_C_START

/* phnt */

typedef enum _HOT_PATCH_INFORMATION_CLASS
{
    ManageHotPatchLoadPatch = 0, // MANAGE_HOT_PATCH_LOAD_PATCH
    ManageHotPatchUnloadPatch = 1, // MANAGE_HOT_PATCH_UNLOAD_PATCH
    ManageHotPatchQueryPatches = 2, // MANAGE_HOT_PATCH_QUERY_PATCHES
    ManageHotPatchLoadPatchForUser = 3, // MANAGE_HOT_PATCH_LOAD_PATCH
    ManageHotPatchUnloadPatchForUser = 4, // MANAGE_HOT_PATCH_UNLOAD_PATCH
    ManageHotPatchQueryPatchesForUser = 5, // MANAGE_HOT_PATCH_QUERY_PATCHES
    ManageHotPatchQueryActivePatches = 6, // MANAGE_HOT_PATCH_QUERY_ACTIVE_PATCHES
    ManageHotPatchApplyImagePatch = 7, // MANAGE_HOT_PATCH_APPLY_IMAGE_PATCH
    ManageHotPatchQuerySinglePatch = 8, // MANAGE_HOT_PATCH_QUERY_SINGLE_PATCH
    ManageHotPatchCheckEnabled = 9, // MANAGE_HOT_PATCH_CHECK_ENABLED
    ManageHotPatchCreatePatchSection = 10, // MANAGE_HOT_PATCH_CREATE_PATCH_SECTION
    ManageHotPatchMax
} HOT_PATCH_INFORMATION_CLASS;

typedef struct _HOT_PATCH_IMAGE_INFO
{
    ULONG CheckSum;
    ULONG TimeDateStamp;
} HOT_PATCH_IMAGE_INFO, *PHOT_PATCH_IMAGE_INFO;

typedef struct _MANAGE_HOT_PATCH_LOAD_PATCH
{
    ULONG Version;
    UNICODE_STRING PatchPath;
    union
    {
        SID Sid;
        UCHAR Buffer[SECURITY_MAX_SID_SIZE];
    } UserSid;
    HOT_PATCH_IMAGE_INFO BaseInfo;
} MANAGE_HOT_PATCH_LOAD_PATCH, *PMANAGE_HOT_PATCH_LOAD_PATCH;

typedef struct _MANAGE_HOT_PATCH_UNLOAD_PATCH
{
    ULONG Version;
    HOT_PATCH_IMAGE_INFO BaseInfo;
    union
    {
        SID Sid;
        UCHAR Buffer[SECURITY_MAX_SID_SIZE];
    } UserSid;
} MANAGE_HOT_PATCH_UNLOAD_PATCH, *PMANAGE_HOT_PATCH_UNLOAD_PATCH;

typedef struct _MANAGE_HOT_PATCH_QUERY_PATCHES
{
    ULONG Version;
    union
    {
        SID Sid;
        UCHAR Buffer[SECURITY_MAX_SID_SIZE];
    } UserSid;
    ULONG PatchCount;
    PUNICODE_STRING PatchPathStrings;
    PHOT_PATCH_IMAGE_INFO BaseInfos;
} MANAGE_HOT_PATCH_QUERY_PATCHES, *PMANAGE_HOT_PATCH_QUERY_PATCHES;

typedef struct _MANAGE_HOT_PATCH_QUERY_ACTIVE_PATCHES
{
    ULONG Version;
    HANDLE ProcessHandle;
    ULONG PatchCount;
    PUNICODE_STRING PatchPathStrings;
    PHOT_PATCH_IMAGE_INFO BaseInfos;
    PULONG PatchSequenceNumbers;
} MANAGE_HOT_PATCH_QUERY_ACTIVE_PATCHES, *PMANAGE_HOT_PATCH_QUERY_ACTIVE_PATCHES;

typedef struct _MANAGE_HOT_PATCH_APPLY_IMAGE_PATCH
{
    ULONG Version;
    union
    {
        struct
        {
            ULONG ApplyReversePatches : 1;
            ULONG ApplyForwardPatches : 1;
            ULONG Spare : 29;
        };
        ULONG AllFlags;
    };
    HANDLE ProcessHandle;
    PVOID BaseImageAddress;
    PVOID PatchImageAddress;
} MANAGE_HOT_PATCH_APPLY_IMAGE_PATCH, *PMANAGE_HOT_PATCH_APPLY_IMAGE_PATCH;

typedef struct _MANAGE_HOT_PATCH_QUERY_SINGLE_PATCH
{
    ULONG Version;
    HANDLE ProcessHandle;
    PVOID BaseAddress;
    ULONG Flags;
    UNICODE_STRING PatchPathString;
} MANAGE_HOT_PATCH_QUERY_SINGLE_PATCH, *PMANAGE_HOT_PATCH_QUERY_SINGLE_PATCH;

typedef struct _MANAGE_HOT_PATCH_CHECK_ENABLED
{
    ULONG Version;
    ULONG Flags;
} MANAGE_HOT_PATCH_CHECK_ENABLED, *PMANAGE_HOT_PATCH_CHECK_ENABLED;

typedef struct _MANAGE_HOT_PATCH_CREATE_PATCH_SECTION
{
    ULONG Version;
    ULONG Flags;
    ACCESS_MASK DesiredAccess;
    ULONG PageProtection;
    ULONG AllocationAttributes;
    PVOID BaseImageAddress;
    HANDLE SectionHandle;
} MANAGE_HOT_PATCH_CREATE_PATCH_SECTION, *PMANAGE_HOT_PATCH_CREATE_PATCH_SECTION;

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)
// rev
NTSYSCALLAPI
NTSTATUS
NTAPI
NtManageHotPatch(
    _In_ HOT_PATCH_INFORMATION_CLASS HotPatchInformationClass,
    _Out_writes_bytes_opt_(HotPatchInformationLength) PVOID HotPatchInformation,
    _In_ ULONG HotPatchInformationLength,
    _Out_opt_ PULONG ReturnLength
    );
#endif

EXTERN_C_END
