#pragma warning(disable : 4334)
#pragma warning(disable : 4267)

extern "C"
{
#include "..\mini_gzip\miniz.c"
#include "..\mini_gzip\mini_gzip.h"
#include "..\mini_gzip\mini_gzip.c"
}

#pragma pack(push)
#pragma pack(1)

#define PACK4_FILE_VERSION (4)
#define PACK5_FILE_VERSION (5)

struct PAK4_HEADER
{
    uint32_t num_entries;
    uint8_t encodeing;
};

struct PAK5_HEADER
{
    uint32_t encodeing;
    uint16_t resource_count;
    uint16_t alias_count;
};

struct PAK_ENTRY
{
    uint16_t resource_id;
    uint32_t file_offset;
};

struct PAK_ALIAS
{
    uint16_t resource_id;
    uint16_t entry_index;
};
#pragma pack(pop)

bool CheckHeader(uint8_t *buffer, PAK_ENTRY *&pak_entry, PAK_ENTRY *&end_entry)
{
    uint32_t version = *(uint32_t *)buffer;

    if (version != PACK4_FILE_VERSION && version != PACK5_FILE_VERSION)
        return false;

    if (version == PACK4_FILE_VERSION)
    {
        PAK4_HEADER *pak_header = (PAK4_HEADER *)(buffer + sizeof(uint32_t));
        if (pak_header->encodeing != 1)
            return false;

        pak_entry = (PAK_ENTRY *)(buffer + sizeof(uint32_t) + sizeof(PAK4_HEADER));
        end_entry = pak_entry + pak_header->num_entries;
    }

    if (version == PACK5_FILE_VERSION)
    {
        PAK5_HEADER *pak_header = (PAK5_HEADER *)(buffer + sizeof(uint32_t));
        if (pak_header->encodeing != 1)
            return false;

        pak_entry = (PAK_ENTRY *)(buffer + sizeof(uint32_t) + sizeof(PAK5_HEADER));
        end_entry = pak_entry + pak_header->resource_count;
    }

    // 为了保存最后一条的"下一条"，这条特殊的条目的id一定为0
    if (!end_entry || end_entry->resource_id != 0)
        return false;

    return true;
}

template <typename Function>
void PakFind(uint8_t *buffer, uint8_t *pos, Function f)
{
    PAK_ENTRY *pak_entry = NULL;
    PAK_ENTRY *end_entry = NULL;

    // 检查文件头
    if (!CheckHeader(buffer, pak_entry, end_entry))
        return;

    do
    {
        PAK_ENTRY *next_entry = pak_entry + 1;
        if (pos >= buffer + pak_entry->file_offset && pos <= buffer + next_entry->file_offset)
        {
            f(buffer + pak_entry->file_offset, next_entry->file_offset - pak_entry->file_offset);
            break;
        }

        pak_entry = next_entry;
    } while (pak_entry->resource_id != 0);
}

template <typename Function>
void TraversalGZIPFile(uint8_t *buffer, Function f)
{
    PAK_ENTRY *pak_entry = NULL;
    PAK_ENTRY *end_entry = NULL;

    // 检查文件头
    if (!CheckHeader(buffer, pak_entry, end_entry))
        return;

    do
    {
        PAK_ENTRY *next_entry = pak_entry + 1;
        uint32_t old_size = next_entry->file_offset - pak_entry->file_offset;

        if (old_size < 10 * 1024)
        {
            // 小于10k文件跳过
            pak_entry = next_entry;
            continue;
        }

        BYTE gzip[] = {0x1F, 0x8B, 0x08};
        size_t gzip_len = sizeof(gzip);
        if (memcmp(buffer + pak_entry->file_offset, gzip, gzip_len) != 0)
        {
            // 不是gzip文件跳过
            pak_entry = next_entry;
            continue;
        }

        uint32_t original_size = *(uint32_t *)(buffer + next_entry->file_offset - 4);
        uint8_t *unpack_buffer = (uint8_t *)malloc(original_size);
        if (!unpack_buffer)
            return;

        struct mini_gzip gz;
        mini_gz_start(&gz, buffer + pak_entry->file_offset, old_size);
        int unpack_len = mini_gz_unpack(&gz, unpack_buffer, original_size);

        if (original_size == unpack_len)
        {
            uint32_t new_len = old_size;
            bool changed = f(unpack_buffer, unpack_len, new_len);
            if (changed)
            {
                // 如果有改变
                size_t compress_size = 0;
                uint8_t *compress_buffer = (uint8_t *)gzip_compress(unpack_buffer, new_len, &compress_size);
                if (compress_buffer && compress_size < old_size)
                {
                    /*FILE *fp = fopen("test.gz", "wb");
                    fwrite(compress_buffer, compress_size, 1, fp);
                    fclose(fp);*/

                    // gzip头
                    memcpy(buffer + pak_entry->file_offset, compress_buffer, 10);

                    // extra
                    buffer[pak_entry->file_offset + 3] = 0x04;
                    uint16_t extra_length = old_size - compress_size - 2;
                    memcpy(buffer + pak_entry->file_offset + 10, &extra_length, sizeof(extra_length));
                    memset(buffer + pak_entry->file_offset + 12, '\0', extra_length);

                    // compress
                    memcpy(buffer + pak_entry->file_offset + 12 + extra_length, compress_buffer + 10, compress_size - 10);

                    /*fp = fopen("test2.gz", "wb");
                    fwrite(buffer + pak_entry->file_offset, old_size, 1, fp);
                    fclose(fp);*/
                }
                else
                {
                    DebugLog(L"gzip compress error %d %d", compress_size, old_size);
                }

                if (compress_buffer)
                    free(compress_buffer);
            }
        }

        free(unpack_buffer);
        pak_entry = next_entry;
    } while (pak_entry->resource_id != 0);
}
