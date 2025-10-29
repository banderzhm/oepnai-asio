// HTTP Client Module
// Provides async HTTP/HTTPS client with coroutine support

module;

// Include Asio SSL headers (asio module doesn't export SSL components)
#include <asio/ssl.hpp>

export module openai.http_client;

import asio;
import fmt;
import std;

export namespace openai::http {

// HTTP Response structure
struct Response {
    int status_code{0};
    std::string body;
    std::map<std::string, std::string> headers;
    bool is_error{false};
    std::string error_message;
};

// HTTP Request structure
struct Request {
    std::string method{"GET"};
    std::string path{"/"};
    std::string host;
    std::string body;
    std::map<std::string, std::string> headers;
    bool use_ssl{true};
};

// Coroutine-based HTTPS Client using Asio
class Client {
public:
    explicit Client(asio::io_context& io_context);

    asio::awaitable<Response> async_request(const Request& req);
    Response request(const Request& req);

private:
    asio::awaitable<Response> async_https_request(const Request& req);
    asio::awaitable<Response> async_http_request(const Request& req);
    std::string build_request_string(const Request& req) const;

    asio::io_context& io_context_;
    asio::ssl::context ssl_context_;
};

} // namespace openai::http
