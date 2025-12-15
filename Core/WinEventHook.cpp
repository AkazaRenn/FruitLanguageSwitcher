#pragma once

#include <Windows.h>
#include <unordered_set>
#include <mutex>

#include "Singleton.cpp"

template<typename T>
class WinEventHook: protected Singleton<T> {
public:
    static void Start() {
        if (T::Instance().started) {
            return;
        }
        T::Instance().started = true;
        T::Instance().hook = SetWinEventHook(T::Event, T::Event, T::hModWinEventProc, T::OnWinEvent, T::idProcess, T::idThread, T::flags);
    }

    static void RegisterReceiverThread(DWORD threadId) {
        std::lock_guard<std::mutex> lock(T::Instance().receiverThreadIdsMutex);
        T::Instance().receiverThreadIds.insert(threadId);
    }

    static bool UnregisterReceiverThread(DWORD threadId) {
        std::lock_guard<std::mutex> lock(T::Instance().receiverThreadIdsMutex);
        return T::Instance().receiverThreadIds.erase(threadId) > 0;
    }

private:
    bool started = false;
    HWINEVENTHOOK hook = nullptr;

protected:
    inline const static HMODULE hModWinEventProc = NULL;
    const static DWORD idProcess = 0;
    const static DWORD idThread = 0;
    const static DWORD flags = WINEVENT_OUTOFCONTEXT;

    std::unordered_set<DWORD> receiverThreadIds;
    std::mutex receiverThreadIdsMutex;
};
