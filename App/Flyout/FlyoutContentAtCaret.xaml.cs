using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using System;
using System.Numerics;

namespace App.Flyout;

public sealed partial class FlyoutContentAtCaret: UserControl, IFlyoutContent {
    static string GetLanguageGlyph(UInt32 lcid) {
        const UInt32 zh_TW = 0x404; // Chinese (Traditional, Taiwan)
        const UInt32 ja_JP = 0x411; // Japanese (Japan)
        const UInt32 ko_KR = 0x412; // Korean (Korea)
        const UInt32 am_ET = 0x45E; // Amharic (Ethiopia)
        const UInt32 ti_ET = 0x473; // Tigrinya (Ethiopia)
        const UInt32 zh_CN = 0x804; // Chinese (Simplified, PRC)
        const UInt32 ti_ER = 0x873; // Tigrinya (Eritrea)
        const UInt32 zh_HK = 0xC04; // Chinese (Traditional, Hong Kong S.A.R.)
        const UInt32 zh_SG = 0x1004; // Chinese (Simplified, Singapore)
        const UInt32 zh_MO = 0x1404; // Chinese (Traditional, Macao S.A.R.)

        switch (lcid) {
        case zh_TW:
        case zh_CN:
        case zh_HK:
        case zh_SG:
        case zh_MO:
            return "\uE982"; // QWERTYOn
        case ko_KR:
            return "\uE97D"; // Korean
        case ja_JP:
            return "\uE985"; // Japanese
        case am_ET:
        case ti_ET:
        case ti_ER:
            return "\uEC31"; // KeyboardFull
        default:
            return "\uE92E"; // KeyboardStandard
        }
    }

    public FlyoutContentAtCaret() {
        InitializeComponent();
    }

    void Update(bool isActive, string glyph) {
        FontIcon currentFlyoutContent;
        FontIcon newFlyoutContent;
        if (FlyoutContent.Opacity > 0) {
            currentFlyoutContent = FlyoutContent;
            newFlyoutContent = FlyoutContentAlt;
        } else {
            currentFlyoutContent = FlyoutContentAlt;
            newFlyoutContent = FlyoutContent;
        }

        newFlyoutContent.Glyph = glyph;
        currentFlyoutContent.Opacity = 0;

        if (isActive) {
            newFlyoutContent.Opacity = 1;
            FlyoutActiveIndicator.Scale = new Vector3(1, 1, 1);
            FlyoutActiveIndicator.Background =
                (Brush)Application.Current.Resources["AccentTextFillColorTertiaryBrush"];
        } else {
            newFlyoutContent.Opacity = 0.5;
            FlyoutActiveIndicator.Scale = new Vector3(0.67f, 1, 1);
            FlyoutActiveIndicator.Background =
                (Brush)Application.Current.Resources["TextFillColorDisabledBrush"];
        }
    }

    public void SetContentCapsLock() {
        Update(true, "\uE72E"); // Lock icon
    }

    public void SetContentLanguage(bool active, UInt32 lcid) {
        Update(active, GetLanguageGlyph(lcid));
    }
}
