#include "pakpatch.h"

#include <windows.h>

#include <sddl.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <iterator>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "detours.h"

#include "pakfile.h"
#include "portable.h"
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

HANDLE resources_pak_map = nullptr;

auto RawCreateFileMapping = CreateFileMappingW;
auto RawMapViewOfFile = MapViewOfFile;

// `PakPatch()` runs in the browser and in every renderer (see the loader in
// chrome++.cc: with in-process WebUI resource loading the renderer serves
// `chrome://settings` from its own pak mapping, so it must patch that mapping
// itself). Re-running the content scan in each renderer decompressed every
// entry up to the target and recompressed the patched one, on the
// renderer main thread during `PreSandboxStartup` -- a freeze on every new
// tab and cross-site navigation. Instead the browser scans once and hands the
// result to its children through two inherited environment variables: the
// target's pak resource id, so a renderer that has to decompress handles
// exactly one entry, and the name of a read-only section holding the
// browser's already-patched entry, which a renderer applies with a single
// memcpy and no decompression at all.
constexpr wchar_t kPakTargetIdEnv[] = L"CHROME_PLUS_PAK_RES_ID";
constexpr wchar_t kPakBlobEnv[] = L"CHROME_PLUS_PAK_BLOB";
// The browser sets the id variable to this when its pak has no target, as in a
// Chromium fork without the about page, so its renderers, which map the same
// pak, skip the scan instead of each repeating it in full.
constexpr wchar_t kPakTargetMissing[] = L"none";

struct PakBlobHeader {
  uint32_t resource_id;
  uint32_t length;
};

// Keeps the published section alive for the browser's lifetime so child
// processes can open it by name.
[[maybe_unused]] HANDLE published_blob_section = nullptr;

// The loader calls `PakPatch()` only in the browser and in `--type=renderer`
// children (chrome++.cc `Loader`), so an empty `--type` means the browser.
bool IsBrowserProcess() {
  return GetSwitchValue(GetCommandLineW(), L"--type").value_or(L"").empty();
}

uint16_t GetPakTargetId() {
  wchar_t value[8];
  DWORD len = GetEnvironmentVariableW(kPakTargetIdEnv, value, ARRAYSIZE(value));
  if (len == 0 || len >= ARRAYSIZE(value)) {
    return 0;
  }
  wchar_t* end = nullptr;
  unsigned long id = wcstoul(value, &end, 10);
  if (end == value || id == 0 || id > 0xFFFF) {
    return 0;
  }
  return static_cast<uint16_t>(id);
}

bool IsPakTargetMissing() {
  wchar_t value[8];
  const DWORD len =
      GetEnvironmentVariableW(kPakTargetIdEnv, value, ARRAYSIZE(value));
  return len > 0 && len < ARRAYSIZE(value) &&
         std::wstring_view(value, len) == kPakTargetMissing;
}

// Browser side: copy the patched slot into a named read-only section and
// publish the name through the environment. Read-only is load-bearing: the
// bytes feed the privileged settings WebUI in every renderer, so a writable
// section would be an HTML injection vector into `chrome://settings`. The
// DACL grants read to Everyone and to RestrictedCode (the renderer's
// restricting SID), so a renderer's pre-lockdown token can open the section
// at the same point it maps the pak.
void PublishPatchedEntry(uint8_t* buffer, uint16_t resource_id) {
  // After an in-app restart the new browser inherits the old browser's
  // section name, which dies with that process; drop it and republish under
  // this pid.
  SetEnvironmentVariableW(kPakBlobEnv, nullptr);

  const auto slot = FindResourceSlot(buffer, resource_id);
  if (!slot) {
    return;
  }

  SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, FALSE};
  if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(
          L"D:(A;;GR;;;WD)(A;;GR;;;RC)", SDDL_REVISION_1,
          &sa.lpSecurityDescriptor, nullptr)) {
    return;
  }

  const std::wstring name =
      L"Local\\ChromePlusPakBlob_" + std::to_wstring(GetCurrentProcessId());
  const DWORD size = static_cast<DWORD>(sizeof(PakBlobHeader) + slot->length);
  HANDLE section = CreateFileMappingW(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE,
                                      0, size, name.c_str());
  const DWORD create_error = GetLastError();
  LocalFree(sa.lpSecurityDescriptor);
  if (!section) {
    return;
  }
  // A pre-existing name means another process squatted it; leave children on
  // the decompress fallback rather than trust its contents.
  if (create_error == ERROR_ALREADY_EXISTS) {
    CloseHandle(section);
    return;
  }

  auto* view =
      static_cast<uint8_t*>(MapViewOfFile(section, FILE_MAP_WRITE, 0, 0, size));
  if (!view) {
    CloseHandle(section);
    return;
  }
  auto* header = reinterpret_cast<PakBlobHeader*>(view);
  header->resource_id = resource_id;
  header->length = slot->length;
  memcpy(view + sizeof(PakBlobHeader), buffer + slot->offset, slot->length);
  UnmapViewOfFile(view);

  published_blob_section = section;
  SetEnvironmentVariableW(kPakBlobEnv, name.c_str());
  DebugLog(L"PakPatch: published resource {} ({} bytes) as {}", resource_id,
           slot->length, name);
}

// Renderer fast path: overwrite this process's copy-on-write pak view with
// the browser's already-patched bytes. The slot is re-derived from this
// process's own pak index and the lengths must match, so a section built from
// a different pak is rejected and the copy cannot write outside the slot.
bool ApplyPatchedEntry(uint8_t* buffer) {
  wchar_t name[64];
  DWORD len = GetEnvironmentVariableW(kPakBlobEnv, name, ARRAYSIZE(name));
  if (len == 0 || len >= ARRAYSIZE(name)) {
    return false;
  }

  HANDLE section = OpenFileMappingW(FILE_MAP_READ, FALSE, name);
  if (!section) {
    return false;
  }

  bool applied = false;
  if (const auto* view = static_cast<const uint8_t*>(
          MapViewOfFile(section, FILE_MAP_READ, 0, 0, 0))) {
    const auto* header = reinterpret_cast<const PakBlobHeader*>(view);
    std::optional<PakResourceSlot> slot;
    if (header->resource_id <= 0xFFFF) {
      slot =
          FindResourceSlot(buffer, static_cast<uint16_t>(header->resource_id));
    }
    if (slot && slot->length == header->length) {
      memcpy(buffer + slot->offset, view + sizeof(PakBlobHeader), slot->length);
      applied = true;
      DebugLog(L"PakPatch: applied published resource {}", header->resource_id);
    }
    UnmapViewOfFile(view);
  }
  CloseHandle(section);
  return applied;
}

// The #172 settings-page injection, run on each candidate decompressed pak
// entry until it finds the one holding the settings-about-page HTML.
bool PatchSettingsHtml(uint8_t* begin, uint32_t size, size_t& new_len) {
  BYTE search_start[] = R"(</settings-about-page>)";
  auto match = SearchMemory(
      std::span<uint8_t>(begin, size),
      std::span<const uint8_t>(search_start, sizeof(search_start) - 1));
  if (match.empty()) {
    return false;
  }

  // Compress the HTML for writing patch information.
  std::string html(reinterpret_cast<char*>(begin), size);
  compression_html(html);

  // RemoveUpdateError
  ReplaceStringInPlace(html, R"(?hidden="${!this.showUpdateStatus_}")",
                       R"(hidden="true")");
  ReplaceStringInPlace(html, R"(?hidden="${!this.shouldShowIcons_()}")",
                       R"(hidden="true")");

  const char product_title[] =
      R"({aboutBrowserVersion}</div><div class="secondary">Powered by <a target="_blank" href="https://github.com/Bush2021/chrome_plus">Chrome++ Next</a> )" RELEASE_VER_STR BUILD_ARCH
      R"(</div>)";
  ReplaceStringInPlace(html, R"({aboutBrowserVersion}</div>)", product_title);

  if (html.length() > size) {
    return false;
  }
  memcpy(begin, html.c_str(), html.length());
  new_len = html.length();
  return true;
}

// A file the cache below holds results for. The identity changes whenever
// Chrome replaces resources.pak, which a new version always does.
struct PakFileIdentity {
  uint32_t volume_serial;
  uint64_t file_index;
  uint64_t size;
  uint64_t last_write_time;

  friend bool operator==(const PakFileIdentity&,
                         const PakFileIdentity&) = default;
};

std::optional<PakFileIdentity> GetFileIdentity(HANDLE file) {
  BY_HANDLE_FILE_INFORMATION info;
  if (!GetFileInformationByHandle(file, &info)) {
    return std::nullopt;
  }
  return PakFileIdentity{
      .volume_serial = info.dwVolumeSerialNumber,
      .file_index = (uint64_t{info.nFileIndexHigh} << 32) | info.nFileIndexLow,
      .size = (uint64_t{info.nFileSizeHigh} << 32) | info.nFileSizeLow,
      .last_write_time = (uint64_t{info.ftLastWriteTime.dwHighDateTime} << 32) |
                         info.ftLastWriteTime.dwLowDateTime,
  };
}

// Set where the pak's mapping is created, before `PatchResourcesPak` runs.
std::optional<PakFileIdentity> resources_pak_identity;

// The browser keeps its result on disk, so only the first start with a new
// resources.pak or a new Chrome++ build pays for the scan and the recompression
// (a few hundred milliseconds on the browser's main thread). The file is
// followed by the patched entry. Renderers never read it; they still get the
// browser's result through the read-only section.
struct PakCacheHeader {
  uint32_t magic;
  uint64_t build_hash;
  PakFileIdentity pak;
  // 0 when the pak has no target.
  uint16_t resource_id;
  uint32_t length;
  // Guards against a damaged file, which would otherwise hand Chrome a broken
  // entry on every start.
  uint64_t entry_hash;
};

constexpr uint32_t kPakCacheMagic = 0x43505043;
constexpr wchar_t kPakCacheName[] = L"Chrome++_PakCache.bin";
// The patched bytes embed the build's version and architecture.
constexpr char kPatchBuild[] = RELEASE_VER_STR BUILD_ARCH;
// Bounds the read of a file whose size field is damaged.
constexpr int64_t kMaxPakCacheSize = 64 * 1024 * 1024;

uint64_t HashEntry(std::span<const uint8_t> entry) {
  return Fnv1aHash(std::as_bytes(entry));
}

// Asks chrome_elf for the user data directory, the answer Chrome itself uses
// (`InitializeUserDataDir` in chrome/app/chrome_main_delegate.cc): the
// `--user-data-dir` switch, overridden by the UserDataDir policy, or the
// default. Whoever can write there can already change the profile, so the
// cached entry handed to chrome://settings needs no further check.
std::optional<std::wstring> GetPakCacheDir() {
  using GetUserDataDirectoryThunkFn =
      bool (*)(wchar_t*, size_t, wchar_t*, size_t, bool*);
  HMODULE chrome_elf = GetModuleHandleW(L"chrome_elf.dll");
  if (!chrome_elf) {
    return std::nullopt;
  }
  auto get_user_data_dir = reinterpret_cast<GetUserDataDirectoryThunkFn>(
      GetProcAddress(chrome_elf, "GetUserDataDirectoryThunk"));
  if (!get_user_data_dir) {
    return std::nullopt;
  }
  wchar_t dir[MAX_PATH] = {};
  wchar_t invalid_dir[MAX_PATH] = {};
  if (!get_user_data_dir(dir, std::size(dir), invalid_dir,
                         std::size(invalid_dir), nullptr) ||
      dir[0] == L'\0') {
    return std::nullopt;
  }
  return std::wstring(dir);
}

std::wstring GetPakCachePath(const std::wstring& dir) {
  return dir + L"\\" + kPakCacheName;
}

std::optional<std::vector<uint8_t>> ReadPakCache(const std::wstring& path) {
  HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                            FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return std::nullopt;
  }
  std::optional<std::vector<uint8_t>> data;
  LARGE_INTEGER size;
  if (GetFileSizeEx(file, &size) && size.QuadPart <= kMaxPakCacheSize) {
    std::vector<uint8_t> bytes(static_cast<size_t>(size.QuadPart));
    DWORD read = 0;
    if (ReadFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &read,
                 nullptr) &&
        read == bytes.size()) {
      data = std::move(bytes);
    }
  }
  CloseHandle(file);
  return data;
}

// Applies the cached result for `identity` to `buffer`. Returns the resource
// id it records, 0 when it records that the pak has no target, or nullopt
// when there is no usable result.
std::optional<uint16_t> ApplyCachedResult(uint8_t* buffer,
                                          const PakFileIdentity& identity) {
  const std::optional<std::wstring> dir = GetPakCacheDir();
  if (!dir) {
    return std::nullopt;
  }
  const std::optional<std::vector<uint8_t>> data =
      ReadPakCache(GetPakCachePath(*dir));
  if (!data || data->size() < sizeof(PakCacheHeader)) {
    return std::nullopt;
  }
  PakCacheHeader header;
  memcpy(&header, data->data(), sizeof(header));
  const auto entry = std::span(*data).subspan(sizeof(header));
  if (header.magic != kPakCacheMagic ||
      header.build_hash != Fnv1aHash(std::as_bytes(std::span(kPatchBuild))) ||
      header.pak != identity || entry.size() != header.length ||
      HashEntry(entry) != header.entry_hash) {
    return std::nullopt;
  }
  if (header.resource_id == 0) {
    DebugLog(L"PakPatch: cached result: no target");
    return 0;
  }
  const std::optional<PakResourceSlot> slot =
      FindResourceSlot(buffer, header.resource_id);
  if (!slot || slot->length != entry.size()) {
    return std::nullopt;
  }
  std::ranges::copy(entry, buffer + slot->offset);
  DebugLog(L"PakPatch: applied cached resource {}", header.resource_id);
  return header.resource_id;
}

void SaveResultToCache(uint8_t* buffer,
                       uint16_t resource_id,
                       const PakFileIdentity& identity) {
  const std::optional<std::wstring> dir = GetPakCacheDir();
  if (!dir) {
    return;
  }
  std::span<const uint8_t> entry;
  if (resource_id != 0) {
    const std::optional<PakResourceSlot> slot =
        FindResourceSlot(buffer, resource_id);
    if (!slot) {
      return;
    }
    entry = std::span(buffer + slot->offset, slot->length);
  }
  PakCacheHeader header;
  // Zeroes the padding too, so no stray stack bytes reach the file.
  memset(&header, 0, sizeof(header));
  header.magic = kPakCacheMagic;
  header.build_hash = Fnv1aHash(std::as_bytes(std::span(kPatchBuild)));
  header.pak = identity;
  header.resource_id = resource_id;
  header.length = static_cast<uint32_t>(entry.size());
  header.entry_hash = HashEntry(entry);

  // Written under a temporary name and moved over the old file, so a reader
  // never sees a partly written one. The name is per process because a second
  // chrome.exe that hands its command line to a running browser also gets here.
  const std::wstring path = GetPakCachePath(*dir);
  const std::wstring temp_path =
      path + L"." + std::to_wstring(GetCurrentProcessId()) + L".tmp";
  HANDLE file = CreateFileW(temp_path.c_str(), GENERIC_WRITE, 0, nullptr,
                            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return;
  }
  DWORD written = 0;
  bool ok = WriteFile(file, &header, sizeof(header), &written, nullptr) &&
            written == sizeof(header);
  if (ok && !entry.empty()) {
    ok = WriteFile(file, entry.data(), static_cast<DWORD>(entry.size()),
                   &written, nullptr) &&
         written == entry.size();
  }
  CloseHandle(file);
  if (!ok || !MoveFileExW(temp_path.c_str(), path.c_str(),
                          MOVEFILE_REPLACE_EXISTING)) {
    DeleteFileW(temp_path.c_str());
  }
}

// One flow per process kind: the browser applies its cached result or locates
// and patches the entry itself, then publishes the id and the patched bytes
// for its children, or that there is no target; a renderer takes the cheapest
// tier available --
// nothing to patch, published bytes (no decompression), targeted decompress
// (one entry), full content scan. Runs
// inside `MyMapViewOfFile` after both hooks have detached themselves, so the
// section create/open/map calls in the publish and apply helpers reach the
// real APIs, not our hooks.
void PatchResourcesPak(uint8_t* buffer) {
  const bool is_browser = IsBrowserProcess();
  if (!is_browser && (IsPakTargetMissing() || ApplyPatchedEntry(buffer))) {
    return;
  }

  std::optional<uint16_t> cached_id;
  if (is_browser && resources_pak_identity) {
    cached_id = ApplyCachedResult(buffer, *resources_pak_identity);
  }
  uint16_t matched_id = 0;
  if (cached_id) {
    matched_id = *cached_id;
  } else {
    const uint16_t target_id = GetPakTargetId();
    matched_id = PatchPakResource(buffer, PatchSettingsHtml, target_id);
    if (matched_id == 0 && target_id != 0) {
      // The inherited id missed, so the pak was replaced (browser updated
      // between sessions); redo the full content scan.
      matched_id = PatchPakResource(buffer, PatchSettingsHtml, 0);
    }
    if (is_browser && resources_pak_identity) {
      SaveResultToCache(buffer, matched_id, *resources_pak_identity);
    }
  }

  if (!is_browser) {
    return;
  }
  if (matched_id == 0) {
    DebugLog(L"PakPatch: no target in resources.pak");
    SetEnvironmentVariableW(kPakTargetIdEnv, kPakTargetMissing);
    return;
  }
  SetEnvironmentVariableW(kPakTargetIdEnv, std::to_wstring(matched_id).c_str());
  PublishPatchedEntry(buffer, matched_id);
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
    DetourDetach(&RawMapViewOfFile, MyMapViewOfFile);
    auto status = DetourTransactionCommit();
    if (status != NO_ERROR) {
      DebugLog(L"Unhook RawMapViewOfFile failed {}", status);
    }

    if (buffer) {
      PatchResourcesPak(static_cast<uint8_t*>(buffer));
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
    resources_pak_identity = GetFileIdentity(hFile);
    // Force copy-on-write so the mapped view can be patched in memory.
    resources_pak_map =
        RawCreateFileMapping(hFile, lpAttributes, PAGE_WRITECOPY,
                             dwMaximumSizeHigh, dwMaximumSizeLow, lpName);

    // No more hook needed.
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&RawCreateFileMapping, MyCreateFileMapping);
    auto status = DetourTransactionCommit();
    if (status != NO_ERROR) {
      DebugLog(L"Unhook RawCreateFileMapping failed {}", status);
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&RawMapViewOfFile, MyMapViewOfFile);
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
  DetourAttach(&RawCreateFileMapping, MyCreateFileMapping);
  auto status = DetourTransactionCommit();
  if (status != NO_ERROR) {
    DebugLog(L"Hook RawCreateFileMapping failed {}", status);
  }
}
