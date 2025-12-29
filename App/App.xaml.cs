using App.NotifyIcon;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Input;
using System;
using System.Collections.Generic;

namespace App;

public partial class App: Application {
    readonly HashSet<IDisposable> disposables = [];
    Interop.Core? core;
    Flyout.Window? window;
    TaskbarIcon? taskbarIcon;

    public App() {
        InitializeComponent();
    }

    protected override void OnLaunched(LaunchActivatedEventArgs args) {
        core = new();
        disposables.Add(core);

        window = new(core);
        disposables.Add(window);

        taskbarIcon = new();
        taskbarIcon.ExitEvent += Exit;
        disposables.Add(taskbarIcon);
    }

    private new void Exit() {
        foreach (var disposable in disposables) {
            disposable?.Dispose();
        }
    }
}
