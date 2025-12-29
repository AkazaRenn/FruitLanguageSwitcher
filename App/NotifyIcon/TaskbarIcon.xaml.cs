using Microsoft.UI.Xaml.Input;

namespace App.NotifyIcon;

internal sealed partial class TaskbarIcon: H.NotifyIcon.TaskbarIcon {
    public delegate void CommandEventHandler();

    public event CommandEventHandler? ExitEvent;

    public TaskbarIcon() {
        InitializeComponent();
        ForceCreate();

        ExitCommand.ExecuteRequested += (_, _) => ExitEvent?.Invoke();
    }
}
