#include "inputhook.h"

#include <windows.h>

#include <algorithm>
#include <vector>

#include "utils.h"

namespace {

template <typename Handler>
struct HandlerEntry {
  Handler handler;
  int priority;
};

std::vector<HandlerEntry<KeyboardHandler>> keyboard_handlers;
std::vector<HandlerEntry<MouseHandler>> mouse_handlers;

HHOOK keyboard_hook = nullptr;
HHOOK mouse_hook = nullptr;
POINT lbutton_down_point = {-1, -1};

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HC_ACTION) {
    for (const auto& entry : keyboard_handlers) {
      if (entry.handler(wParam, lParam)) {
        return 1;
      }
    }
  }
  return CallNextHookEx(keyboard_hook, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode != HC_ACTION) {
    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
  }

  if (wParam == WM_MOUSEMOVE || wParam == WM_NCMOUSEMOVE) {
    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
  }

  PMOUSEHOOKSTRUCT pmouse = reinterpret_cast<PMOUSEHOOKSTRUCT>(lParam);

  if (pmouse->dwExtraInfo == GetMagicCode()) {
    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
  }

  if (wParam == WM_LBUTTONDOWN) {
    lbutton_down_point = pmouse->pt;
  }

  for (const auto& entry : mouse_handlers) {
    if (entry.handler(wParam, lParam)) {
      return 1;
    }
  }

  return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
}

}  // namespace

void RegisterKeyboardHandler(KeyboardHandler handler,
                             HandlerPriority priority) {
  keyboard_handlers.emplace_back(std::move(handler), static_cast<int>(priority));
  std::ranges::sort(keyboard_handlers, [](const auto& a, const auto& b) {
    return a.priority < b.priority;
  });
}

void RegisterMouseHandler(MouseHandler handler, HandlerPriority priority) {
  mouse_handlers.emplace_back(std::move(handler), static_cast<int>(priority));
  std::ranges::sort(mouse_handlers, [](const auto& a, const auto& b) {
    return a.priority < b.priority;
  });
}

bool IsKeyPressed(int vk) {
  return vk && (::GetKeyState(vk) & 0x8000) != 0;
}

void InstallInputHooks() {
  keyboard_hook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hInstance,
                                   GetCurrentThreadId());
  mouse_hook =
      SetWindowsHookEx(WH_MOUSE, MouseProc, hInstance, GetCurrentThreadId());
}
