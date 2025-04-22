#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <boost/asio.hpp>
#include <boost/json.hpp> // 修改JSON头文件

class HTTPServer {
public:
    HTTPServer(int port);
    void start();
private:
    int port;
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;

    void do_accept();
    void handle_request(boost::asio::ip::tcp::socket socket);
    std::string parse_request(const std::string& request_data);
    void send_response(boost::asio::ip::tcp::socket& socket, const std::string& response);
};