#pragma once

#include "EventHookSystemForegound.cpp"
#include "GetMessageThread.cpp"
#include "WindowsHookKeyboardLL.cpp"

namespace Core {
class Main {
private:
    GetMessageThread thread = GetMessageThread({
        {Message::ForegroundChanged, a},
        {Message::CapitalkeyDown, b},
        {Message::WinKeyUp, c},
    });

    static void a(const MSG& msg) {
        printf("Main: received message: ForegroundChanged\n");
    }
    static void b(const MSG& msg) {
        printf("Main: received message: CapitalkeyDown\n");
    }
    static void c(const MSG& msg) {
        printf("Main: received message: WinKeyUp\n");
    }

public:
    Main() {
        Core::EventHookSystemForegound::Instance().Start();
        Core::WindowsHookKeyboardLL::Instance().Start();
    }
};
}
