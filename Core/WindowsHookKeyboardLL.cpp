import "KeyRemapCapital.cpp";
import "KeyRemapLWin.cpp";
import "KeyRemapRMenu.cpp";
import "MessageDispatcher.cpp";
import "WindowsHook.cpp";

namespace Core {
class WindowsHookKeyboardLL: public WindowsHook<WindowsHookKeyboardLL> {
    friend class WindowsHook<WindowsHookKeyboardLL>;

private:
    const static int IdHook = WH_KEYBOARD_LL;

private:
    bool OnEvent(int nCode, WPARAM wParam, LPARAM lParam) override {
        bool blockEvent = false;

        if (nCode == HC_ACTION) {
            auto* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            if (kb->flags & LLKHF_INJECTED) {
                return false;
            }

            //printf("vkCode: %u\n", kb->vkCode);
            //printf("scanCode: %u\n", kb->scanCode);
            //printf("flags: 0x%X\n", kb->flags);
            //printf("time: %u\n", kb->time);
            //printf("dwExtraInfo: 0x%p\n", kb->dwExtraInfo);

            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                switch (kb->vkCode) {
                case VK_LWIN:
                    KeyRemapLWin::Instance().OnLWinDown();
                    break;
                case VK_RMENU:
                    KeyRemapRMenu::Instance().OnRMenuDown();
                    break;
                case VK_CAPITAL:
                    KeyRemapCapital::Instance().OnCapitalKeyDown();
                    blockEvent = true;
                    break;
                default:
                    KeyRemapRMenu::Instance().OnOtherKeyDown();
                    KeyRemapLWin::Instance().OnOtherKeyDown();
                    break;
                }
            } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                switch (kb->vkCode) {
                case VK_LWIN:
                    blockEvent = KeyRemapLWin::Instance().OnLWinUp();
                    [[fallthrough]];
                case VK_RWIN:
                    MessageDispatcher::Instance().PostMessage(Message::WinKeyUp);
                    break;
                case VK_RMENU:
                    blockEvent = KeyRemapRMenu::Instance().OnRMenuUp();
                    break;
                case VK_CAPITAL:
                    KeyRemapCapital::Instance().OnCapitalKeyUp();
                    blockEvent =  true;
                    break;
                default:
                    break;
                }
            }
        }

        return blockEvent;
    }
};
}
