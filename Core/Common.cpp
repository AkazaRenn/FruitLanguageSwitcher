#pragma once

import <memory>;
import <vector>;
import <Windows.h>;

namespace Core {
inline void SetKey(WORD key, std::unique_ptr<INPUT[]>& inputs, size_t downIndex, size_t upIndex) {
    DWORD flags = 0;
    if (key == VK_RMENU || key == VK_RCONTROL || key == VK_RSHIFT) {
        flags = KEYEVENTF_EXTENDEDKEY;
    }
    
    inputs[downIndex] = INPUT {
        .type = INPUT_KEYBOARD,
        .ki = {
            .wVk = key,
            .dwFlags = flags,
        },
    };
    inputs[upIndex] = INPUT {
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
        SetKey(keys[i], inputs, i * 2, i * 2 + 1);
    }

    return SendInput(static_cast<UINT>(numInputs), inputs.get(), sizeof(INPUT));
}

inline UINT SendKeyCombination(std::vector<WORD> keys) {
    size_t numInputs = keys.size() * 2;
    auto inputs = std::make_unique<INPUT[]>(numInputs);

    for (size_t i = 0; i < keys.size(); i++) {
        SetKey(keys[i], inputs, i, numInputs - i - 1);
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
}
