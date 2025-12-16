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
    System::IntPtr pProcessMessageDelegate = System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(processMessageDelegate);

    System::Collections::Generic::List<System::IntPtr>^ allocatedPointers = gcnew System::Collections::Generic::List<System::IntPtr>();

    void ProcessMessage(const MSG& msg) {
        OnEvent();
    }

    ::Core::GetMessageThread::ProcessMessageFunctionPtr ToProcessMessageFunctionPtr(System::IntPtr ptr) {
        return static_cast<::Core::GetMessageThread::ProcessMessageFunctionPtr>(ptr.ToPointer());
    }

public:
    delegate void EventHandler();
    event EventHandler^ OnEvent;

    Core() {
        allocatedPointers->Add(System::IntPtr(new ::Core::GetMessageThread({
            {::Core::Message::ForegroundChanged, ToProcessMessageFunctionPtr(pProcessMessageDelegate)},
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
