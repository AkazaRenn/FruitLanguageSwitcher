#pragma once

#include <Windows.h>
#include <unordered_set>

namespace LanguageSwitcher {
    class Core {
    public:
        static void Start() {
            if (Instance().started) {
                return;
			}
            Instance().started = true;
            Instance().eventHooks.insert(SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, OnEventSystemForegorund, 0, 0, WINEVENT_OUTOFCONTEXT));
		}

        const char* GetMessage() {
            return "Hello from LanguageSwitcher Core!";
        }

        static void RegisterReceiverThread(DWORD threadId) {
            receiverThreadIds.insert(threadId);
		}

        static bool UnregisterReceiverThread(DWORD threadId) {
            return receiverThreadIds.erase(threadId) > 0;
        }

    private:
		bool started = false;
        std::unordered_set<HWINEVENTHOOK> eventHooks;
        inline static std::unordered_set<DWORD> receiverThreadIds = {};

        Core() = default;
        Core(const Core&) = delete;
        Core& operator=(const Core&) = delete;

        ~Core() {
            for (auto hook : eventHooks) {
                UnhookWinEvent(hook);
            }
		}

        static Core& Instance() {
            static Core instance;
            return instance;
        }

        static void CALLBACK OnEventSystemForegorund(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
            if (hwnd == GetForegroundWindow()) {
                for (auto threadId : receiverThreadIds) {
                    PostThreadMessage(threadId, WM_USER + 1, 0, 0);
				}
            }
        }
    };
}