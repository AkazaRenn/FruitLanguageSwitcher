#pragma once

#include <Windows.h>

#include "GetMessageThreadManager.cpp"

namespace Core{
class GetMessageThread {
private:
    static DWORD WINAPI ThreadProc(LPVOID lpParameter) {
        GetMessageThread* pThis = static_cast<GetMessageThread*>(lpParameter);

        pThis->threadId = GetCurrentThreadId();
        GetMessageThreadManager::Instance().RegisterReceiverThread(WM_USER + 1, pThis->threadId);

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

    DWORD threadId = 0;
    HANDLE hThread = CreateThread(nullptr, 0, ThreadProc, this, 0, &threadId);

    // Return true if the message is processed, false otherwise.
    virtual bool processMessage(const MSG& msg);

public:
    ~GetMessageThread() {
        PostThreadMessage(threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(hThread, 10000);
        CloseHandle(hThread);
    }
};
}
