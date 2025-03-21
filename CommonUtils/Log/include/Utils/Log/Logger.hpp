#include "LogPolicy.hpp"
#include <vector>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <future>
#include <sstream>
#include <chrono>

namespace Utils::Log {

    class Logger {
    public:
        Logger() : stop_(false) {
            thread_ = std::thread(&Logger::ProcessQueue, this);
            timer_thread_ = std::thread(&Logger::TimerTask, this);
        }

        ~Logger() {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                stop_ = true;
            }
            condition_.notify_all();
            thread_.join();
            timer_thread_.join();
        }

        void AddPolicy(std::shared_ptr<LogPolicy> policy) {
            policies_.push_back(policy);
        }

        template <typename... Args>
        void Log(LogLevel level, const std::string& format, Args... args) {
            std::ostringstream oss;
            FormatMessage(oss, format, args...);
            {
                std::lock_guard<std::mutex> lock(mutex_);
                queue_.push({level, oss.str()});
            }
            condition_.notify_one();
        }

        void Flush() {
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto& policy : policies_) {
                policy->Flush();
            }
        }

        template <typename... Args>
        void Debug(const std::string& format, Args... args) {
            Log(LogLevel::Debug, format, args...);
        }

        template <typename... Args>
        void Info(const std::string& format, Args... args) {
            Log(LogLevel::Info, format, args...);
        }

        template <typename... Args>
        void Error(const std::string& format, Args... args) {
            Log(LogLevel::Error, format, args...);
        }

        template <typename... Args>
        void Warning(const std::string& format, Args... args) {
            Log(LogLevel::Warning, format, args...);
        }

        template <typename... Args>
        void Trace(const std::string& format, Args... args) {
            Log(LogLevel::Trace, format, args...);
        }

    private:
        struct LogMessage {
            LogLevel level;
            std::string message;
        };

        std::vector<std::shared_ptr<LogPolicy>> policies_;
        std::mutex mutex_;
        std::queue<LogMessage> queue_;
        std::condition_variable condition_;
        std::thread thread_;
        std::thread timer_thread_;
        std::atomic<bool> stop_;

        template <typename T, typename... Args>
        void FormatMessage(std::ostringstream& oss, const std::string& format, T value, Args... args) {
            size_t pos = format.find("{}");
            if (pos != std::string::npos) {
                oss << format.substr(0, pos) << value;
                FormatMessage(oss, format.substr(pos + 2), args...);
            } else {
                oss << format;
            }
        }

        static void FormatMessage(std::ostringstream& oss, const std::string& format) {
            oss << format;
        }

        void ProcessQueue() {
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

        void TimerTask() {
            while (!stop_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                Flush();
            }
        }
    };

}