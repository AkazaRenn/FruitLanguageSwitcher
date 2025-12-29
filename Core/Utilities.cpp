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

inline void SetKey(WORD key, INPUT& downInput, INPUT& upInput) {
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

inline UINT SendKeySequence(std::vector<WORD> keys) {
    size_t numInputs = keys.size() * 2;
    auto inputs = std::make_unique<INPUT[]>(numInputs);

    for (size_t i = 0; i < keys.size(); i++) {
        SetKey(keys[i], inputs[i * 2], inputs[i * 2 + 1]);
    }

    return SendInput(static_cast<UINT>(numInputs), inputs.get(), sizeof(INPUT));
}

inline UINT SendKeyCombination(std::vector<WORD> keys) {
    size_t numInputs = keys.size() * 2;
    auto inputs = std::make_unique<INPUT[]>(numInputs);

    for (size_t i = 0; i < keys.size(); i++) {
        SetKey(keys[i], inputs[i], inputs[numInputs - i - 1]);
    }

    return SendInput(static_cast<UINT>(numInputs), inputs.get(), sizeof(INPUT));
}

inline bool GetCapsLockState() {
    return (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
}

// Return true if the state was changed, false otherwise
inline bool SetCapsLockState(bool on) {
    if (GetCapsLockState() != on) {
        SendKeySequence({VK_CAPITAL});
        return true;
    }
    return false;
}

inline bool GetScrollLockState() {
    return (GetKeyState(VK_SCROLL) & 0x0001) != 0;
}

// Return true if the state was changed, false otherwise
inline bool SetScrollLockState(bool on) {
    if (GetScrollLockState() != on) {
        SendKeySequence({VK_SCROLL});
        return true;
    }
    return false;
}

inline bool IsPackageInstalled(const std::wstring& packageFamilyName) {
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
}
