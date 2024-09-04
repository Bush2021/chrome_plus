#ifndef GREEN_H_
#define GREEN_H_

#include <lmaccess.h>

auto RawUpdateProcThreadAttribute = UpdateProcThreadAttribute;
auto RawCryptUnprotectData = CryptUnprotectData;
auto RawLogonUserW = LogonUserW;
auto RawIsOS = IsOS;
auto RawNetUserGetInfo = NetUserGetInfo;
auto RawGetVolumeInformationW = GetVolumeInformationW;

BOOL WINAPI FakeGetComputerName(_Out_ LPTSTR lpBuffer,
                                _Inout_ LPDWORD lpnSize) {
  return false;
}

BOOL WINAPI FakeGetVolumeInformation(_In_opt_ LPCTSTR lpRootPathName,
                                     _Out_opt_ LPTSTR lpVolumeNameBuffer,
                                     _In_ DWORD nVolumeNameSize,
                                     _Out_opt_ LPDWORD lpVolumeSerialNumber,
                                     _Out_opt_ LPDWORD lpMaximumComponentLength,
                                     _Out_opt_ LPDWORD lpFileSystemFlags,
                                     _Out_opt_ LPTSTR lpFileSystemNameBuffer,
                                     _In_ DWORD nFileSystemNameSize) {
  if (lpVolumeSerialNumber != nullptr) {
    DebugLog(L"GetVolumeInformation modified");
    return false;
  } else {
    DebugLog(L"GetVolumeInformation passthrough");
    return RawGetVolumeInformationW(
        lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize,
        lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags,
        lpFileSystemNameBuffer, nFileSystemNameSize);
  }
}

enum ProcessCreationMitigationPolicy : DWORD64 {
  BlockNonMicrosoftBinariesAlwaysOn = 0x00000001ui64 << 44,
  Win32kSystemCallDisableAlwaysOn = 0x00000001ui64 << 28
};

BOOL WINAPI MyUpdateProcThreadAttribute(
    __inout LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
    __in DWORD dwFlags,
    __in DWORD_PTR Attribute,
    __in_bcount_opt(cbSize) PVOID lpValue,
    __in SIZE_T cbSize,
    __out_bcount_opt(cbSize) PVOID lpPreviousValue,
    __in_opt PSIZE_T lpReturnSize) {
  if (Attribute == PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY &&
      cbSize >= sizeof(DWORD64)) {
    // https://source.chromium.org/chromium/chromium/src/+/main:sandbox/win/src/process_mitigations.cc;l=362;drc=4c2fec5f6699ffeefd93137d2bf8c03504c6664c
    PDWORD64 policy_value_1 = &((PDWORD64)lpValue)[0];
    *policy_value_1 &= ~static_cast<DWORD64>(
        ProcessCreationMitigationPolicy::BlockNonMicrosoftBinariesAlwaysOn);
    // *policy_value_1 &=
    //     ~PROCESS_CREATION_MITIGATION_POLICY_WIN32K_SYSTEM_CALL_DISABLE_ALWAYS_ON;
  }
  return RawUpdateProcThreadAttribute(lpAttributeList, dwFlags, Attribute,
                                      lpValue, cbSize, lpPreviousValue,
                                      lpReturnSize);
}

BOOL WINAPI
MyCryptProtectData(_In_ DATA_BLOB* pDataIn,
                   _In_opt_ LPCWSTR szDataDescr,
                   _In_opt_ DATA_BLOB* pOptionalEntropy,
                   _Reserved_ PVOID pvReserved,
                   _In_opt_ CRYPTPROTECT_PROMPTSTRUCT* pPromptStruct,
                   _In_ DWORD dwFlags,
                   _Out_ DATA_BLOB* pDataOut) {
  pDataOut->cbData = pDataIn->cbData;
  pDataOut->pbData = (BYTE*)LocalAlloc(LMEM_FIXED, pDataOut->cbData);
  memcpy(pDataOut->pbData, pDataIn->pbData, pDataOut->cbData);
  return true;
}

BOOL WINAPI
MyCryptUnprotectData(_In_ DATA_BLOB* pDataIn,
                     _Out_opt_ LPWSTR* ppszDataDescr,
                     _In_opt_ DATA_BLOB* pOptionalEntropy,
                     _Reserved_ PVOID pvReserved,
                     _In_opt_ CRYPTPROTECT_PROMPTSTRUCT* pPromptStruct,
                     _In_ DWORD dwFlags,
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

DWORD WINAPI MyLogonUserW(LPCWSTR lpszUsername,
                          LPCWSTR lpszDomain,
                          LPCWSTR lpszPassword,
                          DWORD dwLogonType,
                          DWORD dwLogonProvider,
                          PHANDLE phToken) {
  DWORD ret = RawLogonUserW(lpszUsername, lpszDomain, lpszPassword, dwLogonType,
                            dwLogonProvider, phToken);

  SetLastError(ERROR_ACCOUNT_RESTRICTION);
  return ret;
}

BOOL WINAPI MyIsOS(DWORD dwOS) {
  DWORD ret = RawIsOS(dwOS);
  if (dwOS == OS_DOMAINMEMBER) {
    return false;
  }

  return ret;
}

NET_API_STATUS WINAPI MyNetUserGetInfo(LPCWSTR servername,
                                       LPCWSTR username,
                                       DWORD level,
                                       LPBYTE* bufptr) {
  NET_API_STATUS ret = RawNetUserGetInfo(servername, username, level, bufptr);
  if (level == 1 && ret == 0) {
    LPUSER_INFO_1 user_info = (LPUSER_INFO_1)*bufptr;
    user_info->usri1_password_age = 0;
  }

  return ret;
}

void MakeGreen() {
  auto RawGetComputerNameW = GetComputerNameW;
  // auto RawGetVolumeInformationW = GetVolumeInformationW;
  auto RawCryptProtectData = CryptProtectData;

  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());

  // kernel32.dll
  DetourAttach((LPVOID*)&RawGetComputerNameW, FakeGetComputerName);
  DetourAttach((LPVOID*)&RawGetVolumeInformationW, FakeGetVolumeInformation);
  DetourAttach((LPVOID*)&RawUpdateProcThreadAttribute,
               MyUpdateProcThreadAttribute);

  // components/os_crypt/os_crypt_win.cc
  // crypt32.dll
  DetourAttach((LPVOID*)&RawCryptProtectData, MyCryptProtectData);
  DetourAttach((LPVOID*)&RawCryptUnprotectData, MyCryptUnprotectData);

  // advapi32.dll
  DetourAttach((LPVOID*)&RawLogonUserW, MyLogonUserW);

  // shlwapi.dll
  DetourAttach((LPVOID*)&RawIsOS, MyIsOS);

  // netapi32.dll
  DetourAttach((LPVOID*)&RawNetUserGetInfo, MyNetUserGetInfo);

  auto status = DetourTransactionCommit();
  if (status != NO_ERROR) {
    DebugLog(L"MakeGreen failed: %d", status);
  } else {
    DebugLog(L"MakeGreen success");
  }
}

#endif  // GREEN_H_
