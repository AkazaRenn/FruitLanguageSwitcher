import <algorithm>;
import <array>;
import <memory>;
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

// From https://github.com/microsoft/PowerToys/blob/main/src/common/utils/game_mode.h
inline bool IsRunningD3DFullscreen() {
    QUERY_USER_NOTIFICATION_STATE state;
    if (SHQueryUserNotificationState(&state) != S_OK) {
        return false; // unable to determine -> treat as not fullscreen
    }
    return (state == QUNS_RUNNING_D3D_FULL_SCREEN);
}
}
