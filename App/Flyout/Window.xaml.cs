using Microsoft.UI.Dispatching;
using System;
using WinUIEx;

namespace App.Flyout {
    public sealed partial class Window: WindowEx {
        readonly DispatcherQueueTimer hideFlyoutTimer;
        readonly CaretFlyoutContent caretFlyoutContent;
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

            caretFlyoutContent = new(FlyoutControl);

            FlyoutAnchor.Loaded += (_, _) => ShowFlyoutLanguage(false, 0x404); // To be removed; for testing
            FlyoutControl.Opening += (_, _) => this.Show();
            FlyoutControl.Closing += (_, _) => hideFlyoutTimer.Stop();
            FlyoutControl.Closed += (_, _) => this.Hide();

            core.ShowFlyoutLanguageEvent += ShowFlyoutLanguage;
            core.ShowFlyoutCapsLockEvent += ShowFlyoutCapsLock;

            DispatcherQueue.TryEnqueue(() => this.Hide());
        }

        void ShowFlyoutLanguage(bool on, UInt32 imeLanguageLcid) {
            DispatcherQueue.TryEnqueue(() => {
                FlyoutControl.Content = caretFlyoutContent; // Depending on caret position
                caretFlyoutContent.SetContentLanguage(on, imeLanguageLcid);
                FlyoutControl.ShowAt(FlyoutAnchor);
                hideFlyoutTimer.Start();
            });
        }

        void ShowFlyoutCapsLock() {
            DispatcherQueue.TryEnqueue(() => {
                FlyoutControl.Content = caretFlyoutContent; // Depending on caret position
                caretFlyoutContent.SetContentCapsLock();
                FlyoutControl.ShowAt(FlyoutAnchor);
                hideFlyoutTimer.Start();
            });
        }
    }
}
