#ifndef CHROME_PLUS_SRC_PAKFILE_H_
#define CHROME_PLUS_SRC_PAKFILE_H_

#include <cstdint>
#include <functional>

// Walks the pak's gzip entries and runs `f` on each decompressed entry; `f`
// returns true once it has patched its target. When `target_resource_id` is
// non-zero, only the entry with that pak resource id is decompressed (the rest
// are skipped without inflating), which is the per-renderer fast path: the
// browser searches the whole pak by content once and hands the matching id to
// renderer children. Returns the resource id that was patched, or 0 if none.
uint16_t TraversalGZIPFile(uint8_t* buffer,
                           std::function<bool(uint8_t*, uint32_t, size_t&)>&& f,
                           uint16_t target_resource_id = 0);

// Locates a resource's byte slot in the pak by id (index walk, no inflation).
// Fills the entry's file offset and length and returns true if found. Used to
// copy the patched entry out (browser) and to find where to write it back
// (renderer) without re-walking the gzip entries.
bool FindResourceSlot(uint8_t* buffer,
                      uint16_t resource_id,
                      uint32_t* out_offset,
                      uint32_t* out_length);

#endif  // CHROME_PLUS_SRC_PAKFILE_H_
