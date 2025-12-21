import <functional>;
import <thread>;
import "MessageDispatcher.cpp";

namespace Core {
class GetMessageThread {
private:
    void GetMessageProc() {
        threadId = GetCurrentThreadId();
        for (const auto& [message, processMessageFunction] : messageToProcessFunctionMap) {
            if (processMessageFunction) {
                MessageDispatcher::Instance().RegisterThread(message, threadId);
            }
        }

        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            if (msg.message == WM_QUIT) {
                return;
            }

            const auto it = messageToProcessFunctionMap.find(static_cast<Message>(msg.message));
            if (it != messageToProcessFunctionMap.end()) {
                const auto& processMessageFunction = it->second;
                if (processMessageFunction) {
                    processMessageFunction(msg);
                    continue;
                }
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    using ProcessMessageFunction = std::function<void(const MSG&)>;
    using MessageToProcessFunctionMap = std::unordered_map<Message, ProcessMessageFunction>;

    const MessageToProcessFunctionMap messageToProcessFunctionMap;
    std::thread thread;
    DWORD threadId = 0;

public:
    GetMessageThread(const MessageToProcessFunctionMap _messageToProcessFunctionMap)
        : messageToProcessFunctionMap(_messageToProcessFunctionMap),
        thread(&GetMessageThread::GetMessageProc, this) {}

    ~GetMessageThread() {
        PostThreadMessage(threadId, WM_QUIT, 0, 0);
        if (thread.joinable()) {
            thread.join();
        }
    }
};
}
