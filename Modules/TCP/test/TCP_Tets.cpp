#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // 创建socket文件描述符
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }


    // 绑定socket到端口
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    // 监听连接
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    // 接受连接
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        std::cerr << "Accept failed" << std::endl;
        return -1;
    }

    // 读取客户端发送的数据
    int valread = read(new_socket, buffer, BUFFER_SIZE);
    std::cout << "Message from client: " << buffer << std::endl;

    // 发送响应给客户端
    const char *response = "Hello from server";
    send(new_socket, response, strlen(response), 0);
    std::cout << "Response sent to client" << std::endl;

    // 关闭socket
    close(new_socket);
    close(server_fd);

    return 0;
}