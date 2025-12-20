#pragma once

import "GetMessageThreadManager.cpp";
import "WinEventHook.cpp";

namespace Core {
class EventHookObjectDestroy: public WinEventHook<EventHookObjectDestroy> {
public:
	const static DWORD Event = EVENT_OBJECT_DESTROY;

public:
	void CALLBACK OnEvent(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) override {
		if (dwEvent == EVENT_OBJECT_DESTROY && idObject == OBJID_WINDOW) {
			GetMessageThreadManager::Instance().PostMessage(Message::WindowDestroyed, 0, reinterpret_cast<LPARAM>(hwnd));
		}
	}
};
}
