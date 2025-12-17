#pragma once

#include <Windows.h>
#include "Interop.hpp"

namespace Interop {
public ref class Core sealed {
private:
    delegate void ProcessMessageDelegate(const MSG& msg);
    ProcessMessageDelegate^ processMessageDelegate = gcnew ProcessMessageDelegate(this, &Core::ProcessMessage);

    System::IntPtr pUnmanaged;

    ::Core::ProcessMessageFunctionPtr ToProcessMessageFunctionPtr(ProcessMessageDelegate^ processMessageDelegate) {
        return static_cast<::Core::ProcessMessageFunctionPtr>(
            System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(processMessageDelegate).ToPointer());
    }

    void ProcessMessage(const MSG& msg) {
        OnEvent();
    }

public:
    delegate void EventHandler();
    event EventHandler^ OnEvent;

    Core() {
        pUnmanaged = System::IntPtr(new ::Core::Interop({
            {::Core::Message::ForegroundChanged, ToProcessMessageFunctionPtr(processMessageDelegate)},
            }));
    }

    ~Core() {
        free(pUnmanaged.ToPointer());
    }
};
}
