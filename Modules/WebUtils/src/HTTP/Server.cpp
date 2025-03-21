#include "Backend/HTTP/Server.hpp"

HTTPServer::HTTPServer(int port) : port(port), serverSocket(0) {}

void HTTPServer::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to bind socket" << std::endl;
        close(serverSocket);
        return;
    }

    if (listen(serverSocket, 10) == -1) {
        std::cerr << "Failed to listen on socket" << std::endl;
        close(serverSocket);
        return;
    }

    std::cout << "Server started on port " << port << std::endl;

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        handleClient(clientSocket);
        close(clientSocket);
    }
}

void HTTPServer::handleClient(int clientSocket) {
    std::string request = parseRequest(clientSocket);
    if (request.empty()) {
        sendResponse(clientSocket, "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n");
        return;
    }

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    std::istringstream requestStream(request);

    if (!Json::parseFromStream(builder, requestStream, &root, &errs)) {
        sendResponse(clientSocket, "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n");
        return;
    }

    // Process the JSON request and generate a JSON response
    Json::Value responseRoot;
    responseRoot["status"] = "success";
    responseRoot["message"] = "Received your JSON data";

    Json::StreamWriterBuilder writer;
    std::string response = Json::writeString(writer, responseRoot);

    std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " +
                               std::to_string(response.length()) + "\r\n\r\n" + response;

    sendResponse(clientSocket, httpResponse);
}

std::string HTTPServer::parseRequest(int clientSocket) {
    char buffer[1024];
    std::string request;
    ssize_t bytesRead;

    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesRead] = '\0';
        request += buffer;

        // Check if the request is complete (simple check for demonstration purposes)
        if (request.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    }

    if (bytesRead == -1) {
        std::cerr << "Failed to read request" << std::endl;
        return "";
    }

    return request;
}

void HTTPServer::sendResponse(int clientSocket, const std::string& response) {
    if (send(clientSocket, response.c_str(), response.length(), 0) == -1) {
        std::cerr << "Failed to send response" << std::endl;
    }
}