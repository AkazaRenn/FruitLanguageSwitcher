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
    ShowPopup, // wParam: LanguageState, lParam: LCID
    ClosePopup,
    UserInput,
};

constexpr DWORD VAL(Message msg) {
    return static_cast<DWORD>(msg);
}

enum class LanguageState {
    On,
    Off,
    CapsLockOn,
};
}
