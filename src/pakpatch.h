#ifndef PAKPATCH_H_
#define PAKPATCH_H_

#include "pakfile.h"

DWORD resources_pak_size = 0;
HANDLE resources_pak_map = nullptr;
HANDLE resources_pak_file = nullptr;

auto RawCreateFile = CreateFileW;
auto RawCreateFileMapping = CreateFileMappingW;
auto RawMapViewOfFile = MapViewOfFile;

HANDLE WINAPI MyMapViewOfFile(_In_ HANDLE hFileMappingObject,
                              _In_ DWORD dwDesiredAccess,
                              _In_ DWORD dwFileOffsetHigh,
                              _In_ DWORD dwFileOffsetLow,
                              _In_ SIZE_T dwNumberOfBytesToMap) {
  if (hFileMappingObject == resources_pak_map) {
    // Modify it to be modifiable.
    LPVOID buffer =
        RawMapViewOfFile(hFileMappingObject, FILE_MAP_COPY, dwFileOffsetHigh,
                         dwFileOffsetLow, dwNumberOfBytesToMap);

    // No more hook needed.
    resources_pak_map = nullptr;
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach((LPVOID*)&RawMapViewOfFile, MyMapViewOfFile);
    auto status = DetourTransactionCommit();
    if (status != NO_ERROR) {
      DebugLog(L"Unhook RawMapViewOfFile failed %d", status);
    } else {
      DebugLog(L"Unhook RawMapViewOfFile success");
    }

    if (buffer) {
      // Traverse the gzip file.
      TraversalGZIPFile((BYTE*)buffer, [=](uint8_t* begin, uint32_t size,
                                           uint32_t& new_len) {
        bool changed = false;

        BYTE search_start[] = R"(</settings-about-page>)";
        uint8_t* pos =
            memmem(begin, size, search_start, sizeof(search_start) - 1);
        if (pos) {
          // Compress the HTML for writing patch information.
          std::string html((char*)begin, size);
          compression_html(html);

          // RemoveUpdateError
          // if (IsNeedPortable())
          {
            ReplaceStringInPlace(html, R"(hidden="[[!showUpdateStatus_]]")",
                                 R"(hidden="true")");
            ReplaceStringInPlace(
                html, R"(hidden="[[!shouldShowIcons_(showUpdateStatus_)]]")",
                R"(hidden="true")");
          }

          const char prouct_title[] = u8R"({aboutBrowserVersion}</div><div class="secondary"><a target="_blank" href="https://github.com/Bush2021/chrome_plus">Chrome++</a> )" RELEASE_VER_STR u8R"( modified version</div>)";
          ReplaceStringInPlace(html, R"({aboutBrowserVersion}</div>)",
                               prouct_title);

          if (html.length() <= size) {
            // Write modifications.
            memcpy(begin, html.c_str(), html.length());

            // Modify length.
            new_len = html.length();
            changed = true;
          }
        }

        return changed;
      });
    }

    return buffer;
  }

  return RawMapViewOfFile(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh,
                          dwFileOffsetLow, dwNumberOfBytesToMap);
}

HANDLE WINAPI MyCreateFileMapping(_In_ HANDLE hFile,
                                  _In_opt_ LPSECURITY_ATTRIBUTES lpAttributes,
                                  _In_ DWORD flProtect,
                                  _In_ DWORD dwMaximumSizeHigh,
                                  _In_ DWORD dwMaximumSizeLow,
                                  _In_opt_ LPCTSTR lpName) {
  if (hFile == resources_pak_file) {
    // Modify it to be modifiable.
    resources_pak_map =
        RawCreateFileMapping(hFile, lpAttributes, PAGE_WRITECOPY,
                             dwMaximumSizeHigh, dwMaximumSizeLow, lpName);

    // No more hook needed.
    resources_pak_file = nullptr;
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach((LPVOID*)&RawCreateFileMapping, MyCreateFileMapping);
    auto status = DetourTransactionCommit();
    if (status != NO_ERROR) {
      DebugLog(L"Unhook RawCreateFileMapping failed %d", status);
    } else {
      DebugLog(L"Unhook RawCreateFileMapping success");
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((LPVOID*)&RawMapViewOfFile, MyMapViewOfFile);
    status = DetourTransactionCommit();
    if (status != NO_ERROR) {
      DebugLog(L"Hook RawMapViewOfFile failed %d", status);
    } else {
      DebugLog(L"Hook RawMapViewOfFile success");
    }

    return resources_pak_map;
  }
  return RawCreateFileMapping(hFile, lpAttributes, flProtect, dwMaximumSizeHigh,
                              dwMaximumSizeLow, lpName);
}

HANDLE WINAPI MyCreateFile(_In_ LPCTSTR lpFileName,
                           _In_ DWORD dwDesiredAccess,
                           _In_ DWORD dwShareMode,
                           _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                           _In_ DWORD dwCreationDisposition,
                           _In_ DWORD dwFlagsAndAttributes,
                           _In_opt_ HANDLE hTemplateFile) {
  HANDLE file = RawCreateFile(lpFileName, dwDesiredAccess, dwShareMode,
                              lpSecurityAttributes, dwCreationDisposition,
                              dwFlagsAndAttributes, hTemplateFile);

  if (isEndWith(lpFileName, L"resources.pak")) {
    resources_pak_file = file;
    resources_pak_size = GetFileSize(resources_pak_file, nullptr);

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((LPVOID*)&RawCreateFileMapping, MyCreateFileMapping);
    auto status = DetourTransactionCommit();
    if (status != NO_ERROR) {
      DebugLog(L"Hook RawCreateFileMapping failed %d", status);
    } else {
      DebugLog(L"Hook RawCreateFileMapping success");
    }

    // No more hook needed.
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach((LPVOID*)&RawCreateFile, MyCreateFile);
    status = DetourTransactionCommit();
    if (status != NO_ERROR) {
      DebugLog(L"Unhook RawCreateFile failed %d", status);
    } else {
      DebugLog(L"Unhook RawCreateFile success");
    }
  }

  return file;
}

void PakPatch() {
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach((LPVOID*)&RawCreateFile, MyCreateFile);
  auto status = DetourTransactionCommit();
  if (status != NO_ERROR) {
    DebugLog(L"Hook RawCreateFile failed %d", status);
  } else {
    DebugLog(L"Hook RawCreateFile success");
  }
}

#endif  // PAKPATCH_H_
