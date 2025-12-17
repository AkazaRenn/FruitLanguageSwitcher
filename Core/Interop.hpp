#pragma once

#include <functional>
#include <unordered_map>
#include <Windows.h>
#include "Message.cpp"

namespace Core{
using ProcessMessageFunction = std::function<void(const MSG&)>;
using ProcessMessageFunctionPtr = void(__stdcall*)(const MSG&);
using MessageToProcessFunctionMap = std::unordered_map<Message, ProcessMessageFunction>;

class Interop {
private:
    void* pThread = nullptr;
public:
    Interop(const MessageToProcessFunctionMap& messageToProcessFunctionMap);
    ~Interop();
};
}
