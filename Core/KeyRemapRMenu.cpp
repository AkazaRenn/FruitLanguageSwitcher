#pragma once

import <Windows.h>;
import "Common.cpp";
import "Singleton.cpp";

namespace Core {
class KeyRemapRMenu: public Singleton<KeyRemapRMenu> {
private:
    bool rMenuKeyDown = false;
    bool otherKeyDown = false;

public:
    bool OnRMenuDown() {
        return false;

        if (rMenuKeyDown) {
            return true;
        }

        otherKeyDown = false;
        rMenuKeyDown = true;
    }

    bool OnRMenuUp() {
        return false;

        rMenuKeyDown = false;

        if (otherKeyDown) {
            return true;
        }

        //SendCommandPaletteShortcut();
        return true;
    }

    void OnOtherKeyDown() {
        if (otherKeyDown) {
            return;
        }

        otherKeyDown = true;
    }
};
}
