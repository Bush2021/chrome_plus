#include "keymapping.h"

#include <windows.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "config.h"
#include "utils.h"

namespace {

HHOOK keyboard_hook = nullptr;

struct KeyMapping {
  UINT vk_from;
  std::vector<UINT> vk_to;
  bool ctrl;
  bool shift;
  bool alt;
};

std::vector<KeyMapping> key_mappings;

KeyMapping ParseKeyMapping(std::wstring_view mapping_str) {
  KeyMapping mapping = {};

  auto equal_pos = mapping_str.find(L'=');
  if (equal_pos == std::wstring_view::npos) {
    return mapping;
  }

  std::wstring from_str(mapping_str.substr(0, equal_pos));
  std::wstring to_str(mapping_str.substr(equal_pos + 1));

  std::ranges::transform(from_str, from_str.begin(), ::towlower);
  std::ranges::transform(to_str, to_str.begin(), ::towlower);

  std::vector<std::wstring> from_parts = StringSplit(from_str, L'+');
  std::vector<std::wstring> to_parts = StringSplit(to_str, L'+');

  // Parse source key
  for (const auto& key : from_parts) {
    UINT vk = ParseKeyString(key);
    if (vk != 0) {
      mapping.vk_from = vk;
      break;
    }
  }

  // Parse target keys
  for (const auto& key : to_parts) {
    if (key == L"ctrl") {
      mapping.ctrl = true;
    } else if (key == L"shift") {
      mapping.shift = true;
    } else if (key == L"alt") {
      mapping.alt = true;
    } else {
      UINT vk = ParseKeyString(key);
      if (vk != 0) {
        mapping.vk_to.push_back(vk);
      }
    }
  }

  return mapping;
}

void LoadKeyMappings() {
  key_mappings.clear();

  std::wstring mapping_list = GetIniString(L"keymapping", L"mappings", L"");
  if (mapping_list.empty()) {
    return;
  }

  std::vector<std::wstring> mappings = StringSplit(mapping_list, L';');
  for (const auto& mapping_str : mappings) {
    if (mapping_str.empty()) {
      continue;
    }
    KeyMapping mapping = ParseKeyMapping(mapping_str);
    if (mapping.vk_from != 0 && !mapping.vk_to.empty()) {
      key_mappings.push_back(mapping);
    }
  }
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
    KBDLLHOOKSTRUCT* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

    for (const auto& mapping : key_mappings) {
      if (kb->vkCode == mapping.vk_from) {
        // Simulate target key combination
        std::vector<INPUT> inputs;

        // Press modifier keys
        if (mapping.ctrl) {
          INPUT input = {0};
          input.type = INPUT_KEYBOARD;
          input.ki.wVk = VK_CONTROL;
          input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
          inputs.push_back(input);
        }
        if (mapping.shift) {
          INPUT input = {0};
          input.type = INPUT_KEYBOARD;
          input.ki.wVk = VK_SHIFT;
          input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
          inputs.push_back(input);
        }
        if (mapping.alt) {
          INPUT input = {0};
          input.type = INPUT_KEYBOARD;
          input.ki.wVk = VK_MENU;
          input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
          inputs.push_back(input);
        }

        // Press target keys
        for (UINT vk : mapping.vk_to) {
          INPUT input = {0};
          input.type = INPUT_KEYBOARD;
          input.ki.wVk = vk;
          input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
          inputs.push_back(input);
        }

        // Release target keys
        for (auto it = mapping.vk_to.rbegin(); it != mapping.vk_to.rend();
             ++it) {
          INPUT input = {0};
          input.type = INPUT_KEYBOARD;
          input.ki.wVk = *it;
          input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
          inputs.push_back(input);
        }

        // Release modifier keys
        if (mapping.alt) {
          INPUT input = {0};
          input.type = INPUT_KEYBOARD;
          input.ki.wVk = VK_MENU;
          input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
          inputs.push_back(input);
        }
        if (mapping.shift) {
          INPUT input = {0};
          input.type = INPUT_KEYBOARD;
          input.ki.wVk = VK_SHIFT;
          input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
          inputs.push_back(input);
        }
        if (mapping.ctrl) {
          INPUT input = {0};
          input.type = INPUT_KEYBOARD;
          input.ki.wVk = VK_CONTROL;
          input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
          inputs.push_back(input);
        }

        SendInput(static_cast<UINT>(inputs.size()), inputs.data(),
                  sizeof(INPUT));

        // Block the original key
        return 1;
      }
    }
  }

  return CallNextHookEx(keyboard_hook, nCode, wParam, lParam);
}

}  // namespace

void InstallKeyMapping() {
  LoadKeyMappings();

  if (key_mappings.empty()) {
    return;
  }

  if (keyboard_hook == nullptr) {
    keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0);
  }
}

void UninstallKeyMapping() {
  if (keyboard_hook != nullptr) {
    UnhookWindowsHookEx(keyboard_hook);
    keyboard_hook = nullptr;
  }
}
