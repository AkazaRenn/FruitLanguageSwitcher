#pragma once

import <map>;
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
    static const HKL GetCurrentWindowKeyboardLayout() {
        HWND hwnd = GetForegroundWindow();
        DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
        return GetKeyboardLayout(threadId);
    }

    static void OnForegroundChanged(const MSG& msg) {
        const HWND hwnd = GetForegroundWindow();
        auto& windowToLanguageMap = Instance().windowToLanguageMap;
        auto& hklToLanguageMap = Instance().hklToLanguageMap;
        HKL hkl = GetCurrentWindowKeyboardLayout();

        // If HWND in windowToLanguageMap, use saved language,
        // else update windowToLanguageMap.
        // Always activate the language.
        auto it = windowToLanguageMap.find(hwnd);
        if (it != windowToLanguageMap.end()) {
            Instance().activeLanguage = it->second.get();
        } else {
            auto hklToLanguageMapIt = hklToLanguageMap.find(hkl);
            if (hklToLanguageMapIt != hklToLanguageMap.end()) {
                Instance().activeLanguage = hklToLanguageMapIt->second;
            } else {
                hklToLanguageMap.emplace(hkl, Language(hkl));
                Instance().activeLanguage = hklToLanguageMap.at(hkl);
            }
            windowToLanguageMap.emplace(hwnd, Instance().activeLanguage);
        }

        Instance().activeLanguage.get().Activate(hwnd, hkl);
        if (Instance().activeLanguage.get().isImeLanguage) {
            Instance().activeImeLanguage = Instance().activeLanguage;
        } else {
            Instance().activeLatinLanguage = Instance().activeLanguage;
        }
    }

    static const std::vector<HKL> GetHklList() {
        int count = GetKeyboardLayoutList(0, nullptr);
        std::vector<HKL> layouts(count);
        GetKeyboardLayoutList(count, layouts.data());
        return layouts;
    }

    static const std::map<HKL, Language> GetHklToLanguageMap() {
        auto layouts = GetHklList();

        std::map<HKL, Language> map;
        for (const HKL& hkl : layouts) {
            map.emplace(hkl, Language(hkl));
        }
        return map;
    }

    static Language& GetFirstLanguage(std::map<HKL, Language>& hklToLanguageMap, bool imeLanguage) {
        for (const HKL layout : GetHklList()) {
            auto it = hklToLanguageMap.find(layout);
            if ((it != hklToLanguageMap.end()) && (it->second.isImeLanguage == imeLanguage)) {
                return it->second;
            }
        }
        throw std::runtime_error("No language found");
    }

private:
    std::map<HKL, Language> hklToLanguageMap = GetHklToLanguageMap();
    std::reference_wrapper<Language> activeLanguage = hklToLanguageMap.at(GetHklList()[0]);
    std::reference_wrapper<Language> activeLatinLanguage = GetFirstLanguage(hklToLanguageMap, false);
    std::reference_wrapper<Language> activeImeLanguage = GetFirstLanguage(hklToLanguageMap, true);
    std::unordered_map<HWND, std::reference_wrapper<Language>> windowToLanguageMap;

    GetMessageThread getMessageThread = GetMessageThread({
        { Message::ForegroundChanged, &OnForegroundChanged },
    });
};
}