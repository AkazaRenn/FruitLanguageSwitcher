import <atomic>;
import <functional>;
import <stdexcept>;
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
        SetThreadpoolTimer(pThis->threadpoolTimer, NULL, 0, 0);
    }

private:
    PTP_TIMER threadpoolTimer = nullptr;
    std::function<void()> onTimerExpiry;
    std::atomic<bool> handled = false;

public:
    Timer(std::function<void()> _onTimerExpiry)
        : onTimerExpiry(_onTimerExpiry) {
        threadpoolTimer = CreateThreadpoolTimer(StaticOnTimerExpiry, this, nullptr);
        if (!threadpoolTimer) {
            throw std::runtime_error("CreateThreadpoolTimer failed");
        }
    }

    ~Timer() {
        Cancel();
        WaitForThreadpoolTimerCallbacks(threadpoolTimer, true);
        CloseThreadpoolTimer(threadpoolTimer);
    }

    void Reset(LONGLONG timerExpiryMs) {
        handled = false;

        // Negative value = relative time, in 100ns units
        LONGLONG dueTime = -timerExpiryMs * 10000;
        FILETIME ft = {
            .dwLowDateTime = static_cast<DWORD>(dueTime & 0xFFFFFFFF),
            .dwHighDateTime = static_cast<DWORD>(dueTime >> 32),
        };

        // Arm the timer for one-shot (period = 0)
        SetThreadpoolTimer(threadpoolTimer, &ft, 0, 0);
    }

    // Return false if the timer has already been handled (cancellation failed), true otherwise
    bool Cancel() {
        if (handled.exchange(true)) {
            return false;
        }
        SetThreadpoolTimer(threadpoolTimer, NULL, 0, 0);
        return true;
    }
};
}
