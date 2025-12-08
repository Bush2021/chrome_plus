#pragma once

#include "../../../NDK.h"

EXTERN_C_START

__inline
_Post_satisfies_(return >= 8 && return <= SECURITY_MAX_SID_SIZE)
ULONG
NTAPI
_Inline_RtlLengthSid(
    _In_ PSID Sid)
{
    return UFIELD_OFFSET(SID, SubAuthority[Sid->SubAuthorityCount]);
}

__inline
NTSTATUS
NTAPI
_Inline_RtlCopySid(
    _In_ ULONG DestinationSidLength,
    _Out_writes_bytes_(DestinationSidLength) PSID DestinationSid,
    _In_ PSID SourceSid)
{
    ULONG Length;
    
    Length = _Inline_RtlLengthSid(SourceSid);
    if (Length > DestinationSidLength)
    {
        return STATUS_BUFFER_TOO_SMALL;
    }
 
    memmove(DestinationSid, SourceSid, Length);
    return STATUS_SUCCESS;
}

_Must_inspect_result_
__inline
BOOLEAN
NTAPI
_Inline_RtlEqualSid(
    _In_ PSID Sid1,
    _In_ PSID Sid2)
{
    return Sid1->Revision == Sid2->Revision && memcmp(Sid1, Sid2, _Inline_RtlLengthSid(Sid1)) == 0;
}

EXTERN_C_END
