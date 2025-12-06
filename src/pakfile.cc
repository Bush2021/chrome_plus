#include "pakfile.h"

#include <windows.h>

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <memory>
#include <ranges>
#include <span>
#include <vector>

#pragma warning(disable : 4334)
#pragma warning(disable : 4267)
#pragma warning(disable : 4838)

extern "C" {
#include "..\mini_gzip\miniz.c"  // Must be included first

#include "..\mini_gzip\mini_gzip.c"
#include "..\mini_gzip\mini_gzip.h"
}

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

[[maybe_unused]] void PakFind(uint8_t* buffer,
                              uint8_t* pos,
                              std::function<void(uint8_t*, uint32_t)> f) {
  PakEntry* pak_entry = nullptr;
  PakEntry* end_entry = nullptr;

  if (!CheckHeader(buffer, pak_entry, end_entry)) {
    return;
  }

  do {
    PakEntry* next_entry = pak_entry + 1;
    if (pos >= buffer + pak_entry->file_offset &&
        pos <= buffer + next_entry->file_offset) {
      f(buffer + pak_entry->file_offset,
        next_entry->file_offset - pak_entry->file_offset);
      break;
    }

    pak_entry = next_entry;
  } while (pak_entry->resource_id != 0);
}
}  // namespace

void TraversalGZIPFile(uint8_t* buffer,
                       std::function<bool(uint8_t*, uint32_t, size_t&)>&& f) {
  PakEntry* pak_entry = nullptr;
  PakEntry* end_entry = nullptr;

  if (!CheckHeader(buffer, pak_entry, end_entry)) {
    return;
  }

  do {
    PakEntry* next_entry = pak_entry + 1;
    size_t old_size = next_entry->file_offset - pak_entry->file_offset;

    if (old_size < 10 * 1024) {
      pak_entry = next_entry;
      continue;
    }

    constexpr uint8_t kGzipMagic[] = {0x1F, 0x8B, 0x08};
    std::span<uint8_t> entry_data(buffer + pak_entry->file_offset, old_size);
    if (entry_data.size() < sizeof(kGzipMagic) ||
        !std::ranges::equal(entry_data.subspan(0, sizeof(kGzipMagic)),
                            kGzipMagic)) {
      // Not a GZIP file, skipping
      pak_entry = next_entry;
      continue;
    }

    uint32_t original_size =
        *reinterpret_cast<uint32_t*>(buffer + next_entry->file_offset - 4);

    auto unpack_buffer =
        std::make_unique_for_overwrite<uint8_t[]>(original_size);

    if (!unpack_buffer) {
      pak_entry = next_entry;
      continue;
    }

    struct mini_gzip gz;
    mini_gz_start(&gz, buffer + pak_entry->file_offset, old_size);
    uint32_t unpack_len =
        mini_gz_unpack(&gz, unpack_buffer.get(), original_size);

    if (original_size == unpack_len) {
      size_t new_len = old_size;
      bool changed = f(unpack_buffer.get(), unpack_len, new_len);

      if (changed) {
        size_t compress_size = 0;
        // `gzip_compress` is written in C style, so we free it using
        // `std::free`
        std::unique_ptr<void, decltype(&std::free)> compress_buffer_ptr(
            gzip_compress(unpack_buffer.get(), new_len, &compress_size),
            std::free);

        auto* compress_buffer =
            static_cast<uint8_t*>(compress_buffer_ptr.get());

        if (compress_buffer && compress_size < old_size) {
          std::span<uint8_t> src_span(compress_buffer, compress_size);
          std::ranges::copy(src_span.subspan(0, 10), entry_data.begin());
          entry_data[3] = 0x04;
          uint16_t extra_length =
              static_cast<uint16_t>(old_size - compress_size - 2);
          auto extra_len_dest = reinterpret_cast<uint16_t*>(&entry_data[10]);
          *extra_len_dest = extra_length;
          std::ranges::fill(entry_data.subspan(12, extra_length), 0);
          std::ranges::copy(src_span.subspan(10),
                            entry_data.begin() + 12 + extra_length);
        }
      }
    }
    pak_entry = next_entry;
  } while (pak_entry->resource_id != 0);
}
