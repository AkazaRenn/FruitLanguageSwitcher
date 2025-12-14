#pragma once

#include "LanguageSwitcher.cpp"

using namespace System;

namespace Interop {
    public ref class Core
    {
    public:
        static String^ GetMessage()
        {
            return gcnew String(LanguageSwitcher::Core::Instance().GetMessage());
        }
    };
}
