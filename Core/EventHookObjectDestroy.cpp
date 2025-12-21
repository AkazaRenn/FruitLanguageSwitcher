import "MessageDispatcher.cpp";
import "WinEventHook.cpp";

namespace Core {
class EventHookObjectDestroy: public WinEventHook<EventHookObjectDestroy> {
    friend class WinEventHook<EventHookObjectDestroy>;

private:
	const static DWORD Event = EVENT_OBJECT_DESTROY;

private:
	void CALLBACK OnEvent(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) override {
		if (dwEvent == EVENT_OBJECT_DESTROY && idObject == OBJID_WINDOW) {
			MessageDispatcher::Instance().PostMessage(Message::WindowDestroyed, 0, reinterpret_cast<LPARAM>(hwnd));
		}
	}
};
}
