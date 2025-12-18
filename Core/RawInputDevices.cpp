#pragma once

import <memory>;
import <Windows.h>;
import "GetMessageThreadManager.cpp";
import "KeyRemapLWin.cpp";

namespace Core {
class RawInputDevices: public Singleton<RawInputDevices> {
private:
    static constexpr USHORT RAWMOUSE_FLAG_BUTTON_DOWN = 0x1;

    static LRESULT CALLBACK OnInput(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_INPUT:
        {
            UINT dwSize = 0;
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));

            auto lpb = std::make_unique<BYTE[]>(dwSize);
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb.get(), &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {

                RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb.get());
                if (raw->header.hDevice == nullptr) {
                    // ignore synthetic input
                    break;
                }

                if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                    const RAWKEYBOARD& kb = raw->data.keyboard;

                    //printf("Keyboard: MakeCode=%04x, Flags=%04x, VKey=%04x, Msg=%04x, Extra=%08x, hDevice=%p\n",
                    //       kb.MakeCode,
                    //       kb.Flags,
                    //       kb.VKey,
                    //       kb.Message,
                    //       kb.ExtraInformation,
                    //       raw->header.hDevice);

                    if (kb.Flags & RI_KEY_BREAK) {
                        switch (kb.VKey) {
                        case VK_LWIN:
                        case VK_RWIN:
                            GetMessageThreadManager::Instance().PostMessage(Message::WinKeyUp);
                            break;
                        default:
                            break;
                        }
                    } else {
                        switch (kb.VKey) {
                        case VK_LWIN:
                            KeyRemapLWin::Instance().OnLWinDown();
                            break;
                        default:
                            break;
                        }
                    }
                } else if (raw->header.dwType == RIM_TYPEMOUSE) {
                    const RAWMOUSE& ms = raw->data.mouse;

                    //printf("Mouse: Flags=%04x, Buttons=%04x, ButtonFlags=%04x, ButtonData=%04x, RawX=%ld, RawY=%ld, hDevice=%p\n",
                    //       ms.usFlags,
                    //       ms.ulButtons,
                    //       ms.usButtonFlags,
                    //       ms.usButtonData,
                    //       ms.lLastX,
                    //       ms.lLastY,
                    //       raw->header.hDevice);

                    if (ms.usButtonFlags & RAWMOUSE_FLAG_BUTTON_DOWN) {
                    }
                }
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    HINSTANCE hInstance = GetModuleHandle(nullptr);
    const LPCTSTR className = L"RawInputWindow";
    const WNDCLASS windowClass = {
        .lpfnWndProc = OnInput,
        .hInstance = hInstance,
        .lpszClassName = className,
    };
    HWND hwnd = nullptr;

public:
    RawInputDevices() {
        RegisterClass(&windowClass);
        hwnd = CreateWindowEx(0, className, className, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, hInstance, nullptr);

        RAWINPUTDEVICE rid[2] = {{
            .usUsagePage = 0x01, // Generic desktop controls
            .usUsage = 0x02, // Mouse
            .dwFlags = RIDEV_INPUTSINK,
            .hwndTarget = hwnd,
        }, {
            .usUsagePage = 0x01, // Generic desktop controls
            .usUsage = 0x06, // Keyboard
            .dwFlags = RIDEV_INPUTSINK,
            .hwndTarget = hwnd,
        }, };

        RegisterRawInputDevices(rid, 2, sizeof(rid[0]));
    }

    ~RawInputDevices() {
        RAWINPUTDEVICE rid[2] = {{
            .usUsagePage = 0x01, // Generic desktop controls
            .usUsage = 0x02, // Mouse
            .dwFlags = RIDEV_REMOVE,
            .hwndTarget = nullptr,
        }, {
            .usUsagePage = 0x01, // Generic desktop controls
            .usUsage = 0x06, // Keyboard
            .dwFlags = RIDEV_REMOVE,
            .hwndTarget = nullptr,
        }, };

        RegisterRawInputDevices(rid, 2, sizeof(rid[0]));
        DestroyWindow(hwnd);
        UnregisterClass(className, hInstance);
    }
};
}
