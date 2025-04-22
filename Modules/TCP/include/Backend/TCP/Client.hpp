#pragma once

#include <memory>
#include <iostream>

#include "BasicType.hpp"

namespace Utils::TCP {

class SingleTCPClient {
public:
    SingleTCPClient();
    SingleTCPClient(std::string serverAddress, int port);
    SingleTCPClient(std::string serverAddress, int port, IPType ipType);
    ~SingleTCPClient();

    bool ConnectToServer();
    bool SendData(const std::string& data);
    bool RecvData(std::string& data);
    bool CloseConnection();

private:
    std::string serverAddress{"127.0.0.1"};
    int port{1717};
    IPType ipType{IPType::IPV4};
    TCPSocketPtr serverSocketPtr;
};

}