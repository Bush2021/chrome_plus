#ifndef HOTKEY_H_
#define HOTKEY_H_

#include <iterator>

UINT ParseHotkeys(const wchar_t* keys) {
  UINT mo = 0;
  UINT vk = 0;

  std::wstring temp = keys;
  std::vector<std::wstring> key_parts = StringSplit(temp, L'+', L"");

  std::unordered_map<std::wstring, UINT> keyMap = {
      {L"shift", MOD_SHIFT},  {L"ctrl", MOD_CONTROL}, {L"alt", MOD_ALT},
      {L"win", MOD_WIN},      {L"left", VK_LEFT},     {L"right", VK_RIGHT},
      {L"up", VK_UP},         {L"down", VK_DOWN},     {L"←", VK_LEFT},
      {L"→", VK_RIGHT},       {L"↑", VK_UP},          {L"↓", VK_DOWN},
      {L"esc", VK_ESCAPE},    {L"tab", VK_TAB},       {L"backspace", VK_BACK},
      {L"enter", VK_RETURN},  {L"space", VK_SPACE},   {L"prtsc", VK_SNAPSHOT},
      {L"scroll", VK_SCROLL}, {L"pause", VK_PAUSE},   {L"insert", VK_INSERT},
      {L"delete", VK_DELETE}, {L"end", VK_END},       {L"home", VK_HOME},
      {L"pageup", VK_PRIOR},  {L"pagedown", VK_NEXT},
  };

  for (auto& key : key_parts) {
    std::wstring lowerKey;
    std::transform(key.begin(), key.end(), std::back_inserter(lowerKey),
                   ::tolower);

    if (keyMap.count(lowerKey)) {
      if (lowerKey == L"shift" || lowerKey == L"ctrl" || lowerKey == L"alt" ||
          lowerKey == L"win") {
        mo |= keyMap[lowerKey];
      } else {
        vk = keyMap[lowerKey];
      }
    } else {
      TCHAR wch = key[0];
      if (key.length() == 1)  // Parse single characters A-Z, 0-9, etc.
      {
        if (isalnum(wch))
          vk = toupper(wch);
        else
          vk = LOWORD(VkKeyScan(wch));
      } else if (wch == 'F' || wch == 'f')  // Parse the F1-F24 function keys.
      {
        if (isdigit(key[1])) {
          int FX = _wtoi(&key[1]);
          if (FX >= 1 && FX <= 24)
            vk = VK_F1 + FX - 1;
        }
      }
    }
  }

#define MOD_NOREPEAT 0x4000
  mo |= MOD_NOREPEAT;

  return MAKELPARAM(mo, vk);
}

static bool is_hide = false;

static std::vector<HWND> hwnd_list;

BOOL CALLBACK SearchChromeWindow(HWND hWnd, LPARAM lParam) {
  if (IsWindowVisible(hWnd)) {
    wchar_t buff[256];
    GetClassNameW(hWnd, buff, 255);
    if (wcscmp(buff, L"Chrome_WidgetWin_1") ==
        0)  // || wcscmp(buff, L"Chrome_WidgetWin_2")==0 || wcscmp(buff, L"SysShadow")==0 )
    {
      ShowWindow(hWnd, SW_HIDE);
      hwnd_list.push_back(hWnd);
    }
  }
  return true;
}

void HideAndShow() {
  if (!is_hide) {
    EnumWindows(SearchChromeWindow, 0);
  } else {
    for (auto r_iter = hwnd_list.rbegin(); r_iter != hwnd_list.rend();
         r_iter++) {
      ShowWindow(*r_iter, SW_SHOW);
    }
    hwnd_list.clear();
  }
  is_hide = !is_hide;
}

void Translate() {
  ExecuteCommand(IDC_SHOW_TRANSLATE);
  keybd_event(VK_RIGHT, 0, 0, 0);
  keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
}

typedef void (*HotkeyAction)();

void OnHotkey(HotkeyAction action) {
  action();
}

void Hotkey(const std::wstring& keys, HotkeyAction action) {
  if (keys.empty()) {
    return;
  } else {
    UINT flag = ParseHotkeys(keys.c_str());

    std::thread th([flag, action]() {
      RegisterHotKey(NULL, 0, LOWORD(flag), HIWORD(flag));

      MSG msg;
      while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
          OnHotkey(action);
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    });
    th.detach();
  }
}

void GetHotkey() {
  std::wstring bossKey = GetBosskey();
  if (!bossKey.empty()) {
    Hotkey(bossKey, HideAndShow);
  }

  std::wstring translateKey = GetTranslateKey();
  if (!translateKey.empty()) {
    Hotkey(translateKey, Translate);
  }
}

#endif  // HOTKEY_H_
