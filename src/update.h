#ifndef CHROME_PLUS_SRC_UPDATE_H_
#define CHROME_PLUS_SRC_UPDATE_H_

#include <optional>
#include <string>

inline constexpr wchar_t kFinishUpdateSwitch[] = L"--chromeplus-finish-update";

[[nodiscard]] std::optional<std::wstring> StagedChromeVersion();
[[nodiscard]] std::wstring FinishUpdateCommandLine();
int FinishUpdate();

#endif  // CHROME_PLUS_SRC_UPDATE_H_
