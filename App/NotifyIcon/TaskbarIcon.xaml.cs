using System;

namespace App.NotifyIcon;

internal sealed partial class TaskbarIcon: H.NotifyIcon.TaskbarIcon {
    public event Action? ExitEvent;
    public event Action? RestartEvent;

    public TaskbarIcon() {
        InitializeComponent();
        ForceCreate();

        RestartCommand.ExecuteRequested += (_, _) => RestartEvent?.Invoke();
        ExitCommand.ExecuteRequested += (_, _) => ExitEvent?.Invoke();
    }
}
