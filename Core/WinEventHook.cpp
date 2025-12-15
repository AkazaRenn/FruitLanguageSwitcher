#pragma once

#include <Windows.h>
#include <unordered_set>
#include <mutex>

#include "Singleton.cpp"

template<typename T>
class WinEventHook: public Singleton<T> {
private:
    HWINEVENTHOOK hook = nullptr;

protected:
    inline const static HMODULE hModWinEventProc = NULL;
    const static DWORD idProcess = 0;
    const static DWORD idThread = 0;
    const static DWORD flags = WINEVENT_OUTOFCONTEXT;

    ~WinEventHook() {
        UnhookWinEvent(hook);
    }

public:
    void Start() {
        if (hook) {
            return;
        }
        hook = SetWinEventHook(T::Event, T::Event, T::hModWinEventProc, T::OnWinEvent, T::idProcess, T::idThread, T::flags);
    }
};
