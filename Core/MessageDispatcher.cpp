import <mutex>;
import <unordered_map>;
import <unordered_set>;
import "Enumerations.cpp";
import "Singleton.cpp";
import "Utilities.cpp";

namespace Core {
class MessageDispatcher: public Singleton<MessageDispatcher> {
    friend class Singleton<MessageDispatcher>;

private:
    std::unordered_map<Message, std::unordered_set<DWORD>> messageToThreadIdsMap;
    std::mutex messageToThreadIdsMapMutex;

    ~MessageDispatcher() {
        std::lock_guard<std::mutex> lock(messageToThreadIdsMapMutex);
        messageToThreadIdsMap.clear();
    }

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

    void PostMessage(Message message, WPARAM wParam = 0, LPARAM lParam = 0) {
        std::lock_guard<std::mutex> lock(messageToThreadIdsMapMutex);
        const auto it = messageToThreadIdsMap.find(message);
        if (it != messageToThreadIdsMap.end()) {
            for (auto threadId : it->second) {
                PostThreadMessage(threadId, VAL(message), wParam, lParam);
            }
        }
    }
};
}
