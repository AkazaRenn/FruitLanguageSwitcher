using FlaUI.Core.Definitions;
using FlaUI.UIA3;
using Microsoft.UI;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Windowing;
using System;
using System.Drawing;
using Vanara.PInvoke;
using WinUIEx;

namespace App.Flyout;

internal sealed partial class Window: WindowEx {
    readonly Interop.Core core;
    readonly DispatcherQueueTimer hideFlyoutTimer;
    readonly FlyoutContentAtCaret flyoutContentAtCaret = new();
    readonly FlyoutContentFallback flyoutContentFallback = new();

    public Window(Interop.Core _core) {
        core = _core;
        InitializeComponent();

        this.SetWindowStyle(WindowStyle.Popup);
        this.SetExtendedWindowStyle(ExtendedWindowStyle.NoActivate);

        IsAlwaysOnTop = true;
        IsResizable = false;
        IsMaximizable = false;
        IsMinimizable = false;
        IsShownInSwitchers = false;

        Width = 0;
        Height = 0;

        DispatcherQueue.TryEnqueue(() => this.Hide());

        hideFlyoutTimer = DispatcherQueue.CreateTimer();
        hideFlyoutTimer.IsRepeating = false;
        hideFlyoutTimer.Interval = TimeSpan.FromSeconds(2);
        hideFlyoutTimer.Tick += (_, _) => FlyoutControl.Hide();

        FlyoutControl.Opening += (_, _) => this.Show();
        FlyoutControl.Closing += (_, _) => hideFlyoutTimer.Stop();
        FlyoutControl.Closed += (_, _) => this.Hide();

        core.ShowFlyoutLanguageEvent += ShowFlyoutLanguage;
        core.ShowFlyoutCapsLockEvent += ShowFlyoutCapsLock;
    }

    void MoveAndResize(CaretInfo caretInfo) {
        this.MoveAndResize(caretInfo.X, caretInfo.Y, 0, caretInfo.Height / this.Content.XamlRoot.RasterizationScale);
    }

    void ShowFlyout() {
        FlyoutControl.ShowAt(FlyoutAnchor);
        hideFlyoutTimer.Start();
    }

    bool MoveFlyout() {
        if (!GetCaretPosition(out CaretInfo? caretInfoOrNull)) {
            return false;
        }
        DispatcherQueue.TryEnqueue(() => {
            if (caretInfoOrNull is CaretInfo caretInfo) {
                FlyoutControl.Content = flyoutContentAtCaret;
                MoveAndResize(caretInfo);
            } else {
                FlyoutControl.Content = flyoutContentFallback;
                this.MoveAndResize(GetFallbackCaretPosition());
            }
        });
        return true;
    }

    void ShowFlyoutLanguage(LCID activeLcid, LCID activeImeLcid) {
        if (!MoveFlyout()) {
            return;
        }
        DispatcherQueue.TryEnqueue(() => {
            ((IFlyoutContent)FlyoutControl.Content).SetContentLanguage(activeLcid == activeImeLcid, activeImeLcid);
            ShowFlyout();
        });
    }

    void ShowFlyoutCapsLock() {
        if (!MoveFlyout()) {
            return;
        }
        DispatcherQueue.TryEnqueue(() => {
            ((IFlyoutContent)FlyoutControl.Content).SetContentCapsLock();
            ShowFlyout();
        });
    }

    struct CaretInfo {
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
            X = (double)(rect.Left + rect.Right) / 2;
            Y = rect.Top;
            Height = rect.Height;
        }
    }

    static bool GetCaretPosition(out CaretInfo? caretInfo) {
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

    CaretInfo GetFallbackCaretPosition() {
        HWND hwnd = User32.GetForegroundWindow();
        WindowId windowId = Win32Interop.GetWindowIdFromWindow(hwnd.DangerousGetHandle());
        DisplayArea display = DisplayArea.GetFromWindowId(windowId, DisplayAreaFallback.Primary);
        return new CaretInfo(
            (double)display.WorkArea.Width / 2,
            display.WorkArea.Height - 10 * this.Content.XamlRoot.RasterizationScale,
            0
        );
    }
}
