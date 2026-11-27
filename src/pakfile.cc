#include "pakfile.h"

#include <windows.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <memory>
#include <ranges>
#include <span>

#pragma warning(push)
#pragma warning(disable : 4334)
#pragma warning(disable : 4267)
#pragma warning(disable : 4838)
extern "C" {
#include "mini_gzip.h"
void* gzip_compress(uint8_t* data, size_t len, size_t* out_len);
int mini_gz_start(struct mini_gzip* gz_ptr, const void* mem, size_t mem_len);
int mini_gz_unpack(struct mini_gzip* gz_ptr, void* mem_out, size_t mem_out_len);
}
#pragma warning(pop)

#define ZSTD_STATIC_LINKING_ONLY
#include "zstd.h"

#include "utils.h"

namespace {
#pragma pack(push)
#pragma pack(1)

constexpr int kPack4FileVersion = 4;
constexpr int kPack5FileVersion = 5;

struct Pak4Header {
  uint32_t num_entries;
  uint8_t encoding;
};

struct Pak5Header {
  uint32_t encoding;
  uint16_t resource_count;
  uint16_t alias_count;
};

struct PakEntry {
  uint16_t resource_id;
  uint32_t file_offset;
};

struct PakAlias {
  uint16_t resource_id;
  uint16_t entry_index;
};
#pragma pack(pop)

bool CheckHeader(uint8_t* buffer, PakEntry*& pak_entry, PakEntry*& end_entry) {
  uint32_t version = *reinterpret_cast<uint32_t*>(buffer);

  if (version != kPack4FileVersion && version != kPack5FileVersion)
    return false;

  if (version == kPack4FileVersion) {
    auto* pak_header = reinterpret_cast<Pak4Header*>(buffer + sizeof(uint32_t));
    if (pak_header->encoding != 1)
      return false;

    pak_entry = reinterpret_cast<PakEntry*>(buffer + sizeof(uint32_t) +
                                            sizeof(Pak4Header));
    end_entry = pak_entry + pak_header->num_entries;
  }

  if (version == kPack5FileVersion) {
    auto* pak_header = reinterpret_cast<Pak5Header*>(buffer + sizeof(uint32_t));
    if (pak_header->encoding != 1)
      return false;

    pak_entry = reinterpret_cast<PakEntry*>(buffer + sizeof(uint32_t) +
                                            sizeof(Pak5Header));
    end_entry = pak_entry + pak_header->resource_count;
  }

  // In order to save the "next item" of the last item,
  // the id of this special item must be 0
  if (!end_entry || end_entry->resource_id != 0) {
    return false;
  }

  return true;
}

using PatchCallback = std::function<bool(uint8_t*, uint32_t, size_t&)>;

// The patch target is far larger, so smaller entries are skipped without
// being decompressed.
constexpr size_t kMinCandidateSize = 10 * 1024;
// Bounds the allocation for a size read from a damaged entry. Chrome 160's
// largest resource decompresses to under 3 MiB.
constexpr uint64_t kMaxContentSize = 32 * 1024 * 1024;
constexpr uint8_t kGzipMagic[] = {0x1F, 0x8B, 0x08};
constexpr uint8_t kZstdMagic[] = {0x28, 0xB5, 0x2F, 0xFD};
// The level grit compresses resources with (tools/grit/grit/node/zstd_util.py
// in Chromium), so the patched entry compresses about as well as the original
// and fits its slot.
constexpr int kZstdLevel = 15;

bool StartsWith(std::span<const uint8_t> data,
                std::span<const uint8_t> prefix) {
  return data.size() >= prefix.size() &&
         std::ranges::equal(data.first(prefix.size()), prefix);
}

// Returns whether `f` claimed the entry. The entry is rewritten only when the
// recompressed data fits, padded through the gzip FEXTRA field.
bool PatchGzipEntry(std::span<uint8_t> entry, const PatchCallback& f) {
  uint32_t original_size =
      *reinterpret_cast<uint32_t*>(entry.data() + entry.size() - 4);
  if (original_size > kMaxContentSize) {
    return false;
  }

  auto unpack_buffer = std::make_unique_for_overwrite<uint8_t[]>(original_size);

  struct mini_gzip gz;
  mini_gz_start(&gz, entry.data(), entry.size());
  uint32_t unpack_len = mini_gz_unpack(&gz, unpack_buffer.get(), original_size);
  if (original_size != unpack_len) {
    return false;
  }

  size_t new_len = entry.size();
  if (!f(unpack_buffer.get(), unpack_len, new_len)) {
    return false;
  }

  size_t compress_size = 0;
  // `gzip_compress` is written in C style, so we free it using `std::free`
  std::unique_ptr<void, decltype(&std::free)> compress_buffer_ptr(
      gzip_compress(unpack_buffer.get(), new_len, &compress_size), std::free);

  auto* compress_buffer = static_cast<uint8_t*>(compress_buffer_ptr.get());

  // The FEXTRA field takes 2 bytes for its length and pads by at most 65535.
  if (compress_buffer && compress_size + 2 <= entry.size() &&
      entry.size() - compress_size - 2 <= UINT16_MAX) {
    std::span<uint8_t> src_span(compress_buffer, compress_size);
    std::ranges::copy(src_span.subspan(0, 10), entry.begin());
    entry[3] = 0x04;
    uint16_t extra_length =
        static_cast<uint16_t>(entry.size() - compress_size - 2);
    auto extra_len_dest = reinterpret_cast<uint16_t*>(&entry[10]);
    *extra_len_dest = extra_length;
    std::ranges::fill(entry.subspan(12, extra_length), 0);
    std::ranges::copy(src_span.subspan(10), entry.begin() + 12 + extra_length);
  }
  return true;
}

// Encodes `content` over `entry` as one frame followed by a skippable frame
// that fills the rest, since the pak index fixes the entry's length. Checks the
// result the way Chrome reads it, and leaves `entry` untouched when the frame
// doesn't fit or doesn't decode back to `content`.
bool WriteZstdEntry(std::span<const uint8_t> content,
                    std::span<uint8_t> entry) {
  auto encoded = std::make_unique_for_overwrite<uint8_t[]>(entry.size());
  const size_t frame_size = ZSTD_compress(
      encoded.get(), entry.size(), content.data(), content.size(), kZstdLevel);
  if (ZSTD_isError(frame_size)) {
    return false;
  }

  const size_t padding = entry.size() - frame_size;
  if (padding != 0) {
    if (padding < ZSTD_SKIPPABLEHEADERSIZE) {
      return false;
    }
    // Little-endian, as zstd requires and every Windows target is.
    const uint32_t header[] = {
        ZSTD_MAGIC_SKIPPABLE_START,
        static_cast<uint32_t>(padding - ZSTD_SKIPPABLEHEADERSIZE)};
    static_assert(sizeof(header) == ZSTD_SKIPPABLEHEADERSIZE);
    memcpy(encoded.get() + frame_size, header, sizeof(header));
    memset(encoded.get() + frame_size + sizeof(header), 0,
           padding - sizeof(header));
  }

  if (ZSTD_getFrameContentSize(encoded.get(), entry.size()) != content.size()) {
    return false;
  }
  auto decoded = std::make_unique_for_overwrite<uint8_t[]>(content.size());
  const size_t decoded_size = ZSTD_decompress(decoded.get(), content.size(),
                                              encoded.get(), entry.size());
  if (decoded_size != content.size() ||
      !std::ranges::equal(std::span(decoded.get(), decoded_size), content)) {
    return false;
  }

  std::ranges::copy(std::span(encoded.get(), entry.size()), entry.begin());
  return true;
}

// Returns whether `f` claimed the entry. See `WriteZstdEntry` for when the
// entry is rewritten.
bool PatchZstdEntry(std::span<uint8_t> entry, const PatchCallback& f) {
  // grit stores the content size in every frame, and Chrome CHECKs that it is
  // there (`DecompressIfNeeded` in ui/base/resource/resource_bundle.cc).
  const uint64_t content_size =
      ZSTD_getFrameContentSize(entry.data(), entry.size());
  if (content_size == ZSTD_CONTENTSIZE_UNKNOWN ||
      content_size == ZSTD_CONTENTSIZE_ERROR ||
      content_size > kMaxContentSize) {
    return false;
  }

  auto content = std::make_unique_for_overwrite<uint8_t[]>(content_size);
  if (ZSTD_decompress(content.get(), content_size, entry.data(),
                      entry.size()) != content_size) {
    return false;
  }

  size_t new_len = content_size;
  if (!f(content.get(), static_cast<uint32_t>(content_size), new_len)) {
    return false;
  }

  if (!WriteZstdEntry(std::span<const uint8_t>(content.get(), new_len),
                      entry)) {
    DebugLog(L"PakPatch: patched entry doesn't fit its {}-byte slot",
             entry.size());
  }
  return true;
}

}  // namespace

uint16_t PatchPakResource(uint8_t* buffer,
                          std::function<bool(uint8_t*, uint32_t, size_t&)>&& f,
                          uint16_t target_resource_id) {
  PakEntry* pak_entry = nullptr;
  PakEntry* end_entry = nullptr;

  if (!CheckHeader(buffer, pak_entry, end_entry)) {
    return 0;
  }

  do {
    PakEntry* next_entry = pak_entry + 1;
    std::span<uint8_t> entry(buffer + pak_entry->file_offset,
                             next_entry->file_offset - pak_entry->file_offset);
    const bool is_candidate =
        target_resource_id == 0 || pak_entry->resource_id == target_resource_id;
    if (is_candidate && entry.size() >= kMinCandidateSize) {
      bool claimed = false;
      if (StartsWith(entry, kGzipMagic)) {
        claimed = PatchGzipEntry(entry, f);
      } else if (StartsWith(entry, kZstdMagic)) {
        claimed = PatchZstdEntry(entry, f);
      }
      // Only one resource is the patch target; once the callback has handled
      // it there is nothing left to find, so stop scanning the rest of the
      // pak to avoid decompressing every remaining entry in each renderer
      // process.
      if (claimed) {
        return pak_entry->resource_id;
      }
    }
    pak_entry = next_entry;
  } while (pak_entry->resource_id != 0);

  return 0;
}

std::optional<PakResourceSlot> FindResourceSlot(uint8_t* buffer,
                                                uint16_t resource_id) {
  PakEntry* pak_entry = nullptr;
  PakEntry* end_entry = nullptr;

  if (!CheckHeader(buffer, pak_entry, end_entry)) {
    return std::nullopt;
  }

  do {
    PakEntry* next_entry = pak_entry + 1;
    if (pak_entry->resource_id == resource_id) {
      return PakResourceSlot{pak_entry->file_offset,
                             next_entry->file_offset - pak_entry->file_offset};
    }
    pak_entry = next_entry;
  } while (pak_entry->resource_id != 0);

  return std::nullopt;
}
