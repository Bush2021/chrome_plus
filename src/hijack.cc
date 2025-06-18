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

void LoadVersion(HINSTANCE module_handle) {
  auto image_base = reinterpret_cast<PBYTE>(module_handle);
  auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(image_base);
  if (dos_header->e_magic == IMAGE_DOS_SIGNATURE) {
    auto nt_headers =
        reinterpret_cast<PIMAGE_NT_HEADERS>(image_base + dos_header->e_lfanew);
    if (nt_headers->Signature == IMAGE_NT_SIGNATURE) {
      auto export_directory = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(
          image_base +
          nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]
              .VirtualAddress);
      auto name_table = reinterpret_cast<DWORD*>(
          image_base + export_directory->AddressOfNames);
      auto function_table = reinterpret_cast<DWORD*>(
          image_base + export_directory->AddressOfFunctions);
      auto ordinals_table = reinterpret_cast<WORD*>(
          image_base + export_directory->AddressOfNameOrdinals);

      wchar_t system_directory[MAX_PATH + 1];
      GetSystemDirectory(system_directory, MAX_PATH);

      wchar_t dll_path[MAX_PATH + 1];
      lstrcpy(dll_path, system_directory);
      lstrcat(dll_path, TEXT("\\version.dll"));

      HINSTANCE original_dll_handle = LoadLibrary(dll_path);
      if (!original_dll_handle) {
        return;
      }

      for (size_t i = 0; i < export_directory->NumberOfNames; ++i) {
        auto function_name =
            reinterpret_cast<char*>(image_base + name_table[i]);
        auto original_function = reinterpret_cast<PBYTE>(
            GetProcAddress(original_dll_handle, function_name));
        if (original_function) {
          InstallDetours(image_base + function_table[ordinals_table[i]],
                         original_function);
        }
      }
    }
  }
}
}  // namespace

void LoadSysDll(HINSTANCE hModule) {
  LoadVersion(hModule);
}