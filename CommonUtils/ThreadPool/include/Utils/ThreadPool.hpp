#pragma once

#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace Utils :: ThreadPool {

class ThreadPool {
public:
    ThreadPool(size_t threads_num = 16, int max_task_num = 65536);
    ~ThreadPool();

    void enqueue(std::function<void()> task);

private:
    std::queue<std::function<void()>> tasks;  // 任务队列
    std::vector<std::thread> workers;         // 工作线程集合
    std::mutex task_mutex;                    // 保护任务队列的互斥锁
    std::condition_variable condition;        // 条件变量，用于线程间通信
    bool stop_pool;
    int max_task_num, cur_task_num;
};

}
