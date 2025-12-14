#pragma once

#include <Windows.h>
#include <unordered_set>

#include "EventHookSystemForegound.cpp"

namespace Core {
    class Main {
    public:
        static void Start() {
            Core::EventHookSystemForegound::Start();
		}

        static void RegisterReceiverThread(DWORD threadId) {
			Core::EventHookSystemForegound::RegisterReceiverThread(threadId);
		}

        static bool UnregisterReceiverThread(DWORD threadId) {
			return Core::EventHookSystemForegound::UnregisterReceiverThread(threadId);
        }

    private:
        Main() = default;
        Main(const Main&) = delete;
        Main& operator=(const Main&) = delete;

        ~Main() = default;

        static Main& Instance() {
            static Main instance;
            return instance;
        }
    };
}