#include "Backend/Log/FileLogPolicy.hpp"
#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>

namespace Backend::Log {

    FileLogPolicy::FileLogPolicy(std::string  file_path, size_t /*buffer_size*/)
        : file_path_(std::move(file_path)), running(true) {
        file_.open(file_path_, std::ios::out | std::ios::app);
        if (!file_.is_open()) {
            throw std::runtime_error("Failed to open log file: " + file_path_);
        }

        processingThread = std::jthread([this](std::stop_token st) {
            this->processLogs(st);
        });
    }

    FileLogPolicy::~FileLogPolicy() {
        running = false;
        if (processingThread.joinable()) {
            processingThread.request_stop();
            processingThread.join();
        }
        Flush();
        if (file_.is_open()) {
            file_.close();
        }
    }

    void FileLogPolicy::Write(LogLevel level, const std::string& message) {
        std::string formattedMessage = FormatMessage(level, message);
        size_t shardId = getShardId();

        for (size_t attempt = 0; attempt < NUM_SHARDS; ++attempt) {
            if (shards[shardId].push(std::move(formattedMessage))) {
                return;
            }
            shardId = (shardId + 1) % NUM_SHARDS;
        }

        // Fallback to direct write if all shards are full
        file_ << formattedMessage << std::endl;
        file_.flush();
    }

    void FileLogPolicy::Flush() {
        for (auto& shard : shards) {
            std::string msg;
            while (shard.pop(msg)) {
                file_ << msg << std::endl;
            }
        }
        file_.flush();
    }

    void FileLogPolicy::processLogs(std::stop_token st) {
        std::vector<std::string> batchBuffer;
        batchBuffer.reserve(4096);

        while (!st.stop_requested()) {
            bool messagesProcessed = false;

            for (size_t i = 0; i < NUM_SHARDS; ++i) {
                std::string msg;
                while (shards[i].pop(msg)) {
                    batchBuffer.push_back(std::move(msg));
                    messagesProcessed = true;
                }
            }

            if (!batchBuffer.empty()) {
                for (const auto& msg : batchBuffer) {
                    file_ << msg << std::endl;
                }
                file_.flush();
                batchBuffer.clear();
            }

            if (!messagesProcessed) {
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        }
    }

    static std::string GetLogLevelString(LogLevel level) {
        switch (level) {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Trace:
            return "TRACE";
        case LogLevel::Warning:
            return "WARNING";
        default:
            return "UNKNOWN";
        }
    }

    std::string FileLogPolicy::FormatMessage(LogLevel level, const std::string& message) {
        std::ostringstream oss;
        oss << "[" << GetLogLevelString(level) << "] " << message << std::endl;
        return oss.str();
    }

    size_t FileLogPolicy::getShardId() noexcept {
        return nextShardId.fetch_add(1, std::memory_order_relaxed) % NUM_SHARDS;
    }

}