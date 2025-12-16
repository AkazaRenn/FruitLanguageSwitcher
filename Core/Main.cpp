#pragma once

#include <Windows.h>

#include "Common.cpp"
#include "EventHookSystemForegound.cpp"
#include "GetMessageThread.cpp"
#include "Singleton.cpp"

namespace Core {
class Main {
private:
    GetMessageThread thread = GetMessageThread(a, {Message::ForegroundChanged});

    static bool a(const MSG& msg) {
        switch (msg.message) {
        case VAL(Message::ForegroundChanged):
            printf("Main: received message\n");
            return true;
        default:
            return false;
        }
    }

public:
    Main() {
        Core::EventHookSystemForegound::Instance().Start();
    }
};
}
