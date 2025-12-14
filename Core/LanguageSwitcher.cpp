#pragma once

namespace LanguageSwitcher {
    class Core {
    public:
        static Core& Instance() {
            static Core instance;
            return instance;
        }
        const char* GetMessage() {
            return "Hello from LanguageSwitcher Core!";
        }

    private:
        Core() = default;
        Core(const Core&) = delete;
        Core& operator=(const Core&) = delete;
    };
}