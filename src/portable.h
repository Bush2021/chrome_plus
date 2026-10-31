#ifndef CHROME_PLUS_SRC_PORTABLE_H_
#define CHROME_PLUS_SRC_PORTABLE_H_

#include <windows.h>

#include <optional>
#include <string>
#include <string_view>

void Portable(LPWSTR param);

// Returns the value of the first switch called `name` in `param`, empty for a
// switch without one and `std::nullopt` when `param` doesn't have it. A URL or
// path in `param` can contain any text, so only the arguments Chrome reads as
// switches count.
std::optional<std::wstring> GetSwitchValue(LPWSTR param,
                                           std::wstring_view name);

#endif  // CHROME_PLUS_SRC_PORTABLE_H_
