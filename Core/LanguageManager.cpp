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

    static void OnForegroundChanged(const MSG& msg) {
        const HWND hwnd = GetForegroundWindow();

        // If HWND in windowToLanguageMap, use saved language,
        // else update windowToLanguageMap.
        // Always activate the language.
        auto it = Instance().windowToLanguageMap.find(hwnd);
        if (it != Instance().windowToLanguageMap.end()) {
            Instance().ActivateLanguage(it->second, hwnd);
        } else {
            Instance().ActivateLanguage(GetCurrentWindowKeyboardLayout(hwnd), hwnd);
        }

    }

    static void OnSwapCategoryTriggered(const MSG& msg) {
        const HWND hwnd = GetForegroundWindow();

        if (Instance().activeLanguage.get().isImeLanguage) {
            Instance().ActivateLanguage(Instance().activeLatinLanguage, hwnd);
        } else {
            Instance().ActivateLanguage(Instance().activeImeLanguage, hwnd);
        }
    }

    static void OnWinKeyUp(const MSG& msg) {
        Sleep(300); // Wait for IME value to update
        const HWND hwnd = GetForegroundWindow();
        HKL hkl = GetCurrentWindowKeyboardLayout(hwnd);
        Instance().ActivateLanguage(hkl, hwnd);
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

private:
    std::unordered_map<HKL, Language> hklToLanguageMap = GetHklToLanguageMap();
    std::reference_wrapper<Language> activeLanguage = hklToLanguageMap.at(GetHklList()[0]);
    std::reference_wrapper<Language> activeLatinLanguage = GetFirstLanguage(hklToLanguageMap, false);
    std::reference_wrapper<Language> activeImeLanguage = GetFirstLanguage(hklToLanguageMap, true);
    std::unordered_map<HWND, std::reference_wrapper<Language>> windowToLanguageMap;

    GetMessageThread getMessageThread = GetMessageThread({
        { Message::ForegroundChanged, &OnForegroundChanged },
        { Message::SwapCategoryTriggered, &OnSwapCategoryTriggered },
        { Message::WinKeyUp, &OnWinKeyUp },
    });

    void ActivateLanguage(HKL hkl, HWND hwnd) {
        if (!hklToLanguageMap.contains(hkl)) {
            hklToLanguageMap.emplace(hkl, Language(hkl));
        }
        ActivateLanguage(hklToLanguageMap.at(hkl), hwnd);
    }

    void ActivateLanguage(Language& language, HWND hwnd) {
        language.Activate(hwnd, activeLanguage.get().hkl);

        activeLanguage = language;
        windowToLanguageMap.insert_or_assign(hwnd, activeLanguage);

        if (activeLanguage.get().isImeLanguage) {
            activeImeLanguage = activeLanguage;
        } else {
            activeLatinLanguage = activeLanguage;
        }
    }
};
}