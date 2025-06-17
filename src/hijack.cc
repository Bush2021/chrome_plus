#include "hijack.h"

#include <intrin.h>
#include <stdint.h>

#include "detours.h"

#define NOP_FUNC        \
  {                     \
    __nop();            \
    __nop();            \
    __nop();            \
    __nop();            \
    __nop();            \
    __nop();            \
    __nop();            \
    __nop();            \
    __nop();            \
    __nop();            \
    __nop();            \
    __nop();            \
    return __COUNTER__; \
  }

#define EXPORT(api) int __cdecl api() NOP_FUNC

#pragma comment( \
    linker,      \
    "/export:GetFileVersionInfoA=?GetFileVersionInfoA@hijack@@YAHXZ,@1")
#pragma comment( \
    linker,      \
    "/export:GetFileVersionInfoByHandle=?GetFileVersionInfoByHandle@hijack@@YAHXZ,@2")
#pragma comment( \
    linker,      \
    "/export:GetFileVersionInfoExA=?GetFileVersionInfoExA@hijack@@YAHXZ,@3")
#pragma comment( \
    linker,      \
    "/export:GetFileVersionInfoExW=?GetFileVersionInfoExW@hijack@@YAHXZ,@4")
#pragma comment( \
    linker,      \
    "/export:GetFileVersionInfoSizeA=?GetFileVersionInfoSizeA@hijack@@YAHXZ,@5")
#pragma comment( \
    linker,      \
    "/export:GetFileVersionInfoSizeExA=?GetFileVersionInfoSizeExA@hijack@@YAHXZ,@6")
#pragma comment( \
    linker,      \
    "/export:GetFileVersionInfoSizeExW=?GetFileVersionInfoSizeExW@hijack@@YAHXZ,@7")
#pragma comment( \
    linker,      \
    "/export:GetFileVersionInfoSizeW=?GetFileVersionInfoSizeW@hijack@@YAHXZ,@8")
#pragma comment( \
    linker,      \
    "/export:GetFileVersionInfoW=?GetFileVersionInfoW@hijack@@YAHXZ,@9")
#pragma comment(linker, "/export:VerFindFileA=?VerFindFileA@hijack@@YAHXZ,@10")
#pragma comment(linker, "/export:VerFindFileW=?VerFindFileW@hijack@@YAHXZ,@11")
#pragma comment(linker, \
                "/export:VerInstallFileA=?VerInstallFileA@hijack@@YAHXZ,@12")
#pragma comment(linker, \
                "/export:VerInstallFileW=?VerInstallFileW@hijack@@YAHXZ,@13")
#pragma comment( \
    linker, "/export:VerLanguageNameA=?VerLanguageNameA@hijack@@YAHXZ,@14")
#pragma comment( \
    linker, "/export:VerLanguageNameW=?VerLanguageNameW@hijack@@YAHXZ,@15")
#pragma comment(linker, \
                "/export:VerQueryValueA=?VerQueryValueA@hijack@@YAHXZ,@16")
#pragma comment(linker, \
                "/export:VerQueryValueW=?VerQueryValueW@hijack@@YAHXZ,@17")

// Make it globally visible
namespace hijack {
EXPORT(GetFileVersionInfoA)
EXPORT(GetFileVersionInfoByHandle)
EXPORT(GetFileVersionInfoExA)
EXPORT(GetFileVersionInfoExW)
EXPORT(GetFileVersionInfoSizeA)
EXPORT(GetFileVersionInfoSizeExA)
EXPORT(GetFileVersionInfoSizeExW)
EXPORT(GetFileVersionInfoSizeW)
EXPORT(GetFileVersionInfoW)
EXPORT(VerFindFileA)
EXPORT(VerFindFileW)
EXPORT(VerInstallFileA)
EXPORT(VerInstallFileW)
EXPORT(VerLanguageNameA)
EXPORT(VerLanguageNameW)
EXPORT(VerQueryValueA)
EXPORT(VerQueryValueW)
}  // namespace hijack

namespace {
// Internal helper functions are kept in the anonymous namespace
void InstallDetours(PBYTE pTarget, PBYTE pDetour) {
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach(&(PVOID&)pTarget, pDetour);
  DetourTransactionCommit();
}

void LoadVersion(HINSTANCE hModule) {
  PBYTE pImageBase = (PBYTE)hModule;
  PIMAGE_DOS_HEADER pimDH = (PIMAGE_DOS_HEADER)pImageBase;
  if (pimDH->e_magic == IMAGE_DOS_SIGNATURE) {
    PIMAGE_NT_HEADERS pimNH = (PIMAGE_NT_HEADERS)(pImageBase + pimDH->e_lfanew);
    if (pimNH->Signature == IMAGE_NT_SIGNATURE) {
      PIMAGE_EXPORT_DIRECTORY pimExD =
          (PIMAGE_EXPORT_DIRECTORY)(pImageBase +
                                    pimNH->OptionalHeader
                                        .DataDirectory
                                            [IMAGE_DIRECTORY_ENTRY_EXPORT]
                                        .VirtualAddress);
      DWORD* pName = (DWORD*)(pImageBase + pimExD->AddressOfNames);
      DWORD* pFunction = (DWORD*)(pImageBase + pimExD->AddressOfFunctions);
      WORD* pNameOrdinals = (WORD*)(pImageBase + pimExD->AddressOfNameOrdinals);

      wchar_t szSysDirectory[MAX_PATH + 1];
      GetSystemDirectory(szSysDirectory, MAX_PATH);

      wchar_t szDLLPath[MAX_PATH + 1];
      lstrcpy(szDLLPath, szSysDirectory);
      lstrcat(szDLLPath, TEXT("\\version.dll"));

      HINSTANCE module = LoadLibrary(szDLLPath);
      for (size_t i = 0; i < pimExD->NumberOfNames; ++i) {
        PBYTE Original =
            (PBYTE)GetProcAddress(module, (char*)(pImageBase + pName[i]));
        if (Original) {
          InstallDetours(pImageBase + pFunction[pNameOrdinals[i]], Original);
        }
      }
    }
  }
}
}  // namespace

void LoadSysDll(HINSTANCE hModule) {
  LoadVersion(hModule);
}