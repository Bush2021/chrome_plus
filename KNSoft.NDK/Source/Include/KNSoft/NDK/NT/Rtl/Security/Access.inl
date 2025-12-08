#pragma once

#include "../../../NDK.h"

EXTERN_C_START

__inline
VOID
NTAPI
_Inline_RtlMapGenericMask(
    _Inout_ PACCESS_MASK AccessMask,
    _In_ const GENERIC_MAPPING* GenericMapping)
{
    if (*AccessMask & GENERIC_READ)
    {
        *AccessMask |= GenericMapping->GenericRead;
    }
    if (*AccessMask & GENERIC_WRITE)
    {
        *AccessMask |= GenericMapping->GenericWrite;
    }
    if (*AccessMask & GENERIC_EXECUTE)
    {
        *AccessMask |= GenericMapping->GenericExecute;
    }
    if (*AccessMask & GENERIC_ALL)
    {
        *AccessMask |= GenericMapping->GenericAll;
    }
    *AccessMask &= ~(GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | GENERIC_ALL);
}

__inline
BOOLEAN
NTAPI
_Inline_RtlAreAllAccessesGranted(
    _In_ ACCESS_MASK GrantedAccess,
    _In_ ACCESS_MASK DesiredAccess)
{
    return (~GrantedAccess & DesiredAccess) == 0;
}

__inline
BOOLEAN
NTAPI
_Inline_RtlAreAnyAccessesGranted(
    _In_ ACCESS_MASK GrantedAccess,
    _In_ ACCESS_MASK DesiredAccess)
{
    return (GrantedAccess & DesiredAccess) != 0;
}

EXTERN_C_END
