#pragma once

#include <functional>
#include <Windows.h>

#include "Common.cpp"
#include "GetMessageThreadManager.cpp"

namespace Core {
class GetMessageThread {
public:
    using ProcessMessageFunction = std::function<bool(const MSG&)>;
    using ProcessMessageFunctionPtr = bool (__stdcall *)(const MSG&);

private:
    static DWORD WINAPI ThreadProc(LPVOID lpParameter) {
        GetMessageThread* pThis = static_cast<GetMessageThread*>(lpParameter);

        pThis->threadId = GetCurrentThreadId();
        for (Message message : pThis->processedMessages) {
            GetMessageThreadManager::Instance().RegisterReceiverThread(message, pThis->threadId);
        }

        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            if (msg.message == WM_QUIT) {
                return 0;
            } else if (!pThis->processMessage(msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        return 0;
    }

    // Return true if the message is processed, false otherwise.
    ProcessMessageFunction processMessage;
    const std::unordered_set<Message> processedMessages;
    DWORD threadId = 0;
    HANDLE hThread;

public:
    GetMessageThread(ProcessMessageFunctionPtr _processMessage, const std::unordered_set<Message>& _processedMessages):
        processMessage(_processMessage),
        processedMessages(_processedMessages),
        hThread(CreateThread(nullptr, 0, ThreadProc, this, 0, &threadId)) {}

    ~GetMessageThread() {
        PostThreadMessage(threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(hThread, 10000);
        CloseHandle(hThread);
    }
};
}
