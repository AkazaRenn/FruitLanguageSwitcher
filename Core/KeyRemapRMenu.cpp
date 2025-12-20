#pragma once

import <Windows.h>;
import "LanguageManager.cpp";
import "Singleton.cpp";
import "Utilities.cpp";

namespace Core {
class KeyRemapRMenu: public Singleton<KeyRemapRMenu> {
private:
    static constexpr DWORD VK_DUMMY = 0xff;

    bool rMenuKeyDown = false;
    bool otherKeyDown = false;

    static void SendIgnoreRMenuKeyCombination() {
        SendKeyCombination({VK_RMENU,VK_DUMMY});
    }

public:
    constexpr void OnRMenuDown() {
        if (rMenuKeyDown) {
            return;
        }

        otherKeyDown = false;
        rMenuKeyDown = true;
        return;
    }

    bool OnRMenuUp() {
        rMenuKeyDown = false;

        if (otherKeyDown) {
            return false;
        }

        if (LanguageManager::Instance().InImeLanguage()) {
            SendIgnoreRMenuKeyCombination();
            LanguageManager::Instance().OnRMenuUp();
            return true;
        }
        return false;
    }

    constexpr void OnOtherKeyDown() {
        if (otherKeyDown) {
            return;
        }

        otherKeyDown = true;
    }
};
}
