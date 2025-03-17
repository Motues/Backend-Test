#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include "Backend/TCP/Server.hpp"

int main() {
    Utils::TCP::SingleTCPServer server(1717, "0.0.0.0");

    if (!server.CreateServer()) {
        std::cerr << "Failed to create server" << std::endl;
        return 1;
    }

    if (!server.BindPort()) {
        std::cerr << "Failed to bind port" << std::endl;
        return 1;
    }

    while (true) {
        if (!server.ListenServer()) {
            std::cerr << "Failed to listen on port" << std::endl;
            continue;
        }

        if (!server.AcceptClient()) {
            std::cerr << "Failed to accept client" << std::endl;
            continue;
        }

        std::ofstream outFile("server_log.txt", std::ios::app);
        if (!outFile.is_open()) {
            std::cerr << "Failed to open log file" << std::endl;
            server.CloseClient();
            continue;
        }

        std::string data;
        while (server.RecData(data)) {
            // 获取当前时间
            std::time_t now = std::time(nullptr);
            std::tm* localTime = std::localtime(&now);
            std::ostringstream timeStream;
            timeStream << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");

            // 将时间信息与接收到的数据拼接
            outFile << "[" << timeStream.str() << "] " << data << std::endl;
            outFile.flush();

            if (!server.SendData("Message received")) {
                std::cerr << "Failed to send confirmation" << std::endl;
            }
        }

        outFile.close();
        server.CloseClient();
        std::cout << "Client disconnected, waiting for next connection..." << std::endl;
    }

    server.CloseServer();
    return 0;
}