#pragma once

#include "LogPolicy.hpp"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <future>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>

namespace Utils::Log {

    class ConsoleLogPolicy : public LogPolicy {
    public:
        ConsoleLogPolicy();
        ~ConsoleLogPolicy() override;

        void Write(LogLevel level, const std::string& message) override;
        void Flush() override;

    private:
        struct LogMessage {
            LogLevel level;
            std::string message;
        };

        std::mutex mutex_;
        std::queue<LogMessage> message_queue_;
        std::mutex queue_mutex_;
        std::condition_variable condition_;
        std::thread thread_;
        std::atomic<bool> stop_;

        void ProcessQueue();
        static std::string FormatMessage(LogLevel level, const std::string& message);
    };

}