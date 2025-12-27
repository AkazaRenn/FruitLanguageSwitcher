using System;

namespace App.Flyout;

internal interface IFlyoutContent {
    public void SetContentCapsLock();

    public void SetContentLanguage(bool active, UInt32 lcid);
}
