
typedef LONG NTSTATUS, *PNTSTATUS;

#ifndef NT_SUCCESS
#define NT_SUCCESS(x) ((x)>=0)
#define STATUS_SUCCESS ((NTSTATUS)0)
#endif

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef NTSTATUS(WINAPI *pLdrLoadDll) //LdrLoadDll function prototype
(
    IN PWCHAR PathToFile OPTIONAL,
    IN ULONG Flags OPTIONAL,
    IN PUNICODE_STRING ModuleFileName,
    OUT PHANDLE ModuleHandle
    );

pLdrLoadDll RawLdrLoadDll = nullptr;

NTSTATUS WINAPI MyLdrLoadDll(
    IN PWCHAR PathToFile OPTIONAL,
    IN ULONG Flags OPTIONAL,
    IN PUNICODE_STRING ModuleFileName,
    OUT PHANDLE ModuleHandle
)
{
	static bool flash_loaded = false;
	static bool chrome_loaded = true;

    NTSTATUS ntstatus = RawLdrLoadDll(PathToFile, Flags, ModuleFileName, ModuleHandle);
    if (NT_SUCCESS(ntstatus))
    {
        if (wcsstr(ModuleFileName->Buffer, L"pepflashplayer") != 0 && !flash_loaded)
        {
			flash_loaded = true;

			{
	            BYTE search[] = { '\0' , '\0', 'C', 'N', '\0' , '\0' };
	            uint8_t *fuck_flash = SearchModuleRaw2((HMODULE)*ModuleHandle, search, sizeof(search));
	            if (fuck_flash)
	            {
	                BYTE patch[] = { '\0' , '\0', 'F', 'K', '\0' , '\0' };
	                WriteMemory(fuck_flash, patch, sizeof(patch));
	                //DebugLog(L"patch flash ok");
	            }
	            else
	            {
	                DebugLog(L"patch flash cn failed %p", *ModuleHandle);
	            }
			}

			{
	            BYTE search[] = { 0x00, 0x00, 0x40, 0x46, 0x3E, 0x6F, 0x77, 0x42 };
	            uint8_t *fuck_flash = SearchModuleRaw2((HMODULE)*ModuleHandle, search, sizeof(search));
	            if (fuck_flash)
	            {
	                BYTE patch[] = { 0x00, 0x00, 0x40, 0x46, 0x3E, 0x6F, 0x77, 0x43 };
	                WriteMemory(fuck_flash, patch, sizeof(patch));
	                //DebugLog(L"patch flash ok");
	            }
	            else
	            {
	                DebugLog(L"patch flash time failed %p", *ModuleHandle);
	            }
			}
        }
		else if (wcsstr(ModuleFileName->Buffer, L"chrome.dll") != 0 && !chrome_loaded)
		{
			chrome_loaded = true;
			{
#ifdef _WIN64
				// "enable-automation"
				// "ExtensionBubble.ExtensionWipeoutCount"
				BYTE search[] = { 0x84, 0xC0, 0x75, 0x59, 0x48, 0x8D, 0x8F };
				uint8_t* DevWarning = SearchModuleRaw((HMODULE)* ModuleHandle, search, sizeof(search));
				if (DevWarning)
				{
					BYTE patch[] = { 0xEB };
					WriteMemory(DevWarning + 0x2, patch, sizeof(patch));
					//DebugLog(L"patch DevWarning ok");
				}
				else
				{
					DebugLog(L"patch DevWarning failed %p", *ModuleHandle);
				}
#else
				BYTE search[] = { 0x84, 0xC0 , 0x74 , 0x2E , 0xEB , 0x3B };
				uint8_t* DevWarning = SearchModuleRaw((HMODULE)* ModuleHandle, search, sizeof(search));
				if (DevWarning && *(DevWarning + 0x22) == 0x83)
				{
					BYTE patch[] = { 0x05 };
					WriteMemory(DevWarning + 0x24, patch, sizeof(patch));
				}
				else
				{
					BYTE search[] = { 0x83, 0xF8, 0x02, 0x0F, 0x8F, 0x67 };
					uint8_t* DevWarning = SearchModuleRaw((HMODULE)* ModuleHandle, search, sizeof(search));
					if (DevWarning)
					{
						BYTE patch[] = { 0x05 };
						WriteMemory(DevWarning + 0x2, patch, sizeof(patch));
						//DebugLog(L"patch DevWarning ok");
					}
					else
					{
						DebugLog(L"patch DevWarning failed %p", *ModuleHandle);
					}
				}
#endif
			}
			{
				//00007FFC633A3B21 | 48:8D0D 6012EF02 | lea rcx, qword ptr ds : [7FFC66294D88] | rcx : "MZx", 00007FFC66294D88 : &"FlashDeprecationWarning"
//#ifdef _WIN64
//				BYTE search[] = { 0x83, 0xC4, 0x04, 0x84, 0xC0, 0x74, 0x2C, 0x8B, 0x45, 0x08, 0x31, 0xDB };
//				uint8_t* DevWarning = SearchModuleRaw((HMODULE)* ModuleHandle, search, sizeof(search));
//				if (DevWarning && *(DevWarning + 0x26) == 0x83)
//				{
//					BYTE patch[] = { 0x05 };
//					WriteMemory(DevWarning + 0x28, patch, sizeof(patch));
//					//DebugLog(L"patch DevWarning ok");
//				}
//				else
//				{
//					DebugLog(L"patch FlashWarning failed %p", *ModuleHandle);
//				}
//#else
//				BYTE search[] = { 0x83, 0xC4, 0x04, 0x84, 0xC0, 0x74, 0x2C, 0x8B, 0x45, 0x08, 0x31, 0xDB };
//				uint8_t* FlashWarning = SearchModuleRaw((HMODULE)* ModuleHandle, search, sizeof(search));
//				if (FlashWarning)
//				{
//					BYTE patch[] = { 0x32, 0xC0, 0xEB };
//					WriteMemory(FlashWarning + 0x3, patch, sizeof(patch));
//				}
//				else
//				{
//					DebugLog(L"patch FlashWarning failed %p", *ModuleHandle);
//				}
//#endif
			}
		}
    }
    return ntstatus;
};


void MakePatch()
{
    HMODULE ntdll = GetModuleHandle(L"ntdll.dll");
	if (ntdll)
	{
		PBYTE LdrLoadDll = (PBYTE)GetProcAddress(ntdll, "LdrLoadDll");
		MH_STATUS status = MH_CreateHook(LdrLoadDll, MyLdrLoadDll, (LPVOID*)& RawLdrLoadDll);
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
