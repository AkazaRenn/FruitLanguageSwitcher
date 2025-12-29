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

internal sealed partial class Window: WindowEx, IDisposable {
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

    ~Window() => Dispose();

    public void Dispose() {
        core.ShowFlyoutLanguageEvent -= ShowFlyoutLanguage;
        core.ShowFlyoutCapsLockEvent -= ShowFlyoutCapsLock;
        this.Close();
        GC.SuppressFinalize(this);
    }

    void MoveAndResize(Utilities.CaretInfo caretInfo) {
        this.MoveAndResize(caretInfo.X, caretInfo.Y, 0, caretInfo.Height / this.Content.XamlRoot.RasterizationScale);
    }

    void ShowFlyout() {
        FlyoutControl.ShowAt(FlyoutAnchor);
        hideFlyoutTimer.Start();
    }

    bool MoveFlyout() {
        if (!Utilities.GetCaretPosition(out Utilities.CaretInfo? caretInfoOrNull)) {
            return false;
        }
        DispatcherQueue.TryEnqueue(() => {
            if (caretInfoOrNull is Utilities.CaretInfo caretInfo) {
                FlyoutControl.Content = flyoutContentAtCaret;
                MoveAndResize(caretInfo);
            } else {
                FlyoutControl.Content = flyoutContentFallback;
                this.MoveAndResize(Utilities.GetFallbackCaretPosition());
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
}
