# pragma once

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace Utils :: TCP{

    enum IPType{
        IPV4 = 0,
        IPV6 = 1
    };

    class TCPServer {
    public:
        TCPServer();
        TCPServer(int port);
        TCPServer(int port, IPType ipType);
        TCPServer(int port, IPType ipType, int bufferSize);

         ~TCPServer();

         bool CreateServer(); // 创建服务器socket文件描述符

         bool BindServer();


    private:
        int Port{1717}; // 开放的端口号
        IPType IPType{IPV4};
        int BufferSize{1024}; // 缓冲区大小
        int ServerSocket{-1}; // 服务器socket文件描述符
        int ClientSocket{-1}; // 客户端socket文件描述符
    };


}
