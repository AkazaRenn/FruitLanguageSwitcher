#pragma once

import <Windows.h>;
import "Singleton.cpp";

template<typename T>
class WindowsHook: public Singleton<T> {
private:
    static LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (T::Instance().OnEvent(nCode, wParam, lParam)) {
            return true;
        } else {
            return CallNextHookEx(T::Instance().hook, nCode, wParam, lParam);
        }
    }

    HHOOK hook = SetWindowsHookEx(T::IdHook, T::HookProc, T::hmod, T::dwThreadId);

protected:
    inline const static HINSTANCE hmod = GetModuleHandle(nullptr);
    const static DWORD dwThreadId = 0;

protected:
    // Return true to block the event, false to pass it to next hook
    virtual bool OnEvent(int nCode, WPARAM wParam, LPARAM lParam) = 0;

    ~WindowsHook() {
        if (!hook) {
            return;
        }
        UnhookWindowsHookEx(hook);
        hook = nullptr;
    }
};
