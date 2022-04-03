//https://docs.microsoft.com/en-us/windows/desktop/api/shlobj_core/nf-shlobj_core-isuseranadmin
/*
typedef BOOL(WINAPI* pGetTokenInformation)
(
	HANDLE                  TokenHandle,
	TOKEN_INFORMATION_CLASS TokenInformationClass,
	LPVOID                  TokenInformation,
	DWORD                   TokenInformationLength,
	PDWORD                  ReturnLength
	);

pGetTokenInformation RawGetTokenInformation = nullptr;

BOOL WINAPI MyGetTokenInformation(
	HANDLE                  TokenHandle,
	TOKEN_INFORMATION_CLASS TokenInformationClass,
	LPVOID                  TokenInformation,
	DWORD                   TokenInformationLength,
	PDWORD                  ReturnLength
)
{
	BOOL result = RawGetTokenInformation(TokenHandle, TokenInformationClass, TokenInformation, TokenInformationLength, ReturnLength);
	if (result)
	{
		DebugLog(L"RawGetTokenInformation %d", TokenInformationClass);
		//return false;
	}
	if (result && TokenInformationClass == TokenElevation)
	{
		TOKEN_ELEVATION* elevation = (TOKEN_ELEVATION*)TokenInformation;
		DebugLog(L"TokenIsElevated %d", elevation->TokenIsElevated);
		elevation->TokenIsElevated = false;
	}
	return result;
};


typedef BOOL(WINAPI* pOpenProcessToken)
(
	HANDLE  ProcessHandle,
	DWORD   DesiredAccess,
	PHANDLE TokenHandle
	);

pOpenProcessToken RawOpenProcessToken = nullptr;

BOOL WINAPI MyOpenProcessToken(
	HANDLE  ProcessHandle,
	DWORD   DesiredAccess,
	PHANDLE TokenHandle
)
{
	if (DesiredAccess == TOKEN_QUERY)
	{
		DebugLog(L"MyOpenProcessToken");
		return false;
	}
	BOOL result = RawOpenProcessToken(ProcessHandle, DesiredAccess, TokenHandle);
	return result;
};
*/
typedef BOOL(WINAPI* pIsUserAnAdmin) ();

pIsUserAnAdmin RawIsUserAnAdmin = nullptr;

BOOL WINAPI MyIsUserAnAdmin()
{
	return false;
};

void RemoveAdminWarn()
{
	/*
    HMODULE Advapi32 = GetModuleHandle(L"Advapi32.dll");

    PBYTE GetTokenInformation = (PBYTE)GetProcAddress(Advapi32, "GetTokenInformation");
    MH_STATUS status = MH_CreateHook(GetTokenInformation, MyGetTokenInformation, (LPVOID*)&RawGetTokenInformation);
    if (status == MH_OK)
    {
        MH_EnableHook(GetTokenInformation);
    }
    else
    {
        DebugLog(L"MH_CreateHook GetTokenInformation failed:%d", status);
    }
	PBYTE OpenProcessToken = (PBYTE)GetProcAddress(Advapi32, "OpenProcessToken");
	MH_STATUS status = MH_CreateHook(OpenProcessToken, MyOpenProcessToken, (LPVOID*)& RawOpenProcessToken);
	if (status == MH_OK)
	{
		MH_EnableHook(OpenProcessToken);
		DebugLog(L"MH_CreateHook OpenProcessToken failed:%d", status);
	}
	else
	{
		DebugLog(L"MH_CreateHook OpenProcessToken failed:%d", status);
	}*/
	HMODULE Shell32 = GetModuleHandle(L"Shell32.dll");

	PBYTE IsUserAnAdmin = (PBYTE)GetProcAddress(Shell32, "IsUserAnAdmin");
	MH_STATUS status = MH_CreateHook(IsUserAnAdmin, MyIsUserAnAdmin, (LPVOID*)& RawIsUserAnAdmin);
	if (status == MH_OK)
	{
		MH_EnableHook(IsUserAnAdmin);
	}
	else
	{
		DebugLog(L"MH_CreateHook IsUserAnAdmin failed:%d", status);
	}
}