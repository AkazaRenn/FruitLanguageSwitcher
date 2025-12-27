using Microsoft.UI.Dispatching;
using System;
using WinUIEx;

namespace App.Flyout;

public sealed partial class Window: WindowEx {
    readonly DispatcherQueueTimer hideFlyoutTimer;
    readonly FlyoutContentAtCaret flyoutContentAtCaret;
    readonly FlyoutContentFallback flyoutContentFallback;
    readonly Interop.Core core;

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

        flyoutContentAtCaret = new();
        flyoutContentFallback = new();

        //FlyoutAnchor.Loaded += (_, _) => ShowFlyoutLanguage(0, 0x404); // To be removed; for testing
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

    void ShowFlyoutLanguage(UInt32 activeLcid, UInt32 activeImeLcid) {
        DispatcherQueue.TryEnqueue(() => {
            FlyoutControl.Content = flyoutContentFallback; // Depending on caret position
            ((IFlyoutContent)FlyoutControl.Content).SetContentLanguage(activeLcid == activeImeLcid, activeImeLcid);
            ShowFlyout();
        });
    }

    void ShowFlyoutCapsLock() {
        DispatcherQueue.TryEnqueue(() => {
            FlyoutControl.Content = flyoutContentFallback; // Depending on caret position
            ((IFlyoutContent)FlyoutControl.Content).SetContentCapsLock();
            ShowFlyout();
        });
    }
}
