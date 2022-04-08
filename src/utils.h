#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <functional>

#include "FastSearch.h"

std::wstring Format(const wchar_t* format, va_list args)
{
    std::vector<wchar_t> buffer;

    size_t length = _vscwprintf(format, args);

    buffer.resize((length + 1) * sizeof(wchar_t));

    _vsnwprintf_s(&buffer[0], length + 1, length, format, args);

    return std::wstring(&buffer[0]);;
}

std::wstring Format(const wchar_t* format, ...)
{
    va_list args;

    va_start(args, format);
    auto str = Format(format, args);
    va_end(args);

    return str;
}

void DebugLog(const wchar_t* format, ...)
{
    va_list args;

    va_start(args, format);
    auto str = Format(format, args);
    va_end(args);

    str = Format(L"[chrome++]%s\n", str.c_str());

    OutputDebugStringW(str.c_str());
}

// 搜索内存
uint8_t* memmem(uint8_t* src, int n, const uint8_t* sub, int m)
{
    return (uint8_t*)FastSearch(src, n, sub, m);
}

uint8_t* SearchModuleRaw(HMODULE module, const uint8_t* sub, int m)
{
    uint8_t* buffer = (uint8_t*)module;

    PIMAGE_NT_HEADERS nt_header = (PIMAGE_NT_HEADERS)(buffer + ((PIMAGE_DOS_HEADER)buffer)->e_lfanew);
    PIMAGE_SECTION_HEADER section = (PIMAGE_SECTION_HEADER)((char*)nt_header + sizeof(DWORD) +
        sizeof(IMAGE_FILE_HEADER) + nt_header->FileHeader.SizeOfOptionalHeader);

    for (int i = 0; i < nt_header->FileHeader.NumberOfSections; i++)
    {
        if (strcmp((const char*)section[i].Name, ".text") == 0)
        {
            return memmem(buffer + section[i].PointerToRawData, section[i].SizeOfRawData, sub, m);
            break;
        }
    }
    return NULL;
}

uint8_t* SearchModuleRaw2(HMODULE module, const uint8_t* sub, int m)
{
    uint8_t* buffer = (uint8_t*)module;

    PIMAGE_NT_HEADERS nt_header = (PIMAGE_NT_HEADERS)(buffer + ((PIMAGE_DOS_HEADER)buffer)->e_lfanew);
    PIMAGE_SECTION_HEADER section = (PIMAGE_SECTION_HEADER)((char*)nt_header + sizeof(DWORD) +
        sizeof(IMAGE_FILE_HEADER) + nt_header->FileHeader.SizeOfOptionalHeader);

    for (int i = 0; i < nt_header->FileHeader.NumberOfSections; i++)
    {
        if (strcmp((const char*)section[i].Name, ".rdata") == 0)
        {
            return memmem(buffer + section[i].PointerToRawData, section[i].SizeOfRawData, sub, m);
            break;
        }
    }
    return NULL;
}
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

// 获得程序所在文件夹
std::wstring GetAppDir()
{
    wchar_t path[MAX_PATH];
    ::GetModuleFileName(NULL, path, MAX_PATH);
    ::PathRemoveFileSpec(path);
    return path;
}

#define IDC_NEW_TAB                     34014
#define IDC_CLOSE_TAB                   34015
#define IDC_SELECT_NEXT_TAB             34016
#define IDC_SELECT_PREVIOUS_TAB         34017
#define IDC_SELECT_TAB_0                34018
#define IDC_SELECT_TAB_1                34019
#define IDC_SELECT_TAB_2                34020
#define IDC_SELECT_TAB_3                34021
#define IDC_SELECT_TAB_4                34022
#define IDC_SELECT_TAB_5                34023
#define IDC_SELECT_TAB_6                34024
#define IDC_SELECT_TAB_7                34025
#define IDC_SELECT_LAST_TAB             34026

#define IDC_UPGRADE_DIALOG              40024

HWND GetTopWnd(HWND hwnd)
{
    while (::GetParent(hwnd) && ::IsWindowVisible(::GetParent(hwnd)))
    {
        hwnd = ::GetParent(hwnd);
    }
    return hwnd;
}

void ExecuteCommand(int id, HWND hwnd = 0)
{
    if (hwnd == 0) hwnd = GetForegroundWindow();
    //hwnd = GetTopWnd(hwnd);
    //hwnd = GetForegroundWindow();
    //PostMessage(hwnd, WM_SYSCOMMAND, id, 0);
    ::SendMessageTimeoutW(hwnd, WM_SYSCOMMAND, id, 0, 0, 1000, 0);
}

// 发送按键
template<typename ... T>
void SendKey(T ... keys)
{
    std::vector <INPUT> inputs;
    std::vector <int> keys_ = { keys ... };
    for (auto & key : keys_)
    {
        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
        input.ki.wVk = (WORD)key;
        input.ki.dwExtraInfo = MAGIC_CODE;

        // 修正鼠标消息
        switch (input.ki.wVk)
        {
        case VK_RBUTTON:
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = ::GetSystemMetrics(SM_SWAPBUTTON) == TRUE ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
            input.mi.dwExtraInfo = MAGIC_CODE;
            break;
        case VK_LBUTTON:
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = ::GetSystemMetrics(SM_SWAPBUTTON) == TRUE ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN;
            input.mi.dwExtraInfo = MAGIC_CODE;
            break;
        case VK_MBUTTON:
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
            input.mi.dwExtraInfo = MAGIC_CODE;
            break;
        }

        inputs.push_back(input);
    }
    std::reverse(keys_.begin(), keys_.end());
    for (auto & key : keys_)
    {
        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
        input.ki.wVk = (WORD)key;
        input.ki.dwExtraInfo = MAGIC_CODE;

        // 修正鼠标消息
        switch (input.ki.wVk)
        {
        case VK_RBUTTON:
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = ::GetSystemMetrics(SM_SWAPBUTTON) == TRUE ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
            input.mi.dwExtraInfo = MAGIC_CODE;
            break;
        case VK_LBUTTON:
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = ::GetSystemMetrics(SM_SWAPBUTTON) == TRUE ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP;
            input.mi.dwExtraInfo = MAGIC_CODE;
            break;
        case VK_MBUTTON:
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            input.mi.dwExtraInfo = MAGIC_CODE;
            break;
        }

        inputs.push_back(input);
    }
    //for (auto & key : inputs)
    //{
    //    DebugLog(L"%X %X", key.ki.wVk, key.mi.dwFlags);
    //}

    ::SendInput((UINT)inputs.size(), &inputs[0], sizeof(INPUT));
}


//发送鼠标消息
void SendOneMouse(int mouse)
{
    // 交换左右键
    if (::GetSystemMetrics(SM_SWAPBUTTON) == TRUE)
    {
        if (mouse == MOUSEEVENTF_RIGHTDOWN) mouse = MOUSEEVENTF_LEFTDOWN;
        else if (mouse == MOUSEEVENTF_RIGHTUP) mouse = MOUSEEVENTF_LEFTUP;
    }

    INPUT input[1];
    memset(input, 0, sizeof(input));

    input[0].type = INPUT_MOUSE;

    input[0].mi.dwFlags = mouse;
    input[0].mi.dwExtraInfo = MAGIC_CODE;
    ::SendInput(1, input, sizeof(INPUT));
}

bool isEndWith(const wchar_t *s, const wchar_t *sub)
{
    if (!s || !sub) return false;
    size_t len1 = wcslen(s);
    size_t len2 = wcslen(sub);
    if (len2 > len1) return false;
    return !_memicmp(s + len1 - len2, sub, len2 * sizeof(wchar_t));
}

// 压缩HTML
std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {return !std::isspace(ch); }));
    return s;
}
std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {return !std::isspace(ch); }).base(), s.end());
    return s;
}

std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

std::vector<std::string> split(const std::string &text, char sep) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        std::string temp = text.substr(start, end - start);
        tokens.push_back(temp);
        start = end + 1;
    }
    std::string temp = text.substr(start);
    tokens.push_back(temp);
    return tokens;
}

void compression_html(std::string& html)
{
    auto lines = split(html, '\n');
    html.clear();
    for (auto &line : lines)
    {
        html += "\n";
        html += trim(line);
    }
}

// 替换字符串
bool ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace)
{
    bool find = false;
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
        find = true;
    }
    return find;
}
//bool WriteMemory(PBYTE BaseAddress, PBYTE Buffer, DWORD nSize)
//{
//    DWORD ProtectFlag = 0;
//    if (VirtualProtectEx(GetCurrentProcess(), BaseAddress, nSize, PAGE_EXECUTE_READWRITE, &ProtectFlag))
//    {
//        memcpy(BaseAddress, Buffer, nSize);
//        FlushInstructionCache(GetCurrentProcess(), BaseAddress, nSize);
//        VirtualProtectEx(GetCurrentProcess(), BaseAddress, nSize, ProtectFlag, &ProtectFlag);
//        return true;
//    }
//    return false;
//}
