#pragma once

#include <atomic>

#include <Windows.h>

#include "GetMessageThreadManager.cpp"

namespace Core {
class KeyRemapCapital: public Singleton<KeyRemapCapital> {
private:
    static bool GetCapslockState() {
        return (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
    }

    static void SetCapslockState(bool on) {
        if (GetCapslockState() != on) {
            SendKeySequence({VK_CAPITAL});
        }
    }

    static void CALLBACK OnTimerExpiry(PTP_CALLBACK_INSTANCE, PVOID, PTP_TIMER) {
        if (Instance().handled.exchange(true)) {
            return;
        }
        SetCapslockState(true);
    }

    static constexpr DWORD timerExpiryMs = 500;

    bool capitalKeyDown = false;
    std::atomic<bool> handled = false;
    PTP_TIMER timer = CreateThreadpoolTimer(OnTimerExpiry, nullptr, nullptr);

    void ResetTimer() {
        handled = false;

        FILETIME ft;
        // Negative value = relative time, in 100ns units
        LONGLONG dueTime = -static_cast<LONGLONG>(timerExpiryMs) * 10000;
        ft.dwLowDateTime = static_cast<DWORD>(dueTime & 0xFFFFFFFF);
        ft.dwHighDateTime = static_cast<DWORD>(dueTime >> 32);

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
        if (capitalKeyDown) {
            return;
        } else {
            capitalKeyDown = true;
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
            GetMessageThreadManager::Instance().PostMessage(Message::CapitalkeyDown);
        }
        capitalKeyDown = false;
    }
};
}
