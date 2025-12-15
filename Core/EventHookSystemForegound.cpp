#pragma once

#include "WinEventHook.cpp"

namespace Core {
class EventHookSystemForegound: public WinEventHook<EventHookSystemForegound> {
public:
	const static DWORD Event = EVENT_SYSTEM_FOREGROUND;

	static void CALLBACK OnWinEvent(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
		if (hwnd == GetForegroundWindow()) {
			std::lock_guard<std::mutex> lock(Instance().receiverThreadIdsMutex);
			for (auto threadId : Instance().receiverThreadIds) {
				PostThreadMessage(threadId, WM_USER + 1, 0, 0);
			}
		}
	}
};
}