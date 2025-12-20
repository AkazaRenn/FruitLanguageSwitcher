#pragma once

import <atomic>;
import <functional>;
import <Windows.h>;

namespace Core {
class Timer {
private:
    static void CALLBACK StaticOnTimerExpiry(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_TIMER timer) {
        auto* pThis = static_cast<Timer*>(context);
        if ((!pThis) || (pThis->handled.exchange(true))) {
            return;
        }
        pThis->onTimerExpiry();
    }

    DWORD timerExpiryMs = 500;

private:
    std::atomic<bool> handled = false;
    PTP_TIMER timer = CreateThreadpoolTimer(StaticOnTimerExpiry, this, nullptr);
    std::function<void()> onTimerExpiry;

public:
    Timer(DWORD _timerExpiryMs, std::function<void()> _onTimerExpiry)
        : timerExpiryMs(_timerExpiryMs)
        , onTimerExpiry(_onTimerExpiry) {}

    ~Timer() {
        Cancel();
        WaitForThreadpoolTimerCallbacks(timer, true);
        CloseThreadpoolTimer(timer);
    }

    void Reset() {
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

    // Return false if the timer has already been handled, true otherwise
    bool Cancel() {
        bool timerHandled = handled.exchange(true);
        SetThreadpoolTimer(timer, NULL, 0, 0);
        return !timerHandled;
    }
};
}
