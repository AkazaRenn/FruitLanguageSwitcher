#pragma once

#include "GetMessageThreadManager.cpp"
#include "WindowsHook.cpp"
#include "KeyRemapCapital.cpp"
#include "KeyRemapLWin.cpp"

namespace Core {
class WindowsHookKeyboardLL: public WindowsHook<WindowsHookKeyboardLL> {
private:
    KeyRemapLWin keyRemapLWin;
    KeyRemapCapital& keyRemapCapital = KeyRemapCapital::Instance();

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
                    keyRemapLWin.OnLWinDown();
                    break;
                case VK_CAPITAL:
                    blockEvent = true;
                    keyRemapCapital.OnCapitalKeyDown();
                    [[fallthrough]];
                default:
                    keyRemapLWin.OnOtherKeyDown();
                    break;
                }
            } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                switch (kb->vkCode) {
                case VK_LWIN:
                    blockEvent = keyRemapLWin.OnLWinUp();
                    [[fallthrough]];
                case VK_RWIN:
                    GetMessageThreadManager::Instance().PostMessage(Message::WinKeyUp);
                    break;
                case VK_CAPITAL:
                    blockEvent = true;
                    keyRemapCapital.OnCapitalKeyUp();
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
