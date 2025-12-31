import <memory>;
import <unordered_map>;
import <Windows.h>;
import "Enumerations.cpp";
import "GetMessageThread.cpp";
import "Language.cpp";
import "Singleton.cpp";
import "Task.cpp";
import "Timer.cpp";
import "Utilities.cpp";

namespace Core {
class LanguageManager {
private:
    static const HKL GetWindowKeyboardLayout(HWND hwnd) {
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

private:
    std::unordered_map<HWND, std::reference_wrapper<Language>> windowToLanguageMap;
    std::unordered_map<HKL, Language> hklToLanguageMap = GetHklToLanguageMap();
    std::reference_wrapper<Language> activeLanguage = hklToLanguageMap.at(GetHklList()[0]);
    std::reference_wrapper<Language> activeLatinLanguage = GetFirstLanguage(hklToLanguageMap, false);
    std::reference_wrapper<Language> activeImeLanguage = GetFirstLanguage(hklToLanguageMap, true);
    std::reference_wrapper<Language> activeLanguageBeforeCapsLock = activeLanguage;
    std::reference_wrapper<Language> defaultLanguage = activeLanguage;

    HWND activeWindow = GetForegroundWindow();

    bool pollingLanguageUpdate = false;
    Task updateLanguageTask;

    GetMessageThread getMessageThread = GetMessageThread({
        { Message::ForegroundChanged, [this](const MSG& msg) {OnForegroundChanged(msg); } },
        { Message::WindowDestroyed, [this](const MSG& msg) {OnWindowDestroyed(msg); } },
        { Message::SwapCategoryTriggered, [this](const MSG& msg) {OnSwapCategoryTriggered(msg); } },
        { Message::WinKeyUp, [this](const MSG& msg) {OnWinKeyUp(msg); } },
        { Message::RMenuKeyUp, [this](const MSG& msg) {OnRMenuKeyUp(msg); } },
        { Message::CapsLockOn, [this](const MSG& msg) {OnCapsLockOn(msg); } },
        { Message::CapsLockOff,[this](const MSG& msg) {OnCapsLockOff(msg); } },
    });

    void OnForegroundChanged(const MSG& msg) {
        SetCapsLockState(false);
        activeWindow = reinterpret_cast<HWND>(msg.lParam);

        // If HWND in windowToLanguageMap, use saved language,
        // else update windowToLanguageMap.
        // Always activate the language.
        auto it = windowToLanguageMap.find(activeWindow);
        if (it != windowToLanguageMap.end()) {
            ActivateLanguage(it->second);
        } else {
            ActivateLanguage(defaultLanguage);
        }
    }

    void OnWindowDestroyed(const MSG& msg) {
        const HWND hwnd = reinterpret_cast<HWND>(msg.lParam);
        windowToLanguageMap.erase(hwnd);
    }

    void OnSwapCategoryTriggered(const MSG& msg) {
        ShowFlyout(GetActiveLanguage(!activeLanguage.get().isImeLanguage).lcid);

        if (activeLanguage.get().isImeLanguage) {
            ActivateLanguage(activeLatinLanguage);
        } else {
            ActivateLanguage(activeImeLanguage);
        }
    }

    void OnWinKeyUp(const MSG& msg) {
        pollingLanguageUpdate = true;
        updateLanguageTask.Start();
    }

    void OnRMenuKeyUp(const MSG& msg) {
        if (activeLanguage.get().isImeLanguage) {
            static constexpr DWORD VK_DUMMY = 0xff;
            SendKeyCombination({VK_RMENU,VK_DUMMY});
            activeLanguage.get().OnRMenuUp();
        } else {
            SendKeyCombination({VK_RMENU});
        }
    }

    void OnCapsLockOn(const MSG& msg) {
        ShowFlyout(0);

        activeLanguageBeforeCapsLock = activeLanguage;
        if (activeLanguage.get().isImeLanguage) {
            ActivateLanguage(activeLatinLanguage, false);
        }
    }

    void OnCapsLockOff(const MSG& msg) {
        ShowFlyout(GetActiveLanguage(activeLanguageBeforeCapsLock.get().isImeLanguage).lcid);

        if (activeLanguageBeforeCapsLock.get().isImeLanguage) {
            ActivateLanguage(activeLanguageBeforeCapsLock);
        }
    }

    Language& GetLanguageForHkl(HKL hkl) {
        if (!hklToLanguageMap.contains(hkl)) {
            hklToLanguageMap.emplace(hkl, Language(hkl));
        }
        return hklToLanguageMap.at(hkl);
    }

    void ActivateLanguage(HKL hkl) {
        ActivateLanguage(GetLanguageForHkl(hkl));
    }

    void ActivateLanguage(Language& language, bool updateWindowToLanguageMap = true) {
        const HKL activeHkl = GetWindowKeyboardLayout(activeWindow);
        language.Activate(activeWindow, activeHkl == language.hkl);

        activeLanguage = language;
        if (activeLanguage.get().isImeLanguage) {
            activeImeLanguage = activeLanguage;
        } else {
            activeLatinLanguage = activeLanguage;
        }
        SetScrollLockState(activeLanguage.get().isImeLanguage);
        pollingLanguageUpdate = false;

        if (updateWindowToLanguageMap) {
            windowToLanguageMap.insert_or_assign(activeWindow, activeLanguage);
        }

    }

    void ShowFlyout(LCID activeLcid) const {
        MessageDispatcher::Instance().PostMessage(Message::ShowFlyout,
            static_cast<WPARAM>(activeLcid),
            static_cast<LPARAM>(activeImeLanguage.get().lcid));
    }

    void UpdateLanguage() {
        const HWND hwnd = GetForegroundWindow();
        if (hwnd != activeWindow) {
            return;
        }
        // Poll the HKL update as it would have a delay
        const HKL oldHkl = activeLanguage.get().hkl;
        HKL newHkl;
        int retry = 10;
        while (((newHkl = GetWindowKeyboardLayout(hwnd)) == oldHkl) && (retry-- > 0)) {
            Sleep(50);
            if (!pollingLanguageUpdate) {
                return;
            }
        }

        if (oldHkl == newHkl) {
            return;
        }

        Language& newLanguage = GetLanguageForHkl(newHkl);
        MessageDispatcher::Instance().PostMessage(Message::ShowFlyout,
            static_cast<WPARAM>(newLanguage.lcid),
            static_cast<LPARAM>(newLanguage.isImeLanguage ? newLanguage.lcid : activeImeLanguage.get().lcid));
        ActivateLanguage(newHkl);
        // Turn off capslock if the language changes
        SetCapsLockState(false);
    }

    constexpr Language& GetActiveLanguage(bool imeLanguage) const {
        return imeLanguage ? activeImeLanguage.get() : activeLatinLanguage.get();
    }

public:
    LanguageManager()
        : updateLanguageTask([this]() {
        UpdateLanguage();
    }) {}
};
}