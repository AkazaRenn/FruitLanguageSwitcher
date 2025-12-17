#pragma once

#include <Windows.h>

#include "Singleton.cpp"

template<typename T>
class WinEventHook: public Singleton<T> {
private:
    HWINEVENTHOOK hook = nullptr;

    static void CALLBACK HookProc(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
        T::Instance().OnEvent(hWinEventHook, dwEvent, hwnd, idObject, idChild, dwEventThread, dwmsEventTime);
    }

protected:
    inline const static HMODULE hModWinEventProc = nullptr;
    const static DWORD idProcess = 0;
    const static DWORD idThread = 0;
    const static DWORD flags = WINEVENT_OUTOFCONTEXT;

    virtual void OnEvent(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) = 0;

    ~WinEventHook() {
        Stop();
    }

public:
    void Start() {
        if (hook) {
            return;
        }
        hook = SetWinEventHook(T::Event, T::Event, T::hModWinEventProc, T::HookProc, T::idProcess, T::idThread, T::flags);
    }

    void Stop() {
        if (!hook) {
            return;
        }
        UnhookWinEvent(hook);
        hook = nullptr;
    }
};
