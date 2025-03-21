#include "Backend/TCP/Client.hpp"

namespace Utils::TCP {

SingleTCPClient::SingleTCPClient() = default;

SingleTCPClient::SingleTCPClient(std::string serverAddress, int port) :
    serverAddress(std::move(serverAddress)), port(port) {}

SingleTCPClient::SingleTCPClient(std::string serverAddress, int port, IPType ipType) :
    serverAddress(std::move(serverAddress)), port(port), ipType(ipType) {}

SingleTCPClient::~SingleTCPClient() {
    CloseConnection();
}

bool SingleTCPClient::ConnectToServer() {
    IOContext io_context;
    TCPResolver resolver(io_context);
    TCPEndPoint endpoint;
    BoostErrorCode ec;

    auto endpoints = resolver.resolve(serverAddress, std::to_string(port), ec);
    if (ec) {
        std::cerr << "Resolution failed: " << ec.message() << std::endl;
        return false;
    }
    endpoint = *endpoints.begin();

    serverSocketPtr = std::make_shared<TCPSocket>(io_context);
    serverSocketPtr->connect(endpoint, ec);
    if (ec) {
        std::cerr << "Connection failed: " << ec.message() << std::endl;
        return false;
    }
    std::cout << "Connected to server " << serverAddress << ":" << port << std::endl;
    return true;
}


bool SingleTCPClient::SendData(const std::string& data) {
    if (!serverSocketPtr || !serverSocketPtr->is_open()) {
        std::cerr << "Client socket not open" << std::endl;
        return false;
    }
    if (data.empty()) {
        std::cerr << "Data is empty" << std::endl;
        return false;
    }
    BoostErrorCode ec;
    boost::asio::write(*serverSocketPtr, boost::asio::buffer(data), ec);
    if (ec) {
        std::cerr << "Failed to send data: " << ec.message() << std::endl;
        return false;
    }
    std::cout << "Data sent:" << data << std::endl;
    return true;
}

bool SingleTCPClient::RecvData(std::string& data) {
    if (!serverSocketPtr || !serverSocketPtr->is_open()) {
        std::cerr << "Client socket not open" << std::endl;
        return false;
    }
    BoostStreamBuffer buffer;
    BoostErrorCode ec;
    std::size_t bytes_transferred = boost::asio::read(*serverSocketPtr, buffer,
        boost::asio::transfer_at_least(1), ec);
    if (ec == boost::asio::error::eof) {
        std::cerr << "Server disconnected." << std::endl;
        return false;
    }
    if (ec) {
        std::cerr << "Failed to receive data: " << ec.message() << std::endl;
        return false;
    }
    data = std::string(boost::asio::buffer_cast<const char*>(buffer.data()), bytes_transferred);
    std::cout << "Data received: " << data << std::endl;
    return true;
}

bool SingleTCPClient::CloseConnection() {
    if (serverSocketPtr && serverSocketPtr->is_open()) {
        serverSocketPtr->close();
    }
    return true;
}

}