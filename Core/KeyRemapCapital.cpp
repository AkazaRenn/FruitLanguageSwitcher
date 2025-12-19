#pragma once

import <atomic>;
import "GetMessageThreadManager.cpp";

namespace Core {
class KeyRemapCapital: public Singleton<KeyRemapCapital> {
private:
    static bool GetCapslockState() {
        return (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
    }

    static void SetCapslockState(bool on) {
        if (GetCapslockState() != on) {
            SendKeySequence({VK_CAPITAL});
            GetMessageThreadManager::Instance().PostMessage(on ? Message::CapsLockOn : Message::CapsLockOff);
        }
    }

    static void CALLBACK OnTimerExpiry(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_TIMER timer) {
        auto* pThis = static_cast<KeyRemapCapital*>(context);
        if ((!pThis) || (pThis->handled.exchange(true))) {
            return;
        }
        SetCapslockState(true);
    }

    static constexpr DWORD timerExpiryMs = 500;

private:
    bool CapitalKeyDown = false;
    std::atomic<bool> handled = false;
    PTP_TIMER timer = CreateThreadpoolTimer(OnTimerExpiry, this, nullptr);

    void ResetTimer() {
        handled = false;

        // Negative value = relative time, in 100ns units
        LONGLONG dueTime = -static_cast<LONGLONG>(timerExpiryMs) * 10000;
        FILETIME ft = {
            .dwLowDateTime = static_cast<DWORD>(dueTime & 0xFFFFFFFF),
            .dwHighDateTime = static_cast<DWORD>(dueTime >> 32),
        };

        // Arm the timer for one-shot (period = 0)
        SetThreadpoolTimer(timer, &ft, 0, 0);
    }

    void CancelTimer() {
        SetThreadpoolTimer(timer, NULL, 0, 0);
    }

public:
    ~KeyRemapCapital() {
        CancelTimer();
        CloseThreadpoolTimer(timer);
    }

    void OnCapitalKeyDown() {
        if (CapitalKeyDown) {
            return;
        } else {
            CapitalKeyDown = true;
        }

        if (GetCapslockState()) {
            SetCapslockState(false);
        } else {
            ResetTimer();
        }
    }

    void OnCapitalKeyUp() {
        if (!handled.exchange(true)) {
            CancelTimer();
            GetMessageThreadManager::Instance().PostMessage(Message::SwapCategoryTriggered);
        }
        CapitalKeyDown = false;
    }
};
}
