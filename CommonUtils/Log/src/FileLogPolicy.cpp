#include "Utils/Log/FileLogPolicy.hpp"


namespace Utils::Log {

FileLogPolicy::FileLogPolicy(const std::string &file_path) : file_path_(file_path) {
    file_ptr_ = std::make_unique<std::ofstream>();
    buffer_size_ = 1024;
    file_ptr_->open(file_path, std::ios::out | std::ios::app);
    if (!file_ptr_->is_open()) {
        throw std::runtime_error("Failed to open log file: " + file_path);
    }
    buffers_[0].reserve(buffer_size_);
    buffers_[1].reserve(buffer_size_);
}
FileLogPolicy::~FileLogPolicy() {
    Flush();
    if (file_ptr_->is_open()) {
        file_ptr_->close();
    }
    thread_pool_.;
}
void FileLogPolicy::SwapBuffers() {
    current_buffer_ = !current_buffer_;
}
void FileLogPolicy::Write(Utils::Log::LogLevel level, const std::string &message) {
    std::lock_guard<std::mutex> lock(mutex_);
    buffers_[current_buffer_].push_back(FormatMessage(level, message));
    if (buffers_[current_buffer_].size() >= buffer_size_) {
        SwapBuffers();
        Flush();
    }
}
/*
 * 逻辑还需要修改
 */
void FileLogPolicy::Flush() {
    // std::lock_guard<std::mutex> lock(mutex_);
    if (!buffers_[!current_buffer_].empty()) {
        auto buffers_ptr = std::make_shared<std::vector<std::string>>(buffers_[!current_buffer_]);
        thread_pool_.enqueue([this, buffers_ptr]() {
            for (const auto& msg : *buffers_ptr) {
                file_ptr_->write(msg.c_str(), msg.size());
            }
            file_ptr_->flush(); // 确保文件流被刷新
        });
        buffers_[!current_buffer_].clear();
    }
}

std::string FileLogPolicy::FormatMessage(LogLevel level, const std::string& message) {
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
    oss << message << std::endl;
    return oss.str();
}
}