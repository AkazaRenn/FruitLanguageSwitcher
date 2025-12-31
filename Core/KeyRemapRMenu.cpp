import <Windows.h>;
import "LanguageManager.cpp";
import "MessageDispatcher.cpp";
import "Utilities.cpp";

namespace Core {
class KeyRemapRMenu {
private:
    bool rMenuKeyDown = false;
    bool otherKeyDown = false;

public:
    constexpr void OnRMenuDown() {
        if (rMenuKeyDown) {
            return;
        }

        otherKeyDown = false;
        rMenuKeyDown = true;
        return;
    }

    bool OnRMenuUp() {
        rMenuKeyDown = false;

        if (otherKeyDown) {
            return false;
        }

        MessageDispatcher::Instance().PostMessage(Message::RMenuKeyUp);
        return true;
    }

    constexpr void OnOtherKeyDown() {
        if (otherKeyDown) {
            return;
        }

        otherKeyDown = true;
    }
};
}
