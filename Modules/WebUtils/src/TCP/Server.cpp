#include <arpa/inet.h>
#include <sys/select.h>
#include <utility>
#include "Backend/TCP/Server.hpp"

namespace Utils :: TCP{

#pragma region SingleTCPServer

SingleTCPServer::SingleTCPServer() = default;

SingleTCPServer::SingleTCPServer(int port) :
    port(port) {}
SingleTCPServer::SingleTCPServer(std::string serverAddress) :
     serverAddress(std::move(serverAddress)){}
SingleTCPServer::SingleTCPServer(int port, std::string serverAddress) :
    port(port), serverAddress(std::move(serverAddress)) {}
SingleTCPServer::SingleTCPServer(int port, std::string serverAddress, int bufferSize) :
    port(port), serverAddress(std::move(serverAddress)), bufferSize(bufferSize) {}
SingleTCPServer::SingleTCPServer(int port, IPType ipType, int bufferSize) :
    port(port), ipType(ipType), bufferSize(bufferSize) {}
SingleTCPServer::SingleTCPServer(int port, IPType ipType, int bufferSize, std::string  serverAddress) :
    port(port), ipType(ipType), bufferSize(bufferSize), serverAddress(std::move(serverAddress)) {}

SingleTCPServer::~SingleTCPServer() {
    CloseClient();
    CloseServer();
}


bool SingleTCPServer::CreateServer() {
    int domain;
    if (ipType == IPType::IPV4) domain = AF_INET;
    else domain = AF_INET6;
    serverSocket = socket(domain, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    int opt = 1; // 启用 SO_REUSEADDR 选项
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Failed to set SO_REUSEADDR option" << std::endl;
        close(serverSocket);
        return false;
    }

    std::cout << "Socket created" << std::endl;
    return true;
}

bool SingleTCPServer::BindPort() {
    int domain;
    if (ipType == IPType::IPV4) domain = AF_INET;
    else domain = AF_INET6;

    sockaddr_in address = {0};
    address.sin_family = domain;

    // 使用 serverAddress 而不是固定绑定到 INADDR_ANY
    if (inet_pton(domain, serverAddress.c_str(), &address.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return false;
    }

    address.sin_port = htons(port);
    if (bind(serverSocket, (sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind socket to port" << std::endl;
        return false;
    }
    std::cout << "Socket bound to address " << serverAddress << " and port " << port << std::endl;
    return true;
}

bool SingleTCPServer::ListenServer(){
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return false;
    }
    std::cout << "Server listening on port " << port << std::endl;
    return true;
}

bool SingleTCPServer::AcceptClient() {
    if (serverSocket == -1) {
        std::cerr << "Server socket not created" << std::endl;
        return false;
    }
    sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket < 0) {
        std::cerr << "Failed to accept connection." << std::endl;
        return false;
    }
    std::cout << "Client connected." << std::endl;
    std::cout << "Client address and port: " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) <<std::endl;
    return true;
}

bool SingleTCPServer::SendData(const std::string& data) const {
    if (clientSocket == -1) {
        std::cerr << "Client socket not created" << std::endl;
        return false;
    }
    if (data.empty()) {
        std::cerr << "Data is empty" << std::endl;
        return false;
    }
    if (data.size() > bufferSize) {
        std::cerr << "Data is too large" << std::endl;
        return false;
    }
    if (send(clientSocket, data.c_str(), data.size(), 0) < 0) {
        std::cerr << "Failed to send data." << std::endl;
        return false;
    }
    std::cout << "Data sent:" << data << std::endl;
    return true;
}

bool SingleTCPServer::RecData(std::string& data) {
    if (clientSocket == -1) {
        std::cerr << "Client socket not created" << std::endl;
        return false;
    }
    char buffer[bufferSize];
    ssize_t bytesRead = recv(clientSocket, buffer, bufferSize, 0);
    if (bytesRead < 0) {
        std::cerr << "Failed to receive data." << std::endl;
        return false;
    }
    if (bytesRead == 0) {
        std::cerr << "Client disconnected." << std::endl;
        return false;
    }
    data = std::string(buffer, bytesRead);
    std::cout << "Data received: " << data << std::endl;
    return true;
}

bool SingleTCPServer::CloseServer() {
    if (serverSocket != -1) {
        close(serverSocket);
        serverSocket = -1;
    }
    return true;
}

bool SingleTCPServer::CloseClient() {
    if (clientSocket != -1) {
        close(clientSocket);
        clientSocket = -1;
    }
    return true;
}

#pragma endregion SingleTCPServer

#pragma region AsyncTCPServer

AsyncTCPServer::AsyncTCPServer() = default;
AsyncTCPServer::AsyncTCPServer(int port) :
    port(port) {}
AsyncTCPServer::AsyncTCPServer(std::string serverAddress) :
    serverAddress(std::move(serverAddress)){}
AsyncTCPServer::AsyncTCPServer(int port, std::string serverAddress) :
    port(port), serverAddress(std::move(serverAddress)) {}
AsyncTCPServer::AsyncTCPServer(int port, std::string serverAddress, int bufferSize) :
    port(port), serverAddress(std::move(serverAddress)), bufferSize(bufferSize) {}
AsyncTCPServer::AsyncTCPServer(int port, IPType ipType, int bufferSize) :
    port(port), ipType(ipType), bufferSize(bufferSize) {}
AsyncTCPServer::AsyncTCPServer(int port, IPType ipType, int bufferSize, std::string  serverAddress) :
    port(port), ipType(ipType), bufferSize(bufferSize), serverAddress(std::move(serverAddress)) {}
AsyncTCPServer::~AsyncTCPServer() {
    CloseServer();
    for (auto& client : clientSockets) {
        CloseClient(client);
    }
}

bool AsyncTCPServer::CreateServer() {
    int domain;
    if (ipType == IPType::IPV4) domain = AF_INET;
    else domain = AF_INET6;
    serverSocket = socket(domain, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    int opt = 1; // 启用 SO_REUSEADDR 选项
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Failed to set SO_REUSEADDR option" << std::endl;
        close(serverSocket);
        return false;
    }

    std::cout << "Socket created" << std::endl;
    return true;
}
bool AsyncTCPServer::BindPort() {
    int domain;
    if (ipType == IPType::IPV4) domain = AF_INET;
    else domain = AF_INET6;

    sockaddr_in address = {0};
    address.sin_family = domain;

    // 使用 serverAddress 而不是固定绑定到 INADDR_ANY
    if (inet_pton(domain, serverAddress.c_str(), &address.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return false;
    }

    address.sin_port = htons(port);
    if (bind(serverSocket, (sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind socket to port" << std::endl;
        return false;
    }
    std::cout << "Socket bound to address " << serverAddress << " and port " << port << std::endl;
    return true;
}
bool AsyncTCPServer::ListenServer() {
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return false;
    }
    std::cout << "Server listening on port " << port << std::endl;
    return true;
}
bool AsyncTCPServer::AcceptClient() {
    if (serverSocket == -1) {
        std::cerr << "Server socket not created" << std::endl;
        return false;
    }
    sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket < 0) {
        std::cerr << "Failed to accept connection." << std::endl;
        return false;
    }
    clientSockets.push_back(clientSocket);
    std::cout << "Client connected." << std::endl;
    std::cout << "Client address and port: " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) <<std::endl;
    return true;
}

bool AsyncTCPServer::AsyncHandleClient() {
    fd_set read_fds;
    int activity;
    int sd;
    int max_sd;

    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(serverSocket, &read_fds);
        max_sd = serverSocket;

        for (auto& clientSocket : clientSockets) {
            FD_SET(clientSocket, &read_fds);
            if (clientSocket > max_sd) {
                max_sd = clientSocket;
            }
        }

        activity = select(max_sd + 1, &read_fds, nullptr, nullptr, nullptr);

        if ((activity < 0) && (errno != EINTR)) {
            std::cerr << "Select error" << std::endl;
        }

        if (FD_ISSET(serverSocket, &read_fds)) {
            if (!AcceptClient()) {
                std::cerr << "Failed to accept client" << std::endl;
            }
        }

        for (auto it = clientSockets.begin(); it != clientSockets.end();) {
            sd = *it;
            if (sd == -1) {
                clientSockets.erase(it);
                continue;
            }
            if (FD_ISSET(sd, &read_fds)) {
                std::string recData;
                if (!RecData(recData, sd)) {
                    std::cerr << "Failed to receive data." << std::endl;
                    CloseClient(sd);
                    it = clientSockets.erase(it);
                    continue;
                }

                if (!SendData("Message Received", sd)) {
                    std::cerr << "Failed to send data." << std::endl;
                }
            }
            ++it;
        }
    }
}
bool AsyncTCPServer::SingleHandleClient(int &clientSocket) {
    if (clientSocket == -1) {
        std::cerr << "Client socket not created" << std::endl;
        return false;
    }
    while (true) {
        std::string recData;
        if (!RecData(recData, clientSocket)) {
            std::cerr << "Failed to receive data." << std::endl;
            CloseClient(clientSocket);
            return false;
        }
        if (!SendData("Message Received", clientSocket)) {
            std::cerr << "Failed to send data." << std::endl;
            return false;
        }
    }
}

bool AsyncTCPServer::SendData(const std::string& data, int clientSocket) const {
    if (clientSocket == -1) {
        std::cerr << "Client socket not created" << std::endl;
        return false;
    }
    if (data.empty()) {
        std::cerr << "Data is empty" << std::endl;
        return false;
    }
    if (data.size() > bufferSize) {
        std::cerr << "Data is too large" << std::endl;
        return false;
    }
    if (send(clientSocket, data.c_str(), data.size(), 0) < 0) {
        std::cerr << "Failed to send data." << std::endl;
        return false;
    }
    std::cout << "Data sent:" << data << std::endl;
    return true;
}
bool AsyncTCPServer::RecData(std::string& data, int clientSocket) {
    if (clientSocket == -1) {
        std::cerr << "Client socket not created" << std::endl;
        return false;
    }
    char buffer[bufferSize];
    ssize_t bytesRead = recv(clientSocket, buffer, bufferSize, 0);
    if (bytesRead < 0) {
        std::cerr << "Failed to receive data." << std::endl;
        return false;
    }
    if (bytesRead == 0) {
        std::cerr << "Client disconnected." << std::endl;
        return false;
    }
    data = std::string(buffer, bytesRead);
    std::cout << "Data received: " << data << std::endl;
    return true;
}

bool AsyncTCPServer::CloseServer() {
    if (serverSocket != -1) {
        close(serverSocket);
        serverSocket = -1;
    }
    return true;
}
bool AsyncTCPServer::CloseClient(int &clientSocket) {
    if (clientSocket != -1) {
        close(clientSocket);
        clientSocket = -1;
    }
    return true;
}

#pragma region AsyncTCPServer

}
