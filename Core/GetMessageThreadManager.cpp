#pragma once

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <Windows.h>

#include "Common.cpp"
#include "Singleton.cpp"

namespace Core {
class GetMessageThreadManager: public Singleton<GetMessageThreadManager> {
private:
    std::unordered_map<Message, std::unordered_set<DWORD>> messageToReceiverThreadIdsMap;
    std::mutex messageToReceiverThreadIdsMapMutex;

public:
    void RegisterReceiverThread(Message message, DWORD threadId) {
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

    void PostMessage(Message message) {
        std::lock_guard<std::mutex> lock(messageToReceiverThreadIdsMapMutex);
        const auto it = messageToReceiverThreadIdsMap.find(message);
        if (it != messageToReceiverThreadIdsMap.end()) {
            for (auto threadId : it->second) {
                PostThreadMessage(threadId, VAL(message), 0, 0);
            }
        }
    }
};
}
