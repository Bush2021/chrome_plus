#ifndef CHROME_PLUS_SRC_INPUTHOOK_H_
#define CHROME_PLUS_SRC_INPUTHOOK_H_

#include <windows.h>

#include <functional>

using KeyboardHandler = std::function<bool(WPARAM wParam, LPARAM lParam)>;
using MouseHandler = std::function<bool(WPARAM wParam, LPARAM lParam)>;

enum class HandlerPriority {
  kHighest = 0,
  kHigh = 100,
  kNormal = 200,
  kLow = 300,
  kLowest = 400,
};

void RegisterKeyboardHandler(
    KeyboardHandler handler,
    HandlerPriority priority = HandlerPriority::kNormal);

void RegisterMouseHandler(MouseHandler handler,
                          HandlerPriority priority = HandlerPriority::kNormal);

bool IsKeyPressed(int vk);

void InstallInputHooks();

#endif  // CHROME_PLUS_SRC_INPUTHOOK_H_
