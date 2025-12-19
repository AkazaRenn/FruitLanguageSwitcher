#pragma once

#include <Windows.h>

namespace Core {
enum class Message: DWORD {
    Unused = WM_APP,
    ForegroundChanged,
    WindowDestroyed,
    WinKeyUp,
    SwapCategoryTriggered,
    CapsLockOn,
    CapsLockOff,
};

constexpr DWORD VAL(Message msg) {
    return static_cast<DWORD>(msg);
}
}
