#include "Utils/Log/Logger.hpp"

namespace Utils :: Log {
    Logger::Logger() : stop_(false) {
        thread_ = std::thread(&Logger::ProcessQueue, this);
        timer_thread_ = std::thread(&Logger::TimerTask, this);
    }
    Logger::~Logger() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = true;
        }
        condition_.notify_all();
        thread_.join();
        timer_thread_.join();
    }

    void Logger::AddPolicy(std::shared_ptr<LogPolicy> policy) {
        policies_.push_back(policy);
    }
    void Logger::Flush() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& policy : policies_) {
            policy->Flush();
        }
    }

    void Logger::FormatMessage(std::ostringstream& oss, const std::string& format) {
        oss << format;
    }

    void Logger::ProcessQueue()  {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this] { return stop_ || !queue_.empty(); });
            if (stop_ && queue_.empty()) {
                return;
            }
            LogMessage message = std::move(queue_.front());
            queue_.pop();
            lock.unlock();

            for (auto& policy : policies_) {
                policy->Write(message.level, message.message);
            }
        }
    }
    void Logger::TimerTask() {
        while (!stop_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            Flush();
        }
    }
}