#pragma once

#include "../../NDK.h"

EXTERN_C_START

EXTERN_C const DECLSPEC_SELECTANY PCWSTR _Inline_RtlNtdllName = L"ntdll.dll";
EXTERN_C const DECLSPEC_SELECTANY UNICODE_STRING _Inline_RtlDosPathSeperatorsString = RTL_CONSTANT_STRING(L"\\/");
EXTERN_C const DECLSPEC_SELECTANY UNICODE_STRING _Inline_RtlAlternateDosPathSeperatorString = RTL_CONSTANT_STRING(L"/");
EXTERN_C const DECLSPEC_SELECTANY UNICODE_STRING _Inline_RtlNtPathSeperatorString = RTL_CONSTANT_STRING(L"\\");

EXTERN_C_END
