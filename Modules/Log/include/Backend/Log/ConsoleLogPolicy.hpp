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

namespace Backend::Log {

    class ConsoleLogPolicy : public LogPolicy {
    public:
        ConsoleLogPolicy() : stop_(false) {
            thread_ = std::thread(&ConsoleLogPolicy::ProcessQueue, this);
        }

        ~ConsoleLogPolicy() override {
            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                stop_ = true;
            }
            condition_.notify_all();
            if (thread_.joinable()) {
                thread_.join();
            }
            while (!message_queue_.empty()) {
                auto msg = message_queue_.front();
                std::cout << FormatMessage(msg.level, msg.message) << std::endl;
                message_queue_.pop();
            }
        }

        void Write(LogLevel level, const std::string& message) override {
            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                message_queue_.push({level, message});
            }
            condition_.notify_one();
        }

        void Flush() override {
            std::lock_guard<std::mutex> lock(mutex_);
            std::cout.flush();
        }

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

        void ProcessQueue() {
            while (true) {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                condition_.wait(lock, [this] { return stop_ || !message_queue_.empty(); });
                if (stop_ && message_queue_.empty()) {
                    return;
                }
                LogMessage message = std::move(message_queue_.front());
                message_queue_.pop();
                lock.unlock();

                std::lock_guard<std::mutex> cout_lock(mutex_);
                std::cout << FormatMessage(message.level, message.message) << std::endl;
            }
        }

        static std::string FormatMessage(LogLevel level, const std::string& message) {
            auto now = std::chrono::system_clock::now();
            auto now_c = std::chrono::system_clock::to_time_t(now);
            auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
            auto tm = *std::localtime(&now_c);
            std::ostringstream oss;
            oss << "\033[32m[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "." << std::setw(3) << std::setfill('0') << now_ms.count() << "]\033[0m ";
            switch (level) {
                case LogLevel::Info:
                    oss << "\033[32m[INFO] \033[0m";
                    break;
                case LogLevel::Debug:
                    oss << "\033[34m[DEBUG] \033[0m";
                    break;
                case LogLevel::Error:
                    oss << "\033[31m[ERROR] \033[0m";
                    break;
                case LogLevel::Warning:
                    oss << "\033[33m[WARNING] \033[0m";
                    break;
                case LogLevel::Trace:
                    oss << "\033[36m[TRACE] \033[0m";
                    break;
            }
            oss << message;
            return oss.str();
        }
    };

}