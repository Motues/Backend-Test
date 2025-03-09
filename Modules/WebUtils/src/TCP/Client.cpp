#include <arpa/inet.h>

#include <utility>
#include "Backend/TCP/Client.hpp"

namespace Utils::TCP {

SingleTCPClient::SingleTCPClient() = default;

SingleTCPClient::SingleTCPClient(std::string serverAddress, int port) :
    serverAddress(std::move(serverAddress)), port(port) {}

SingleTCPClient::SingleTCPClient(std::string serverAddress, int port, IPType ipType) :
    serverAddress(std::move(serverAddress)), port(port), ipType(ipType) {}

SingleTCPClient::SingleTCPClient(std::string serverAddress, int port, IPType ipType, int bufferSize) :
    serverAddress(std::move(serverAddress)), port(port), ipType(ipType), bufferSize(bufferSize) {}

SingleTCPClient::~SingleTCPClient() {
    CloseConnection();
}

bool SingleTCPClient::ConnectToServer() {
    int domain;
    if (ipType == IPType::IPV4) domain = AF_INET;
    else domain = AF_INET6;
    clientSocket = socket(domain, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }
    std::cout << "Socket created" << std::endl;

    sockaddr_in serverAddressStruct = {0};
    serverAddressStruct.sin_family = domain;
    if (inet_pton(domain, serverAddress.c_str(), &serverAddressStruct.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return false;
    }
    serverAddressStruct.sin_port = htons(port);

    if (connect(clientSocket, (struct sockaddr *)&serverAddressStruct, sizeof(serverAddressStruct)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return false;
    }
    std::cout << "Connected to server " << serverAddress << ":" << port << std::endl;
    return true;
}

bool SingleTCPClient::SendData(const std::string& data) {
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

bool SingleTCPClient::RecvData(std::string& data) {
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
        std::cerr << "Server disconnected." << std::endl;
        return false;
    }
    data = std::string(buffer, bytesRead);
    std::cout << "Data received: " << data << std::endl;
    return true;
}

bool SingleTCPClient::CloseConnection() {
    if (clientSocket != -1) {
        close(clientSocket);
        clientSocket = -1;
    }
    return true;
}

}