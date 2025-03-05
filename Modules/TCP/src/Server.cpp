#include <arpa/inet.h>
#include "Backend/TCP/Server.hpp"

namespace Utils :: TCP{

SingleTCPServer::SingleTCPServer() = default;
SingleTCPServer::SingleTCPServer(int port) : port(port) {}
SingleTCPServer::SingleTCPServer(int port, IPType ipType) : port(port), ipType(ipType) {}
SingleTCPServer::SingleTCPServer(int port, IPType ipType, int bufferSize) : port(port), ipType(ipType), bufferSize(bufferSize) {}
SingleTCPServer::SingleTCPServer(int port, IPType ipType, int bufferSize, const std::string& serverAddress)
    : port(port), ipType(ipType), bufferSize(bufferSize), serverAddress(serverAddress) {}

SingleTCPServer::~SingleTCPServer() {
    CloseServer();
    CloseClient();
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

    // 设置 SO_REUSEADDR 选项
    int optval = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        std::cerr << "Failed to set SO_REUSEADDR option" << std::endl;
        close(serverSocket);
        return false;
    }

    std::cout << "Socket created" << std::endl;
    return true;
}

bool SingleTCPServer::BindPort(int Port) {
    int domain;
    if (ipType == IPType::IPV4) domain = AF_INET;
    else domain = AF_INET6;

    sockaddr_in address = {0};
    port = Port;
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

bool SingleTCPServer::ListenServer() {
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

bool SingleTCPServer::SendData(const std::string& data) {
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

bool SingleTCPServer::RecvData(std::string& data) {
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

}
