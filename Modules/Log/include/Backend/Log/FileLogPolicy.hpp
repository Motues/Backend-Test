#pragma once

#include "LogPolicy.hpp"
#include <fstream>
#include <ctime>
#include <mutex>
#include <vector>
#include <string>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

namespace Backend::Log {

    class FileLogPolicy : public LogPolicy {
    public:
        explicit FileLogPolicy(std::string  file_path, size_t buffer_size = 512);
        ~FileLogPolicy() override;

        void Write(LogLevel level, const std::string& message) override;
        void Flush() override;

    private:
        struct alignas(64) BufferShard {
            static constexpr size_t SHARD_SIZE = 1 << 18; // 2^18 = 262,144 (256KB)
            alignas(64) std::array<std::string, SHARD_SIZE> messages;
            alignas(64) std::atomic<size_t> head{0};
            alignas(64) std::atomic<size_t> tail{0};

            bool push(std::string &&msg) noexcept {
                auto current_tail = tail.load(std::memory_order_relaxed);
                auto next_tail = (current_tail + 1) & (SHARD_SIZE - 1);

                if (next_tail == head.load(std::memory_order_relaxed)) {
                    return false;
                }

                messages[current_tail] = std::move(msg);
                tail.store(next_tail, std::memory_order_release);
                return true;
            }

            bool pop(std::string &msg) noexcept {
                auto current_head = head.load(std::memory_order_relaxed);

                if (current_head == tail.load(std::memory_order_relaxed)) {
                    return false;
                }

                msg = std::move(messages[current_head]);
                head.store((current_head + 1) & (SHARD_SIZE - 1), std::memory_order_release);
                return true;
            }
        };

        static constexpr size_t NUM_SHARDS = 4; // 4 shards (256KB * 4 = 1MB)
        std::array<BufferShard, NUM_SHARDS> shards;
        std::atomic<size_t> nextShardId{0};
        std::jthread processingThread;
        std::atomic<bool> running{true};
        std::ofstream file_;
        std::string file_path_;

        void processLogs(std::stop_token st);
        static std::string FormatMessage(LogLevel level, const std::string& message);
        size_t getShardId() noexcept;
    };

}