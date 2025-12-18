#pragma once

#include "Interop.hpp"

import "EventHookSystemForegound.cpp";
import "GetMessageThread.cpp";
import "WindowsHookKeyboardLL.cpp";

namespace Core {
Interop::Interop(const MessageToProcessFunctionMap& messageToProcessFunctionMap) {
    EventHookSystemForegound::Instance().Start();
    WindowsHookKeyboardLL::Instance().Start();

    pThread = static_cast<void*>(new GetMessageThread(messageToProcessFunctionMap));
}

Interop::~Interop() {
    if (pThread) {
        delete static_cast<GetMessageThread*>(pThread);
    }
}
}
