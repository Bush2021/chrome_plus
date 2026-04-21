# Chrome++ Next
[![LICENSE](https://img.shields.io/badge/License-GPL--3.0--only-blue.svg?style=for-the-badge&logo=github "LICENSE")](https://github.com/Bush2021/chrome_plus/blob/main/LICENSE) [![LAST COMMIT](https://img.shields.io/github/last-commit/Bush2021/chrome_plus?color=blue&logo=github&style=for-the-badge "LAST COMMIT")](https://github.com/Bush2021/chrome_plus/commits/main) [![STARS](https://img.shields.io/github/stars/Bush2021/chrome_plus?color=brightgreen&logo=github&style=for-the-badge "STARS")](https://github.com/Bush2021/chrome_plus/stargazers) ![SIZES](https://img.shields.io/github/languages/code-size/Bush2021/chrome_plus?color=brightgreen&logo=github&style=for-the-badge "SIZES")

English | [简体中文](README.zh-CN.md)

Chrome++ Next is a `version.dll` injection project for Google Chrome. It is loaded alongside `chrome.exe` and augments browser behavior at startup with tab, hotkey, portable, command-line, and policy-related features.

## Overview
- Targets Google Chrome on Windows.
- Works by placing `version.dll` next to `chrome.exe`.
- Focuses on practical browser behavior changes instead of UI wrappers or extensions.
- Prioritizes capabilities that browser extensions cannot implement well, or that external tools do not solve cleanly.

## Support Policy
- Bug reports are accepted only for the latest stable Google Chrome.
- Other Chromium-based browsers may work, but they are not supported targets.
- Reporting requirements are enforced in the GitHub Issues templates.

## Download
- [Latest releases](https://github.com/Bush2021/chrome_plus/releases)

## Installation
- Put `version.dll` in the same directory as `chrome.exe`.
- The recommended installation method is to use the [Chrome offline installer package](https://github.com/Bush2021/chrome_installer), extract it twice, and use the unpacked Chrome program files directly.
- The project is intended for portable Chrome deployments. If you keep updater components or other Chrome remnants on the system, you are responsible for the resulting environment-specific behavior.
- If `version.dll` is not loaded correctly, you can try [setdll](https://github.com/Bush2021/setdll/).

## Capability Overview
### Tab and bookmark behavior
- Double-click to close tabs.
- Right-click to close tabs, with `Shift` preserving the original menu.
- Keep the last tab from closing the browser window.
- Switch tabs with the mouse wheel over the tab strip.
- Switch tabs with the mouse wheel while holding the right mouse button.
- Open omnibox input or bookmarks in a new tab.
- Control new-tab detection through `new_tab_disable` and `new_tab_disable_name`.

### Hotkeys and input remapping
- Configure a boss key to hide and restore Chrome windows, and mute or restore audio along with those actions.
- Configure a translate hotkey.
- Remap hotkeys to other key combinations or Chrome command IDs through `keymapping`.

### Portable deployment and startup behavior
- Override `data_dir` and `cache_dir` for portable use.
- Append Chromium switches through `command_line`.
- Run commands or programs with `launch_on_startup` and `launch_on_exit`.

### Browser environment controls
- Ignore enterprise policies with `ignore_policies`.
- Enable the `win32k` fallback only when Chrome++ itself causes startup crashes.
- Additional public options such as `show_password` remain documented in [`src/chrome++.ini`](src/chrome++.ini).

## Configuration Reference
- See [`src/chrome++.ini`](src/chrome++.ini) for the full public configuration surface.

## License
- Versions 1.5.4 and earlier are licensed under MIT, with all rights reserved by [Shuax](https://github.com/shuax/).
- Versions 1.5.5 through 1.5.9 are licensed under MIT, with modifications by contributors in this repository based on Shuax's version.
- Versions 1.6.0 and later are licensed under [GPL-3.0](LICENSE).

## Thanks
- All [contributors](https://github.com/Bush2021/chrome_plus/graphs/contributors)
- Original author [Shuax](https://github.com/shuax/)
- Revision code [provider](https://forum.ru-board.com/topic.cgi?forum=5&topic=51073&start=620&limit=1&m=1#1) for version 1.5.5
- [面向大海](https://github.com/mxdh/)
- [Ho Cheung](https://github.com/gz83/)
