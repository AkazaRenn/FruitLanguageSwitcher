#pragma once

import "GetMessageThreadManager.cpp";
import "WindowsHook.cpp";
import "KeyRemapCapital.cpp";
import "KeyRemapLWin.cpp";

namespace Core {
class WindowsHookKeyboardLL: public WindowsHook<WindowsHookKeyboardLL> {
public:
    const static int IdHook = WH_KEYBOARD_LL;

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
                    return false;
                case VK_CAPITAL:
                    KeyRemapCapital::Instance().OnCapitalKeyDown();
                    return true;
                default:
                    KeyRemapLWin::Instance().OnOtherKeyDown();
                    break;
                }
            } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                switch (kb->vkCode) {
                case VK_LWIN:
                    return KeyRemapLWin::Instance().OnLWinUp();
                case VK_CAPITAL:
                    KeyRemapCapital::Instance().OnCapitalKeyUp();
                    return true;
                default:
                    break;
                }
            }
        }

        return blockEvent;
    }
};
}
