#pragma once

import "GetMessageThreadManager.cpp";
import "WinEventHook.cpp";

namespace Core {
class EventHookSystemForegound: public WinEventHook<EventHookSystemForegound> {
public:
	const static DWORD Event = EVENT_SYSTEM_FOREGROUND;

	void CALLBACK OnEvent(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) override {
		if (hwnd == GetForegroundWindow()) {
			GetMessageThreadManager::Instance().PostMessage(Message::ForegroundChanged);
		}
	}
};
}
