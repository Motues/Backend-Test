#include "Utils/Log.hpp"


int main() {
    // 创建日志策略
    auto filePolicy = std::make_shared<Utils::Log::FileLogPolicy>("log.txt");
    auto consolePolicy = std::make_shared<Utils::Log::ConsoleLogPolicy>();

    // 初始化 Logger
    Utils::Log::Logger logger;
    logger.AddPolicy(filePolicy);
    logger.AddPolicy(consolePolicy);

    // 记录日志
    logger.Info("Application started.");
    logger.Debug("Debugging with value: {}, {}", 42, 66);
    logger.Warning("This is a warning message.");
    logger.Error("An error occurred: {}", "file not found");
    logger.Trace("Tracing execution flow.");

    // 刷新日志
    logger.Flush();

    return 0;
}
