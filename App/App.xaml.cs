using App.Interop;
using App.NotifyIcon;
using Microsoft.UI.Xaml;
using Microsoft.Windows.AppLifecycle;
using Vanara.PInvoke;

namespace App;

public partial class App: Application {
    readonly HashSet<IDisposable> disposables = [];
    readonly Mutex singleInstanceMutex;
    Core? core;
    Flyout.Window? window;
    TaskbarIcon? taskbarIcon;

    public App() {
        singleInstanceMutex = new Mutex(true, Windows.ApplicationModel.Package.Current.Id.FamilyName, out bool createdNew);
        if (!createdNew) {
            Current.Exit();
        }

        InitializeComponent();
    }

    protected override void OnLaunched(LaunchActivatedEventArgs args) {
        Kernel32.RegisterApplicationRestart(Utilities.StartupArguments, Kernel32.ApplicationRestartFlags.RESTART_NO_HANG | Kernel32.ApplicationRestartFlags.RESTART_NO_CRASH);
        _ = Utilities.RequestStartup();

        core = new();
        disposables.Add(core);

        window = new(core);
        disposables.Add(window);

        taskbarIcon = new();
        taskbarIcon.RestartEvent += () => AppInstance.Restart(Utilities.StartupArguments);
        taskbarIcon.ExitEvent += Exit;
        disposables.Add(taskbarIcon);
    }

    private new void Exit() {
        foreach (var disposable in disposables) {
            disposable?.Dispose();
        }
    }
}
