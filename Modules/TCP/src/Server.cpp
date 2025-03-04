#include "Backend/TCP/Server.hpp"

namespace Utils :: TCP{

//bool TCPServer::CreateServer() {
//    ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
//    if (ServerSocket == -1) {
//        return false;
//    }
//    return true;
//}

TCPServer::~TCPServer() {
    if (ServerSocket != -1) {
        close(ServerSocket);
    }
    if (ClientSocket != -1) {
        close(ClientSocket);
    }
}


bool TCPServer::CreateServer() {
    int domain;
    if (IPType == IPV4) domain = AF_INET;
    else domain = AF_INET6;
    ServerSocket = socket(domain, SOCK_STREAM, 0);
    if (ServerSocket == -1) {
        std :: cerr << "Failed to create socket" << std :: endl;
        return false;
    }
    std::cout << "Socket created" << std::endl;
    return true;
}

}
