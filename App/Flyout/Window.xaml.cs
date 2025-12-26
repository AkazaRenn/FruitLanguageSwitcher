using Microsoft.UI.Dispatching;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Media.Animation;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Win32; // This is where CsWin32 puts everything
using Windows.Win32.UI.WindowsAndMessaging;
using WinUIEx;
using WinUIEx.Messaging;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace App.Flyout {
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Window: WindowEx {
        DispatcherQueueTimer HideFlyoutTimer;

        public Window() { 
            InitializeComponent();

            this.SetWindowStyle(WindowStyle.Popup);
            //MakePopup(this.GetWindowHandle());

            Width = 0; 
            Height = 0;

            IsAlwaysOnTop = true;

            HideFlyoutTimer = DispatcherQueue.CreateTimer();
            HideFlyoutTimer.Interval = TimeSpan.FromSeconds(2);
            HideFlyoutTimer.Tick += HideFlyout;

            FlyoutAnchor.Loaded += ShowFlyout; // To be removed; for testing
            FlyoutControl.Opened += (_, _) => HideFlyoutTimer.Start();
        }

        private void MainWindow_Activated() {
            var appWindow = this.AppWindow;

            // Get display area for the current window
            var displayArea = DisplayArea.GetFromWindowId(appWindow.Id, DisplayAreaFallback.Primary);

            // Center the window
            int x = (displayArea.WorkArea.Width - appWindow.Size.Width) / 2;
            int y = (displayArea.WorkArea.Height - appWindow.Size.Height) / 2;

            this.Move(x, y);
        }

        void HideFlyout(object? sender, object e) {
            FlyoutControl.Hide();
            HideFlyoutTimer.Stop();
        }

        private async void ShowFlyout(object sender, RoutedEventArgs e) {
            Thread.Sleep(200);
            FlyoutControl.ShowAt(FlyoutAnchor);
            HideFlyoutTimer.Start();
        }

        private void UpdateFlyoutVisuals(bool isActive) {
            if (isActive) {
                FlyoutContent.Glyph = "\uE985"; // active icon
                FlyoutActiveIndicator.Width = 16;
                FlyoutActiveIndicator.Background =
                    (Brush)Application.Current.Resources["TextFillColorPrimary"];
            } else {
                FlyoutContent.Glyph = "\uE982"; // inactive icon
                FlyoutActiveIndicator.Width = 8;
                FlyoutActiveIndicator.Background =
                    (Brush)Application.Current.Resources["TextFillColorTertiary"];
            }
        }

        #region TODO: replace by WinUIEx calls https://github.com/dotMorten/WinUIEx/pull/249
        const int GWL_STYLE = -16;
        const int WS_POPUP = unchecked((int)0x80000000);
        const int WS_OVERLAPPEDWINDOW = 0x00CF0000;

        [DllImport("user32.dll", SetLastError = true)]
        static extern int GetWindowLong(IntPtr hWnd, int nIndex);

        [DllImport("user32.dll", SetLastError = true)]
        static extern int SetWindowLong(IntPtr hWnd, int nIndex, int dwNewLong);

        private void MakePopup(IntPtr hwnd) {
            int style = GetWindowLong(hwnd, GWL_STYLE);
            style &= ~WS_OVERLAPPEDWINDOW; // remove overlapped styles
            style |= WS_POPUP;             // add popup style
            SetWindowLong(hwnd, GWL_STYLE, style);
        }
        #endregion

    }
}
