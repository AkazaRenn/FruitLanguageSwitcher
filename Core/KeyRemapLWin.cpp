import <Windows.h>;
import "Singleton.cpp";
import "Utilities.cpp";

namespace Core {
class KeyRemapLWin: public Singleton<KeyRemapLWin> {
private:
    static void SendCommandPaletteShortcut() {
        SendKeyCombination({VK_LWIN,VK_MENU,VK_SPACE});
    }

private:
    bool lWinKeyDown = false;
    bool otherKeyDown = false;
    const bool commandPaletteInstalled = IsPackageInstalled(L"Microsoft.CommandPalette_8wekyb3d8bbwe");

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

        if (otherKeyDown || (!commandPaletteInstalled)) {
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
