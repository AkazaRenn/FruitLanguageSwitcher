using Linearstar.Windows.RawInput;
using System;
using System.Runtime.InteropServices;
using Vanara.PInvoke;

namespace App.Flyout;

internal sealed class RawInput: IDisposable {
    const Linearstar.Windows.RawInput.Native.RawMouseButtonFlags ANY_BUTTON_DOWN =
        Linearstar.Windows.RawInput.Native.RawMouseButtonFlags.LeftButtonDown |
        Linearstar.Windows.RawInput.Native.RawMouseButtonFlags.RightButtonDown |
        Linearstar.Windows.RawInput.Native.RawMouseButtonFlags.MiddleButtonDown |
        Linearstar.Windows.RawInput.Native.RawMouseButtonFlags.Button4Down |
        Linearstar.Windows.RawInput.Native.RawMouseButtonFlags.Button5Down |
        Linearstar.Windows.RawInput.Native.RawMouseButtonFlags.MouseWheel |
        Linearstar.Windows.RawInput.Native.RawMouseButtonFlags.MouseHorizontalWheel;

    bool started = false;

    readonly nuint uIdSubclass = (nuint)Random.Shared.Next();
    readonly HWND hwnd;
    readonly ComCtl32.SUBCLASSPROC subclassProc;
    readonly User32.RAWINPUTDEVICE[] rawInputDevicesRegister;
    readonly User32.RAWINPUTDEVICE[] rawInputDevicesUnregister;

    public event Action? UserInputEvent;

    public RawInput(HWND _hwnd) {
        hwnd = _hwnd;
        subclassProc = OnRawInput;
        ComCtl32.SetWindowSubclass(hwnd, subclassProc, uIdSubclass, 0);

        rawInputDevicesRegister = [
            new User32.RAWINPUTDEVICE {
                usUsagePage = (ushort)Hid.USAGE.HID_USAGE_PAGE_GENERIC,
                usUsage = (ushort)Hid.USAGE.HID_USAGE_GENERIC_MOUSE,
                dwFlags = User32.RIDEV.RIDEV_INPUTSINK,
                hwndTarget = hwnd,
            },
            new User32.RAWINPUTDEVICE {
                usUsagePage = (ushort)Hid.USAGE.HID_USAGE_PAGE_GENERIC,
                usUsage = (ushort)Hid.USAGE.HID_USAGE_GENERIC_KEYBOARD,
                dwFlags = User32.RIDEV.RIDEV_INPUTSINK,
                hwndTarget = hwnd,
            },
        ];

        rawInputDevicesUnregister = [
            new User32.RAWINPUTDEVICE {
                usUsagePage = (ushort)Hid.USAGE.HID_USAGE_PAGE_GENERIC,
                usUsage = (ushort)Hid.USAGE.HID_USAGE_GENERIC_MOUSE,
                dwFlags = User32.RIDEV.RIDEV_REMOVE,
                hwndTarget = HWND.NULL,
            },
            new User32.RAWINPUTDEVICE {
                usUsagePage = (ushort)Hid.USAGE.HID_USAGE_PAGE_GENERIC,
                usUsage = (ushort)Hid.USAGE.HID_USAGE_GENERIC_KEYBOARD,
                dwFlags = User32.RIDEV.RIDEV_REMOVE,
                hwndTarget = HWND.NULL,
            },
        ];
    }

    ~RawInput() => Dispose();

    public void Dispose() {
        Stop();
        ComCtl32.RemoveWindowSubclass(hwnd, subclassProc, uIdSubclass);
        GC.SuppressFinalize(this);
    }

    private IntPtr OnRawInput(HWND hwnd, uint uMsg, nint wParam, nint lParam, nuint uldSubclass, nint dwRefData) {
        do {
            if (uMsg == (uint)User32.WindowMessage.WM_INPUT) {
                var data = RawInputData.FromHandle(lParam);
                if (Linearstar.Windows.RawInput.Native.RawInputDeviceHandle.GetRawValue(data.Header.DeviceHandle) == 0)
                    break;

                switch (data) {
                case RawInputMouseData mouse:
                    if ((mouse.Mouse.Buttons & ANY_BUTTON_DOWN) != 0) {
                        UserInputEvent?.Invoke();
                    }
                    break;
                case RawInputKeyboardData keyboard:
                    if ((keyboard.Keyboard.Flags & Linearstar.Windows.RawInput.Native.RawKeyboardFlags.Up) == 0) {
                        UserInputEvent?.Invoke();
                    }
                    break;
                default:
                    break;
                }
            }
        } while (false);

        return ComCtl32.DefSubclassProc(hwnd, uMsg, wParam, lParam);
    }

    public void Start() {
        if (started) {
            return;
        }
        started = true;
        User32.RegisterRawInputDevices(rawInputDevicesRegister, (uint)rawInputDevicesRegister.Length, (uint)Marshal.SizeOf<User32.RAWINPUTDEVICE>());
    }

    public void Stop() {
        if (!started) {
            return;
        }
        started = false;
        User32.RegisterRawInputDevices(rawInputDevicesUnregister, (uint)rawInputDevicesUnregister.Length, (uint)Marshal.SizeOf<User32.RAWINPUTDEVICE>());
    }
}
