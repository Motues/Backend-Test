#include "Backend/HTTP/Server.hpp"

HTTPServer::HTTPServer(int port)
    : port(port),
      acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      socket(io_context) {}

void HTTPServer::start() {
    do_accept();
    io_context.run();
}

void HTTPServer::do_accept() {
    acceptor.async_accept(socket, [this](const boost::system::error_code& ec) {
        if (!ec) {
            handle_request(std::move(socket));
        }
        do_accept(); // 保持accept循环
    });
}

void HTTPServer::handle_request(boost::asio::ip::tcp::socket socket) {
    boost::asio::streambuf request_buf;
    boost::asio::async_read_until(socket, request_buf, "\r\n\r\n", [this, socket = std::move(socket), &request_buf](const boost::system::error_code& ec, size_t bytes_transferred) mutable {
        if (!ec) {
            std::string request_data(boost::asio::buffer_cast<const char*>(request_buf.data()), bytes_transferred);

            // 使用Boost JSON解析
            boost::json::error_code ec;
            auto root = boost::json::parse(request_data, ec); // 解析并处理错误
            if (!ec) {
                // 构建响应对象
                boost::json::object responseRoot{
                    {"status", "success"},
                    {"message", "Received your JSON data"}
                };

                std::string response = boost::json::serialize(responseRoot); // 序列化为JSON字符串

                std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " +
                                           std::to_string(response.length()) + "\r\n\r\n" + response;

                boost::asio::async_write(socket, boost::asio::buffer(httpResponse), [socket = std::move(socket)](const boost::system::error_code&, size_t) mutable {
                    socket.close();
                });
            } else {
                send_response(socket, "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n");
            }
        }
    });
}

void HTTPServer::send_response(boost::asio::ip::tcp::socket& socket, const std::string& response) {
    boost::asio::async_write(socket, boost::asio::buffer(response), [socket = std::move(socket)](const boost::system::error_code&, size_t) mutable {
        socket.close();
    });
}
