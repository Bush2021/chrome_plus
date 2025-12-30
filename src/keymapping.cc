#include "keymapping.h"

#include <windows.h>

#include <string>
#include <string_view>
#include <vector>

#include "config.h"
#include "inputhook.h"
#include "utils.h"

namespace {

struct KeyMapping {
  UINT source_vk;
  UINT source_modifiers;
  UINT target_vk;
  UINT target_modifiers;
  int target_command;
};

std::vector<KeyMapping> key_mappings;

bool CheckModifiers(UINT modifiers) {
  const bool shift_ok = !(modifiers & MOD_SHIFT) || IsKeyPressed(VK_SHIFT);
  const bool ctrl_ok = !(modifiers & MOD_CONTROL) || IsKeyPressed(VK_CONTROL);
  const bool alt_ok = !(modifiers & MOD_ALT) || IsKeyPressed(VK_MENU);
  const bool win_ok =
      !(modifiers & MOD_WIN) || IsKeyPressed(VK_LWIN) || IsKeyPressed(VK_RWIN);

  const bool no_extra_shift =
      (modifiers & MOD_SHIFT) || !IsKeyPressed(VK_SHIFT);
  const bool no_extra_ctrl =
      (modifiers & MOD_CONTROL) || !IsKeyPressed(VK_CONTROL);
  const bool no_extra_alt = (modifiers & MOD_ALT) || !IsKeyPressed(VK_MENU);
  const bool no_extra_win = (modifiers & MOD_WIN) ||
                            (!IsKeyPressed(VK_LWIN) && !IsKeyPressed(VK_RWIN));

  return shift_ok && ctrl_ok && alt_ok && win_ok && no_extra_shift &&
         no_extra_ctrl && no_extra_alt && no_extra_win;
}

void AddModifierInput(std::vector<INPUT>& inputs, WORD vk, bool key_up) {
  INPUT input = {};
  input.type = INPUT_KEYBOARD;
  input.ki.wVk = vk;
  input.ki.dwFlags = key_up ? KEYEVENTF_KEYUP : 0;
  input.ki.dwExtraInfo = GetMagicCode();
  inputs.emplace_back(input);
}

void SendModifiers(UINT modifiers, bool key_up) {
  std::vector<INPUT> inputs;

  if (modifiers & MOD_CONTROL) {
    AddModifierInput(inputs, VK_CONTROL, key_up);
  }
  if (modifiers & MOD_SHIFT) {
    AddModifierInput(inputs, VK_SHIFT, key_up);
  }
  if (modifiers & MOD_ALT) {
    AddModifierInput(inputs, VK_MENU, key_up);
  }
  if (modifiers & MOD_WIN) {
    AddModifierInput(inputs, VK_LWIN, key_up);
  }

  if (!inputs.empty()) {
    SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
  }
}

void RestoreModifiersIfHeld(UINT modifiers) {
  std::vector<INPUT> inputs;

  if ((modifiers & MOD_CONTROL) && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
    AddModifierInput(inputs, VK_CONTROL, false);
  }
  if ((modifiers & MOD_SHIFT) && (GetAsyncKeyState(VK_SHIFT) & 0x8000)) {
    AddModifierInput(inputs, VK_SHIFT, false);
  }
  if ((modifiers & MOD_ALT) && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
    AddModifierInput(inputs, VK_MENU, false);
  }
  if ((modifiers & MOD_WIN) && ((GetAsyncKeyState(VK_LWIN) & 0x8000) ||
                                (GetAsyncKeyState(VK_RWIN) & 0x8000))) {
    AddModifierInput(inputs, VK_LWIN, false);
  }

  if (!inputs.empty()) {
    SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
  }
}

void SendMappedKey(const KeyMapping& mapping) {
  // - to_release: source has but target doesn't (user is holding, need to
  // release)
  // - to_press: target has but source doesn't (need to press)
  const UINT to_release = mapping.source_modifiers & ~mapping.target_modifiers;
  const UINT to_press = mapping.target_modifiers & ~mapping.source_modifiers;

  SendModifiers(to_release, true);
  SendModifiers(to_press, false);

  std::vector<INPUT> inputs;
  {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = static_cast<WORD>(mapping.target_vk);
    input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
    input.ki.dwExtraInfo = GetMagicCode();
    inputs.emplace_back(input);
  }
  {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = static_cast<WORD>(mapping.target_vk);
    input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
    input.ki.dwExtraInfo = GetMagicCode();
    inputs.emplace_back(input);
  }
  SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));

  // Release modifiers we pressed
  SendModifiers(to_press, true);

  // Restore modifiers we released (if user still holding them)
  RestoreModifiersIfHeld(to_release);
}

void ExecuteMappedCommand(const KeyMapping& mapping) {
  // For commands, we need to release source modifiers temporarily
  const UINT to_release = mapping.source_modifiers;

  SendModifiers(to_release, true);
  ExecuteCommand(mapping.target_command);
  RestoreModifiersIfHeld(to_release);
}

bool KeyMappingHandler(WPARAM wParam, LPARAM lParam) {
  if (lParam & 0x80000000) {
    return false;
  }

  for (const auto& mapping : key_mappings) {
    if (wParam == mapping.source_vk &&
        CheckModifiers(mapping.source_modifiers)) {
      if (mapping.target_command != 0) {
        ExecuteMappedCommand(mapping);
      } else {
        SendMappedKey(mapping);
      }
      return true;
    }
  }
  return false;
}

int ParseCommand(std::wstring_view str) {
  int result = 0;
  for (const wchar_t c : str) {
    if (c < L'0' || c > L'9') {
      return 0;
    }
    result = result * 10 + (c - L'0');
  }
  return result;
}

}  // namespace

void InitKeyMapping() {
  const auto& mappings = config.GetKeyMappings();

  for (const auto& [source, target] : mappings) {
    KeyMapping mapping = {};

    // Pass false for no_repeat since we don't need MOD_NOREPEAT in key mappings
    UINT source_parsed = ParseHotkeys(source, /*no_repeat=*/false);
    mapping.source_modifiers = LOWORD(source_parsed);
    mapping.source_vk = HIWORD(source_parsed);

    if (mapping.source_vk == 0) {
      DebugLog(L"KeyMapping: Invalid source key '{}'", source);
      continue;
    }

    if (target.starts_with(L"command:")) {
      std::wstring_view command_str = target;
      command_str.remove_prefix(8);
      mapping.target_command = ParseCommand(command_str);
      if (mapping.target_command == 0) {
        DebugLog(L"KeyMapping: Invalid command '{}'", target);
        continue;
      }
    } else {
      UINT target_parsed = ParseHotkeys(target, /*no_repeat=*/false);
      mapping.target_modifiers = LOWORD(target_parsed);
      mapping.target_vk = HIWORD(target_parsed);

      if (mapping.target_vk == 0) {
        DebugLog(L"KeyMapping: Invalid target key '{}'", target);
        continue;
      }
    }

    key_mappings.emplace_back(mapping);
    DebugLog(L"KeyMapping: Loaded {} -> {}", source, target);
  }

  if (!key_mappings.empty()) {
    RegisterKeyboardHandler(KeyMappingHandler, HandlerPriority::kHigh);
    DebugLog(L"KeyMapping: Registered {} mappings", key_mappings.size());
  }
}
