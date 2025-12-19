#pragma once

import <Windows.h>;
import "Singleton.cpp";

template<typename T>
class WinEventHook: public Singleton<T> {
private:
    static void CALLBACK HookProc(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
        T::Instance().OnEvent(hWinEventHook, dwEvent, hwnd, idObject, idChild, dwEventThread, dwmsEventTime);
    }

    HWINEVENTHOOK hook = SetWinEventHook(T::Event, T::Event, T::hModWinEventProc, T::HookProc, T::idProcess, T::idThread, T::flags);

protected:
    inline const static HMODULE hModWinEventProc = nullptr;
    const static DWORD idProcess = 0;
    const static DWORD idThread = 0;
    const static DWORD flags = WINEVENT_OUTOFCONTEXT;

protected:
    virtual void OnEvent(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) = 0;

    ~WinEventHook() {
        if (!hook) {
            return;
        }
        UnhookWinEvent(hook);
        hook = nullptr;
    }
};
