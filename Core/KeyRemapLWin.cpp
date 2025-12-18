#pragma once

import <Windows.h>;
import "Common.cpp";
import "Singleton.cpp";

namespace Core {
class KeyRemapLWin: public Singleton<KeyRemapLWin> {
private:
    bool lWinKeyDown = false;
    bool otherKeyDown = false;

    static void SendCommandPaletteShortcut() {
        SendKeyCombination({VK_LWIN,VK_MENU,VK_SPACE});
    }

public:
    void OnLWinDown() {
        if (lWinKeyDown) {
            return;
        }
        otherKeyDown = false;
        lWinKeyDown = true;
    }

    bool OnLWinUp() {
        lWinKeyDown = false;

        if (!otherKeyDown) {
            SendCommandPaletteShortcut();
            return true;
        }

        return false;
    }

    void OnOtherKeyDown() {
        if (otherKeyDown) {
            return;
        }
        otherKeyDown = true;
    }
};
}
