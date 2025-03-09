#include <iostream>
#include <thread>
#include "Backend/WebUtils.hpp"

// 处理客户端连接的函数
void HandleClient(Utils::TCP::SingleTCPServer& server) {
    while (true) {
        std::string receivedData;
        if (server.RecData(receivedData)) {
            std::cout << "Server received data: " << receivedData << std::endl;

            // 发送确认信号
            std::string response = "Message received by server";
            server.SendData(response);
        } else {
            // 如果接收数据失败，关闭客户端连接并退出线程
            server.CloseClient();
            break;
        }
    }
}

void RunServer() {
    Utils::TCP::SingleTCPServer server(1717, Utils::IPType::IPV4, 1024, "127.0.0.1");
    if (!server.CreateServer()) {
        std::cerr << "Failed to create server." << std::endl;
        return;
    }
    if (!server.BindPort()) {
        std::cerr << "Failed to bind port." << std::endl;
        return;
    }
    if (!server.ListenServer()) {
        std::cerr << "Failed to listen on server." << std::endl;
        return;
    }

    while (true) {
        if (!server.AcceptClient()) {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }

        // 为每个客户端连接创建一个新的线程
        std::thread clientThread(HandleClient, std::ref(server));
        clientThread.detach(); // 分离线程，让其独立运行
    }
}

int main() {
    RunServer();
    return 0;
}