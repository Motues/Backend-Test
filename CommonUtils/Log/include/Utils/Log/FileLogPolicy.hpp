#pragma once

#include "LogPolicy.hpp"
#include "Utils/ThreadPool.hpp"

#include <fstream>
#include <ctime>
#include <mutex>
#include <vector>
#include <string>
#include <atomic>
#include <iomanip>

namespace Utils::Log {

class FileLogPolicy : public LogPolicy {
public:
    FileLogPolicy(const std::string& file_path);
    ~FileLogPolicy() override;

    void SwapBuffers();
    void Write(LogLevel level, const std::string& message) override;
    void Flush() override;

private:
    std::shared_ptr<std::ofstream> file_ptr_;
    std::mutex mutex_;
    std::string file_path_;
    std::vector<std::string> buffers_[2];
    size_t buffer_size_;
    std::atomic<int> current_buffer_;
    ThreadPool::ThreadPool thread_pool_;
    static std::string FormatMessage(LogLevel level, const std::string& message);
};

}