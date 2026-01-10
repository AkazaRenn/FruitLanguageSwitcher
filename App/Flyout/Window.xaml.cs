using App.Interop;
using Microsoft.UI.Dispatching;
using Vanara.PInvoke;
using WinUIEx;

namespace App.Flyout;

internal sealed partial class Window: WindowEx, IDisposable {
    readonly Core core;
    readonly DispatcherQueueTimer hideFlyoutTimer;
    readonly FlyoutContentAtCaret flyoutContentAtCaret = new();
    readonly FlyoutContentFallback flyoutContentFallback = new();
    readonly InputDetector inputDetector;

    public Window(Core _core) {
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

        inputDetector = new InputDetector(this.GetWindowHandle());
        inputDetector.UserInputEvent += () => FlyoutControl.Hide();

        hideFlyoutTimer = DispatcherQueue.CreateTimer();
        hideFlyoutTimer.IsRepeating = false;
        hideFlyoutTimer.Interval = TimeSpan.FromSeconds(2);
        hideFlyoutTimer.Tick += (_, _) => FlyoutControl.Hide();

        FlyoutControl.Opening += (_, _) => this.Show();
        FlyoutControl.Opened += (_, _) => hideFlyoutTimer.Start();
        FlyoutControl.Opened += (_, _) => inputDetector.Start();
        FlyoutControl.Closing += (_, _) => hideFlyoutTimer.Stop();
        FlyoutControl.Closing += (_, _) => inputDetector.Stop();
        FlyoutControl.Closed += (_, _) => this.Hide();

        core = _core;
        core.ShowFlyoutLanguageEvent += ShowFlyoutLanguage;
        core.ShowFlyoutCapsLockEvent += ShowFlyoutCapsLock;
    }

    ~Window() => Dispose();

    public void Dispose() {
        core?.ShowFlyoutLanguageEvent -= ShowFlyoutLanguage;
        core?.ShowFlyoutCapsLockEvent -= ShowFlyoutCapsLock;
        inputDetector?.Dispose();
        Close();
        GC.SuppressFinalize(this);
    }

    void MoveAndResize(UIAutomation.CaretInfo caretInfo) {
        this.MoveAndResize(caretInfo.X, caretInfo.Y, 0, caretInfo.Height / Content.XamlRoot.RasterizationScale);
    }

    void ShowFlyout() {
        if (FlyoutControl.IsOpen) {
            hideFlyoutTimer.Start();
        } else {
            FlyoutControl.ShowAt(FlyoutAnchor);
        }
    }

    bool MoveFlyout() {
        if (!UIAutomation.GetCaretPosition(out UIAutomation.CaretInfo caretInfo)) {
            return false;
        }
        if (caretInfo.Height > 0) {
            FlyoutControl.Content = flyoutContentAtCaret;
            MoveAndResize(caretInfo);
        } else {
            FlyoutControl.Content = flyoutContentFallback;
            MoveAndResize(Utilities.FallbackCaretPosition);
        }
        return true;
    }

    void ShowFlyoutLanguage(LCID activeLcid, LCID activeImeLcid) {
        DispatcherQueue.TryEnqueue(() => {
            if (!MoveFlyout()) {
                return;
            }
            ((IFlyoutContent)FlyoutControl.Content).SetContentLanguage(activeLcid == activeImeLcid, activeImeLcid);
            ShowFlyout();
        });
    }

    void ShowFlyoutCapsLock() {
        DispatcherQueue.TryEnqueue(() => {
            if (!MoveFlyout()) {
                return;
            }
            ((IFlyoutContent)FlyoutControl.Content).SetContentCapsLock();
            ShowFlyout();
        });
    }
}
