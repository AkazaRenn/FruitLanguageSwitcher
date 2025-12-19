#pragma once

import <memory>;
import <vector>;
import <Windows.h>;

namespace Core {
inline UINT SendKeySequence(std::vector<WORD> keys) {
    size_t numInputs = keys.size() * 2;
    auto inputs = std::make_unique<INPUT[]>(numInputs);

    for (size_t i = 0; i < keys.size(); i++) {
        inputs[i * 2].type = INPUT_KEYBOARD;
        inputs[i * 2].ki.wVk = keys[i];
        inputs[i * 2 + 1].type = INPUT_KEYBOARD;
        inputs[i * 2 + 1].ki.wVk = keys[i];
        inputs[i * 2 + 1].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    return SendInput(static_cast<UINT>(numInputs), inputs.get(), sizeof(INPUT));
}

inline UINT SendKeyCombination(std::vector<WORD> keys) {
    size_t numInputs = keys.size() * 2;
    auto inputs = std::make_unique<INPUT[]>(numInputs);

    for (size_t i = 0; i < keys.size(); i++) {
        inputs[i].type = INPUT_KEYBOARD;
        inputs[i].ki.wVk = keys[i];
        inputs[numInputs - 1 - i].type = INPUT_KEYBOARD;
        inputs[numInputs - 1 - i].ki.wVk = keys[i];
        inputs[numInputs - 1 - i].ki.dwFlags = KEYEVENTF_KEYUP;
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
