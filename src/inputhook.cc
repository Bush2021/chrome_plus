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

// Inserts after every handler of equal priority, so those keep their
// registration order.
template <typename Handler>
void InsertByPriority(std::vector<HandlerEntry<Handler>>& handlers,
                      Handler handler,
                      HandlerPriority priority) {
  const int value = static_cast<int>(priority);
  handlers.insert(std::ranges::upper_bound(handlers, value, {},
                                           &HandlerEntry<Handler>::priority),
                  HandlerEntry<Handler>{std::move(handler), value});
}

HHOOK keyboard_hook = nullptr;
HHOOK mouse_hook = nullptr;

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

  if (wParam == WM_NCMOUSEMOVE) {
    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
  }

  PMOUSEHOOKSTRUCT pmouse = reinterpret_cast<PMOUSEHOOKSTRUCT>(lParam);

  if (pmouse->dwExtraInfo == GetMagicCode()) {
    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
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
  InsertByPriority(keyboard_handlers, std::move(handler), priority);
}

void RegisterMouseHandler(MouseHandler handler, HandlerPriority priority) {
  InsertByPriority(mouse_handlers, std::move(handler), priority);
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
