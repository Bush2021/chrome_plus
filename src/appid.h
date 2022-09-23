#include <shobjidl.h>
#include <propkey.h>
#include <propvarutil.h>

typedef HRESULT(WINAPI *pPSStringFromPropertyKey)(
    REFPROPERTYKEY pkey,
    LPWSTR psz,
    UINT cch);
pPSStringFromPropertyKey RawPSStringFromPropertyKey = nullptr;

HRESULT WINAPI MyPSStringFromPropertyKey(
    REFPROPERTYKEY pkey,
    LPWSTR psz,
    UINT cch)
{
    HRESULT result = RawPSStringFromPropertyKey(pkey, psz, cch);
    if (SUCCEEDED(result))
    {
        if (pkey == PKEY_AppUserModel_ID)
        {
            // DebugLog(L"MyPSStringFromPropertyKey %s", psz);
            return -1;
        }
    }
    return result;
}

void SetAppId()
{
    HMODULE Propsys = LoadLibrary(L"Propsys.dll");

    PBYTE PSStringFromPropertyKey = (PBYTE)GetProcAddress(Propsys, "PSStringFromPropertyKey");
    MH_STATUS status = MH_CreateHook(PSStringFromPropertyKey, MyPSStringFromPropertyKey, (LPVOID *)&RawPSStringFromPropertyKey);
    if (status == MH_OK)
    {
        MH_EnableHook(PSStringFromPropertyKey);
    }
    else
    {
        DebugLog(L"MH_CreateHook PSStringFromPropertyKey failed:%d", status);
    }
}
