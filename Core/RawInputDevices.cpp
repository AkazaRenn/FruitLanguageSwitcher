import <memory>;
import <Windows.h>;
import "KeyRemapLWin.cpp";
import "KeyRemapRMenu.cpp";
import "LanguageManager.cpp";
import "MessageDispatcher.cpp";

namespace Core {
class RawInputDevices: public Singleton<RawInputDevices> {
    friend class Singleton<RawInputDevices>;

private:
    static constexpr USHORT RI_MOUSE_ACTION =
        RI_MOUSE_BUTTON_1_DOWN |
        RI_MOUSE_BUTTON_1_UP |
        RI_MOUSE_BUTTON_2_DOWN |
        RI_MOUSE_BUTTON_2_UP |
        RI_MOUSE_BUTTON_3_DOWN |
        RI_MOUSE_BUTTON_3_UP |
        RI_MOUSE_BUTTON_4_DOWN |
        RI_MOUSE_BUTTON_4_UP |
        RI_MOUSE_BUTTON_5_DOWN |
        RI_MOUSE_BUTTON_5_UP |
        RI_MOUSE_WHEEL |
        RI_MOUSE_HWHEEL;

    static LRESULT CALLBACK OnInput(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        static std::vector<BYTE> lpb;

        switch (msg) {
        case WM_INPUT:
        {
            UINT dwSize = 0;
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
            if (lpb.size() < dwSize)
                lpb.resize(dwSize);
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb.data(), &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {
                RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb.data());
                if (raw->header.hDevice == nullptr) {
                    // ignore synthetic input
                    break;
                }

                if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                    const RAWKEYBOARD& kb = raw->data.keyboard;

                    // printf("Keyboard: MakeCode=%04x, Flags=%04x, VKey=%04x, Msg=%04x, Extra=%08x, hDevice=%p\n",
                    //       kb.MakeCode,
                    //       kb.Flags,
                    //       kb.VKey,
                    //       kb.Message,
                    //       kb.ExtraInformation,
                    //       raw->header.hDevice);

                    if (!(kb.Flags & RI_KEY_BREAK)) {
                        switch (kb.VKey) {
                        case VK_LWIN:
                            KeyRemapLWin::Instance().OnLWinDown();
                            break;
                        case VK_RMENU:
                            [[fallthrough]];
                        case VK_MENU:
                            break;
                        default:
                            KeyRemapRMenu::Instance().OnOtherKeyDown();
                            if (LanguageManager::Instance().CheckUserInput()) {
                                MessageDispatcher::Instance().PostMessage(Message::UserInput);
                            }
                            break;
                        }
                    } else {
                        switch (kb.VKey) {
                        case VK_LWIN:
                        case VK_RWIN:
                            MessageDispatcher::Instance().PostMessage(Message::WinKeyUp);
                            break;
                        default:
                            break;
                        }
                    }
                } else if (raw->header.dwType == RIM_TYPEMOUSE) {
                    const RAWMOUSE& ms = raw->data.mouse;
                    if (ms.usButtonFlags & RI_MOUSE_ACTION) {

                        // printf("Mouse: Flags=%04x, Buttons=%04x, ButtonFlags=%04x, ButtonData=%04x, RawX=%ld, RawY=%ld, hDevice=%p\n",
                        //        ms.usFlags,
                        //        ms.ulButtons,
                        //        ms.usButtonFlags,
                        //        ms.usButtonData,
                        //        ms.lLastX,
                        //        ms.lLastY,
                        //        raw->header.hDevice);

                        if (LanguageManager::Instance().CheckUserInput()) {
                            MessageDispatcher::Instance().PostMessage(Message::UserInput);
                        }
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

private:
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    const LPCTSTR className = L"RawInputWindow";
    const WNDCLASS windowClass = {
        .lpfnWndProc = OnInput,
        .hInstance = hInstance,
        .lpszClassName = className,
    };
    HWND hwnd = nullptr;

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
        }};

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
        }};

        RegisterRawInputDevices(rid, 2, sizeof(rid[0]));
        DestroyWindow(hwnd);
        UnregisterClass(className, hInstance);
    }
};
}
