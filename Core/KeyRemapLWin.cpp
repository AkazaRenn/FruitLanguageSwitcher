#pragma once

import <Windows.h>;
import "Common.cpp";

namespace Core {
class KeyRemapLWin {
private:
    bool lWinKeyDown = false;
    bool otherKeyDown = false;

    static void SendCommandPaletteShortcut() {
        SendKeyCombination({VK_LWIN,VK_MENU,VK_SPACE});
    }

public:
    void OnLWinDown() {
        otherKeyDown = false;
        lWinKeyDown = true;
    }

    bool OnLWinUp() {
        if (!otherKeyDown) {
            SendCommandPaletteShortcut();
            lWinKeyDown = false;
            return true;
        }

        return false;
    }

    void OnOtherKeyDown() {
        otherKeyDown = true;
    }
};
}
