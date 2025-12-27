#pragma once

#include <Windows.h>
#include "Interop.hpp"
#pragma comment(lib, "imm32.lib")

namespace Interop {
public ref class Core sealed {
private:
    delegate void ShowFlyoutDelegate(LCID activeLcid, LCID activeImeLcid);
    ShowFlyoutDelegate^ showFlyoutDelegate = gcnew ShowFlyoutDelegate(this, &Core::ShowFlyout);
    ::Core::Interop* pUnmanaged = new ::Core::Interop(ToShowFlyoutFunctionPtr(showFlyoutDelegate));

    ::Core::ShowFlyoutFunction ToShowFlyoutFunctionPtr(ShowFlyoutDelegate^ processMessageDelegate) {
        return static_cast<::Core::ShowFlyoutFunction>(
            System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(processMessageDelegate).ToPointer());
    }

    void ShowFlyout(LCID activeLcid, LCID activeImeLcid) {
        if (activeLcid) {
            ShowFlyoutLanguageEvent(activeLcid, activeImeLcid);
        } else {
            ShowFlyoutCapsLockEvent();
        }
    }

public:
    delegate void ShowFlyoutLanguageEventHandler(LCID activeLcid, LCID activeImeLcid);
    event ShowFlyoutLanguageEventHandler^ ShowFlyoutLanguageEvent;
    delegate void ShowFlyoutCapsLockEventHandler();
    event ShowFlyoutCapsLockEventHandler^ ShowFlyoutCapsLockEvent;

    ~Core() {
        this->!Core();
    }

    !Core() {
        delete pUnmanaged;
    }
};
}
