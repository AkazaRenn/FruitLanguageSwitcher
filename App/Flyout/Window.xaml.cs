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

        Width = 0;
        Height = 0;

        IsAlwaysOnTop = true;
        IsResizable = false;
        IsMaximizable = false;
        IsMinimizable = false;
        IsShownInSwitchers = false;

        hideFlyoutTimer = DispatcherQueue.CreateTimer();
        hideFlyoutTimer.Interval = TimeSpan.FromSeconds(2);
        hideFlyoutTimer.Tick += (_, _) => {
            FlyoutControl.Hide();
            hideFlyoutTimer.Stop();
        };

        flyoutContentAtCaret = new();
        flyoutContentFallback = new();

        FlyoutAnchor.Loaded += (_, _) => ShowFlyoutLanguage(false, 0x404); // To be removed; for testing
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

    void ShowFlyoutLanguage(bool on, UInt32 imeLanguageLcid) {
        DispatcherQueue.TryEnqueue(() => {
            FlyoutControl.Content = flyoutContentFallback; // Depending on caret position
            ((IFlyoutContent)FlyoutControl.Content).SetContentLanguage(on, imeLanguageLcid);
            ShowFlyout();
        });
    }

    void ShowFlyoutCapsLock() {
        DispatcherQueue.TryEnqueue(() => {
            FlyoutControl.Content = flyoutContentAtCaret; // Depending on caret position
            ((IFlyoutContent)FlyoutControl.Content).SetContentCapsLock();
            ShowFlyout();
        });
    }
}
