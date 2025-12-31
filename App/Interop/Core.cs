using Microsoft.Windows.AppNotifications;
using Microsoft.Windows.AppNotifications.Builder;
using System;
using System.Runtime.InteropServices;
using Vanara.PInvoke;

namespace App.Interop;

internal sealed partial class Core: IDisposable {
    public delegate void ShowFlyoutLanguageEventHandler(LCID activeLcid, LCID activeImeLcid);
    public event ShowFlyoutLanguageEventHandler? ShowFlyoutLanguageEvent;

    public delegate void ShowFlyoutCapsLockEventHandler();
    public event ShowFlyoutCapsLockEventHandler? ShowFlyoutCapsLockEvent;

    delegate void ShowFlyoutDelegate(LCID activeLcid, LCID activeImeLcid);
    readonly ShowFlyoutDelegate showFlyoutDelegate;
    readonly IntPtr pUnmanaged;

    public Core() {
        showFlyoutDelegate = ShowFlyout;
        pUnmanaged = CoreStart(Marshal.GetFunctionPointerForDelegate(showFlyoutDelegate));
        if (pUnmanaged == IntPtr.Zero) {
            AppNotificationManager.Default.Show(new AppNotificationBuilder()
                .AddText("Initialization failed!")
                .AddText("Please make sure you have both Latin & IME languages enabled.")
                .BuildNotification());
        }
    }

    ~Core() => Dispose();

    public void Dispose() {
        CoreStop(pUnmanaged);
        GC.SuppressFinalize(this);
    }

    void ShowFlyout(LCID activeLcid, LCID activeImeLcid) {
        if (activeLcid.Value > 0) {
            ShowFlyoutLanguageEvent?.Invoke(activeLcid, activeImeLcid);
        } else {
            ShowFlyoutCapsLockEvent?.Invoke();
        }
    }

    [LibraryImport("Core.dll", EntryPoint = "Start")]
    private static partial IntPtr CoreStart(IntPtr showFlyoutFunction);

    [LibraryImport("Core.dll", EntryPoint = "Stop")]
    private static partial void CoreStop(IntPtr instance);

}
