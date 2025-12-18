#pragma once

#include "Interop.hpp"

namespace Interop {
public ref class Core sealed {
private:
    using ProcessMessageFunctionPtr = void(__stdcall*)(const MSG&);
    delegate void ProcessMessageDelegate(const MSG& msg);
    ProcessMessageDelegate^ processMessageDelegate = gcnew ProcessMessageDelegate(this, &Core::ProcessMessage);

    System::IntPtr pUnmanaged = System::IntPtr(new ::Core::Interop({
            {::Core::Message::ForegroundChanged, ToProcessMessageFunctionPtr(processMessageDelegate)},
        }));;

    ProcessMessageFunctionPtr ToProcessMessageFunctionPtr(ProcessMessageDelegate^ processMessageDelegate) {
        return static_cast<ProcessMessageFunctionPtr>(
            System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(processMessageDelegate).ToPointer());
    }

    void ProcessMessage(const MSG& msg) {
        OnEvent();
    }

public:
    delegate void EventHandler();
    event EventHandler^ OnEvent;

    ~Core() {
        delete static_cast<::Core::Interop*>(pUnmanaged.ToPointer());
    }
};
}
