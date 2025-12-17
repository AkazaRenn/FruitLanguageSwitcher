#pragma once

import <mutex>;
import <unordered_map>;
import <unordered_set>;
import "Common.cpp";
import "Message.cpp";
import "Singleton.cpp";

namespace Core {
class GetMessageThreadManager: public Singleton<GetMessageThreadManager> {
private:
    std::unordered_map<Message, std::unordered_set<DWORD>> messageToThreadIdsMap;
    std::mutex messageToThreadIdsMapMutex;

public:
    void RegisterThread(Message message, DWORD threadId) {
        std::lock_guard<std::mutex> lock(messageToThreadIdsMapMutex);
        messageToThreadIdsMap[message].insert(threadId);
    }

    void UnregisterThread(DWORD threadId) {
        std::lock_guard<std::mutex> lock(messageToThreadIdsMapMutex);
        for (auto& [_, threadIds] : messageToThreadIdsMap) {
            threadIds.erase(threadId);
        }
    }

    void PostMessage(Message message) {
        std::lock_guard<std::mutex> lock(messageToThreadIdsMapMutex);
        const auto it = messageToThreadIdsMap.find(message);
        if (it != messageToThreadIdsMap.end()) {
            for (auto threadId : it->second) {
                PostThreadMessage(threadId, VAL(message), 0, 0);
            }
        }
    }
};
}
