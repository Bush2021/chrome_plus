#include "pakpatch.h"

#include <windows.h>

#include <sddl.h>

#include <cstdint>
#include <cstdlib>
#include <string>
#include <string_view>

#include "detours.h"

#include "pakfile.h"
#include "utils.h"
#include "version.h"

namespace {
#if defined(_M_ARM64)
#define BUILD_ARCH " (ARM64)"
#elif defined(_M_X64)
#define BUILD_ARCH " (64-bit)"
#else
#define BUILD_ARCH " (32-bit)"
#endif

static HANDLE resources_pak_map = nullptr;

static auto RawCreateFileMapping = CreateFileMappingW;
static auto RawMapViewOfFile = MapViewOfFile;

// Hand the patch target's pak resource id from the browser to its renderer
// children. The browser searches `resources.pak` by content once and exports
// the id here; renderers inherit it and decompress only that one resource
// instead of every gzip entry up to it (the per-renderer cost from `19db50`).
constexpr wchar_t kPakTargetEnv[] = L"CHROME_PLUS_PAK_RES_ID";

uint16_t GetPakTargetId() {
  wchar_t buffer[8];
  DWORD len = GetEnvironmentVariableW(kPakTargetEnv, buffer, ARRAYSIZE(buffer));
  if (len == 0 || len >= ARRAYSIZE(buffer)) {
    return 0;
  }
  wchar_t* end = nullptr;
  unsigned long value = wcstoul(buffer, &end, 10);
  if (end == buffer || value == 0 || value > 0xFFFF) {
    return 0;
  }
  return static_cast<uint16_t>(value);
}

void SetPakTargetId(uint16_t id) {
  SetEnvironmentVariableW(kPakTargetEnv, std::to_wstring(id).c_str());
}

// Share the browser's already-patched entry with renderer children so they can
// apply it with a single memcpy instead of inflating + recompressing it (the
// level-9 `gzip_compress`, repeated in every renderer). The browser copies the
// patched slot into a named section and hands the name down through the
// environment. The section is read-only to other openers -- it must not be
// writable, because the renderer feeds it straight into the privileged settings
// WebUI -- but readable by the renderer's restricted token, which opens it
// before sandbox lockdown, at the same PreSandboxStartup point it maps the pak.
constexpr wchar_t kPakBlobEnv[] = L"CHROME_PLUS_PAK_BLOB";

struct PakBlobHeader {
  uint32_t resource_id;
  uint32_t length;
};

static HANDLE published_blob_section = nullptr;

std::wstring GetPakBlobName() {
  wchar_t buffer[64];
  DWORD len = GetEnvironmentVariableW(kPakBlobEnv, buffer, ARRAYSIZE(buffer));
  if (len == 0 || len >= ARRAYSIZE(buffer)) {
    return {};
  }
  return std::wstring(buffer, len);
}

void PublishPatchedEntry(uint8_t* buffer, uint16_t resource_id) {
  uint32_t offset = 0;
  uint32_t length = 0;
  if (!FindResourceSlot(buffer, resource_id, &offset, &length)) {
    return;
  }

  // Grant read to Everyone (the normal-SID check) and RestrictedCode (the
  // renderer's restricting SID); no write ACE, so only this creating handle
  // can change the contents.
  SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, FALSE};
  if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(
          L"D:(A;;GR;;;WD)(A;;GR;;;RC)", SDDL_REVISION_1,
          &sa.lpSecurityDescriptor, nullptr)) {
    return;
  }

  std::wstring name =
      L"Local\\ChromePlusPakBlob_" + std::to_wstring(GetCurrentProcessId());
  DWORD size = sizeof(PakBlobHeader) + length;
  HANDLE section = CreateFileMappingW(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE,
                                      0, size, name.c_str());
  DWORD create_error = GetLastError();
  LocalFree(sa.lpSecurityDescriptor);
  if (!section) {
    return;
  }
  // A name that already exists means another process squatted it; do not trust
  // it, fall back to letting renderers decompress for themselves.
  if (create_error == ERROR_ALREADY_EXISTS) {
    CloseHandle(section);
    return;
  }

  if (auto* view = static_cast<uint8_t*>(
          MapViewOfFile(section, FILE_MAP_WRITE, 0, 0, size))) {
    auto* header = reinterpret_cast<PakBlobHeader*>(view);
    header->resource_id = resource_id;
    header->length = length;
    memcpy(view + sizeof(PakBlobHeader), buffer + offset, length);
    UnmapViewOfFile(view);

    // Hold the handle for the process lifetime so children can open the
    // section; it is released when the browser exits.
    published_blob_section = section;
    SetEnvironmentVariableW(kPakBlobEnv, name.c_str());
    DebugLog(
        L"PakPatch: published patched entry pid={} id={} ({} KB) section={}",
        GetCurrentProcessId(), resource_id, length / 1024, name);
  } else {
    CloseHandle(section);
  }
}

bool ApplyPatchedEntry(uint8_t* buffer) {
  std::wstring name = GetPakBlobName();
  if (name.empty()) {
    return false;
  }
  HANDLE section = OpenFileMappingW(FILE_MAP_READ, FALSE, name.c_str());
  if (!section) {
    return false;
  }

  bool applied = false;
  if (auto* view = static_cast<const uint8_t*>(
          MapViewOfFile(section, FILE_MAP_READ, 0, 0, 0))) {
    const auto* header = reinterpret_cast<const PakBlobHeader*>(view);
    uint32_t offset = 0;
    uint32_t length = 0;
    if (header->resource_id <= 0xFFFF &&
        FindResourceSlot(buffer, static_cast<uint16_t>(header->resource_id),
                         &offset, &length) &&
        length == header->length) {
      memcpy(buffer + offset, view + sizeof(PakBlobHeader), length);
      applied = true;
      DebugLog(L"PakPatch: applied shared patched entry pid={} id={} ({} KB)",
               GetCurrentProcessId(), header->resource_id, length / 1024);
    }
    UnmapViewOfFile(view);
  }
  CloseHandle(section);
  return applied;
}

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
    DetourDetach(reinterpret_cast<LPVOID*>(&RawMapViewOfFile),
                 reinterpret_cast<void*>(MyMapViewOfFile));
    auto status = DetourTransactionCommit();
    if (status != NO_ERROR) {
      DebugLog(L"Unhook RawMapViewOfFile failed {}", status);
    }

    if (buffer && !ApplyPatchedEntry(static_cast<BYTE*>(buffer))) {
      auto patch_settings = [=](uint8_t* begin, uint32_t size,
                                size_t& new_len) {
        bool changed = false;

        BYTE search_start[] = R"(</settings-about-page>)";
        auto match = SearchMemory(
            std::span<uint8_t>(begin, size),
            std::span<const uint8_t>(search_start, sizeof(search_start) - 1));
        if (!match.empty()) {
          // Compress the HTML for writing patch information.
          std::string html(reinterpret_cast<char*>(begin), size);
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

          const char prouct_title[] =
              R"({aboutBrowserVersion}</div><div class="secondary">Powered by <a target="_blank" href="https://github.com/Bush2021/chrome_plus">Chrome++ Next</a> )" RELEASE_VER_STR BUILD_ARCH
              R"(</div>)";
          ReplaceStringInPlace(html, R"({aboutBrowserVersion}</div>)",
                               prouct_title);

          if (html.length() <= size) {
            // Write modifications.
            memcpy(begin, html.c_str(), html.length());

            // Modify length.
            new_len = static_cast<uint32_t>(html.length());
            changed = true;
          }
        }

        return changed;
      };

      // Decompress fallback when the shared patched entry is unavailable (the
      // browser, before it has published; or a renderer whose restricted token
      // could not open the section). Targeted to the handed-off id, else a full
      // scan; a full-scan match publishes both the id and the shared section
      // for renderer children.
      uint16_t target_id = GetPakTargetId();
      uint16_t matched_id = TraversalGZIPFile(static_cast<BYTE*>(buffer),
                                              patch_settings, target_id);
      if (target_id != 0 && matched_id == 0) {
        matched_id =
            TraversalGZIPFile(static_cast<BYTE*>(buffer), patch_settings, 0);
      }
      if (target_id == 0 && matched_id != 0) {
        SetPakTargetId(matched_id);
        PublishPatchedEntry(static_cast<BYTE*>(buffer), matched_id);
      }
    }

    return buffer;
  }

  return RawMapViewOfFile(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh,
                          dwFileOffsetLow, dwNumberOfBytesToMap);
}

// Identify `resources.pak` where it is mapped, by querying the handle, rather
// than where it is opened. Chrome maps the pak with
// `CreateFileMapping`/`MapViewOfFile` (`base::MemoryMappedFile`), so this hook
// catches it directly in every process that loads it -- including the renderer,
// which opens the pak by path itself (`AddDataPackFromPath` in
// `ChromeMainDelegate::PreSandboxStartup`, before sandbox lockdown). The
// previous `CreateFile` hook existed only to record that handle so this hook
// could match it later; querying the handle here drops that second hook.
// `GetFileInformationByHandleEx(FileNameInfo)` reads the path of a handle the
// process already holds, which the sandbox permits -- it brokers new opens
// (`NtCreateFile`/`NtOpenFile`), not operations on existing handles.
// https://chromium.googlesource.com/chromium/src/+/main/docs/design/sandbox.md
bool IsResourcesPak(HANDLE hFile) {
  if (hFile == nullptr || hFile == INVALID_HANDLE_VALUE) {
    return false;
  }
  alignas(FILE_NAME_INFO)
      BYTE buffer[sizeof(FILE_NAME_INFO) + MAX_PATH * 2 * sizeof(wchar_t)];
  auto* info = reinterpret_cast<FILE_NAME_INFO*>(buffer);
  if (!GetFileInformationByHandleEx(hFile, FileNameInfo, info,
                                    sizeof(buffer))) {
    return false;
  }
  std::wstring_view name(info->FileName,
                         info->FileNameLength / sizeof(wchar_t));
  return name.ends_with(L"resources.pak");
}

HANDLE WINAPI MyCreateFileMapping(_In_ HANDLE hFile,
                                  _In_opt_ LPSECURITY_ATTRIBUTES lpAttributes,
                                  _In_ DWORD flProtect,
                                  _In_ DWORD dwMaximumSizeHigh,
                                  _In_ DWORD dwMaximumSizeLow,
                                  _In_opt_ LPCTSTR lpName) {
  if (IsResourcesPak(hFile)) {
    // Force copy-on-write so the mapped view can be patched in memory.
    resources_pak_map =
        RawCreateFileMapping(hFile, lpAttributes, PAGE_WRITECOPY,
                             dwMaximumSizeHigh, dwMaximumSizeLow, lpName);

    // No more hook needed.
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(reinterpret_cast<LPVOID*>(&RawCreateFileMapping),
                 reinterpret_cast<void*>(MyCreateFileMapping));
    auto status = DetourTransactionCommit();
    if (status != NO_ERROR) {
      DebugLog(L"Unhook RawCreateFileMapping failed {}", status);
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(reinterpret_cast<LPVOID*>(&RawMapViewOfFile),
                 reinterpret_cast<void*>(MyMapViewOfFile));
    status = DetourTransactionCommit();
    if (status != NO_ERROR) {
      DebugLog(L"Hook RawMapViewOfFile failed {}", status);
    }

    return resources_pak_map;
  }
  return RawCreateFileMapping(hFile, lpAttributes, flProtect, dwMaximumSizeHigh,
                              dwMaximumSizeLow, lpName);
}

}  // namespace

void PakPatch() {
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach(reinterpret_cast<LPVOID*>(&RawCreateFileMapping),
               reinterpret_cast<void*>(MyCreateFileMapping));
  auto status = DetourTransactionCommit();
  if (status != NO_ERROR) {
    DebugLog(L"Hook RawCreateFileMapping failed {}", status);
  }
}
