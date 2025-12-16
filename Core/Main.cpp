#pragma once

#include <Windows.h>

#include "Common.cpp"
#include "EventHookSystemForegound.cpp"
#include "GetMessageThread.cpp"
#include "Singleton.cpp"

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
    }
};
}
