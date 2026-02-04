#ifndef CHROME_PLUS_SRC_UIA_H_
#define CHROME_PLUS_SRC_UIA_H_

#include <windows.h>

#include <oleacc.h>
#include <uiautomation.h>
#include <wrl/client.h>

#include <optional>
#include <string>

namespace uia {

// Check if a point is on the tab close button using UIA.
// This is more accurate than IAccessible as it can check ClassName directly.
// Returns:
//   - std::nullopt: UIA initialization failed or error occurred (caller should
//                   fallback to IAccessible)
//   - true:  Point is on a tab close button
//   - false: Point is NOT on a tab close button
std::optional<bool> IsOnCloseButton(POINT pt);

}  // namespace uia

#endif  // CHROME_PLUS_SRC_UIA_H_
