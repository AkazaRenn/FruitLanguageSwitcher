#pragma once

#include <Windows.h>

namespace Core {
enum class Message: DWORD {
    Unused = WM_APP,
    ForegroundChanged,
};

static constexpr DWORD VAL(Message msg) {
    return static_cast<DWORD>(msg);
}
}
