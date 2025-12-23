import <unordered_set>;
import <Windows.h>;
import "Utilities.cpp";

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

private:
    void SetConversionMode(HWND hwnd) const {
        int retry = 2;
        const int retryPeriodMs = 100;

        switch (lcid) {
        case zh_TW:
            [[fallthrough]];
        case zh_CN:
            [[fallthrough]];
        case zh_HK:
            [[fallthrough]];
        case zh_SG:
            [[fallthrough]];
        case zh_MO: {
            const DWORD expectedMode = IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE;
            do {
                SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_SETCONVERSIONMODE, expectedMode);
                Sleep(retryPeriodMs);
            } while ((retry-- > 0) && (SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_GETCONVERSIONMODE, 0) != expectedMode));
            return;
        }
        case ko_KR: {
            const DWORD expectedMode = IME_CMODE_NATIVE;
            do {
                SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_SETCONVERSIONMODE, expectedMode);
                Sleep(retryPeriodMs);
            } while ((retry-- > 0) && (SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_GETCONVERSIONMODE, 0) != expectedMode));
            return;
        }
        case ja_JP: {
            do {
                SendKeySequence({VK_IME_ON});
                Sleep(retryPeriodMs);
            } while (retry-- > 0);
            return;
        }
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

    bool operator==(const Language& other) const {
        return hkl == other.hkl;
    }

    void Activate(HWND hwnd, bool isActive) const {
        if (!isActive) {
            SendMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, reinterpret_cast<LPARAM>(hkl));
        }
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
            {
                const HWND hwnd = GetForegroundWindow();
                const DWORD expectedMode = IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE;
                if (SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_GETCONVERSIONMODE, 0) != expectedMode) {
                    SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_SETCONVERSIONMODE, expectedMode);
                } else {
                    SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_SETCONVERSIONMODE, 0);
                }
            }
            return;
        case ko_KR:
            {
                const HWND hwnd = GetForegroundWindow();
                const DWORD expectedMode = IME_CMODE_NATIVE;
                if (SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_GETCONVERSIONMODE, 0) != expectedMode) {
                    SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_SETCONVERSIONMODE, expectedMode);
                } else {
                    SendMessage(ImmGetDefaultIMEWnd(hwnd), WM_IME_CONTROL, IMC_SETCONVERSIONMODE, 0);
                }
            }
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
