#pragma once

#include "Interop.hpp"

import "EventHookSystemForegound.cpp";
import "GetMessageThread.cpp";
import "WindowsHookKeyboardLL.cpp";

namespace Core {
static void a(const MSG& msg) {
    printf("Main: received message: ForegroundChanged\n");
}
static void b(const MSG& msg) {
    printf("Main: received message: CapitalkeyDown\n");
}
static void c(const MSG& msg) {
    printf("Main: received message: WinKeyUp\n");
}

Interop::Interop(const MessageToProcessFunctionMap& messageToProcessFunctionMap) {
    EventHookSystemForegound::Instance().Start();
    WindowsHookKeyboardLL::Instance().Start();
    MessageToProcessFunctionMap map = {
       {Message::ForegroundChanged, a},
       {Message::CapitalkeyDown, b},
       {Message::WinKeyUp, c},
    };
    for (const auto& [message, processMessageFunction] : messageToProcessFunctionMap) {
        map[message] = processMessageFunction;
    }
    pThread = static_cast<void*>(new GetMessageThread(map));
}
Interop::~Interop() {
    delete static_cast<GetMessageThread*>(pThread);
}
}
