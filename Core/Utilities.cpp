import <algorithm>;
import <array>;
import <memory>;
import <string>;
import <vector>;
import <Windows.h>;

namespace Core {
constexpr bool IsExtendedKey(WORD vk) {
    constexpr auto extendedKeys = std::to_array<WORD>({
        VK_RMENU, VK_RCONTROL, VK_INSERT, VK_DELETE,
        VK_HOME, VK_END, VK_PRIOR, VK_NEXT,
        VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT,
        VK_DIVIDE, VK_NUMLOCK, VK_APPS
    });

    return std::ranges::contains(extendedKeys, vk);
}

const inline void SetKey(WORD key, INPUT& downInput, INPUT& upInput) {
    DWORD flags = IsExtendedKey(key) ? KEYEVENTF_EXTENDEDKEY : 0;

    downInput = INPUT {
        .type = INPUT_KEYBOARD,
        .ki = {
            .wVk = key,
            .dwFlags = flags,
        },
    };
    upInput = INPUT {
        .type = INPUT_KEYBOARD,
        .ki = {
            .wVk = key,
            .dwFlags = flags | KEYEVENTF_KEYUP,
        },
    };
}

const inline UINT SendKeySequence(std::vector<WORD> keys) {
    size_t numInputs = keys.size() * 2;
    auto inputs = std::make_unique<INPUT[]>(numInputs);

    for (size_t i = 0; i < keys.size(); i++) {
        SetKey(keys[i], inputs[i * 2], inputs[i * 2 + 1]);
    }

    return SendInput(static_cast<UINT>(numInputs), inputs.get(), sizeof(INPUT));
}

const inline UINT SendKeyCombination(std::vector<WORD> keys) {
    size_t numInputs = keys.size() * 2;
    auto inputs = std::make_unique<INPUT[]>(numInputs);

    for (size_t i = 0; i < keys.size(); i++) {
        SetKey(keys[i], inputs[i], inputs[numInputs - i - 1]);
    }

    return SendInput(static_cast<UINT>(numInputs), inputs.get(), sizeof(INPUT));
}

const inline bool GetCapsLockState() {
    return (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
}

// Return true if the state was changed, false otherwise
const inline bool SetCapsLockState(bool on) {
    if (GetCapsLockState() != on) {
        SendKeySequence({VK_CAPITAL});
        return true;
    }
    return false;
}

const inline bool GetScrollLockState() {
    return (GetKeyState(VK_SCROLL) & 0x0001) != 0;
}

// Return true if the state was changed, false otherwise
const inline bool SetScrollLockState(bool on) {
    if (GetScrollLockState() != on) {
        SendKeySequence({VK_SCROLL});
        return true;
    }
    return false;
}

const inline bool IsPackageInstalled(const std::wstring& packageFamilyName) {
    const std::wstring basePath = L"Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\AppModel\\SystemAppData\\";

    HKEY hKey;
    LONG result = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        (basePath + packageFamilyName).c_str(),
        0,
        KEY_READ,
        &hKey
    );

    if (result == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return true;
    }
    return false;
}

const inline LCID HklToLcid(HKL hkl) {
    return MAKELCID(LOWORD(reinterpret_cast<UINT_PTR>(hkl)), SORT_DEFAULT);
}

// UWP workaround
// UWP are wrapped inside a universal class where setting/getting language will always fail
const inline HWND GetCoreWindow(HWND hostWindow) {
    constexpr static unsigned int WINDOW_CLASS_NAME_ARRAY_SIZE = 256;
    static wchar_t windowClassName[WINDOW_CLASS_NAME_ARRAY_SIZE];

    GetClassNameW(hostWindow, windowClassName, WINDOW_CLASS_NAME_ARRAY_SIZE);
    if (wcscmp(windowClassName, L"ApplicationFrameWindow") == 0) {
        HWND candidateWindow = nullptr;
        while ((candidateWindow = FindWindowExW(hostWindow, candidateWindow, nullptr, nullptr)) != nullptr) {
            GetClassNameW(candidateWindow, windowClassName, WINDOW_CLASS_NAME_ARRAY_SIZE);
            if (wcscmp(windowClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                return candidateWindow;
            }
        }
    }

    return hostWindow;
}
}
