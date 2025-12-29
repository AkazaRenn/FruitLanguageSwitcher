using FlaUI.Core.Definitions;
using FlaUI.UIA3;
using Microsoft.UI;
using Microsoft.UI.Windowing;
using Microsoft.Windows.AppLifecycle;
using System;
using System.Drawing;
using System.Threading.Tasks;
using Vanara.PInvoke;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;

namespace App;

internal static class Utilities {
    public struct CaretInfo {
        // X and Y for the top-center of the caret
        public double X { get; set; }
        public double Y { get; set; }
        public double Height { get; set; }

        public CaretInfo(double x, double y, double height) {
            X = x;
            Y = y;
            Height = height;
        }

        public CaretInfo(Rectangle rect) {
            X = (rect.Left + rect.Right) / 2.0;
            Y = rect.Top;
            Height = rect.Height;
        }
    }

    public static bool GetCaretPosition(out CaretInfo? caretInfo) {
        caretInfo = null;
        using UIA3Automation automation = new();
        var focused = automation.FocusedElement();
        if (focused == null) {
            return false;
        }

        do {
            var textPattern = focused.Patterns.Text2.PatternOrDefault;
            if (textPattern == null) {
                break;
            }
            var selection = textPattern.GetSelection();
            if (selection == null || selection.Length == 0) {
                break;
            }
            var textRange = selection[0];
            if (textRange.CompareEndpoints(TextPatternRangeEndpoint.Start, textRange, TextPatternRangeEndpoint.End) == 0) {
                var caretRectangles = textRange.GetBoundingRectangles();
                if (caretRectangles != null && caretRectangles.Length > 0) {
                    caretInfo = new CaretInfo(caretRectangles[0]);
                    break;
                }
            }
            var caretRange = textPattern.GetCaretRange(out bool _);
            if (caretRange == null) {
                break;
            }
            if (caretRange.CompareEndpoints(TextPatternRangeEndpoint.Start, textRange, TextPatternRangeEndpoint.Start) == 0) {
                textRange.MoveEndpointByRange(TextPatternRangeEndpoint.Start, textRange, TextPatternRangeEndpoint.End);
            } else {
                textRange.MoveEndpointByRange(TextPatternRangeEndpoint.End, textRange, TextPatternRangeEndpoint.Start);
            }
            var anchorRectangles = textRange.GetBoundingRectangles();
            if (anchorRectangles == null || anchorRectangles.Length == 0) {
                break;
            }
            caretInfo = new CaretInfo(anchorRectangles[0]);
        } while (false);

        if (caretInfo == null)
            do {
                var textPattern = focused.Patterns.Text.PatternOrDefault;
                if (textPattern == null) {
                    break;
                }
                var selection = textPattern.GetSelection();
                if (selection == null || selection.Length == 0) {
                    break;
                }
                var textRangeStart = selection[0];
                var textRangeEnd = textRangeStart.Clone();
                textRangeStart.MoveEndpointByRange(TextPatternRangeEndpoint.End, textRangeStart, TextPatternRangeEndpoint.Start);
                var rectangles = textRangeStart.GetBoundingRectangles();
                if (rectangles != null && rectangles.Length > 0) {
                    caretInfo = new CaretInfo(rectangles[0]);
                    break;
                }
                textRangeEnd.MoveEndpointByRange(TextPatternRangeEndpoint.Start, textRangeStart, TextPatternRangeEndpoint.End);
                rectangles = textRangeEnd.GetBoundingRectangles();
                if (rectangles != null && rectangles.Length > 0) {
                    caretInfo = new CaretInfo(rectangles[0]);
                    break;
                }
            } while (false);

        return true;
    }

    public static CaretInfo FallbackCaretPosition {
        get {
            var hwnd = User32.GetForegroundWindow();
            var windowId = Win32Interop.GetWindowIdFromWindow(hwnd.DangerousGetHandle());
            var display = DisplayArea.GetFromWindowId(windowId, DisplayAreaFallback.Primary);
            var dpi = User32.GetDpiForWindow(hwnd);

            return new CaretInfo(
                display.WorkArea.Width / 2.0,
                display.WorkArea.Height - 10 * (dpi / 96.0),
                0
            );
        }
    }

    private static string StartupArguments {
        get {
            var activatedEventArgs = Microsoft.Windows.AppLifecycle.AppInstance.GetCurrent().GetActivatedEventArgs();
            if (activatedEventArgs.Kind == ExtendedActivationKind.CommandLineLaunch) {
                var cmdLineArgs = (CommandLineActivatedEventArgs)activatedEventArgs.Data;
                return cmdLineArgs.Operation.Arguments;
            } else {
                return string.Empty;
            }
        }
    }

    public static void Restart() {
        Microsoft.Windows.AppLifecycle.AppInstance.Restart(StartupArguments);
    }

    public static HRESULT RegisterAutoRestart() {
        return Kernel32.RegisterApplicationRestart(StartupArguments, Kernel32.ApplicationRestartFlags.RESTART_NO_HANG | Kernel32.ApplicationRestartFlags.RESTART_NO_CRASH);
    }

    public static async Task RequestStartup() {
        var startupTask = await StartupTask.GetAsync("MyStartupId");
        switch (startupTask.State) {
        case StartupTaskState.Disabled:
            await startupTask.RequestEnableAsync();
            break;
        default:
            break;
        }
    }
}
