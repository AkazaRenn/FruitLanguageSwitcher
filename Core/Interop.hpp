#pragma once
// Kept because C++/CLI doesn't support modules

#include <Windows.h>
#include "Enumerations.cpp"

namespace Core{
using ShowFlyoutFunction = void(__stdcall*)(LCID, LCID);

class Interop {
private:
    void* pThread = nullptr;
public:
    Interop(ShowFlyoutFunction showFlyoutFunction);
    ~Interop();
};
}
