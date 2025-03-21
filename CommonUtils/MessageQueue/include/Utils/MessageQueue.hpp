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
    void push(const Message& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(msg);
        cond_.notify_one();
    }
    Message pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] { return !queue_.empty(); });
        Message msg = queue_.front();
        queue_.pop();
        return msg;
    }
    [[nodiscard]] bool empty() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

private:
    std::queue<Message> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
    int maxMessageCount{1024};
    int currentMessageCount{0}; // TODO 用于控制队列大小
};

}