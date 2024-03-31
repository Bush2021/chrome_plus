#ifndef GREEN_H_
#define GREEN_H_

#include <lmaccess.h>

BOOL WINAPI FakeGetComputerName(_Out_ LPTSTR lpBuffer,
                                _Inout_ LPDWORD lpnSize) {
  return 0;
}

BOOL WINAPI FakeGetVolumeInformation(_In_opt_ LPCTSTR lpRootPathName,
                                     _Out_opt_ LPTSTR lpVolumeNameBuffer,
                                     _In_ DWORD nVolumeNameSize,
                                     _Out_opt_ LPDWORD lpVolumeSerialNumber,
                                     _Out_opt_ LPDWORD lpMaximumComponentLength,
                                     _Out_opt_ LPDWORD lpFileSystemFlags,
                                     _Out_opt_ LPTSTR lpFileSystemNameBuffer,
                                     _In_ DWORD nFileSystemNameSize) {
  return 0;
}

BOOL WINAPI MyCryptProtectData(
    _In_ DATA_BLOB* pDataIn, _In_opt_ LPCWSTR szDataDescr,
    _In_opt_ DATA_BLOB* pOptionalEntropy, _Reserved_ PVOID pvReserved,
    _In_opt_ CRYPTPROTECT_PROMPTSTRUCT* pPromptStruct, _In_ DWORD dwFlags,
    _Out_ DATA_BLOB* pDataOut) {
  pDataOut->cbData = pDataIn->cbData;
  pDataOut->pbData = (BYTE*)LocalAlloc(LMEM_FIXED, pDataOut->cbData);
  memcpy(pDataOut->pbData, pDataIn->pbData, pDataOut->cbData);
  return true;
}

typedef BOOL(WINAPI* pCryptUnprotectData)(
    _In_ DATA_BLOB* pDataIn, _Out_opt_ LPWSTR* ppszDataDescr,
    _In_opt_ DATA_BLOB* pOptionalEntropy, _Reserved_ PVOID pvReserved,
    _In_opt_ CRYPTPROTECT_PROMPTSTRUCT* pPromptStruct, _In_ DWORD dwFlags,
    _Out_ DATA_BLOB* pDataOut);

pCryptUnprotectData RawCryptUnprotectData = NULL;

BOOL WINAPI MyCryptUnprotectData(
    _In_ DATA_BLOB* pDataIn, _Out_opt_ LPWSTR* ppszDataDescr,
    _In_opt_ DATA_BLOB* pOptionalEntropy, _Reserved_ PVOID pvReserved,
    _In_opt_ CRYPTPROTECT_PROMPTSTRUCT* pPromptStruct, _In_ DWORD dwFlags,
    _Out_ DATA_BLOB* pDataOut) {
  if (RawCryptUnprotectData(pDataIn, ppszDataDescr, pOptionalEntropy,
                            pvReserved, pPromptStruct, dwFlags, pDataOut)) {
    return true;
  }

  pDataOut->cbData = pDataIn->cbData;
  pDataOut->pbData = (BYTE*)LocalAlloc(LMEM_FIXED, pDataOut->cbData);
  memcpy(pDataOut->pbData, pDataIn->pbData, pDataOut->cbData);
  return true;
}

typedef DWORD(WINAPI* pLogonUserW)(LPCWSTR lpszUsername, LPCWSTR lpszDomain,
                                   LPCWSTR lpszPassword, DWORD dwLogonType,
                                   DWORD dwLogonProvider, PHANDLE phToken);

pLogonUserW RawLogonUserW = NULL;

DWORD WINAPI MyLogonUserW(LPCWSTR lpszUsername, LPCWSTR lpszDomain,
                          LPCWSTR lpszPassword, DWORD dwLogonType,
                          DWORD dwLogonProvider, PHANDLE phToken) {
  DWORD ret = RawLogonUserW(lpszUsername, lpszDomain, lpszPassword, dwLogonType,
                            dwLogonProvider, phToken);

  SetLastError(ERROR_ACCOUNT_RESTRICTION);
  return ret;
}

typedef BOOL(WINAPI* pIsOS)(DWORD dwOS);

pIsOS RawIsOS = NULL;

BOOL WINAPI MyIsOS(DWORD dwOS) {
  DWORD ret = RawIsOS(dwOS);
  if (dwOS == OS_DOMAINMEMBER) {
    return false;
  }

  return ret;
}

typedef NET_API_STATUS(WINAPI* pNetUserGetInfo)(LPCWSTR servername,
                                                LPCWSTR username, DWORD level,
                                                LPBYTE* bufptr);

pNetUserGetInfo RawNetUserGetInfo = NULL;

NET_API_STATUS WINAPI MyNetUserGetInfo(LPCWSTR servername, LPCWSTR username,
                                       DWORD level, LPBYTE* bufptr) {
  NET_API_STATUS ret = RawNetUserGetInfo(servername, username, level, bufptr);
  if (level == 1 && ret == 0) {
    LPUSER_INFO_1 user_info = (LPUSER_INFO_1)*bufptr;
    user_info->usri1_password_age = 0;
  }

  return ret;
}

#define PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON \
  (0x00000001ui64 << 44)

typedef BOOL(WINAPI* pUpdateProcThreadAttribute)(
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList, DWORD dwFlags,
    DWORD_PTR Attribute, PVOID lpValue, SIZE_T cbSize, PVOID lpPreviousValue,
    PSIZE_T lpReturnSize);

pUpdateProcThreadAttribute RawUpdateProcThreadAttribute = nullptr;

BOOL WINAPI MyUpdateProcThreadAttribute(
    __inout LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList, __in DWORD dwFlags,
    __in DWORD_PTR Attribute, __in_bcount_opt(cbSize) PVOID lpValue,
    __in SIZE_T cbSize, __out_bcount_opt(cbSize) PVOID lpPreviousValue,
    __in_opt PSIZE_T lpReturnSize) {
  if (Attribute == PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY &&
      cbSize >= sizeof(DWORD64)) {
    PDWORD64 policy_value_1 = &((PDWORD64)lpValue)[0];
    *policy_value_1 &=
        ~PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON;
  }
  return RawUpdateProcThreadAttribute(lpAttributeList, dwFlags, Attribute,
                                      lpValue, cbSize, lpPreviousValue,
                                      lpReturnSize);
}

void MakeGreen() {
  HMODULE kernel32 = LoadLibraryW(L"kernel32.dll");
  if (kernel32) {
    PBYTE GetComputerNameW =
        (PBYTE)GetProcAddress(kernel32, "GetComputerNameW");
    PBYTE GetVolumeInformationW =
        (PBYTE)GetProcAddress(kernel32, "GetVolumeInformationW");

    MH_STATUS status =
        MH_CreateHook(GetComputerNameW, FakeGetComputerName, NULL);
    if (status == MH_OK) {
      MH_EnableHook(GetComputerNameW);
    } else {
      DebugLog(L"MH_CreateHook GetComputerNameW failed:%d", status);
    }
    status =
        MH_CreateHook(GetVolumeInformationW, FakeGetVolumeInformation, NULL);
    if (status == MH_OK) {
      MH_EnableHook(GetVolumeInformationW);
    } else {
      DebugLog(L"MH_CreateHook GetVolumeInformationW failed:%d", status);
    }
  }

  // components/os_crypt/os_crypt_win.cc
  HMODULE Crypt32 = LoadLibraryW(L"Crypt32.dll");
  if (Crypt32) {
    PBYTE CryptProtectData = (PBYTE)GetProcAddress(Crypt32, "CryptProtectData");
    PBYTE CryptUnprotectData =
        (PBYTE)GetProcAddress(Crypt32, "CryptUnprotectData");

    MH_STATUS status =
        MH_CreateHook(CryptProtectData, MyCryptProtectData, NULL);
    if (status == MH_OK) {
      MH_EnableHook(CryptProtectData);
    } else {
      DebugLog(L"MH_CreateHook CryptProtectData failed:%d", status);
    }
    status = MH_CreateHook(CryptUnprotectData, MyCryptUnprotectData,
                           (LPVOID*)&RawCryptUnprotectData);
    if (status == MH_OK) {
      MH_EnableHook(CryptUnprotectData);
    } else {
      DebugLog(L"MH_CreateHook CryptUnprotectData failed:%d", status);
    }
  }

  HMODULE Advapi32 = LoadLibraryW(L"Advapi32.dll");
  if (Advapi32) {
    PBYTE LogonUserW = (PBYTE)GetProcAddress(Advapi32, "LogonUserW");

    MH_STATUS status =
        MH_CreateHook(LogonUserW, MyLogonUserW, (LPVOID*)&RawLogonUserW);
    if (status == MH_OK) {
      MH_EnableHook(LogonUserW);
    } else {
      DebugLog(L"MH_CreateHook LogonUserW failed:%d", status);
    }
  }

  HMODULE Shlwapi = LoadLibraryW(L"Shlwapi.dll");
  if (Shlwapi) {
    PBYTE IsOS = (PBYTE)GetProcAddress(Shlwapi, "IsOS");

    MH_STATUS status = MH_CreateHook(IsOS, MyIsOS, (LPVOID*)&RawIsOS);
    if (status == MH_OK) {
      MH_EnableHook(IsOS);
    } else {
      DebugLog(L"MH_CreateHook IsOS failed:%d", status);
    }
  }

  HMODULE Netapi32 = LoadLibraryW(L"Netapi32.dll");
  if (Netapi32) {
    PBYTE NetUserGetInfo = (PBYTE)GetProcAddress(Netapi32, "NetUserGetInfo");

    MH_STATUS status = MH_CreateHook(NetUserGetInfo, MyNetUserGetInfo,
                                     (LPVOID*)&RawNetUserGetInfo);
    if (status == MH_OK) {
      MH_EnableHook(NetUserGetInfo);
    } else {
      DebugLog(L"MH_CreateHook NetUserGetInfo failed:%d", status);
    }
  }

  LPVOID ppUpdateProcThreadAttribute = nullptr;
  MH_STATUS status = MH_CreateHookApiEx(
      L"kernel32", "UpdateProcThreadAttribute", &MyUpdateProcThreadAttribute,
      (LPVOID*)&RawUpdateProcThreadAttribute, &ppUpdateProcThreadAttribute);
  if (status == MH_OK) {
    MH_EnableHook(ppUpdateProcThreadAttribute);
  } else {
    DebugLog(L"MH_CreateHookApiEx UpdateProcThreadAttribute failed: %d",
             status);
  }
}

#endif  // GREEN_H_
