typedef LONG NTSTATUS, *PNTSTATUS;

#ifndef NT_SUCCESS
#define NT_SUCCESS(x) ((x) >= 0)
#define STATUS_SUCCESS ((NTSTATUS)0)
#endif

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef NTSTATUS(WINAPI *pLdrLoadDll)(IN PWCHAR PathToFile OPTIONAL, IN ULONG Flags OPTIONAL,
                                      IN PUNICODE_STRING ModuleFileName, OUT PHANDLE ModuleHandle);

pLdrLoadDll RawLdrLoadDll = nullptr;

void Outdated(HMODULE module)
{
    // "OutdatedUpgradeBubble.Show"
#ifdef _WIN64
    BYTE search[] = {0x48, 0x89, 0x8C, 0x24, 0xF0, 0x00, 0x00, 0x00, 0x80, 0x3D};
    uint8_t *match = SearchModuleRaw(module, search, sizeof(search));
#else
    BYTE search[] = {0x31, 0xE8, 0x89, 0x45, 0xF0, 0x88, 0x5D, 0xEF, 0x80, 0x3D};
    uint8_t *match = SearchModuleRaw(module, search, sizeof(search));
#endif
    if (match)
    {
        if (*(match + 0xF) == 0x74)
        {
            BYTE patch[] = {0x90, 0x90};
            WriteMemory(match + 0xF, patch, sizeof(patch));
        }
    }
    else
    {
        DebugLog(L"patch Outdated failed %p", module);
    }
}

void DevWarning(HMODULE module)
{
    // "enable-automation"
}

NTSTATUS WINAPI MyLdrLoadDll(IN PWCHAR PathToFile OPTIONAL,
                             IN ULONG Flags OPTIONAL,
                             IN PUNICODE_STRING ModuleFileName,
                             OUT PHANDLE ModuleHandle)
{
    static bool chrome_loaded = false;

    NTSTATUS ntstatus = RawLdrLoadDll(PathToFile, Flags, ModuleFileName, ModuleHandle);
    if (NT_SUCCESS(ntstatus))
    {
        if (wcsstr(ModuleFileName->Buffer, L"chrome.dll") != 0 && !chrome_loaded)
        {
            chrome_loaded = true;
            Outdated((HMODULE)*ModuleHandle);
            DevWarning((HMODULE)*ModuleHandle);
        }
    }
    return ntstatus;
};

void MakePatch()
{
    // HMODULE chrome = GetModuleHandle(L"chrome.dll");
    // if (chrome)
    // {
    //     Outdated(chrome);
    //     DevWarning(chrome);
    //     return;
    // }
    HMODULE ntdll = GetModuleHandle(L"ntdll.dll");
    if (ntdll)
    {
        PBYTE LdrLoadDll = (PBYTE)GetProcAddress(ntdll, "LdrLoadDll");
        MH_STATUS status = MH_CreateHook(LdrLoadDll, MyLdrLoadDll, (LPVOID *)&RawLdrLoadDll);
        if (status == MH_OK)
        {
            MH_EnableHook(LdrLoadDll);
        }
        else
        {
            DebugLog(L"MH_CreateHook LdrLoadDll failed:%d", status);
        }
    }
}
