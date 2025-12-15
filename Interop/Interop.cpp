#pragma once

#include <Windows.h>
#include "Main.cpp"

using namespace System;
using namespace System::Threading;
using namespace Core;

namespace Interop {
    public ref class Core sealed
    {
    public:
        static property Core^ Instance
        {
            Core^ get() { return instance; }
        }

        delegate void EventHandler();
		event EventHandler^ OnEvent;

    private:
        static initonly Core^ instance = gcnew Core();
        Thread^ loopThread = gcnew Thread(gcnew ThreadStart(this, &Core::RunLoop));
        DWORD threadId;

        Core()
        {
            loopThread->IsBackground = true;
            loopThread->Start();
        }

        ~Core() {
            Main::UnregisterReceiverThread(threadId);
            ::PostThreadMessage(threadId, WM_QUIT, 0, 0);
            loopThread->Join();
        }

        void RunLoop()
        {
            threadId = ::GetCurrentThreadId();
			Main::RegisterReceiverThread(threadId);
            Main::Start();

            MSG msg;
            while (::GetMessage(&msg, nullptr, 0, 0))
            {
                switch (msg.message)
                {
                case WM_USER + 1:
                    OnEvent();
                    break;
                case WM_QUIT:
					return;
                default:
                    ::TranslateMessage(&msg);
                    ::DispatchMessage(&msg);
                    break;
                }
            }
        }

    };
}
