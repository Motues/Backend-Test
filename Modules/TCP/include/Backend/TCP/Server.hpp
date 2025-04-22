#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <iostream>

#include "BasicType.hpp"

#include "../../../../../CommonUtils/MessageQueue/include/Utils/MessageQueue.hpp"

namespace Utils :: TCP {

using Utils::MessageQueue::MessageQueue;

class SingleTCPServer {
public:
    SingleTCPServer();
    SingleTCPServer(int port);
    SingleTCPServer(std::string serverAddress);
    SingleTCPServer(int port, std::string serverAddress);
    SingleTCPServer(int port, IPType ipType);
    SingleTCPServer(int port, std::string serverAddress, IPType ipType);
    ~SingleTCPServer();

    bool CreateServer();
    bool BindPort();
    bool ListenServer();
    bool AcceptClient();
    bool SendData(const std::string& data) const;
    bool RecData(std::string& data);

    bool CloseServer();
    bool CloseClient();

private:
    int port{1717};
    std::string serverAddress{"127.0.0.1"};
    IPType ipType{IPType::IPV4};
    int maxClientsNumber{1024};
    IOContextPtr ioContextPtr;
    TCPAcceptorPtr acceptorPtr;
    TCPClientPtr clientPtr;
};

class AsyncTCPServer {
public:
    AsyncTCPServer();
    AsyncTCPServer(int port);
    AsyncTCPServer(std::string serverAddress);
    AsyncTCPServer(int port, std::string serverAddress);
    AsyncTCPServer(int port, IPType ipType);
    AsyncTCPServer(int port, std::string serverAddress, IPType ipType);
    ~AsyncTCPServer();

    bool CreateServer();
    bool BindPort();
    bool ListenServer();
    bool AcceptClient();
    bool SendData(const std::string& data, TCPClientPtr& clientPtr) const;
    bool RecData(std::string& data, TCPClientPtr& clientPtr);
    bool GetMessage(TCPMessage &message);

    bool StartServer();

    bool CloseServer();
    bool CloseClient(const TCPClientPtr& clientPtr);

private:
    int port{1717};
    std::string serverAddress{"127.0.0.1"};
    IPType ipType{IPType::IPV4};
    int maxClientsNumber{1024};
    IOContextPtr ioContextPtr;
    TCPAcceptorPtr acceptorPtr;
    std::vector<TCPClientPtr> clientsPtr;
    MessageQueue<TCPMessage> messageQueue;
    std::thread serverThread;

    void DoAccept();
    void StartRead(const TCPClientPtr& clientPtr);
};

}