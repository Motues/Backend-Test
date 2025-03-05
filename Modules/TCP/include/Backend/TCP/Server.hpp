# pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "Backend/BasicType.hpp"

namespace Utils :: TCP{

    class SingleTCPServer {
    public:
        SingleTCPServer();
        SingleTCPServer(int port);
        SingleTCPServer(int port, IPType ipType);
        SingleTCPServer(int port, IPType ipType, int bufferSize);
        SingleTCPServer(int port, IPType ipType, int bufferSize, const std::string& serverAddress);
        ~SingleTCPServer();

        bool CreateServer();
        bool BindPort(int Port = 1717);
        bool ListenServer();
        bool AcceptClient();
        bool SendData(const std::string& data);
        bool RecvData(std::string& data);

        bool CloseServer();
        bool CloseClient();

    private:
        int port{1717};
        IPType ipType{IPType::IPV4};
        int bufferSize{1024};
        int serverSocket{-1};
        int clientSocket{-1};
        std::string serverAddress{"0.0.0.0"};
    };


}
