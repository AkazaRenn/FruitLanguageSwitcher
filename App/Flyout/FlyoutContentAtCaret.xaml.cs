using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using System;
using System.Numerics;
using Vanara.PInvoke;

namespace App.Flyout;

internal sealed partial class FlyoutContentAtCaret: UserControl, IFlyoutContent {
    static string GetLanguageGlyph(LCID lcid) {
        const uint zh_TW = 0x404; // Chinese (Traditional, Taiwan)
        const uint ja_JP = 0x411; // Japanese (Japan)
        const uint ko_KR = 0x412; // Korean (Korea)
        const uint am_ET = 0x45E; // Amharic (Ethiopia)
        const uint ti_ET = 0x473; // Tigrinya (Ethiopia)
        const uint zh_CN = 0x804; // Chinese (Simplified, PRC)
        const uint ti_ER = 0x873; // Tigrinya (Eritrea)
        const uint zh_HK = 0xC04; // Chinese (Traditional, Hong Kong S.A.R.)
        const uint zh_SG = 0x1004; // Chinese (Simplified, Singapore)
        const uint zh_MO = 0x1404; // Chinese (Traditional, Macao S.A.R.)

        return lcid.Value switch {
            zh_TW or zh_CN or zh_HK or zh_SG or zh_MO => "\uE982",// QWERTYOn
            ko_KR => "\uE97D",// Korean
            ja_JP => "\uE985",// Japanese
            am_ET or ti_ET or ti_ER => "\uEC31",// KeyboardFull
            _ => "\uE92E",// KeyboardStandard
        };
    }

    public FlyoutContentAtCaret() {
        InitializeComponent();
    }

    void Update(bool isActive, string glyph) {
        FontIcon flyoutContentCurrent;
        FontIcon flyoutContentNew;
        if (FlyoutContent.Opacity > 0) {
            flyoutContentCurrent = FlyoutContent;
            flyoutContentNew = FlyoutContentAlt;
        } else {
            flyoutContentCurrent = FlyoutContentAlt;
            flyoutContentNew = FlyoutContent;
        }

        flyoutContentNew.Glyph = glyph;
        flyoutContentCurrent.Opacity = 0;

        if (isActive) {
            flyoutContentNew.Opacity = 1;
            FlyoutActiveIndicator.Scale = new Vector3(1, 1, 1);
            FlyoutActiveIndicator.Background =
                (Brush)Application.Current.Resources["AccentTextFillColorTertiaryBrush"];
        } else {
            flyoutContentNew.Opacity = 0.5;
            FlyoutActiveIndicator.Scale = new Vector3(0.67f, 1, 1);
            FlyoutActiveIndicator.Background =
                (Brush)Application.Current.Resources["TextFillColorDisabledBrush"];
        }
    }

    public void SetContentCapsLock() {
        Update(true, "\uE72E"); // Lock icon
    }

    public void SetContentLanguage(bool active, LCID lcid) {
        Update(active, GetLanguageGlyph(lcid));
    }
}
