import <functional>;
import <stdexcept>;
import <Windows.h>;

namespace Core {
class Task {
private:
    static void CALLBACK StaticTask(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WORK work) {
        auto* pThis = static_cast<Task*>(context);
        if (!pThis) {
            return;
        }
        pThis->task();
    }

private:
    PTP_WORK threadpoolWork = nullptr;
    std::function<void()> task;

public:
    Task(std::function<void()> _task)
        : task(_task) {
        threadpoolWork = CreateThreadpoolWork(StaticTask, this, nullptr);
        if (!threadpoolWork) {
            throw std::runtime_error("CreateThreadpoolTask failed");
        }
    }

    ~Task() {
        WaitForThreadpoolWorkCallbacks(threadpoolWork, true);
        CloseThreadpoolWork(threadpoolWork);
    }

    void Start() {
        SubmitThreadpoolWork(threadpoolWork);
    }
};
}
