
#include <shobjidl.h>
#include <propkey.h>
#include <propvarutil.h>
/*
typedef HRESULT (WINAPI* pSHGetPropertyStoreForWindow)
(
	HWND   hwnd,
	REFIID riid,
	void** ppv
);
pSHGetPropertyStoreForWindow RawSHGetPropertyStoreForWindow = nullptr;

HRESULT WINAPI MySHGetPropertyStoreForWindow(
	HWND   hwnd,
	REFIID riid,
	void** ppv
)
{
	HRESULT result = RawSHGetPropertyStoreForWindow(hwnd, riid, ppv);
	if (SUCCEEDED(result))
	{
		if (riid == IID_IPropertyStore)
		{
			IPropertyStore* pps = (IPropertyStore*)*ppv;
			DebugLog(L"MySHGetPropertyStoreForWindow %x %p %p", hwnd, &IPropertyStore::SetValue, *pps);

			PROPVARIANT pv;
			pps->SetValue(PKEY_AppUserModel_ID, pv);
		}
	}
	return result;
}
*/

typedef HRESULT(WINAPI* pPSStringFromPropertyKey)
(
	REFPROPERTYKEY pkey,
	LPWSTR         psz,
	UINT           cch
);
pPSStringFromPropertyKey RawPSStringFromPropertyKey = nullptr;

HRESULT WINAPI MyPSStringFromPropertyKey(

	REFPROPERTYKEY pkey,
	LPWSTR         psz,
	UINT           cch
)
{
	HRESULT result = RawPSStringFromPropertyKey(pkey, psz, cch);
	if (SUCCEEDED(result))
	{
		if (pkey == PKEY_AppUserModel_ID)
		{
			//DebugLog(L"MyPSStringFromPropertyKey %s", psz);
			return -1;
		}
	}
	return result;
}

void SetAppId()
{	
	/*
	HMODULE Shell32 = GetModuleHandle(L"Shell32.dll");

	PBYTE SHGetPropertyStoreForWindow = (PBYTE)GetProcAddress(Shell32, "SHGetPropertyStoreForWindow");
	MH_STATUS status = MH_CreateHook(SHGetPropertyStoreForWindow, MySHGetPropertyStoreForWindow, (LPVOID*)& RawSHGetPropertyStoreForWindow);
	if (status == MH_OK)
	{
		MH_EnableHook(SHGetPropertyStoreForWindow);
	}
	else
	{
		DebugLog(L"MH_CreateHook SHGetPropertyStoreForWindow failed:%d", status);
	}

	IPropertyStore* pps;
	HRESULT hr = MySHGetPropertyStoreForWindow(GetForegroundWindow(), IID_PPV_ARGS(&pps));
	*/
	//HMODULE Propsys = LoadLibrary(xorstr(L"Propsys.dll").crypt_get());
	HMODULE Propsys = LoadLibrary(L"Propsys.dll");

	PBYTE PSStringFromPropertyKey = (PBYTE)GetProcAddress(Propsys, "PSStringFromPropertyKey");
	MH_STATUS status = MH_CreateHook(PSStringFromPropertyKey, MyPSStringFromPropertyKey, (LPVOID*)& RawPSStringFromPropertyKey);
	if (status == MH_OK)
	{
		MH_EnableHook(PSStringFromPropertyKey);
	}
	else
	{
		DebugLog(L"MH_CreateHook PSStringFromPropertyKey failed:%d", status);
	}
}