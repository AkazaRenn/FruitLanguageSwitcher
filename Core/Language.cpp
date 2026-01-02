import <unordered_set>;
import <Windows.h>;
import "Utilities.cpp";

#pragma comment(lib, "imm32.lib")

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
        constexpr auto imeLanguages = std::to_array<LCID>({
            zh_TW, ja_JP, ko_KR, am_ET, ti_ET, zh_CN, ti_ER, zh_HK, zh_SG, zh_MO,
        });

        return std::ranges::contains(imeLanguages, lcid);
    }

    const static void SetConversionMode(HWND hwnd, DWORD expectedMode) {
        int retry = 2;
        const int retryPeriodMs = 100;
        hwnd = GetCoreWindow(hwnd);

        do {
            SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_SETCONVERSIONMODE, expectedMode);
            Sleep(retryPeriodMs);
        } while ((retry-- > 0) && (SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_GETCONVERSIONMODE, 0) != expectedMode));
    }

    const static void ToggleConversionMode(DWORD expectedMode) {
        const HWND hwnd = GetCoreWindow(GetForegroundWindow());
        const DWORD newMode = (SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_GETCONVERSIONMODE, 0) != expectedMode) ? expectedMode : 0;
        SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_SETCONVERSIONMODE, newMode);
    }

private:
    void SetConversionMode(HWND hwnd) const {
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
            SetConversionMode(hwnd, IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE);
            return;
        case ko_KR: 
            SetConversionMode(hwnd, IME_CMODE_NATIVE);
            return;
        case ja_JP: 
            SendKeySequence({VK_IME_ON});
            Sleep(100);
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
        , lcid(HklToLcid(hkl))
        , isImeLanguage(IsImeLanguage(lcid)) {}

    bool operator==(const Language& other) const {
        return hkl == other.hkl;
    }

    void Activate(HWND hwnd) const {
        PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, static_cast<LPARAM>(lcid));
        SetScrollLockState(isImeLanguage);
        SetConversionMode(hwnd);
    }

    void OnRMenuUp() const {
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
            ToggleConversionMode(IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE);
            return;
        case ko_KR:
            ToggleConversionMode(IME_CMODE_NATIVE);
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
