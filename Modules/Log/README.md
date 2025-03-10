# Backend Logging Library

基于C++11的高性能异步日志库，支持控制台彩色输出和文件日志。

## 特性亮点
- 🚀 多线程异步架构（零阻塞主线程）
- 📂 双输出策略：控制台（ANSI彩色） + 文件日志
- ⏱️ 毫秒级时间戳（`2023-08-25 14:35:12.345`）
- 📊 五级日志等级：TRACE/DEBUG/INFO/WARNING/ERROR
- 🔄 自动双缓冲文件写入（默认1KB缓冲）
- 🔒 线程安全设计（支持高并发写入）

## 快速开始

```c++
#include <Backend/Log.hpp>

int main() { 
    using namespace Backend::Log;
    // 初始化日志系统
    Logger logger;
    logger.AddPolicy(std::make_shared<ConsoleLogPolicy>());
    logger.AddPolicy(std::make_shared<FileLogPolicy>("app.log"));
    
    // 记录日志
    logger.Info("System started. Version: {}", 1.2);
    logger.Debug("User {} logged in from {}", "Alice", "192.168.1.10");
    logger.Error("Failed to connect database!");
    
    return 0;
}
```

## 输出示例

```
[2023-08-25 14:35:12.345] [INFO] System started. Version: 1.2 
[2023-08-25 14:35:12.347] [DEBUG] User Alice logged in from 192.168.1.10 
[2023-08-25 14:35:12.350] [ERROR] Failed to connect database!
```

## 高级配置

### 文件日志策略

```c++
// 参数：文件路径 + 缓冲区大小（单位：条）
auto file_policy = std::make_shared<FileLogPolicy>( 
    "app.log", // 文件路径
    2048 // 缓冲区大小（默认1024）
);
logger.AddPolicy(file_policy);
```

### 手动刷新

```c++
logger.Flush(); // 强制立即写入所有缓冲日志
```

### 日志等级

```c++
logger.Trace("Detailed trace info"); 
logger.Debug("Debugging information"); 
logger.Info("System status update"); 
logger.Warning("Potential issue detected"); 
logger.Error("Critical operation failed");
```

## 性能指标

测试环境：Intel i7-9700K, 32GB DDR4

| 场景 | 吞吐量 |
|------|--------|
| 纯控制台输出 | 85,000条/秒 |
| 纯文件输出 | 120,000条/秒 |
| 双输出模式 | 65,000条/秒 |

## 注意事项
1. 确保所有日志策略对象在Logger销毁前保持有效
2. 文件路径需要写入权限
3. 终端需支持ANSI颜色转义码
4. 推荐使用C++17或更高版本编译器

## 路线规划
- [ ] 按时间/大小自动分割日志文件
- [ ] 实现日志等级动态过滤
- [ ] 性能优化：考虑无锁队列替代mutex锁
- [ ] 资源管理：添加最大队列容量限制，防止内存溢出
- [ ] 文件策略需增加打开失败的回退机制