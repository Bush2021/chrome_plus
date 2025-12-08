/*
 * Test for the KNSoft.NDK PEB/TEB/KUSER_SHARED_DATA structure definitions
 */

#include "../UnitTest.h"

TEST_FUNC(PebStruct)
{
    NTSTATUS Status;
    PPEB Peb = NtCurrentPeb();

    PROCESS_BASIC_INFORMATION PBI;
    Status = NtQueryInformationProcess(NtCurrentProcess(),
                                       ProcessBasicInformation,
                                       &PBI,
                                       sizeof(PBI),
                                       NULL);
    if (NT_SUCCESS(Status))
    {
        TEST_OK(Peb == PBI.PebBaseAddress);
    } else
    {
        TEST_SKIP("NtQueryInformationProcess failed with 0x%08lX, skip test for Peb\n", Status);
    }

    TEST_OK(Peb->ImageBaseAddress == &__ImageBase);
    TEST_OK(Peb->ProcessHeap == GetProcessHeap());

    PIMAGE_NT_HEADERS pNtHeader = Add2Ptr(&__ImageBase, __ImageBase.e_lfanew);
    TEST_OK(Peb->ImageSubsystemMajorVersion == (ULONG)pNtHeader->OptionalHeader.MajorSubsystemVersion);
    TEST_OK(Peb->ImageSubsystemMinorVersion == (ULONG)pNtHeader->OptionalHeader.MinorSubsystemVersion);

    PROCESS_SESSION_INFORMATION PSI;
    Status = NtQueryInformationProcess(NtCurrentProcess(),
                                       ProcessSessionInformation,
                                       &PSI,
                                       sizeof(PSI),
                                       NULL);
    if (NT_SUCCESS(Status))
    {
        TEST_OK(Peb->SessionId == PSI.SessionId);
    } else
    {
        TEST_SKIP("NtQueryInformationProcess failed with 0x%08lX, skip test for Peb->SessionId\n", Status);
    }
}

TEST_FUNC(TebStruct)
{
    PTEB Teb = NtCurrentTeb();

    TEST_OK(Teb->ProcessEnvironmentBlock == NtCurrentPeb());
    TEST_OK((ULONG_PTR)Teb->ClientId.UniqueProcess == (ULONG_PTR)GetCurrentProcessId());
    TEST_OK((ULONG_PTR)Teb->ClientId.UniqueThread == (ULONG_PTR)GetCurrentThreadId());
    
    Teb->LastErrorValue = ERROR_BAD_FILE_TYPE;
    TEST_OK(GetLastError() == ERROR_BAD_FILE_TYPE);
    SetLastError(ERROR_INVALID_EA_NAME);
    TEST_OK(Teb->LastErrorValue == ERROR_INVALID_EA_NAME);
    
    Teb->LastStatusValue = STATUS_NOT_IMPLEMENTED;
    TEST_OK(RtlGetLastNtStatus() == STATUS_NOT_IMPLEMENTED);
    RtlNtStatusToDosError(STATUS_DYNAMIC_CODE_BLOCKED);
    TEST_OK(Teb->LastStatusValue == STATUS_DYNAMIC_CODE_BLOCKED);

    TEST_OK(Teb->CurrentLocale == GetThreadLocale());
}

TEST_FUNC(KUSDStruct)
{
    TIME_ZONE_INFORMATION TimeZoneInfo;
    WCHAR NtSystemRoot[MAX_PATH];

    DWORD TimeZoneId = GetTimeZoneInformation(&TimeZoneInfo);
    TEST_OK(SharedUserData->TimeZoneId == TimeZoneId);

    if (GetWindowsDirectoryW(NtSystemRoot, ARRAYSIZE(NtSystemRoot)) > 0)
    {
        TEST_OK(_wcsicmp(SharedUserData->NtSystemRoot, NtSystemRoot) == 0);
    } else
    {
        TEST_SKIP("GetWindowsDirectoryW failed with 0x%08lX, skip test for SharedUserData->NtSystemRoot\n",
                  GetLastError());
    }
}
