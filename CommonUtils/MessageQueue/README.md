# CommonUtils.MessageQueue

## 概述
`MessageQueue` 是一个基于 C++ 实现的线程安全消息队列模块，支持指定最大消息数量和覆盖模式；该模块适用于多线程环境下的消息传递和处理场景。

---

## 功能特性
1. **线程安全**：通过 `std::mutex` 和 `std::condition_variable` 确保在多线程环境下的安全性。
2. **最大消息数量限制**：
    - 支持设置消息队列的最大容量。
    - 当队列满时，可以选择丢弃最新消息或覆盖最旧消息（取决于 `coverageMode`）。
3. **简单易用**：提供 `push` 和 `pop` 方法，分别用于入队和出队操作。
4. **空队列检测**：提供 `empty` 方法，用于检测队列是否为空。
5. **模式切换**：支持丢弃最新消息和覆盖最旧消息两种模式。

---

## 使用方法

### 创建实例
```c++
using namespace Utils::MessageQueue;
// 创建一个最大容量为 1024 的消息队列，启用覆盖模式 MessageQueue<int> messageQueue(1024, true);
MessageQueue<int> messageQueue(1024, true);
```

### 检查队列是否为空
```c++
if (messageQueue.empty()) {
    std::cout << "Message queue is empty." << std::endl; 
}
```

### 入队操作
```c++
messageQueue.push(42); // 将整数 42 入队
```
- 如果队列已满且 `coverageMode` 为 `true`，则会覆盖最旧的消息。
- 如果队列已满且 `coverageMode` 为 `false`，则会丢弃最新消息。

### 出队操作
```c++
if (!messageQueue.empty()) { 
    int message = messageQueue.pop();// 获取并移除队首消息
    std::cout << "Popped message: " << message << std::endl;
}
```


## 参数说明

|     参数名称          |    类型     | 描述                                       |
|:-----------------:|:---------:|:-----------------------------------------|
| `maxMessageCount` |   `int`   | 消息队列的最大容量，默认值为 1024。                     |
|  `coverageMode`   |  `bool`   | 是否启用覆盖模式。如果为 `true`，队列满时覆盖最旧消息；否则丢弃最新消息。 |



## 示例代码
以下是一个完整的示例，展示如何在多线程环境下使用 `MessageQueue`
```c++
#include "MessageQueue.hpp"
#include <iostream>
#include <thread>

using namespace Utils::MessageQueue;

void Producer(MessageQueue<int>& messageQueue) {
    for (int i = 0; i < 15; ++i) {
        messageQueue.push(i); 
        std::cout << "Produced: " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
void Consumer(MessageQueue<int>& messageQueue) { 
    while (true) { 
        if (!messageQueue.empty()) { 
            int msg = messageQueue.pop(); 
            std::cout << "Consumed: " << msg << std::endl; 
        } else { 
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
        } 
    } 
}

int main() { 
    MessageQueue<int> messageQueue(10, true);
    std::thread ProducerThread(producer, std::ref(messageQueue));
    std::thread ConsumerThread(consumer, std::ref(messageQueue));

    ProducerThread.join();
    ConsumerThread.join();

    return 0;
}
```

## 待实现功能

- [ ] **日志优化**：将 `std::cout` 替换为更灵活的日志系统，便于调试和部署。
- [ ] **模板参数扩展**：支持更多自定义行为，例如不同的覆盖策略或消息优先级。
