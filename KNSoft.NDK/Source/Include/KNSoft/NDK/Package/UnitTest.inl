#pragma once

#include "../NT/NT.h"
#include "../Win32/Def/WinBase.h"
#include "StrSafe.inl"
#include "UnitTest.h"

#pragma region Prints

static
HANDLE
UnitTest_GetStdOutput(VOID)
{
    HANDLE StdOutputHandle;

    if (NtCurrentPeb()->ProcessParameters->WindowFlags & STARTF_USEMONITOR)
    {
        return NULL;
    }
    StdOutputHandle = NtCurrentPeb()->ProcessParameters->StandardOutput;
    if (StdOutputHandle == INVALID_HANDLE_VALUE)
    {
        return NULL;
    }
    return StdOutputHandle;
}

static
VOID
UnitTest_PrintTitle(VOID)
{
    UnitTest_Print("====================================================================================================\n"
                   "KNSoft.NDK.UnitTest\n\n"
                   "    Lite-weight Unit Test Framework from KNSoft.NDK (https://github.com/KNSoft/KNSoft.NDK)\n"
                   "====================================================================================================\n\n");
}

static
VOID
UnitTest_PrintUsage(VOID)
{
    UnitTest_Print("Usage: Test_Program [-Run | -List] [TestName] [Parameters]\n\n"
                   "e.g.,\n\n"
                   "    Test_Program -List\n"
                   "        List all tests.\n\n"
                   "    Test_Program -Run\n"
                   "        Run all tests.\n\n"
                   "    Test_Program -Run TestName -Switch1 -Switch2\n"
                   "        Run the test that named TestName with two input parameters.\n\n"
                   "Exit with the count of failed tests, or 0 if no test failed.\n\n");
}

static
VOID
UnitTest_PrintList(VOID)
{
    PCUNITTEST_ENTRY Entry;
    BOOL HasEntry = FALSE;

    UnitTest_Print("Test list:\n");
    for (Entry = UnitTestList; Entry->Name.Buffer != NULL; Entry++)
    {
        if (!Entry->Manual)
        {
            UnitTest_PrintF("    %wZ\n", &Entry->Name);
        } else
        {
            UnitTest_PrintF("    %wZ (Manual)\n", &Entry->Name);
        }
        HasEntry = TRUE;
    }
    if (!HasEntry)
    {
        UnitTest_Print("    (Empty)\n");
    }
    UnitTest_Print("\n");
}

#pragma endregion

#pragma region Entry API

BOOL
NTAPI
UnitTest_EnumEntries(
    _In_ __callback FN_UNITTEST_ENUM_PROC* Callback,
    _In_opt_ PVOID Context)
{
    PCUNITTEST_ENTRY Entry;
    BOOL Ret;

    for (Entry = UnitTestList; Entry->Name.Buffer != NULL; Entry++)
    {
        Ret = Callback(Entry, Context);
        if (!Ret)
        {
            return Ret;
        }
    }

    return TRUE;
}

_Ret_maybenull_
_Must_inspect_result_
PCUNITTEST_ENTRY
NTAPI
UnitTest_FindEntry(
    _In_z_ PCWSTR Name)
{
    PCUNITTEST_ENTRY Entry;
    UNICODE_STRING NameString;

    RtlInitUnicodeString(&NameString, Name);

    for (Entry = UnitTestList; Entry->Name.Buffer != NULL; Entry++)
    {
        if (RtlEqualUnicodeString((PUNICODE_STRING)&Entry->Name, &NameString, FALSE))
        {
            return Entry;
        }
    }

    return NULL;
}

#pragma endregion

#pragma region Execute API

VOID
NTAPI
UnitTest_RunEntry(
    _In_ PCUNITTEST_ENTRY Entry,
    _Out_ PUNITTEST_RESULT Result,
    _In_ INT ArgC,
    _In_reads_(ArgC) _Pre_z_ PCWSTR* ArgV)
{
    LARGE_INTEGER PrefCounter1, PrefCounter2, PrefFreq;
    ULONGLONG ElapsedMicroseconds;

    UnitTest_FormatMessage(">>>> Running unit test: %wZ\n", &Entry->Name);
    RtlZeroMemory(Result, sizeof(*Result));

    RtlQueryPerformanceCounter(&PrefCounter1);
    Entry->Proc(Result, ArgC, ArgV);
    RtlQueryPerformanceCounter(&PrefCounter2);
    if (SharedUserData->NtMajorVersion > 6 ||
        SharedUserData->NtMajorVersion == 6 && SharedUserData->NtMinorVersion >= 2)
    {
        PrefFreq.QuadPart = SharedUserData->QpcFrequency;
    } else
    {
        LARGE_INTEGER PerformanceCounter;
        NtQueryPerformanceCounter(&PerformanceCounter, &PrefFreq);
    }

    /* Convert to microseconds before dividing for avoiding loss-of-precision */
    ElapsedMicroseconds = (ULONGLONG)PrefCounter2.QuadPart - (ULONGLONG)PrefCounter1.QuadPart;
    ElapsedMicroseconds *= 1000000;
    ElapsedMicroseconds = (ULONGLONG)((ElapsedMicroseconds / (DOUBLE)PrefFreq.QuadPart) + (DOUBLE)0.5);
    Result->Elapsed = ElapsedMicroseconds;
    UnitTest_FormatMessage("<<<< Result: %lu tests executed (%lu passed, %lu failed, %lu skipped) in %llu ms (%llu us)\n\n",
                           Result->Pass + Result->Fail + Result->Skip,
                           Result->Pass,
                           Result->Fail,
                           Result->Skip,
                           (ULONGLONG)((ElapsedMicroseconds / 1000.0F) + 0.5F),
                           Result->Elapsed);
}

ULONG
NTAPI
UnitTest_RunAll(
    _Out_ PUNITTEST_RESULT Result)
{
    ULONG Ret = 0;
    PCUNITTEST_ENTRY Entry;
    UNITTEST_RESULT EntryResult;

    RtlZeroMemory(Result, sizeof(*Result));

    for (Entry = UnitTestList; Entry->Name.Buffer != NULL; Entry++)
    {
        if (!Entry->Manual)
        {
            UnitTest_RunEntry(Entry, &EntryResult, 0, NULL);
            Result->Pass += EntryResult.Pass;
            Result->Fail += EntryResult.Fail;
            Result->Skip += EntryResult.Skip;
            Result->Elapsed += EntryResult.Elapsed;
            Ret++;
        }
    }

    if (Ret > 0)
    {
        UnitTest_FormatMessage("Totally %lu test entries ran, %lu tests executed (%lu passed, %lu failed, %lu skipped) in %llu ms\n",
                               Ret,
                               Result->Pass + Result->Fail + Result->Skip,
                               Result->Pass,
                               Result->Fail,
                               Result->Skip,
                               (ULONGLONG)((Result->Elapsed / 1000.0F) + 0.5F));
    }
    return Ret;
}

_Success_(return != FALSE)
BOOL
NTAPI
UnitTest_Run(
    _In_z_ PCWSTR Name,
    _Out_ PUNITTEST_RESULT Result,
    _In_ INT ArgC,
    _In_reads_(ArgC) _Pre_z_ PCWSTR * ArgV)
{
    PCUNITTEST_ENTRY Entry = UnitTest_FindEntry(Name);

    if (Entry == NULL)
    {
        return FALSE;
    }

    UnitTest_RunEntry(Entry, Result, ArgC, ArgV);
    return TRUE;
}

_Success_(return == 0)
INT
NTAPI
UnitTest_Main(
    _In_ int argc,
    _In_reads_(argc) _Pre_z_ wchar_t** argv)
{
    UNITTEST_RESULT Result;
    PCUNITTEST_ENTRY Entry;

    UnitTest_PrintTitle();
    if (argc > 1)
    {
        if (_wcsicmp(argv[1], L"-List") == 0)
        {
            UnitTest_PrintList();
            return 0;
        } else if (_wcsicmp(argv[1], L"-Run") == 0)
        {
            if (argc == 2)
            {
                if (UnitTest_RunAll(&Result) == 0)
                {
                    UnitTest_Print("No entry found\n\n");
                    return (INT)STATUS_NOT_FOUND;
                }
                return Result.Fail;
            } else if (argc >= 3)
            {
                Entry = UnitTest_FindEntry(argv[2]);
                if (Entry == NULL)
                {
                    UnitTest_PrintF("Entry \"%ls\" not found\n\n", argv[2]);
                    UnitTest_PrintList();
                    return (INT)STATUS_NOT_FOUND;
                }
                UnitTest_RunEntry(Entry, &Result, argc - 3, (PCWSTR*)(argv + 3));
                return Result.Fail;
            }
        }
    }

    UnitTest_Print("Invalid parameter\n\n");
    UnitTest_PrintUsage();
    return (INT)STATUS_INVALID_PARAMETER;
}

#pragma endregion

#pragma region Utils

static
VOID
__cdecl
UnitTest_PrintFV(
    _In_z_ _Printf_format_string_ PCSTR Format,
    _In_ va_list ArgList)
{
    CHAR sz[512 + 1]; // Same limitation as DbgPrint
    ULONG u, uNew;
    HANDLE hStdOut;
    PSTR psz;
    IO_STATUS_BLOCK IoStatusBlock;

    /* Write standard output if exists */
    hStdOut = UnitTest_GetStdOutput();
    if (hStdOut == NULL)
    {
        return;
    }

    /* Format string */
    u = StrSafe_CchVPrintfA(sz, ARRAYSIZE(sz), Format, ArgList);
    if (u == 0)
    {
        return;
    }

    /* Allocate buffer if sz too small */
    if (u >= ARRAYSIZE(sz))
    {
        psz = (PSTR)RtlAllocateHeap(RtlProcessHeap(), 0, (SIZE_T)u + 1);
        if (psz != NULL)
        {
            uNew = StrSafe_CchVPrintfA(psz, (SIZE_T)u + 1, Format, ArgList);
            if (uNew > 0 && uNew < u)
            {
                u = uNew;
                goto _Print_Stdout;
            }
            RtlFreeHeap(RtlProcessHeap(), 0, psz);
        }

        /* New allocated buffer unavailable, fallback to sz (truncated) */
        u = ARRAYSIZE(sz) - 1;
    }

    psz = sz;

_Print_Stdout:
    NtWriteFile(hStdOut, NULL, NULL, NULL, &IoStatusBlock, psz, u, NULL, NULL);
    if (psz != sz)
    {
        RtlFreeHeap(RtlProcessHeap(), 0, psz);
    }
}

VOID
NTAPI
UnitTest_PrintEx(
    _In_reads_bytes_(TextSize) PCCH Text,
    _In_ ULONG TextSize)
{
    HANDLE StdOutHandle;
    IO_STATUS_BLOCK IoStatusBlock;

    StdOutHandle = UnitTest_GetStdOutput();
    if (StdOutHandle != NULL)
    {
        NtWriteFile(StdOutHandle, NULL, NULL, NULL, &IoStatusBlock, (PVOID)Text, TextSize, NULL, NULL);
    }
}

VOID
__cdecl
UnitTest_PrintF(
    _In_z_ _Printf_format_string_ PCSTR Format,
    ...)
{
    va_list ArgList;

    va_start(ArgList, Format);
    UnitTest_PrintFV(Format, ArgList);
    va_end(ArgList);
}

VOID
__cdecl
UnitTest_FormatMessage(
    _In_z_ _Printf_format_string_ PCSTR Format,
    ...)
{
    va_list ArgList;

    va_start(ArgList, Format);
    vDbgPrintEx(MAXULONG, DPFLTR_ERROR_LEVEL, Format, ArgList);
    UnitTest_PrintFV(Format, ArgList);
    va_end(ArgList);
}

#pragma endregion
