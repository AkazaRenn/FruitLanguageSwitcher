using Linearstar.Windows.RawInput;
using System;
using System.Linq;
using System.Runtime.InteropServices;
using Vanara.PInvoke;

namespace App.Flyout;

internal sealed class InputDetector: IDisposable {
    bool started = false;

    readonly nuint uIdSubclass = (nuint)Random.Shared.Next();
    readonly HWND hwnd;
    readonly ComCtl32.SUBCLASSPROC subclassProc;
    readonly Hid.USAGE[] hidUsages = [Hid.USAGE.HID_USAGE_GENERIC_MOUSE, Hid.USAGE.HID_USAGE_GENERIC_KEYBOARD];
    readonly User32.RAWINPUTDEVICE[] rawInputDevicesRegister;
    readonly User32.RAWINPUTDEVICE[] rawInputDevicesUnregister;

    public event Action? UserInputEvent;

    public InputDetector(HWND _hwnd) {
        hwnd = _hwnd;
        subclassProc = OnRawInput;
        ComCtl32.SetWindowSubclass(hwnd, subclassProc, uIdSubclass, 0);

        rawInputDevicesRegister = [.. hidUsages
            .Select(usage => new User32.RAWINPUTDEVICE {
                usUsagePage = (ushort)Hid.USAGE.HID_USAGE_PAGE_GENERIC,
                usUsage = (ushort)usage,
                dwFlags = User32.RIDEV.RIDEV_INPUTSINK,
                hwndTarget = hwnd
            })];
        rawInputDevicesUnregister = [.. hidUsages
            .Select(usage => new User32.RAWINPUTDEVICE {
                usUsagePage = (ushort)Hid.USAGE.HID_USAGE_PAGE_GENERIC,
                usUsage = (ushort)usage,
                dwFlags = User32.RIDEV.RIDEV_REMOVE,
                hwndTarget = HWND.NULL
            })];
    }

    ~InputDetector() => Dispose();

    public void Dispose() {
        Stop();
        ComCtl32.RemoveWindowSubclass(hwnd, subclassProc, uIdSubclass);
        GC.SuppressFinalize(this);
    }

    private IntPtr OnRawInput(HWND hwnd, uint uMsg, nint wParam, nint lParam, nuint uldSubclass, nint dwRefData) {
        do {
            if (!started) {
                break;
            }

            if (uMsg != (uint)User32.WindowMessage.WM_INPUT) {
                break;
            }

            switch (RawInputData.FromHandle(lParam)) {
            case RawInputMouseData mouse:
                if (mouse.Mouse.Buttons != 0) {
                    UserInputEvent?.Invoke();
                }
                break;
            case RawInputKeyboardData keyboard:
                if (Linearstar.Windows.RawInput.Native.RawInputDeviceHandle.GetRawValue(keyboard.Header.DeviceHandle) != 0) {
                    UserInputEvent?.Invoke();
                }
                break;
            default:
                break;
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
