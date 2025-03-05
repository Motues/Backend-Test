#include <iostream>
#include <thread>

#include "Backend/TCP.hpp"

void RunServer() {
    Utils::TCP::SingleTCPServer server(1717, Utils::IPType::IPV4, 1024, "127.0.0.1");
    if (!server.CreateServer()) {
        std::cerr << "Failed to create server." << std::endl;
        return;
    }
    if (!server.BindPort(1717)) {
        std::cerr << "Failed to bind port." << std::endl;
        return;
    }
    if (!server.ListenServer()) {
        std::cerr << "Failed to listen on server." << std::endl;
        return;
    }
    if (!server.AcceptClient()) {
        std::cerr << "Failed to accept client connection." << std::endl;
        return;
    }

    std::string receivedData;
    if (server.RecvData(receivedData)) {
        std::cout << "Server received data: " << receivedData << std::endl;
        std::string response = "Hello from server";
        server.SendData(response);
    }
}

void RunClient() {
    Utils::TCP::TCPClient client("127.0.0.1", 1717, Utils::IPType::IPV4, 1024);
    if (!client.ConnectToServer()) {
        std::cerr << "Failed to connect to server." << std::endl;
        return;
    }

    std::string message = "Hello from client";
    if (!client.SendData(message)) {
        std::cerr << "Failed to send data to server." << std::endl;
        return;
    }

    std::string response;
    if (client.RecvData(response)) {
        std::cout << "Client received response: " << response << std::endl;
    }
}

int main() {
    // 启动服务器线程
    std::thread serverThread(RunServer);

    // 等待服务器启动
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 启动客户端
    RunClient();

    // 等待服务器线程结束
    serverThread.join();

    return 0;
}