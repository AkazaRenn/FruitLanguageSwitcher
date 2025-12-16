#pragma once

#include <Windows.h>

#include "Common.cpp"
#include "GetMessageThread.cpp"
#include "Main.cpp"

namespace Interop {
public ref class Core sealed {
private:
    delegate void ProcessMessageDelegate(const MSG& msg);
    ProcessMessageDelegate^ processMessageDelegate = gcnew ProcessMessageDelegate(this, &Core::ProcessMessage);

    System::Collections::Generic::List<System::IntPtr>^ allocatedPointers = gcnew System::Collections::Generic::List<System::IntPtr>();

    ::Core::GetMessageThread::ProcessMessageFunctionPtr ToProcessMessageFunctionPtr(System::Delegate^ del) {
        return static_cast<::Core::GetMessageThread::ProcessMessageFunctionPtr>(
            System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(del).ToPointer());
    }

    void ProcessMessage(const MSG& msg) {
        OnEvent();
    }

public:
    delegate void EventHandler();
    event EventHandler^ OnEvent;

    Core() {
        allocatedPointers->Add(System::IntPtr(new ::Core::GetMessageThread({
            {::Core::Message::ForegroundChanged, ToProcessMessageFunctionPtr(processMessageDelegate)},
        })));
        allocatedPointers->Add(System::IntPtr(new ::Core::Main())); // to be removed
    }

    ~Core() {
        for each(System::IntPtr ptr in allocatedPointers) {
            free(ptr.ToPointer());
        }
        allocatedPointers->Clear();
    }
};
}
