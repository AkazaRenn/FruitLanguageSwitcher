namespace App.NotifyIcon;

internal sealed partial class TaskbarIcon: H.NotifyIcon.TaskbarIcon {
    public delegate void CommandEventHandler();

    public event CommandEventHandler? ExitEvent;
    public event CommandEventHandler? RestartEvent;

    public TaskbarIcon() {
        InitializeComponent();
        ForceCreate();

        RestartCommand.ExecuteRequested += (_, _) => RestartEvent?.Invoke();
        ExitCommand.ExecuteRequested += (_, _) => ExitEvent?.Invoke();
    }
}
