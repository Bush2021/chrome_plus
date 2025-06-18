#ifndef CHROME_PLUS_SRC_PAKFILE_H_
#define CHROME_PLUS_SRC_PAKFILE_H_

#include <functional>

void TraversalGZIPFile(uint8_t* buffer,
                       std::function<bool(uint8_t*, uint32_t, size_t&)>&& f);

#endif  // CHROME_PLUS_SRC_PAKFILE_H_
