#pragma once

import <memory>;
import <unordered_map>;
import <Windows.h>;
import "GetMessageThread.cpp";
import "Language.cpp";
import "Message.cpp";
import "Singleton.cpp";

namespace Core {
class LanguageManager: public Singleton<LanguageManager> {
private:
    static const HKL GetCurrentWindowKeyboardLayout(HWND hwnd) {
        DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
        return GetKeyboardLayout(threadId);
    }

    static const std::vector<HKL> GetHklList() {
        int count = GetKeyboardLayoutList(0, nullptr);
        std::vector<HKL> layouts(count);
        GetKeyboardLayoutList(count, layouts.data());
        return layouts;
    }

    static const std::unordered_map<HKL, Language> GetHklToLanguageMap() {
        auto layouts = GetHklList();

        std::unordered_map<HKL, Language> map;
        for (const HKL& hkl : layouts) {
            map.emplace(hkl, Language(hkl));
        }
        return map;
    }

    static Language& GetFirstLanguage(std::unordered_map<HKL, Language>& hklToLanguageMap, bool imeLanguage) {
        for (const HKL layout : GetHklList()) {
            auto it = hklToLanguageMap.find(layout);
            if ((it != hklToLanguageMap.end()) && (it->second.isImeLanguage == imeLanguage)) {
                return it->second;
            }
        }
        throw std::runtime_error("No language found");
    }

    static void OnForegroundChanged(const MSG& msg) {
        SetCapsLockState(false);
        const HWND hwnd = reinterpret_cast<HWND>(msg.lParam);
        auto& instance = Instance();

        // If HWND in windowToLanguageMap, use saved language,
        // else update windowToLanguageMap.
        // Always activate the language.
        auto it = instance.windowToLanguageMap.find(hwnd);
        if (it != instance.windowToLanguageMap.end()) {
            instance.ActivateLanguage(it->second, hwnd);
        } else {
            instance.ActivateLanguage(GetCurrentWindowKeyboardLayout(hwnd), hwnd);
        }
    }

    static void OnSwapCategoryTriggered(const MSG& msg) {
        SetCapsLockState(false);
        const HWND hwnd = GetForegroundWindow();
        auto& instance = Instance();

        if (instance.activeLanguage.get().isImeLanguage) {
            instance.ActivateLanguage(instance.activeLatinLanguage, hwnd);
        } else {
            instance.ActivateLanguage(instance.activeImeLanguage, hwnd);
        }
    }

    static void OnWinKeyUp(const MSG& msg) {
        Sleep(300); // Wait for IME value to update
        auto& instance = Instance();
        const HWND hwnd = GetForegroundWindow();
        HKL newHkl = GetCurrentWindowKeyboardLayout(hwnd);
        HKL oldHkl = instance.activeLanguage.get().hkl;

        instance.ActivateLanguage(newHkl, hwnd);
        if (newHkl != oldHkl) {
            // Turn off capslock if the language changes
            SetCapsLockState(false);
        }
    }

    static void OnCapsLockOn(const MSG& msg) {
        auto& instance = Instance();
        instance.activeLanguageBeforeCapsLock = instance.activeLanguage;
        if (instance.activeLanguage.get().isImeLanguage) {
            const HWND hwnd = GetForegroundWindow();
            instance.ActivateLanguage(instance.activeLatinLanguage, hwnd, false);
        }
    }

    static void OnCapsLockOff(const MSG& msg) {
        auto& instance = Instance();
        if (instance.activeLanguageBeforeCapsLock.get().isImeLanguage) {
            const HWND hwnd = GetForegroundWindow();
            instance.ActivateLanguage(instance.activeLanguageBeforeCapsLock, hwnd);
        }
    }

    static void OnWindowDestroyed(const MSG& msg) {
        auto& instance = Instance();
        const HWND hwnd = reinterpret_cast<HWND>(msg.lParam);
        instance.windowToLanguageMap.erase(hwnd);
    }

private:
    std::unordered_map<HKL, Language> hklToLanguageMap = GetHklToLanguageMap();
    std::reference_wrapper<Language> activeLanguage = hklToLanguageMap.at(GetHklList()[0]);
    std::reference_wrapper<Language> activeLatinLanguage = GetFirstLanguage(hklToLanguageMap, false);
    std::reference_wrapper<Language> activeImeLanguage = GetFirstLanguage(hklToLanguageMap, true);
    std::unordered_map<HWND, std::reference_wrapper<Language>> windowToLanguageMap;
    std::reference_wrapper<Language> activeLanguageBeforeCapsLock = activeLanguage;

    GetMessageThread getMessageThread = GetMessageThread({
        { Message::ForegroundChanged, &OnForegroundChanged },
        { Message::WindowDestroyed, &OnWindowDestroyed },
        { Message::SwapCategoryTriggered, &OnSwapCategoryTriggered },
        { Message::WinKeyUp, &OnWinKeyUp },
        { Message::CapsLockOn, &OnCapsLockOn },
        { Message::CapsLockOff, &OnCapsLockOff },
    });

    void ActivateLanguage(HKL hkl, HWND hwnd) {
        if (!hklToLanguageMap.contains(hkl)) {
            hklToLanguageMap.emplace(hkl, Language(hkl));
        }
        ActivateLanguage(hklToLanguageMap.at(hkl), hwnd);
    }

    void ActivateLanguage(Language& language, HWND hwnd, bool updateWindowToLanguageMap = true) {
        language.Activate(hwnd, activeLanguage);

        activeLanguage = language;
        if (activeLanguage.get().isImeLanguage) {
            activeImeLanguage = activeLanguage;
        } else {
            activeLatinLanguage = activeLanguage;
        }
        SetScrollLockState(activeLanguage.get().isImeLanguage);

        if (updateWindowToLanguageMap) {
            windowToLanguageMap.insert_or_assign(hwnd, activeLanguage);
        }
    }
};
}