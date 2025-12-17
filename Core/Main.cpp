#pragma once

#include "EventHookSystemForegound.cpp"
#include "GetMessageThread.cpp"
#include "WindowsHookKeyboardLL.cpp"

namespace Core {
class Main {
private:
    GetMessageThread thread = GetMessageThread({{Message::ForegroundChanged, a}});

    static void a(const MSG& msg) {
        printf("Main: received message\n");
    }

public:
    Main() {
        Core::EventHookSystemForegound::Instance().Start();
        Core::WindowsHookKeyboardLL::Instance().Start();
    }
};
}
