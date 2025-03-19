#include <iostream>
#include "Backend/TCP/Server.hpp"

using namespace Utils::TCP;



int main() {
    AsyncTCPServer server(1717, "127.0.0.1", 1024);
    if (!server.CreateServer() || !server.BindPort() || !server.ListenServer()) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }

    server.AsyncHandleClient();


    return 0;
}