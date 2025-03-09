#include <iostream>
#include <string>
#include "Backend/WebUtils.hpp"

void RunClient() {
    Utils::TCP::SingleTCPClient client("127.0.0.1", 1717, Utils::IPType::IPV4, 1024);
    if (!client.ConnectToServer()) {
        std::cerr << "Failed to connect to server." << std::endl;
        return;
    }

    while (true) {
        // 从控制台读取用户输入
        std::string message;
        std::cout << "Enter message to send: ";
        std::getline(std::cin, message);

        if (message.empty()) {
            std::cout << "Empty message, skipping..." << std::endl;
            continue;
        }

        // 发送消息到服务器
        if (!client.SendData(message)) {
            std::cerr << "Failed to send data to server." << std::endl;
            continue;
        }

        // 接收服务器响应
        std::string response;
        if (client.RecvData(response)) {
            std::cout << "Server response: " << response << std::endl;
        }
    }
}

int main() {
    RunClient();
    return 0;
}