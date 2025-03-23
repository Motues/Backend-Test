#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>
#include <iostream>

namespace Utils :: MessageQueue {

template<typename Message>
class MessageQueue {
public:
    explicit MessageQueue(int maxMessageCount = 1024, bool coverageMode = false) :
        maxMessageCount(maxMessageCount), currentMessageCount(0), coverageMode(coverageMode) {}
    MessageQueue(const MessageQueue&) = delete;
    MessageQueue& operator=(const MessageQueue&) = delete;

    void push(const Message& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (currentMessageCount >= maxMessageCount) {
            if (coverageMode) {
                std::cout << "MessageQueue is full, drop the oldest message." << std::endl;
                queue_.pop();
                currentMessageCount--;
            }else{
                std::cout << "MessageQueue is full, drop the latest message." << std::endl;
                return;
            }
        }
        queue_.push(msg);
        currentMessageCount++;
        cond_.notify_one();
    }
    Message pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] { return currentMessageCount > 0; });
        Message msg = queue_.front();
        queue_.pop();
        currentMessageCount--;
        return msg;
    }
    [[nodiscard]] bool empty() {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentMessageCount == 0;
    }

private:
    std::queue<Message> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
    const int maxMessageCount;
    int currentMessageCount;
    bool coverageMode; // 新数据覆盖旧数据模式
};

}