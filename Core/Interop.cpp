import "Interop.hpp";

import "EventHookSystemForegound.cpp";
import "GetMessageThread.cpp";
import "LanguageManager.cpp";
import "WindowsHookKeyboardLL.cpp";

namespace Core {
Interop::Interop(ShowFlyoutFunction showFlyoutFunction) {
    EventHookSystemForegound::Instance();
    LanguageManager::Instance();
    WindowsHookKeyboardLL::Instance();

    pThread = static_cast<void*>(new GetMessageThread({
        {Message::ShowFlyout, [showFlyoutFunction](const MSG& msg) {
            showFlyoutFunction(static_cast<LCID>(msg.wParam), static_cast<LCID>(msg.lParam));
        }},
    }));
}

Interop::~Interop() {
    if (pThread) {
        delete static_cast<GetMessageThread*>(pThread);
    }
}
}
