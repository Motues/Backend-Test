#pragma once

#include <string>
#include <mutex>

namespace Utils::Log {

    enum class LogLevel {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
    };

    class LogPolicy {
    public:
        virtual ~LogPolicy() = default;

        // 写入日志的接口
        virtual void Write(LogLevel level, const std::string& message) = 0;

        // 刷新日志的接口
        virtual void Flush() = 0;
    };

}