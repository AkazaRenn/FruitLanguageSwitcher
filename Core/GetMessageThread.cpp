#pragma once

#include <functional>
#include <Windows.h>

#include "Common.cpp"
#include "GetMessageThreadManager.cpp"

namespace Core {
class GetMessageThread {
public:
    using ProcessMessageFunction = std::function<void(const MSG&)>;
    using ProcessMessageFunctionPtr = void(__stdcall*)(const MSG&);
    using MessageToProcessFunctionMap = std::unordered_map<Message, ProcessMessageFunction>;

private:
    static DWORD WINAPI ThreadProc(LPVOID lpParameter) {
        GetMessageThread* pThis = static_cast<GetMessageThread*>(lpParameter);

        pThis->threadId = GetCurrentThreadId();
        for (const auto [message, processMessageFunction] : pThis->messageToProcessFunctionMap) {
            if (!processMessageFunction) {
                continue;
            }
            GetMessageThreadManager::Instance().RegisterReceiverThread(message, pThis->threadId);
        }

        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            if (msg.message == WM_QUIT) {
                return 0;
            }

            const auto it = pThis->messageToProcessFunctionMap.find(static_cast<Message>(msg.message));
            if (it != pThis->messageToProcessFunctionMap.end()) {
                const auto& processMessageFunction = it->second;
                if (processMessageFunction) {
                    processMessageFunction(msg);
                    continue;
                }
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return 0;
    }

    const MessageToProcessFunctionMap messageToProcessFunctionMap;
    HANDLE hThread;
    DWORD threadId = 0;

public:
    GetMessageThread(const MessageToProcessFunctionMap _messageToProcessFunctionMap):
        messageToProcessFunctionMap(_messageToProcessFunctionMap),
        hThread(CreateThread(nullptr, 0, ThreadProc, this, 0, &threadId)) {}

    ~GetMessageThread() {
        PostThreadMessage(threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(hThread, 10000);
        CloseHandle(hThread);
    }
};
}
