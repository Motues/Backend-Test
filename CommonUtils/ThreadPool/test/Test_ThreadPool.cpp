#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include "Utils/ThreadPool.hpp"

using namespace Utils::ThreadPool;

int totalTasks = 0;
std::mutex mtx;


void exampleTask(int id) {
    for (int i = 1; i <= 5; ++i) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            totalTasks++;
        }
        std::cout << "Task " << id << " is running on thread " << std::this_thread::get_id() << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(1));  // 模拟耗时操作
        std::cout << "Task " << id << " is finished on thread " << std::this_thread::get_id() << '\n';
    }
}

int main() {
    ThreadPool pool(10);  // 创建一个包含10个线程的线程池

    for (int i = 1; i <= 20; ++i) {
//        std::cout << "Main thread " << i << "\n";
        pool.enqueue([i]() { exampleTask(i); });
    }
    while (true) {
        if (totalTasks == 100) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    std::cout << "All tasks completed.\nTask:" << totalTasks;
    return 0;
}