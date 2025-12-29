using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using System.Globalization;
using System.Numerics;
using Vanara.PInvoke;

namespace App.Flyout;

internal sealed partial class FlyoutContentFallback: UserControl, IFlyoutContent {
    public FlyoutContentFallback() {
        InitializeComponent();
    }

    void Update(bool isActive, string text) {
        TextBlock flyoutContentCurrent;
        TextBlock flyoutContentNew;
        if (FlyoutContent.Opacity > 0) {
            flyoutContentCurrent = FlyoutContent;
            flyoutContentNew = FlyoutContentAlt;
        } else {
            flyoutContentCurrent = FlyoutContentAlt;
            flyoutContentNew = FlyoutContent;
        }

        flyoutContentNew.Text = text;
        flyoutContentCurrent.Opacity = 0;

        if (isActive) {
            flyoutContentNew.Opacity = 1;
            FlyoutActiveIndicator.Scale = new Vector3(1, 1, 1);
            FlyoutActiveIndicator.Background =
                (Brush)Application.Current.Resources["AccentTextFillColorTertiaryBrush"];
        } else {
            flyoutContentNew.Opacity = 0.5;
            FlyoutActiveIndicator.Scale = new Vector3(0.5f, 1, 1);
            FlyoutActiveIndicator.Background =
                (Brush)Application.Current.Resources["TextFillColorDisabledBrush"];
        }
    }

    public void SetContentCapsLock() {
        Update(true, "CapsLock ON");
    }

    public void SetContentLanguage(bool active, LCID lcid) {
        Update(active, new CultureInfo((int)lcid.Value).NativeName);
    }
}
