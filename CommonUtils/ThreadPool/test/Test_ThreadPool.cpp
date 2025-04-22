#include <iostream>
#include <chrono>
#include <thread>
#include "Utils/ThreadPool.hpp"

using namespace Utils::ThreadPool;
void exampleTask(int id) {
    std::cout << "Task " << id << " is running on thread " << std::this_thread::get_id() << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(2));  // 模拟耗时操作
}

int main() {
    ThreadPool pool;  // 创建一个包含4个线程的线程池

    for (int i = 0; i < 20; ++i) {
//        std::cout << "Main thread " << i << "\n";
        pool.enqueue([i]() { exampleTask(i); });
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));  // 等待任务完成
    return 0;
}