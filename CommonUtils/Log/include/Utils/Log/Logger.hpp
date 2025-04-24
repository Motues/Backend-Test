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
        Logger();
        ~Logger();

        void AddPolicy(std::shared_ptr<LogPolicy> policy);

        void Flush();
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

        static void FormatMessage(std::ostringstream& oss, const std::string& format);
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

        void ProcessQueue();
        void TimerTask();
    };

}