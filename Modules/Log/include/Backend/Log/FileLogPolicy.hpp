#pragma once

#include "LogPolicy.hpp"
#include <fstream>
#include <ctime>
#include <mutex>
#include <vector>
#include <string>
#include <atomic>

namespace Backend::Log {

    class FileLogPolicy : public LogPolicy {
    public:
        explicit FileLogPolicy(const std::string& file_path, size_t buffer_size = 512)
            : file_path_(file_path), buffer_size_(buffer_size), current_buffer_(0) {
            file_.open(file_path, std::ios::out | std::ios::app);
            if (!file_.is_open()) {
                throw std::runtime_error("Failed to open log file: " + file_path);
            }
            buffers_[0].reserve(buffer_size_);
            buffers_[1].reserve(buffer_size_);
        }

        ~FileLogPolicy() override {
            Flush();
            if (file_.is_open()) {
                file_.close();
            }
        }

        void Write(LogLevel level, const std::string& message) override {
            std::lock_guard<std::mutex> lock(mutex_);
            buffers_[current_buffer_].push_back(FormatMessage(level, message));
            if (buffers_[current_buffer_].size() >= buffer_size_) {
                SwapBuffers();
                Flush();
            }
        }

        void Flush() override {
            // std::lock_guard<std::mutex> lock(mutex_);
            if (!buffers_[!current_buffer_].empty()) {
                for (const auto& msg : buffers_[!current_buffer_]) {
                    file_ << msg << std::endl;
                }
                buffers_[!current_buffer_].clear();
                file_.flush(); // 确保文件流被刷新
            }
        }

    private:
        std::ofstream file_;
        std::mutex mutex_;
        std::vector<std::string> buffers_[2];
        std::string file_path_;
        size_t buffer_size_;
        std::atomic<int> current_buffer_;

        void SwapBuffers() {
            current_buffer_ = !current_buffer_;
        }

        static std::string FormatMessage(LogLevel level, const std::string& message) {
            auto now = std::chrono::system_clock::now();
            auto now_c = std::chrono::system_clock::to_time_t(now);
            auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
            auto tm = *std::localtime(&now_c);
            std::ostringstream oss;
            oss << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "." << std::setw(3) << std::setfill('0') << now_ms.count() << "] ";
            switch (level) {
                case LogLevel::Info:
                    oss << "[INFO] ";
                    break;
                case LogLevel::Debug:
                    oss << "[DEBUG] ";
                    break;
                case LogLevel::Error:
                    oss << "[ERROR] ";
                    break;
                case LogLevel::Warning:
                    oss << "[WARNING] ";
                    break;
                case LogLevel::Trace:
                    oss << "[TRACE] ";
                    break;
            }
            oss << message;
            return oss.str();
        }
    };

}