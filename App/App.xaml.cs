using H.NotifyIcon;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Input;
using System;
using System.Collections.Generic;


namespace App;

public partial class App: Application {
    readonly HashSet<IDisposable> disposables = [];
    Interop.Core? core;
    Flyout.Window? window;

    public App() {
        InitializeComponent();
    }

    protected override void OnLaunched(LaunchActivatedEventArgs args) {
        var exitApplicationCommand = (XamlUICommand)Resources["ExitApplicationCommand"];
        exitApplicationCommand.ExecuteRequested += ExitApplicationCommand_ExecuteRequested;

        TaskbarIcon notifyIcon = (TaskbarIcon)Resources["NotifyIcon"];
        notifyIcon.ForceCreate();
        disposables.Add(notifyIcon);

        core = new();
        disposables.Add(core);

        window = new(core);
        window.Show();
    }

    private void ExitApplicationCommand_ExecuteRequested(object? _, ExecuteRequestedEventArgs args) {
        foreach (var disposable in disposables) {
            disposable?.Dispose();
        }

        Exit();
    }
}
