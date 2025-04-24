#pragma once

#include "LogPolicy.hpp"
#include "Utils/ThreadPool.hpp"

#include <iostream>
#include <fstream>
#include <ctime>
#include <mutex>
#include <vector>
#include <string>
#include <atomic>
#include <iomanip>
#include <condition_variable>

namespace Utils::Log {

class FileLogPolicy : public LogPolicy {
public:
    FileLogPolicy(const std::string& file_path);
    ~FileLogPolicy() override;

    void Write(LogLevel level, const std::string& message) override;
    void Flush() override;

private:
    std::shared_ptr<std::ofstream> file_ptr_;
    std::string file_path_;
    std::vector<std::string> buffers_[16];             // 16个环形缓冲池
    std::array<bool, 16> buffer_is_empty_;             // 缓冲区是否为空
    std::array<std::mutex, 16> mutex_buffers_;         // 缓冲区加锁
    size_t buffer_size_;                               // 缓冲区大小
    std::atomic<int> current_buffer_;                  // 当前缓冲区
    std::queue<int> queue_buffers_;                    // 等待写入的缓冲区队列
    std::mutex mutex_queue_;                           // 队列加锁
    std::atomic<bool> stop_thread_;                    // 线程控制信号
    std::thread thread_;                               // 文件写入线程
    std::condition_variable condition_;                // 新增条件变量
    void NextBuffer();
    void WriteToFile();
    static std::string FormatMessage(LogLevel level, const std::string& message);
};

}