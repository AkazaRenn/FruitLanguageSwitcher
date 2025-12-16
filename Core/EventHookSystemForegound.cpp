#pragma once

#include "Common.cpp"
#include "GetMessageThreadManager.cpp"
#include "WinEventHook.cpp"

namespace Core {
class EventHookSystemForegound: public WinEventHook<EventHookSystemForegound> {
public:
	const static DWORD Event = EVENT_SYSTEM_FOREGROUND;

	static void CALLBACK OnWinEvent(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
		if (hwnd == GetForegroundWindow()) {
			GetMessageThreadManager::Instance().PostMessage(Message::ForegroundChanged);
		}
	}
};
}
