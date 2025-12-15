#pragma once

#include <Windows.h>

#include "EventHookSystemForegound.cpp"
#include "GetMessageThread.cpp"
#include "Singleton.cpp"

namespace Core {
class Main: public Singleton<Main> {
private:
    GetMessageThread thread;

public:
    static void Start() {
        Instance();
        Core::EventHookSystemForegound::Instance().Start();
    }

    static void RegisterReceiverThread(DWORD message, DWORD threadId) {
        Core::GetMessageThreadManager::Instance().RegisterReceiverThread(message, threadId);
    }

    static void UnregisterReceiverThread(DWORD threadId) {
        Core::GetMessageThreadManager::Instance().UnregisterReceiverThread(threadId);
    }
};
}
