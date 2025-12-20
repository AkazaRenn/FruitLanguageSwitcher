#pragma once

// Kept because C++/CLI doesn't support modules

#include <functional>
#include <unordered_map>
#include <Windows.h>
#include "Enumerations.cpp"

namespace Core{
using ProcessMessageFunction = std::function<void(const MSG&)>;
using MessageToProcessFunctionMap = std::unordered_map<Message, ProcessMessageFunction>;

class Interop {
private:
    void* pThread = nullptr;
public:
    Interop(const MessageToProcessFunctionMap& messageToProcessFunctionMap);
    ~Interop();
};
}
