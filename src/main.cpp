
#include "log/logger.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

using namespace aime::log;

int main() {
  Logger logger;
  logger << LOG(info) << "Starting web server at localhost:8080";

  try {
    boost::asio::io_context ioc{1};
    tcp::acceptor acceptor{ioc, tcp::endpoint(tcp::v4(), 8080)};
    for (;;) {
      tcp::socket socket{ioc};
      acceptor.accept(socket);
      logger << LOG(info) << "Client connected: " << socket.remote_endpoint();
      boost::beast::flat_buffer buffer;
      http::request<http::string_body> req;
      http::read(socket, buffer, req);
      logger << LOG(info) << "Received request: " << req.method_string() << " "
             << req.target();

      if (req.method() == http::verb::get) {
        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, "Boost.Beast");
        res.set(http::field::content_type, "text/plain");
        res.body() = "Hello, World!\n";
        res.prepare_payload();
        logger << LOG(info) << "Sending response: " << res.result_int()
               << ", body: " << res.body();
        http::write(socket, res);
      } else {
        logger << LOG(warning)
               << "Unsupported request method: " << req.method_string();
      }

      boost::system::error_code ec;
      socket.shutdown(tcp::socket::shutdown_send, ec);
      if (ec) {
        logger << LOG(error) << "Socket shutdown error: " << ec.message();
      }
    }
  } catch (const std::exception &e) {
    logger << LOG(error) << "Error: " << e.what();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
