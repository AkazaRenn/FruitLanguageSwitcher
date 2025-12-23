#pragma once

#include <Windows.h>
#include "Interop.hpp"
#pragma comment(lib, "imm32.lib")

namespace Interop {
public ref class Core sealed {
private:
    using ProcessMessageFunctionPtr = void(__stdcall*)(const MSG&);
    delegate void ProcessMessageDelegate(const MSG& msg);
    System::Collections::Generic::HashSet<ProcessMessageDelegate^>^ delegates = gcnew System::Collections::Generic::HashSet<ProcessMessageDelegate^>();

    System::IntPtr pUnmanaged = System::IntPtr(new ::Core::Interop({
        {::Core::Message::ShowFlyout, ToProcessMessageFunctionPtr(gcnew ProcessMessageDelegate(this, &Core::OnShowFlyout))},
        {::Core::Message::CloseFlyout, ToProcessMessageFunctionPtr(gcnew ProcessMessageDelegate(this, &Core::OnCloseFlyout))},
    }));;;

    ProcessMessageFunctionPtr ToProcessMessageFunctionPtr(ProcessMessageDelegate^ processMessageDelegate) {
        delegates->Add(processMessageDelegate);
        return static_cast<ProcessMessageFunctionPtr>(
            System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(processMessageDelegate).ToPointer());
    }

    void OnShowFlyout(const MSG& msg) {
        if (static_cast<::Core::FlyoutState>(msg.wParam) == ::Core::FlyoutState::CapsLock) {
            ShowFlyoutCapsLockEvent();
        } else {
            ShowFlyoutLanguageEvent(
                static_cast<::Core::FlyoutState>(msg.wParam) == ::Core::FlyoutState::IME,
                static_cast<LCID>(msg.lParam)
            );
        }
    }

    void OnCloseFlyout(const MSG& msg) {
        CloseFlyoutEvent();
    }

public:
    delegate void ShowFlyoutLanguageEventHandler(bool inImeLanguage, LCID lcid);
    event ShowFlyoutLanguageEventHandler^ ShowFlyoutLanguageEvent;
    delegate void ShowFlyoutCapsLockEventHandler();
    event ShowFlyoutCapsLockEventHandler^ ShowFlyoutCapsLockEvent;
    delegate void CloseFlyoutEventHandler();
    event CloseFlyoutEventHandler^ CloseFlyoutEvent;

    ~Core() {
        delete static_cast<::Core::Interop*>(pUnmanaged.ToPointer());
    }
};
}
