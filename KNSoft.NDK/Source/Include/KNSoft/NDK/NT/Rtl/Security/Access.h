#pragma once

#include "../../MinDef.h"

EXTERN_C_START

/* ntddk.h */

_IRQL_requires_max_(APC_LEVEL)
NTSYSAPI
VOID
NTAPI
RtlMapGenericMask(
    _Inout_ PACCESS_MASK AccessMask,
    _In_ const GENERIC_MAPPING* GenericMapping);

/* phnt */

/**
 * Checks if all desired accesses are granted.
 *
 * This function determines whether all the accesses specified in the DesiredAccess
 * mask are granted by the GrantedAccess mask.
 *
 * @param GrantedAccess The access mask that specifies the granted accesses.
 * @param DesiredAccess The access mask that specifies the desired accesses.
 * @return Returns TRUE if all desired accesses are granted, otherwise FALSE.
 */
NTSYSAPI
BOOLEAN
NTAPI
RtlAreAllAccessesGranted(
    _In_ ACCESS_MASK GrantedAccess,
    _In_ ACCESS_MASK DesiredAccess);

/**
 * Checks if any of the desired accesses are granted.
 *
 * This function determines if any of the access rights specified in the DesiredAccess
 * mask are present in the GrantedAccess mask.
 *
 * @param GrantedAccess The access mask that specifies the granted access rights.
 * @param DesiredAccess The access mask that specifies the desired access rights.
 * @return Returns TRUE if any of the desired access rights are granted, otherwise FALSE.
 */
NTSYSAPI
BOOLEAN
NTAPI
RtlAreAnyAccessesGranted(
    _In_ ACCESS_MASK GrantedAccess,
    _In_ ACCESS_MASK DesiredAccess);

EXTERN_C_END
