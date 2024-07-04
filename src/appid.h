#ifndef APPID_H_
#define APPID_H_

#include <propkey.h>
#include <propvarutil.h>
#include <shobjidl.h>

auto RawPSStringFromPropertyKey = PSStringFromPropertyKey;

HRESULT WINAPI MyPSStringFromPropertyKey(REFPROPERTYKEY pkey,
                                         LPWSTR psz,
                                         UINT cch) {
  HRESULT result = RawPSStringFromPropertyKey(pkey, psz, cch);
  if (SUCCEEDED(result)) {
    if (pkey == PKEY_AppUserModel_ID) {
      // DebugLog(L"MyPSStringFromPropertyKey %s", psz);
      return -1;
    }
  }
  return result;
}

void SetAppId() {
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach((LPVOID*)&RawPSStringFromPropertyKey, MyPSStringFromPropertyKey);
  auto status = DetourTransactionCommit();
  if (status != NO_ERROR) {
    DebugLog(L"SetAppId failed %d", status);
  }
}

#endif  // APPID_H_
