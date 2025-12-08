/*
 * Test for the PEB_LDR_DATA structure definitions
 */

#include "../UnitTest.h"

TEST_FUNC(PebLdrStruct)
{
    PPEB_LDR_DATA Ldr = NtCurrentPeb()->Ldr;
    PLDR_DATA_TABLE_ENTRY NtdllLdrEntry;
    PIMAGE_NT_HEADERS NtdllHeader;
    BOOL NtdllBaseValid;

    TEST_OK(CONTAINING_RECORD(Ldr->InLoadOrderModuleList.Flink,
                              LDR_DATA_TABLE_ENTRY,
                              InLoadOrderLinks)->DllBase == GetModuleHandleW(NULL));

    NtdllLdrEntry = CONTAINING_RECORD(Ldr->InInitializationOrderModuleList.Flink,
                                      LDR_DATA_TABLE_ENTRY,
                                      InInitializationOrderLinks);
    TEST_OK(NtdllLdrEntry->EntryPoint == NULL);

    NtdllBaseValid = NtdllLdrEntry->DllBase != NULL && NtdllLdrEntry->DllBase == GetModuleHandleW(L"ntdll.dll");
    TEST_OK(NtdllBaseValid);
    if (NtdllBaseValid)
    {
        NtdllHeader = Add2Ptr(NtdllLdrEntry->DllBase, ((PIMAGE_DOS_HEADER)(NtdllLdrEntry->DllBase))->e_lfanew);
        TEST_OK(NtdllLdrEntry->SizeOfImage == NtdllHeader->OptionalHeader.SizeOfImage);
        TEST_OK(NtdllLdrEntry->TimeDateStamp == NtdllHeader->FileHeader.TimeDateStamp);
    } else
    {
        TEST_SKIP("NtdllBaseValid is FALSE, skip some field tests about PE image\n");
    }
}
