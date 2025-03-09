#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "Backend/BasicType.hpp"

namespace Utils::TCP {

    class SingleTCPClient {
    public:
        SingleTCPClient();
        SingleTCPClient(std::string  serverAddress, int port);
        SingleTCPClient(std::string  serverAddress, int port, IPType ipType);
        SingleTCPClient(std::string  serverAddress, int port, IPType ipType, int bufferSize);

        ~SingleTCPClient();

        bool ConnectToServer(); // 连接到服务器
        bool SendData(const std::string& data); // 发送数据到服务器
        bool RecvData(std::string& data); // 从服务器接收数据
        bool CloseConnection(); // 关闭与服务器的连接

    private:
        std::string serverAddress{"127.0.0.1"}; // 服务器地址
        int port{1717}; // 服务器端口号
        IPType ipType{IPType::IPV4}; // IP类型
        int bufferSize{1024}; // 缓冲区大小
        int clientSocket{-1}; // 客户端socket文件描述符
    };

}