# pragma once

#include <iostream>
#include <string>
#include <cstring>

#include <vector>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>

#include "Backend/BasicType.hpp"

namespace Utils :: TCP{

    class SingleTCPServer {
    public:
        SingleTCPServer();
        explicit SingleTCPServer(int port);
        explicit SingleTCPServer(std::string serverAddress);
        SingleTCPServer(int port, std::string serverAddress);
        SingleTCPServer(int port, std::string serverAddress, int bufferSize);
        SingleTCPServer(int port, IPType ipType, int bufferSize);
        SingleTCPServer(int port, IPType ipType, int bufferSize, std::string  serverAddress);
        ~SingleTCPServer();

        bool CreateServer();
        bool BindPort();
        bool ListenServer();
        bool AcceptClient();
        [[nodiscard]] bool SendData(const std::string& data) const;
        [[nodiscard]] bool RecData(std::string& data);

        bool CloseServer();
        bool CloseClient();

    private:
        int port{1717};
        IPType ipType{IPType::IPV4};
        int bufferSize{1024};
        int serverSocket{-1};
        int clientSocket{-1};
        std::string serverAddress{"127.0.0.1"};
    };

    class AsyncTCPServer {
    public:
        AsyncTCPServer();
        explicit AsyncTCPServer(int port);
        explicit AsyncTCPServer(std::string serverAddress);
        AsyncTCPServer(int port, std::string serverAddress);
        AsyncTCPServer(int port, std::string serverAddress, int bufferSize);
        AsyncTCPServer(int port, IPType ipType, int bufferSize);
        AsyncTCPServer(int port, IPType ipType, int bufferSize, std::string  serverAddress);
        ~AsyncTCPServer();

        bool CreateServer();
        bool BindPort();
        bool ListenServer();
        bool AcceptClient();
        bool AsyncHandleClient();
        bool SingleHandleClient(int &clientSocket);
        [[nodiscard]] bool SendData(const std::string& data, int clientSocket) const;
        [[nodiscard]] bool RecData(std::string& data, int clientSocket);

        bool CloseServer();
        bool CloseClient(int &clientSocket);

    private:
        int port{1717};
        IPType ipType{IPType::IPV4};
        int bufferSize{1024};
        int serverSocket{-1};
        std::vector<int> clientSockets;
        std::string serverAddress{"127.0.0.1"};
    };

}
