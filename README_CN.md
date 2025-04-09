# Chrome++
[![LICENSE](https://img.shields.io/badge/License-GPL--3.0--only-blue.svg?style=for-the-badge&logo=github "LICENSE")](https://github.com/Bush2021/chrome_plus/blob/main/LICENSE) [![LAST COMMIT](https://img.shields.io/github/last-commit/Bush2021/chrome_plus?color=blue&logo=github&style=for-the-badge "LAST COMMIT")](https://github.com/Bush2021/chrome_plus/commits/main)  [![STARS](https://img.shields.io/github/stars/Bush2021/chrome_plus?color=brightgreen&logo=github&style=for-the-badge "STARS")](https://github.com/Bush2021/chrome_plus/stargazers) ![SIZES](https://img.shields.io/github/languages/code-size/Bush2021/chrome_plus?color=brightgreen&logo=github&style=for-the-badge "SIZES")

English instruction: [https://github.com/Bush2021/chrome_plus/blob/main/README.md](https://github.com/Bush2021/chrome_plus/blob/main/README.md)

## 功能
- 双击关闭标签页。
- 右键关闭标签页（按住 Shift 弹出原有菜单）。
- 保留最后标签页（防止关闭最后一个标签页时关闭浏览器，点关闭按钮不行）。
- 鼠标悬停标签栏时使用滚轮切换标签页。
- 按住右键时使用滚轮切换标签页。
- 新建标签页打开地址栏输入的内容（可配置前台或后台打开）。
- 新建标签页打开书签（可配置前台或后台打开）。
- 当前为新标签页时，可以禁用上面两个功能。
- 自定义快捷键快速隐藏浏览器窗口（老板键）。
- 自定义快捷键进行网页翻译。
- 便携化（不兼容原版数据，可以重装系统换电脑不丢数据）。
- 可以自定义 Chromium 命令行开关。
- 更多功能参见 [INI 配置文件](https://github.com/Bush2021/chrome_plus/blob/main/src/chrome%2B%2B.ini)。

## 获取
采用 GitHub Actions 自动编译发布，下载地址：[https://github.com/Bush2021/chrome_plus/releases](https://github.com/Bush2021/chrome_plus/releases)。

## 安装
请确保将 `version.dll` 放入 `chrome.exe` 同一目录。建议下载 [Chrome 离线安装包](https://github.com/Bush2021/chrome_installer)，解压两次得到 Chrome 程序文件，将其放入 [App](https://github.com/Bush2021/chrome_plus/releases/latest) 文件夹即可。

## 兼容性
* 理论上支持所有基于 Chromium 最新稳定分支的浏览器。
* 只可能针对 Chrome 最新稳定版进行测试，不保证维护。
* 如果遇到 DLL 未正确加载的问题，可尝试 [强制注入 DLL](https://github.com/Bush2021/setdll/)。

## 许可证
* 1.5.4 及以前的版本使用 MIT 许可证，版权所有者为 [Shuax](https://github.com/shuax/)。
* 1.5.5 - 1.5.9 版本使用 MIT 许可证，由本仓库贡献者在 Shuax 版本上进行修改。
* 1.6.0 以后的版本使用 GPL-3.0 许可证。

## 致谢
* 原作者 [Shuax](https://github.com/shuax/)
* 1.5.5 修改代码[提供者](https://forum.ru-board.com/topic.cgi?forum=5&topic=51073&start=620&limit=1&m=1#1)
* [虫子樱桃](https://github.com/czyt/)
* [York Waugh](https://github.com/YorkWaugh/)
* [面向大海](https://github.com/mxdh/)
* [Ho Cheung](https://github.com/gz83/)
* [Ritchie1108](https://github.com/Ritchie1108/)