import "MessageDispatcher.cpp";
import "WinEventHook.cpp";

namespace Core {
class EventHookSystemForegound: public WinEventHook<EventHookSystemForegound> {
    friend class WinEventHook<EventHookSystemForegound>;

private:
	const static DWORD Event = EVENT_SYSTEM_FOREGROUND;

private:
	void CALLBACK OnEvent(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) override {
		if (hwnd == GetForegroundWindow()) {
			MessageDispatcher::Instance().PostMessage(Message::ForegroundChanged, 0, reinterpret_cast<LPARAM>(hwnd));
		}
	}
};
}
