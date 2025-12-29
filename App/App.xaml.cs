using App.NotifyIcon;
using Microsoft.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.ApplicationModel;

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
        _ = RequestStartup();

        core = new();
        disposables.Add(core);

        window = new(core);
        disposables.Add(window);

        taskbarIcon = new();
        taskbarIcon.RestartEvent += Utilities.Restart;
        taskbarIcon.ExitEvent += Exit;
        disposables.Add(taskbarIcon);
    }

    private async Task RequestStartup() {
        var startupTask = await StartupTask.GetAsync("MyStartupId");
        switch (startupTask.State) {
        case StartupTaskState.Disabled:
            await startupTask.RequestEnableAsync();
            break;
        case StartupTaskState.DisabledByUser:
        case StartupTaskState.Enabled:
            break;
        }
    }

    private new void Exit() {
        foreach (var disposable in disposables) {
            disposable?.Dispose();
        }
    }
}
