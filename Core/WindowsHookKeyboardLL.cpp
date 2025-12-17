#pragma once

#include "GetMessageThreadManager.cpp"
#include "WindowsHook.cpp"

namespace Core {
class WindowsHookKeyboardLL: public WindowsHook<WindowsHookKeyboardLL> {
public:
    const static int IdHook = WH_KEYBOARD_LL;

    bool OnEvent(int nCode, WPARAM wParam, LPARAM lParam) override {
        printf("nCode=%d wParam=%lu lParam=%p\n", nCode, (unsigned long)wParam, (void*)lParam);

        if (nCode == HC_ACTION) {
            KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
            printf("vkCode=%u scanCode=%u flags=%u time=%lu dwExtraInfo=%p\n",
                   kb->vkCode,
                   kb->scanCode,
                   kb->flags,
                   kb->time,
                   (void*)kb->dwExtraInfo);
        }

        return false;
    }
};
}
