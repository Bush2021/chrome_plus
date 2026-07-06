#ifndef CHROME_PLUS_SRC_PAKFILE_H_
#define CHROME_PLUS_SRC_PAKFILE_H_

#include <cstdint>
#include <functional>
#include <optional>

// Byte range of one resource inside a mapped pak, from the pak index.
struct PakResourceSlot {
  uint32_t offset;
  uint32_t length;
};

// Walks the pak's gzip entries, decompressing each candidate and running `f`
// on it until `f` reports it patched its target; returns that entry's
// resource id, or 0 when nothing was patched. A non-zero `target_resource_id`
// skips every other entry without inflating it -- the per-renderer fast path,
// where the browser has already located the target by content and handed its
// id down (see pakpatch.cc).
uint16_t TraversalGZIPFile(uint8_t* buffer,
                           std::function<bool(uint8_t*, uint32_t, size_t&)>&& f,
                           uint16_t target_resource_id = 0);

// Locates `resource_id` from the pak index alone -- no decompression.
std::optional<PakResourceSlot> FindResourceSlot(uint8_t* buffer,
                                                uint16_t resource_id);

#endif  // CHROME_PLUS_SRC_PAKFILE_H_
