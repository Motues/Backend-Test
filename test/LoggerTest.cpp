#include <gtest/gtest.h>
#include "Backend/Logging/Logger.hpp"

TEST(LoggerTest, LogLevels) {
    Logger logger;

    // 捕获标准输出
    testing::internal::CaptureStdout();
    logger.LogInfo("This is an info message");
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("[INFO]") != std::string::npos);

    testing::internal::CaptureStdout();
    logger.LogError("This is an error message");
    output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("[ERROR]") != std::string::npos);

    testing::internal::CaptureStdout();
    logger.LogWarning("This is a warning message");
    output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("[WARNING]") != std::string::npos);
}