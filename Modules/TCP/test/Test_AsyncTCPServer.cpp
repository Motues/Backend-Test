#include <iostream>
#include "Backend/WebUtils.hpp"
#include "Utils/MessageQueue.hpp"

using namespace Utils::MessageQueue;
using namespace Utils::TCP;


int main() {
    Utils::TCP::AsyncTCPServer server(1717, "127.0.0.1", Utils::IPType::IPV4);
    if (!server.CreateServer() || !server.BindPort() || !server.ListenServer() || !server.StartServer()) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }

    while (true) {
        TCPMessage msg;
        if (server.GetMessage(msg)) {
            std::cout << "Received message: " << msg.data << std::endl;
        }
        sleep(1);
    }

    // 主线程可以继续执行其他任务
    // ...

    server.CloseServer();
    return 0;
}
