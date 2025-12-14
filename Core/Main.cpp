#pragma once

#include <Windows.h>
#include <unordered_set>

#include "EventHookSystemForegound.cpp"
#include "Singleton.cpp"

namespace Core {
    class Main : public Singleton<Main> {
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
    };
}