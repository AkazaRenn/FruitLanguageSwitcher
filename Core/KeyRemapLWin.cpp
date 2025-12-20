#pragma once

import <Windows.h>;
import "Singleton.cpp";
import "Utilities.cpp";

namespace Core {
class KeyRemapLWin: public Singleton<KeyRemapLWin> {
private:
    bool lWinKeyDown = false;
    bool otherKeyDown = false;

    static void SendCommandPaletteShortcut() {
        SendKeyCombination({VK_LWIN,VK_MENU,VK_SPACE});
    }

public:
    constexpr void OnLWinDown() {
        if (lWinKeyDown) {
            return;
        }

        otherKeyDown = false;
        lWinKeyDown = true;
    }

    bool OnLWinUp() {
        lWinKeyDown = false;

        if (otherKeyDown) {
            return false;
        }

        SendCommandPaletteShortcut();
        return true;
    }

    constexpr void OnOtherKeyDown() {
        if (otherKeyDown) {
            return;
        }

        otherKeyDown = true;
    }
};
}
