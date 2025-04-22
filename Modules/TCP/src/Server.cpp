#include "Backend/TCP/Server.hpp"

namespace Utils::TCP {

#pragma region SingleTCPServer

SingleTCPServer::SingleTCPServer() = default;

SingleTCPServer::SingleTCPServer(int port) :
    port(port) {}
SingleTCPServer::SingleTCPServer(std::string serverAddress) :
     serverAddress(std::move(serverAddress)){}
SingleTCPServer::SingleTCPServer(int port, std::string serverAddress) :
    port(port), serverAddress(std::move(serverAddress)) {}
SingleTCPServer::SingleTCPServer(int port, IPType ipType) :
    port(port), ipType(ipType){}
SingleTCPServer::SingleTCPServer(int port, std::string  serverAddress, IPType ipType) :
    port(port), serverAddress(std::move(serverAddress)), ipType(ipType) {}

SingleTCPServer::~SingleTCPServer() {
    CloseClient();
    CloseServer();
}

bool SingleTCPServer::CreateServer() {
    ioContextPtr = std::make_shared<IOContext>();
    acceptorPtr = std::make_shared<TCPAcceptor>(*ioContextPtr);
    clientPtr = std::make_shared<TCPClient>(*ioContextPtr);
    if (!ioContextPtr || !acceptorPtr || !clientPtr) {
        std::cerr << "Failed to create io_context" << std::endl;
        return false;
    }
    return true;
}

bool SingleTCPServer::BindPort() {
    TCPEndPoint endpoint;
    if (ipType == IPType::IPV4) {
        endpoint = TCPEndPoint(
            boost::asio::ip::make_address(serverAddress), port);
    } else {
        endpoint = TCPEndPoint(
            boost::asio::ip::make_address_v6(serverAddress), port);
    }
    acceptorPtr->open(endpoint.protocol());
    acceptorPtr->set_option(boost::asio::socket_base::reuse_address(true));
    acceptorPtr->bind(endpoint);

    if (!acceptorPtr->is_open()) {
        std::cerr << "Failed to bind socket" << std::endl;
        return false;
    }
    std::cout << "Socket bound to address " << serverAddress << " and port " << port << std::endl;
    return true;
}

bool SingleTCPServer::ListenServer() {
    BoostErrorCode ec;
    acceptorPtr->listen(maxClientsNumber, ec);
    if (ec) {
        std::cerr << "Listen failed: " << ec.message() << std::endl;
        return false;
    }
    std::cout << "Server listening on port " << port << std::endl;
    return true;
}


bool SingleTCPServer::AcceptClient() {
    clientPtr->socket = TCPSocket(*ioContextPtr);
    BoostErrorCode ec;
    TCPEndPoint endpoint;
    acceptorPtr->accept(clientPtr->socket, endpoint, ec);
    if (ec) {
        std::cerr << "Accept failed: " << ec.message() << std::endl;
        return false;
    }
    clientPtr->address = endpoint.address().to_string();
    clientPtr->port = endpoint.port();
    std::cout << "Client connected successfully " << std::endl;
    return true;
}


bool SingleTCPServer::SendData(const std::string& data) const {
    if (!clientPtr || !clientPtr->socket.is_open()) {
        std::cerr << "Client socket not open" << std::endl;
        return false;
    }
    if (data.empty()) {
        std::cerr << "Data is empty" << std::endl;
        return false;
    }
    BoostErrorCode ec;
    boost::asio::write(clientPtr->socket, boost::asio::buffer(data), ec);
    if (ec) {
        std::cerr << "Failed to send data: " << ec.message() << std::endl;
        return false;
    }
    std::cout << "Data sent:" << data << std::endl;
    return true;
}


bool SingleTCPServer::RecData(std::string& data) {
    if (!clientPtr || !clientPtr->socket.is_open()) {
        std::cerr << "Client socket not open" << std::endl;
        return false;
    }
    BoostStreamBuffer buffer;
    BoostErrorCode ec;
    std::size_t bytes_transferred = boost::asio::read(clientPtr->socket, buffer,
                                                      boost::asio::transfer_at_least(1), ec);
    if (ec == boost::asio::error::eof) {
        std::cerr << "Server disconnected." << std::endl;
        return false;
    }
    if (ec) {
        std::cerr << "Failed to receive data: " << ec.message() << std::endl;
        return false;
    }
    data = std::string(static_cast<const char*>(buffer.data().data()), bytes_transferred);
//    data = std::string(boost::asio::buffer_cast<const char*>(buffer.data()), bytes_transferred);
    std::cout << "Data received: " << data << std::endl;
    return true;
}

bool SingleTCPServer::CloseClient() {
    if (clientPtr && clientPtr->socket.is_open()) {
        clientPtr->socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        clientPtr->socket.close();
    }
    return true;
}

bool SingleTCPServer::CloseServer() {
    if (acceptorPtr && acceptorPtr->is_open()) {
        acceptorPtr->close();
    }
    if (ioContextPtr) {
        ioContextPtr->stop();
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
AsyncTCPServer::AsyncTCPServer(int port, IPType ipType) :
    port(port), ipType(ipType){}
AsyncTCPServer::AsyncTCPServer(int port, std::string  serverAddress, IPType ipType) :
    port(port), ipType(ipType), serverAddress(std::move(serverAddress)) {}
AsyncTCPServer::~AsyncTCPServer() {
    CloseServer();
    for (auto& clientPtr : clientsPtr) {
        CloseClient(clientPtr);
    }
}

bool AsyncTCPServer::CreateServer() {
    ioContextPtr = std::make_shared<IOContext>();
    acceptorPtr = std::make_shared<TCPAcceptor>(*ioContextPtr);
    if (!ioContextPtr || !acceptorPtr) {
        std::cerr << "Failed to create io_context" << std::endl;
        return false;
    }
    // TODO 用于控制客户端数量
    // clientsPtr.reserve(maxClientsNumber);
    return true;
}
bool AsyncTCPServer::BindPort() {
    TCPEndPoint endpoint;
    if (ipType == IPType::IPV4) {
        endpoint = TCPEndPoint(
            boost::asio::ip::make_address(serverAddress), port);
    } else {
        endpoint = TCPEndPoint(
            boost::asio::ip::make_address_v6(serverAddress), port);
    }
    acceptorPtr->open(endpoint.protocol());
    acceptorPtr->set_option(boost::asio::socket_base::reuse_address(true));
    acceptorPtr->bind(endpoint);

    if (!acceptorPtr->is_open()) {
        std::cerr << "Failed to bind socket" << std::endl;
        return false;
    }
    std::cout << "Socket bound to address " << serverAddress << " and port " << port << std::endl;
    return true;
}
bool AsyncTCPServer::ListenServer() {
    BoostErrorCode ec;
    acceptorPtr->listen(maxClientsNumber, ec);
    if (ec) {
        std::cerr << "Listen failed: " << ec.message() << std::endl;
        return false;
    }
    std::cout << "Server listening on port " << port << std::endl;
    return true;
}
bool AsyncTCPServer::StartServer() {
    DoAccept();
    serverThread = std::thread([this]() {
        ioContextPtr->run();
    });

    return true;
}

//void AsyncTCPServer::StartRead(const TCPClientPtr& clientPtr) {
//    auto buffer = std::make_shared<BoostStreamBuffer>();
//    boost::asio::async_read_until(
//        clientPtr->socket, *buffer, '!',
//        [this, clientPtr, buffer](const BoostErrorCode& ec, std::size_t bytes_transferred) mutable {
//            if (!ec) {
//                std::cout << "Received data from client " << clientPtr->address << ":" << clientPtr->port << std::endl;
//
//                std::string data = std::string(boost::asio::buffer_cast<const char*>(buffer->data()), bytes_transferred);
//                buffer->consume(buffer->size());
//
//                TCPMessage msg;
//                msg.type = MessageType::DATA;
//                msg.data = data;
//                msg.time = std::chrono::system_clock::now();
//                messageQueue.push(msg);
//
//                StartRead(clientPtr); // 递归继续读取
//            } else {
//                std::cerr << "Failed to read data: " << ec.message() << std::endl;
//                CloseClient(clientPtr);
//            }
//        });
//}

void AsyncTCPServer::StartRead(const TCPClientPtr& clientPtr) {
    auto buffer = std::make_shared<std::array<char, 1024>>();
    clientPtr->socket.async_read_some(
        boost::asio::buffer(*buffer),
        [this, clientPtr, buffer](const BoostErrorCode& ec, std::size_t bytes_transferred) {
                if (!ec) {
                    std::string data(buffer->data(), bytes_transferred);
                    // 将接收到的数据直接存入消息队列
                    std::cout << "Received data from client " << clientPtr->address << ":" << clientPtr->port << std::endl;
                    TCPMessage msg;
                    msg.type = MessageType::DATA;
                    msg.data = data;
                    msg.time = std::chrono::system_clock::now();
                    messageQueue.push(msg);

                    // 清空缓冲区并继续读取
                    StartRead(clientPtr);
                } else {
                    if (ec != boost::asio::error::eof) {
                        std::cerr << "Read error: " << ec.message() << std::endl;
                    }
                    CloseClient(clientPtr);
                }
            });
}



void AsyncTCPServer::DoAccept() {
    if (!acceptorPtr || !acceptorPtr->is_open()) {
        std::cerr << "Server socket not open" << std::endl;
        return;
    }

    // 创建新客户端对象（无需预先绑定 socket）
    TCPClient client(*ioContextPtr);
    TCPClientPtr clientPtr = std::make_shared<TCPClient>(std::move(client));

    // 直接使用异步 accept
    acceptorPtr->async_accept(
        clientPtr->socket,  // 目标 socket
        [this, clientPtr](const BoostErrorCode& ec) {
            if (!ec) {
                // 获取客户端地址和端口（通过 socket 的 remote_endpoint()）
                TCPEndPoint endpoint = clientPtr->socket.remote_endpoint();
                clientPtr->address = endpoint.address().to_string();
                clientPtr->port = endpoint.port();

                std::cout << "Client connected. Address: " << clientPtr->address
                          << ", Port: " << clientPtr->port << std::endl;

                clientsPtr.push_back(clientPtr);
                StartRead(clientPtr);
            } else {
                std::cerr << "Failed to accept client: " << ec.message() << std::endl;
            }

            // 继续监听新连接
            DoAccept();
        }
    );
}



bool AsyncTCPServer::SendData(const std::string& data, TCPClientPtr& clientPtr) const {
    if (!clientPtr || !clientPtr->socket.is_open()) {
        std::cerr << "Client socket not open" << std::endl;
        return false;
    }
    if (data.empty()) {
        std::cerr << "Data is empty" << std::endl;
        return false;
    }
    BoostErrorCode ec;
    boost::asio::write(clientPtr->socket, boost::asio::buffer(data), ec);
    if (ec) {
        std::cerr << "Failed to send data: " << ec.message() << std::endl;
        return false;
    }
    std::cout << "Data sent:" << data << std::endl;
    return true;
}
bool AsyncTCPServer::RecData(std::string& data, TCPClientPtr& clientPtr) {
    if (!clientPtr || !clientPtr->socket.is_open()) {
        std::cerr << "Client socket not open" << std::endl;
        return false;
    }
    BoostStreamBuffer buffer;
    BoostErrorCode ec;
    std::size_t bytes_transferred = boost::asio::read(clientPtr->socket, buffer,
                                                      boost::asio::transfer_at_least(1), ec);
    if (ec == boost::asio::error::eof) {
        std::cerr << "Server disconnected." << std::endl;
        return false;
    }
    if (ec) {
        std::cerr << "Failed to receive data: " << ec.message() << std::endl;
        return false;
    }
    const boost::asio::const_buffer data_buffer = buffer.data();
    data = std::string(static_cast<const char*>(data_buffer.data()), bytes_transferred);
//    data = std::string(boost::asio::buffer_cast<const char*>(buffer.data()), bytes_transferred);
    std::cout << "Data received: " << data << std::endl;
    return true;
}

bool AsyncTCPServer::GetMessage(TCPMessage &message) {
    if (messageQueue.empty()) {
        return false;
    }
    message = messageQueue.pop();
    return true;
}

bool AsyncTCPServer::CloseServer() {
    if (acceptorPtr && acceptorPtr->is_open()) {
        acceptorPtr->close();
    }
    if (ioContextPtr) {
        ioContextPtr->stop();
    }
    return true;
}
bool AsyncTCPServer::CloseClient(const TCPClientPtr& clientPtr) {
    if (clientPtr && clientPtr->socket.is_open()) {
        clientPtr->socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        clientPtr->socket.close();
    }
    return true;
}

#pragma region AsyncTCPServer

}
