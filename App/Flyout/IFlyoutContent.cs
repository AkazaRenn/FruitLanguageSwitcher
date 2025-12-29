using Vanara.PInvoke;

namespace App.Flyout;

internal interface IFlyoutContent {
    public void SetContentCapsLock();

    public void SetContentLanguage(bool active, LCID lcid);
}
