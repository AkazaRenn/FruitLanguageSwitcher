#pragma once

import <unordered_set>;
import <Windows.h>;
import "Common.cpp";

namespace Core {
class Language {
private:
    static constexpr UINT IMC_GETCONVERSIONMODE = 0x1;
    static constexpr UINT IMC_SETCONVERSIONMODE = 0x2;

    static constexpr LCID zh_TW = 0x404; // Chinese (Traditional, Taiwan)
    static constexpr LCID ja_JP = 0x411; // Japanese (Japan)
    static constexpr LCID ko_KR = 0x412; // Korean (Korea)
    static constexpr LCID am_ET = 0x45E; // Amharic (Ethiopia)
    static constexpr LCID ti_ET = 0x473; // Tigrinya (Ethiopia)
    static constexpr LCID zh_CN = 0x804; // Chinese (Simplified, PRC)
    static constexpr LCID ti_ER = 0x873; // Tigrinya (Eritrea)
    static constexpr LCID zh_HK = 0xC04; // Chinese (Traditional, Hong Kong S.A.R.)
    static constexpr LCID zh_SG = 0x1004; // Chinese (Simplified, Singapore)
    static constexpr LCID zh_MO = 0x1404; // Chinese (Traditional, Macao S.A.R.)

    static constexpr bool IsImeLanguage(LCID lcid) {
        return
            lcid == zh_TW ||
            lcid == ja_JP ||
            lcid == ko_KR ||
            lcid == am_ET ||
            lcid == ti_ET ||
            lcid == zh_CN ||
            lcid == ti_ER ||
            lcid == zh_HK ||
            lcid == zh_SG ||
            lcid == zh_MO;
    }

private:
    bool GetConversionMode(HWND hwnd) {
        switch (lcid) {
        case zh_TW:
            [[fallthrough]];
        case zh_CN:
            [[fallthrough]];
        case zh_HK:
            [[fallthrough]];
        case zh_SG:
            [[fallthrough]];
        case zh_MO:
            [[fallthrough]];
        case ko_KR:
            return SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_GETCONVERSIONMODE, 0) == 1;
        case ja_JP:
            return false;
        case am_ET:
            [[fallthrough]];
        case ti_ET:
            [[fallthrough]];
        case ti_ER:
            [[fallthrough]];
        default:
            return true;
        }
    }

    void SetConversionMode(HWND hwnd) {
        switch (lcid) {
        case zh_TW:
            [[fallthrough]];
        case zh_CN:
            [[fallthrough]];
        case zh_HK:
            [[fallthrough]];
        case zh_SG:
            [[fallthrough]];
        case zh_MO:
            [[fallthrough]];
        case ko_KR:
            SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_SETCONVERSIONMODE, 1);
            SendKeyCombination({VK_LSHIFT, VK_SPACE});
            return;
        case ja_JP:
            SendKeySequence({VK_IME_ON});
            return;
        case am_ET:
            [[fallthrough]];
        case ti_ET:
            [[fallthrough]];
        case ti_ER:
            [[fallthrough]];
        default:
            return;
        }
    }

public:
    Language(HKL _hkl)
        : hkl(_hkl)
        , lcid(MAKELCID(LOWORD(reinterpret_cast<UINT_PTR>(hkl)), SORT_DEFAULT))
        , isImeLanguage(IsImeLanguage(lcid)) {}

    void Activate(HWND hwnd, HKL currentHkl) {
        if (hkl != currentHkl) {
            SendMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, reinterpret_cast<LPARAM>(hkl));
        }
        do {
            SetConversionMode(hwnd);
            Sleep(50);
        } while (!GetConversionMode(hwnd));
    }

    bool RemapRMenu() {
        return isImeLanguage;
    }

    void OnRMenuUp() {
        switch (lcid) {
        case zh_TW:
            [[fallthrough]];
        case zh_CN:
            [[fallthrough]];
        case zh_HK:
            [[fallthrough]];
        case zh_SG:
            [[fallthrough]];
        case zh_MO:
            SendKeyCombination({VK_LSHIFT, VK_SPACE});
            return;
        case ko_KR:
            return;
        case ja_JP:
            SendKeySequence({VK_NONCONVERT});
            return;
        case am_ET:
            [[fallthrough]];
        case ti_ET:
            [[fallthrough]];
        case ti_ER:
            [[fallthrough]];
        default:
            return;
        }
    }

public:
    const HKL hkl;
    const LCID lcid;
    const bool isImeLanguage;
};
}
