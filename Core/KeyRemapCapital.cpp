import "MessageDispatcher.cpp";
import "Timer.cpp";

namespace Core {
class KeyRemapCapital {
private:
    static void SetCapsLockStateAndPostMessage(bool on) {
        if (SetCapsLockState(on)) {
            MessageDispatcher::Instance().PostMessage(on ? Message::CapsLockOn : Message::CapsLockOff);
        }
    }

private:
    bool CapitalKeyDown = false;
    Timer timer = Timer(500, []() {
        SetCapsLockStateAndPostMessage(true);
    });

public:
    ~KeyRemapCapital() {
        timer.Cancel();
    }

    void OnCapitalKeyDown() {
        if (CapitalKeyDown) {
            return;
        } else {
            CapitalKeyDown = true;
        }

        if (GetCapsLockState()) {
            SetCapsLockStateAndPostMessage(false);
        } else {
            timer.Reset();
        }
    }

    void OnCapitalKeyUp() {
        if (timer.Cancel()) {
            MessageDispatcher::Instance().PostMessage(Message::SwapCategoryTriggered);
        }
        CapitalKeyDown = false;
    }
};
}
