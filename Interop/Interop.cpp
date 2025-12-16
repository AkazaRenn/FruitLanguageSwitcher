#pragma once

#include <Windows.h>

#include "Common.cpp"
#include "GetMessageThread.cpp"
#include "Main.cpp"

namespace Interop {
public ref class Core sealed {
private:
    delegate bool ProcessMessageDelegate(const MSG& msg);
    ProcessMessageDelegate^ processMessageDelegate = gcnew ProcessMessageDelegate(this, &Core::processMessage);
    System::IntPtr pProcessMessageDelegate = System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(processMessageDelegate);

    System::Collections::Generic::List<System::IntPtr>^ allocatedPointers = gcnew System::Collections::Generic::List<System::IntPtr>();

    bool processMessage(const MSG& msg) {
        switch (msg.message) {
        case ::Core::VAL(::Core::Message::ForegroundChanged):
            OnEvent();
            return true;
        default:
            return false;
        }
    }

public:
    delegate void EventHandler();
    event EventHandler^ OnEvent;

    Core() {
        allocatedPointers->Add(System::IntPtr(
            new ::Core::GetMessageThread(
                static_cast<::Core::GetMessageThread::ProcessMessageFunctionPtr>(pProcessMessageDelegate.ToPointer()),
                {::Core::Message::ForegroundChanged})));
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
