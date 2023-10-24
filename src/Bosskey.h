#include <thread>
#include <vector>

#include <tchar.h>

UINT ParseHotkeys(const TCHAR *keys)
{
    UINT mo = 0;
    UINT vk = 0;

    TCHAR *temp = _tcsdup(keys);
    StringSplit(temp, '+', [&mo, &vk](TCHAR *key) {
        // 解析控制键
        if (_tcsicmp(key, _T("Shift")) == 0)
            mo |= MOD_SHIFT;
        else if (_tcsicmp(key, _T("Ctrl")) == 0)
            mo |= MOD_CONTROL;
        else if (_tcsicmp(key, _T("Alt")) == 0)
            mo |= MOD_ALT;
        else if (_tcsicmp(key, _T("Win")) == 0)
            mo |= MOD_WIN;

        TCHAR wch = key[0];
        if (_tcslen(key) == 1) // 解析单个字符A-Z、0-9等
        {
            if (isalnum(wch))
                vk = toupper(wch);
            else
                vk = LOWORD(VkKeyScan(wch));
        }
        else if (wch == 'F' || wch == 'f') // 解析F1-F24功能键
        {
            if (isdigit(key[1]))
            {
                int FX = _ttoi(&key[1]);
                if (FX >= 1 && FX <= 24)
                    vk = VK_F1 + FX - 1;
            }
        }
        else // 解析其他按键
        {
            if (_tcsicmp(key, _T("Left")) == 0)
                vk = VK_LEFT;
            else if (_tcsicmp(key, _T("Right")) == 0)
                vk = VK_RIGHT;
            else if (_tcsicmp(key, _T("Up")) == 0)
                vk = VK_UP;
            else if (_tcsicmp(key, _T("Down")) == 0)
                vk = VK_DOWN;

            else if (_tcsicmp(key, _T("←")) == 0)
                vk = VK_LEFT;
            else if (_tcsicmp(key, _T("→")) == 0)
                vk = VK_RIGHT;
            else if (_tcsicmp(key, _T("↑")) == 0)
                vk = VK_UP;
            else if (_tcsicmp(key, _T("↓")) == 0)
                vk = VK_DOWN;

            else if (_tcsicmp(key, _T("Esc")) == 0)
                vk = VK_ESCAPE;
            else if (_tcsicmp(key, _T("Tab")) == 0)
                vk = VK_TAB;

            else if (_tcsicmp(key, _T("Backspace")) == 0)
                vk = VK_BACK;
            else if (_tcsicmp(key, _T("Enter")) == 0)
                vk = VK_RETURN;
            else if (_tcsicmp(key, _T("Space")) == 0)
                vk = VK_SPACE;

            else if (_tcsicmp(key, _T("PrtSc")) == 0)
                vk = VK_SNAPSHOT;
            else if (_tcsicmp(key, _T("Scroll")) == 0)
                vk = VK_SCROLL;
            else if (_tcsicmp(key, _T("Pause")) == 0)
                vk = VK_PAUSE;

            else if (_tcsicmp(key, _T("Insert")) == 0)
                vk = VK_INSERT;
            else if (_tcsicmp(key, _T("Delete")) == 0)
                vk = VK_DELETE;

            else if (_tcsicmp(key, _T("End")) == 0)
                vk = VK_END;
            else if (_tcsicmp(key, _T("Home")) == 0)
                vk = VK_HOME;

            else if (_tcsicmp(key, _T("PageUp")) == 0)
                vk = VK_PRIOR;
            else if (_tcsicmp(key, _T("PageDown")) == 0)
                vk = VK_NEXT;
        }
    });

    free(temp);

#define MOD_NOREPEAT 0x4000
    mo |= MOD_NOREPEAT;

    return MAKELPARAM(mo, vk);
}

static bool is_hide = false;

static std::vector<HWND> hwnd_list;

BOOL CALLBACK SearchChromeWindow(HWND hWnd, LPARAM lParam)
{
    // 隐藏
    if (IsWindowVisible(hWnd))
    {
        wchar_t buff[256];
        GetClassNameW(hWnd, buff, 255);
        if (wcscmp(buff, L"Chrome_WidgetWin_1") == 0) // || wcscmp(buff, L"Chrome_WidgetWin_2")==0 || wcscmp(buff, L"SysShadow")==0 )
        {
            ShowWindow(hWnd, SW_HIDE);
            hwnd_list.push_back(hWnd);
        }
    }
    return true;
}

void OnBosskey()
{
    if (!is_hide)
    {
        EnumWindows(SearchChromeWindow, 0);
    }
    else
    {
        for (auto r_iter = hwnd_list.rbegin(); r_iter != hwnd_list.rend(); r_iter++)
        {
            ShowWindow(*r_iter, SW_SHOW);
        }
        hwnd_list.clear();
    }
    is_hide = !is_hide;
}

void HotKeyRegister(LPARAM lParam)
{
    RegisterHotKey(NULL, 0, LOWORD(lParam), HIWORD(lParam));

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_HOTKEY)
        {
            OnBosskey();
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Bosskey()
{
    std::wstring keys = GetBosskey();
    if (keys.empty())
    {
        return;
    }
    else
    {
        UINT flag = ParseHotkeys(keys.c_str());

        std::thread th(HotKeyRegister, flag);
        th.detach();
    }
}