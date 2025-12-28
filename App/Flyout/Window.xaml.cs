using FlaUI.Core.Definitions;
using FlaUI.UIA3;
using Microsoft.UI.Dispatching;
using System;
using System.Drawing;
using WinUIEx;

namespace App.Flyout;

public sealed partial class Window: WindowEx {
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

        hideFlyoutTimer = DispatcherQueue.CreateTimer();
        hideFlyoutTimer.Interval = TimeSpan.FromSeconds(2);
        hideFlyoutTimer.Tick += (_, _) => {
            FlyoutControl.Hide();
            hideFlyoutTimer.Stop();
        };

        FlyoutControl.Opening += (_, _) => this.Show();
        FlyoutControl.Closing += (_, _) => hideFlyoutTimer.Stop();
        FlyoutControl.Closed += (_, _) => this.Hide();

        core.ShowFlyoutLanguageEvent += ShowFlyoutLanguage;
        core.ShowFlyoutCapsLockEvent += ShowFlyoutCapsLock;

        DispatcherQueue.TryEnqueue(() => this.Hide());
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
                this.MoveAndResize(caretInfo.X, caretInfo.Y, 0, caretInfo.Height);
            } else {
                FlyoutControl.Content = flyoutContentFallback;
                this.MoveAndResize(1920, 2160 - 100, 0, 0);
            }
        });
        return true;
    }

    void ShowFlyoutLanguage(UInt32 activeLcid, UInt32 activeImeLcid) {
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
        public int X { get; set; }
        public int Y { get; set; }
        public int Height { get; set; }

        public CaretInfo(Rectangle rect) {
            X = (rect.Left + rect.Right) / 2;
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
            var caretRange = textPattern.GetCaretRange(out bool _);
            if (caretRange == null) {
                break;
            }
            if (caretRange.CompareEndpoints(TextPatternRangeEndpoint.Start, textRange, TextPatternRangeEndpoint.Start) == 0) {
                textRange.MoveEndpointByRange(TextPatternRangeEndpoint.Start, textRange, TextPatternRangeEndpoint.End);
            } else {
                textRange.MoveEndpointByRange(TextPatternRangeEndpoint.End, textRange, TextPatternRangeEndpoint.Start);
            }
            var rectangles = textRange.GetBoundingRectangles();
            if (rectangles == null || rectangles.Length == 0) {
                break;
            }
            caretInfo = new CaretInfo(rectangles[0]);
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
}
