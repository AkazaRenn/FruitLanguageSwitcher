import <Windows.h>;
import "EventHookObjectDestroy.cpp";
import "EventHookSystemForegound.cpp";
import "GetMessageThread.cpp";
import "LanguageManager.cpp";
import "Enumerations.cpp";
import "WindowsHookKeyboardLL.cpp";

namespace Core {
using ShowFlyoutFunction = void(__stdcall*)(LCID, LCID);

class Interop {
private:
    GetMessageThread getMessageThread;

public:
    Interop(ShowFlyoutFunction showFlyoutFunction)
        : getMessageThread({
            {Message::ShowFlyout, [showFlyoutFunction](const MSG& msg) {
                    showFlyoutFunction(static_cast<LCID>(msg.wParam), static_cast<LCID>(msg.lParam)); }},
        }) {
        EventHookObjectDestroy::Instance();
        EventHookSystemForegound::Instance();
        LanguageManager::Instance();
        WindowsHookKeyboardLL::Instance();
    }
};
}

extern "C" {
    __declspec(dllexport) Core::Interop* __stdcall Start(Core::ShowFlyoutFunction showFlyoutFunction) {
        return new Core::Interop(showFlyoutFunction);
    }

    __declspec(dllexport) void __stdcall Stop(Core::Interop* instance) {
        delete instance;
    }
}
