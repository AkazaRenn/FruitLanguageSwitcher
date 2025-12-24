#include <Windows.h>

namespace Core {
enum class Message: DWORD {
    Unused = WM_APP,
    ForegroundChanged, // lParam: HWND hwnd
    WindowDestroyed,
    WinKeyUp,
    SwapCategoryTriggered,
    CapsLockOn,
    CapsLockOff,
    ShowFlyout, // wParam: LCID activeLanguage, lParam: LCID activeImeLanguage
};

constexpr DWORD VAL(Message msg) {
    return static_cast<DWORD>(msg);
}
}
