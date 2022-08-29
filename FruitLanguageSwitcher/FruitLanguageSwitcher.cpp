#include <iostream>
#include <windows.h>
#include "LanguageSwitcher.h"
#include "Locale.h"

#define HOTKEY_SWAP_CATEGORY_MODIFIER (MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN | MOD_NOREPEAT)
#define HOTKEY_SWAP_CATEGORY_KEY      ('A')

#define HOTKEY_NEXT_LANG_MODIFIER     (HOTKEY_SWAP_CATEGORY_MODIFIER)
#define HOTKEY_NEXT_LANG_KEY          ('B')

#define HOTKEY_LAST_LANG_MODIFIER     (HOTKEY_SWAP_CATEGORY_MODIFIER)
#define HOTKEY_LAST_LANG_KEY          ('C')

#define HKL_TO_LCID(hkl)              (long(hkl) & 0xffff)

using namespace std;

enum HotkeyMessageId { SwapCategory, NextLanguage, LastLanguage };

bool registerHotkeys() {
    bool rc = true;

    if (!(rc && RegisterHotKey(NULL, SwapCategory, HOTKEY_SWAP_CATEGORY_MODIFIER, HOTKEY_SWAP_CATEGORY_KEY))) {
        cout << "Failed registering hotkey for swapping category " << endl;
    }

    else if (!(rc && RegisterHotKey(NULL, NextLanguage, HOTKEY_NEXT_LANG_MODIFIER, HOTKEY_NEXT_LANG_KEY))) {
        cout << "Failed registering hotkey for next language" << endl;
    }

    else if (!(rc && RegisterHotKey(NULL, LastLanguage, HOTKEY_LAST_LANG_MODIFIER, HOTKEY_LAST_LANG_KEY))) {
        cout << "Failed registering hotkey for last language" << endl;
    }

    return rc;
}

static LanguageSwitcher switcher;

void CALLBACK WinEventProcCallback(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    LCID lcid = HKL_TO_LCID(GetKeyboardLayout(GetWindowThreadProcessId(hwnd, nullptr)));
}

int main()
{
    auto hEvent = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, WinEventProcCallback, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    if (!registerHotkeys()) {
        exit(-1);
    }

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {
        if (msg.message == WM_HOTKEY)
        {
            if (msg.wParam == SwapCategory) {
                switcher.swapCategory();
                cout << "SwapCategory" << endl;
            }
            else if (msg.wParam == NextLanguage) {
                switcher.nextLanguage();
                cout << "nextLanguage" << endl;
            }
            else if (msg.wParam == LastLanguage) {
                switcher.lastLanguage();
                cout << "lastLanguage" << endl;
            }
            Sleep(100); // to avoid flooding
        }
    }
}
