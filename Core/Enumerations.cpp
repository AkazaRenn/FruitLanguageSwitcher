#pragma once

#include <Windows.h>

namespace Core {
enum class Message: DWORD {
    Unused = WM_APP,
    ForegroundChanged, // lParam: HWND
    WindowDestroyed,
    WinKeyUp,
    SwapCategoryTriggered,
    CapsLockOn,
    CapsLockOff,
    ShowPopup, // wParam: isImeLanguage (WPARAM), lParam: lcid (LPARAM)
    ClosePopup,
};

constexpr DWORD VAL(Message msg) {
    return static_cast<DWORD>(msg);
}
}
