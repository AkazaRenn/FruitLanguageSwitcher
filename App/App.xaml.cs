using H.NotifyIcon;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Input;
using System;
using System.Collections.Generic;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace App {
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    public partial class App: Application {
        private readonly HashSet<IDisposable> disposables = [];

        Flyout.Window window;

        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App() {
            InitializeComponent();
        }

        protected override void OnLaunched(LaunchActivatedEventArgs args) {
            var exitApplicationCommand = (XamlUICommand)Resources["ExitApplicationCommand"];
            exitApplicationCommand.ExecuteRequested += ExitApplicationCommand_ExecuteRequested;

            TaskbarIcon notifyIcon = (TaskbarIcon)Resources["NotifyIcon"];
            notifyIcon.ForceCreate();

            disposables.Add(notifyIcon);
            //disposables.Add(new Interop.Core());

            window = new();
            window.Show();
        }

        private void ExitApplicationCommand_ExecuteRequested(object? _, ExecuteRequestedEventArgs args) {
            foreach (var disposable in disposables) {
                disposable.Dispose();
            }

            Exit();
        }
    }
}
