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
    UserInput,
    ShowFlyout, // wParam: FlyoutState, lParam: LCID
    CloseFlyout,
};

constexpr DWORD VAL(Message msg) {
    return static_cast<DWORD>(msg);
}

enum class FlyoutState {
    Latin,
    IME,
    CapsLock,
};
}
