#pragma once

#include "Interop.hpp"

import "EventHookSystemForegound.cpp";
import "GetMessageThread.cpp";
import "RawInputDevices.cpp";
import "WindowsHookKeyboardLL.cpp";

namespace Core {
Interop::Interop(const MessageToProcessFunctionMap& messageToProcessFunctionMap) {
    EventHookSystemForegound::Instance();
    WindowsHookKeyboardLL::Instance();
    RawInputDevices::Instance();

    pThread = static_cast<void*>(new GetMessageThread(messageToProcessFunctionMap));
}

Interop::~Interop() {
    if (pThread) {
        delete static_cast<GetMessageThread*>(pThread);
    }
}
}
