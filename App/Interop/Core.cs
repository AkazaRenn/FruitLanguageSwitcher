using System;
using System.Runtime.InteropServices;

namespace App.Interop;

internal sealed partial class Core: IDisposable {
    public delegate void ShowFlyoutLanguageEventHandler(UInt32 activeLcid, UInt32 activeImeLcid);
    public event ShowFlyoutLanguageEventHandler? ShowFlyoutLanguageEvent;

    public delegate void ShowFlyoutCapsLockEventHandler();
    public event ShowFlyoutCapsLockEventHandler? ShowFlyoutCapsLockEvent;

    delegate void ShowFlyoutDelegate(UInt32 activeLcid, UInt32 activeImeLcid);
    readonly ShowFlyoutDelegate showFlyoutDelegate;
    readonly IntPtr pUnmanaged;

    public Core() {
        showFlyoutDelegate = ShowFlyout;
        pUnmanaged = CoreStart(Marshal.GetFunctionPointerForDelegate(showFlyoutDelegate));
    }

    ~Core() {
        Dispose();
    }

    public void Dispose() {
        CoreStop(pUnmanaged);
        GC.SuppressFinalize(this);
    }

    void ShowFlyout(UInt32 activeLcid, UInt32 activeImeLcid) {
        if (activeLcid > 0) {
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
