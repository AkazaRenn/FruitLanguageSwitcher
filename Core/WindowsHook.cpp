#pragma once

#include <Windows.h>

#include "Singleton.cpp"

template<typename T>
class WindowsHook: public Singleton<T> {
private:
    HHOOK hook = nullptr;

    static LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (T::Instance().OnEvent(nCode, wParam, lParam)) {
            return true;
        } else {
            return CallNextHookEx(T::Instance().hook, nCode, wParam, lParam);
        }
    }

protected:
    inline const static HINSTANCE hmod = GetModuleHandle(nullptr);
    const static DWORD dwThreadId = 0;
    
    virtual bool OnEvent(int nCode, WPARAM wParam, LPARAM lParam) = 0;

    ~WindowsHook() {
        Stop();
    }

public:
    void Start() {
        if (hook) {
            return;
        }
        hook = SetWindowsHookEx(T::IdHook, T::HookProc, T::hmod, T::dwThreadId);
    }

    void Stop() {
        if (!hook) {
            return;
        }
        UnhookWindowsHookEx(hook);
        hook = nullptr;
    }
};
