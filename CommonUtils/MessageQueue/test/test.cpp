#include "Utils/MessageQueue.hpp"

using namespace Utils :: MessageQueue;

int main() {
    MessageQueue<int> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);
    while (!queue.empty()) {
        std::cout << queue.pop() << std::endl;
    }
}