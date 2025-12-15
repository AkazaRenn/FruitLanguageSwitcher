#pragma once

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <Windows.h>

#include "Singleton.cpp"

namespace Core {
class GetMessageThreadManager: public Singleton<GetMessageThreadManager> {
private:
    std::unordered_map<DWORD, std::unordered_set<DWORD>> messageToReceiverThreadIdsMap;
    std::mutex messageToReceiverThreadIdsMapMutex;

public:
    void RegisterReceiverThread(DWORD message, DWORD threadId) {
        std::lock_guard<std::mutex> lock(messageToReceiverThreadIdsMapMutex);
        messageToReceiverThreadIdsMap[message].insert(threadId);
    }

    void UnregisterReceiverThread(DWORD threadId) {
        std::lock_guard<std::mutex> lock(messageToReceiverThreadIdsMapMutex);
        for (auto& [_, threadIds] : messageToReceiverThreadIdsMap) {
            threadIds.erase(threadId);
        }
        PostThreadMessage(threadId, WM_QUIT, 0, 0);
    }

    void PostMessage(DWORD message) {
        std::lock_guard<std::mutex> lock(messageToReceiverThreadIdsMapMutex);
        auto it = messageToReceiverThreadIdsMap.find(message);
        if (it != messageToReceiverThreadIdsMap.end()) {
            for (auto threadId : it->second) {
                PostThreadMessage(threadId, message, 0, 0);
            }
        }
    }
};
}
