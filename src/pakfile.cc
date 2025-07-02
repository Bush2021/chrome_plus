#include "pakfile.h"

#include <windows.h>

#include <functional>

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
    Pak4Header* pak_header =
        reinterpret_cast<Pak4Header*>(buffer + sizeof(uint32_t));
    if (pak_header->encoding != 1)
      return false;

    pak_entry = reinterpret_cast<PakEntry*>(buffer + sizeof(uint32_t) +
                                            sizeof(Pak4Header));
    end_entry = pak_entry + pak_header->num_entries;
  }

  if (version == kPack5FileVersion) {
    Pak5Header* pak_header =
        reinterpret_cast<Pak5Header*>(buffer + sizeof(uint32_t));
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

void PakFind(uint8_t* buffer,
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

    BYTE gzip[] = {0x1F, 0x8B, 0x08};
    size_t gzip_len = sizeof(gzip);
    if (memcmp(buffer + pak_entry->file_offset, gzip, gzip_len) != 0) {
      // Not a gzip file, skip
      pak_entry = next_entry;
      continue;
    }

    uint32_t original_size =
        *reinterpret_cast<uint32_t*>(buffer + next_entry->file_offset - 4);
    uint8_t* unpack_buffer = static_cast<uint8_t*>(malloc(original_size));
    if (!unpack_buffer) {
      return;
    }

    struct mini_gzip gz;
    mini_gz_start(&gz, buffer + pak_entry->file_offset, old_size);
    uint32_t unpack_len = mini_gz_unpack(&gz, unpack_buffer, original_size);

    if (original_size == unpack_len) {
      size_t new_len = old_size;
      bool changed = f(unpack_buffer, unpack_len, new_len);
      if (changed) {
        size_t compress_size = 0;
        uint8_t* compress_buffer = static_cast<uint8_t*>(
            gzip_compress(unpack_buffer, new_len, &compress_size));
        if (compress_buffer && compress_size < old_size) {
          /*FILE *fp = fopen("test.gz", "wb");
          fwrite(compress_buffer, compress_size, 1, fp);
          fclose(fp);*/

          memcpy(buffer + pak_entry->file_offset, compress_buffer, 10);

          // extra
          buffer[pak_entry->file_offset + 3] = 0x04;
          uint16_t extra_length = old_size - compress_size - 2;
          memcpy(buffer + pak_entry->file_offset + 10, &extra_length,
                 sizeof(extra_length));
          memset(buffer + pak_entry->file_offset + 12, '\0', extra_length);

          // compress
          memcpy(buffer + pak_entry->file_offset + 12 + extra_length,
                 compress_buffer + 10, compress_size - 10);

          /*fp = fopen("test2.gz", "wb");
          fwrite(buffer + pak_entry->file_offset, old_size, 1, fp);
          fclose(fp);*/
        } else {
          // DebugLog(L"gzip compress error {} {}", compress_size, old_size);
        }

        if (compress_buffer) {
          free(compress_buffer);
        }
      }
    }

    free(unpack_buffer);
    pak_entry = next_entry;
  } while (pak_entry->resource_id != 0);
}
