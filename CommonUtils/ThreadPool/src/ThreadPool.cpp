#include <iostream>
#include "Utils/ThreadPool.hpp"

namespace Utils::ThreadPool {

ThreadPool::ThreadPool(size_t threads_num, int max_task_num) :
    stop_pool(false), max_task_num(max_task_num), cur_task_num(0) {
    for (size_t i = 0; i < threads_num; ++i)
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->task_mutex);
                    this->condition.wait(lock, [this] { return this->stop_pool || !this->tasks.empty(); });
                    if (this->stop_pool && this->tasks.empty()) {
                        return;
                    }
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                    cur_task_num--; // 任务出队后减少当前任务数量
                }

                task(); // 执行任务

                {
                    std::lock_guard<std::mutex> lock(this->task_mutex);
                    condition.notify_one(); // 通知等待的生产者线程
                }
            }
        });
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(task_mutex);
        stop_pool = true;  // 设置停止标志
    }
    condition.notify_all();  // 唤醒所有线程
    for (std::thread& worker : workers) {
        worker.join();  // 等待所有线程结束
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(task_mutex);
        if (stop_pool) {
            throw std::runtime_error("Enqueue on stopped ThreadPool");
        }
        // 等待直到任务队列有空闲位置
        condition.wait(lock, [this] { return cur_task_num < max_task_num; });
        tasks.emplace(task);
        cur_task_num++; // 任务入队后增加当前任务数量
    }
    condition.notify_one();  // 唤醒一个等待的线程
}

}