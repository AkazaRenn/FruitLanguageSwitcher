import "KeyRemapLWin.cpp";
import "WindowsHook.cpp";

namespace Core {
class WindowsHookMouseLL: public WindowsHook<WindowsHookMouseLL> {
    friend class WindowsHook<WindowsHookMouseLL>;

private:
    const static int IdHook = WH_MOUSE_LL;

private:
    KeyRemapLWin& keyRemapLWin = KeyRemapLWin::Instance();

    bool OnEvent(int nCode, WPARAM wParam, LPARAM lParam) override {
        if (nCode == HC_ACTION) {
            switch (wParam) {
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_XBUTTONDOWN:
                keyRemapLWin.OnOtherKeyDown();
            default:
                break;
            }
        }

        return false;
    }
};
}
