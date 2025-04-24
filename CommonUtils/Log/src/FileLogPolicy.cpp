#include "Utils/Log/FileLogPolicy.hpp"

namespace Utils::Log {
    FileLogPolicy::FileLogPolicy(const std::string &file_path) : file_path_(file_path) {
        file_ptr_ = std::make_shared<std::ofstream>();
        file_ptr_->open(file_path, std::ios::out | std::ios::app);
        if (!file_ptr_->is_open()) {
            throw std::runtime_error("Failed to open log file: " + file_path);
        }
        buffer_size_ = 65536;
        for(int i = 0; i < 16; i++) {
            buffers_[i].reserve(buffer_size_);
            buffer_is_empty_[i] = true;
        }
        stop_thread_ = false;
        current_buffer_ = 0; NextBuffer();
        thread_ = std::thread(&FileLogPolicy::WriteToFile, this);
    }

    FileLogPolicy::~FileLogPolicy() {
        stop_thread_ = true;
        Flush();
        condition_.notify_all();
        if(thread_.joinable()) {
            thread_.join();
        }
        if (file_ptr_->is_open()) {
            file_ptr_->close();
        }
    }

    void FileLogPolicy::NextBuffer() {
        while(!buffer_is_empty_[current_buffer_]) {
            current_buffer_ = current_buffer_ + 1 > 15 ? 0 : current_buffer_ + 1;
        }
    }

    void FileLogPolicy::Write(Utils::Log::LogLevel level, const std::string &message) {
        std::lock_guard<std::mutex> lock(mutex_buffers_[current_buffer_]);
        buffers_[current_buffer_].push_back(FormatMessage(level, message));
        if (buffers_[current_buffer_].size() >= buffer_size_) {
            {
                std::lock_guard<std::mutex> lock(mutex_queue_);
                queue_buffers_.push(current_buffer_);
                buffer_is_empty_[current_buffer_] = false;
            }
            NextBuffer();
            // 通知线程有新的缓冲区需要处理
            condition_.notify_one();
        }
    }

    void FileLogPolicy::Flush() {
        {
            std::lock_guard<std::mutex> lock(mutex_buffers_[current_buffer_]);
            if(!buffers_[current_buffer_].empty()) {
                std::lock_guard<std::mutex> lock(mutex_queue_);
                queue_buffers_.push(current_buffer_);
                buffer_is_empty_[current_buffer_] = false;
                NextBuffer();
                condition_.notify_one();
            }
        }
    }

    void FileLogPolicy::WriteToFile() {
        int buffer_index;
        while (true) {
            {
                std::unique_lock<std::mutex> lock(mutex_queue_);
                // 使用条件变量等待缓冲区准备好
                this->condition_.wait(lock, [this] { return !queue_buffers_.empty() || stop_thread_; });
                if (stop_thread_ && queue_buffers_.empty()) {
                    return;
                }
                buffer_index = std::move(queue_buffers_.front());
                queue_buffers_.pop();
            }
            for (auto &msg : buffers_[buffer_index]) {
                file_ptr_->write(msg.c_str(), msg.size());
            }
            {
                std::lock_guard<std::mutex> lock(mutex_buffers_[buffer_index]);
                buffers_[buffer_index].clear();
                buffer_is_empty_[buffer_index] = true;
            }
            file_ptr_->flush();
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