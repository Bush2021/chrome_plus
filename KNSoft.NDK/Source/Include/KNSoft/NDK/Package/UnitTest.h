/*
 * KNSoft.NDK UnitTest.h package, licensed under the MIT license.
 * Copyright (c) KNSoft.org (https://github.com/KNSoft). All rights reserved.
 * 
 * Lite weight unit test framework.
 */

#pragma once

#include "../NT/MinDef.h"

EXTERN_C_START

typedef struct _UNITTEST_RESULT
{
    ULONG Pass;
    ULONG Fail;
    ULONG Skip;
    ULONGLONG Elapsed; // in μs (us, microsecond)
} UNITTEST_RESULT, *PUNITTEST_RESULT;

typedef
_Function_class_(FN_UNITTEST_PROC)
VOID
NTAPI
FN_UNITTEST_PROC(
    UNITTEST_RESULT* Result,
    _In_ INT ArgC,
    _In_reads_(ArgC) _Pre_z_ PCWSTR* ArgV);

typedef struct _UNITTEST_ENTRY
{
    FN_UNITTEST_PROC* Proc;
    UNICODE_STRING Name;
    BOOLEAN Manual;
} UNITTEST_ENTRY, *PUNITTEST_ENTRY;
typedef const UNITTEST_ENTRY *PCUNITTEST_ENTRY;

typedef
_Function_class_(FN_UNITTEST_ENUM_PROC)
BOOL
CALLBACK
FN_UNITTEST_ENUM_PROC(
    _In_ PCUNITTEST_ENTRY Entry,
    _In_opt_ PVOID Context);

BOOL
NTAPI
UnitTest_EnumEntries(
    _In_ __callback FN_UNITTEST_ENUM_PROC* Callback,
    _In_opt_ PVOID Context);

_Ret_maybenull_
_Must_inspect_result_
PCUNITTEST_ENTRY
NTAPI
UnitTest_FindEntry(
    _In_z_ PCWSTR Name);

VOID
NTAPI
UnitTest_RunEntry(
    _In_ PCUNITTEST_ENTRY Entry,
    _Out_ PUNITTEST_RESULT Result,
    _In_ INT ArgC,
    _In_reads_(ArgC) _Pre_z_ PCWSTR* ArgV);

ULONG
NTAPI
UnitTest_RunAll(
    _Out_ PUNITTEST_RESULT Result);

_Success_(return != FALSE)
BOOL
NTAPI
UnitTest_Run(
    _In_z_ PCWSTR Name,
    _Out_ PUNITTEST_RESULT Result,
    _In_ INT ArgC,
    _In_reads_(ArgC) _Pre_z_ PCWSTR* ArgV);

_Success_(return == 0)
INT
NTAPI
UnitTest_Main(
    _In_ int argc,
    _In_reads_(argc) _Pre_z_ wchar_t** argv);

/* Print string to stdout */
VOID
NTAPI
UnitTest_PrintEx(
    _In_reads_bytes_(TextSize) PCCH Text,
    _In_ ULONG TextSize);

#define UnitTest_Print(Text) UnitTest_PrintEx(Text, sizeof(Text))

/* Format string and print to stdout */
VOID
__cdecl
UnitTest_PrintF(
    _In_z_ _Printf_format_string_ PCSTR Format,
    ...);

/* Format string and output to both of stdout and debugger */
VOID
__cdecl
UnitTest_FormatMessage(
    _In_z_ _Printf_format_string_ PCSTR Format,
    ...);

/* Define a test */

#define TEST_PARAMETER_RESULT _KNSoft_NDK_UnitTest_Result
#define TEST_PARAMETER_ARGC _KNSoft_NDK_UnitTest_ArgC
#define TEST_PARAMETER_ARGV _KNSoft_NDK_UnitTest_ArgV

#define TEST_FUNC(Name) EXTERN_C VOID NTAPI Name(UNITTEST_RESULT* TEST_PARAMETER_RESULT, _In_ INT TEST_PARAMETER_ARGC, _In_reads_(TEST_PARAMETER_ARGC) _Pre_z_ PCWSTR* TEST_PARAMETER_ARGV)
#define TEST_DECL_FUNC(Name) EXTERN_C FN_UNITTEST_PROC Name
#define TEST_DECL_ENTRY(Name) { Name, RTL_CONSTANT_STRING(L###Name), FALSE }

/* Ignored by "Run All", only can be run by "-Run Name" explicitly */
#define TEST_DECL_MANUAL_ENTRY(Name) { Name, RTL_CONSTANT_STRING(L###Name), TRUE }

/* Global test list */
EXTERN_C CONST UNITTEST_ENTRY UnitTestList[];

/* Increase count of test result, parameter can be Pass/Fail/Skip */
#define TEST_RESULT(r) (TEST_PARAMETER_RESULT->r++)

/* Pass if Expr is True, or fail and print assertion otherwise */
#define TEST_OK(Expr) (Expr ? (VOID)TEST_RESULT(Pass) : (TEST_RESULT(Fail), UnitTest_FormatMessage("%hs (Line %d) Assertion failed: %hs\n", __FILE__, __LINE__, #Expr)))

/* Skip and print message */
#define TEST_SKIP(Format, ...) (TEST_RESULT(Skip), UnitTest_FormatMessage("%hs (Line %d) Skipped: "Format"\n", __FILE__, __LINE__, ##__VA_ARGS__))

/* Fail and print message */
#define TEST_FAIL(Format, ...) (TEST_RESULT(Fail), UnitTest_FormatMessage("%hs (Line %d) Failed: "Format"\n", __FILE__, __LINE__, ##__VA_ARGS__))

EXTERN_C_END
