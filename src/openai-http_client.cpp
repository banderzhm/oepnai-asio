// HTTP Client Module - Implementation

module;  // 全局模块片段

#include <openssl/ssl.h>

module openai.http_client;

import asio;
import fmt;
import std;

namespace openai::http {

Client::Client(asio::io_context& io_context)
    : io_context_(io_context)
    , ssl_context_(asio::ssl::context::tlsv12_client) {
    
    ssl_context_.set_default_verify_paths();
    ssl_context_.set_verify_mode(asio::ssl::verify_none);
}

asio::awaitable<Response> Client::async_request(const Request& req) {
    if (req.use_ssl) {
        co_return co_await async_https_request(req);
    } else {
        co_return co_await async_http_request(req);
    }
}

Response Client::request(const Request& req) {
    Response result;
    
    asio::co_spawn(
        io_context_,
        [this, req, &result]() -> asio::awaitable<void> {
            result = co_await async_request(req);
        },
        asio::detached
    );
    
    io_context_.run();
    io_context_.restart();
    
    return result;
}

asio::awaitable<Response> Client::async_https_request(const Request& req) {
    try {
        auto executor = co_await asio::this_coro::executor;
        asio::ssl::stream<asio::ip::tcp::socket> socket(executor, ssl_context_);
        
        SSL_set_tlsext_host_name(socket.native_handle(), req.host.c_str());

        asio::ip::tcp::resolver resolver(executor);
        auto endpoints = co_await resolver.async_resolve(
            req.host, "443", asio::use_awaitable
        );

        co_await asio::async_connect(
            socket.lowest_layer(), endpoints, asio::use_awaitable
        );

        co_await socket.async_handshake(
            asio::ssl::stream_base::client, asio::use_awaitable
        );

        std::string request_str = build_request_string(req);
        co_await asio::async_write(
            socket, asio::buffer(request_str), asio::use_awaitable
        );

        asio::streambuf response_buf;
        co_await asio::async_read_until(
            socket, response_buf, "\r\n", asio::use_awaitable
        );

        std::istream response_stream(&response_buf);
        std::string http_version;
        int status_code;
        std::string status_message;
        
        response_stream >> http_version >> status_code;
        std::getline(response_stream, status_message);

        co_await asio::async_read_until(
            socket, response_buf, "\r\n\r\n", asio::use_awaitable
        );
        
        std::map<std::string, std::string> headers;
        std::string header_line;
        while (std::getline(response_stream, header_line) && header_line != "\r") {
            auto pos = header_line.find(':');
            if (pos != std::string::npos) {
                std::string key = header_line.substr(0, pos);
                std::string value = header_line.substr(pos + 2);
                if (!value.empty() && value.back() == '\r') {
                    value.pop_back();
                }
                headers[key] = value;
            }
        }

        std::ostringstream body_stream;
        if (response_buf.size() > 0) {
            body_stream << &response_buf;
        }

        auto content_length_it = headers.find("Content-Length");
        if (content_length_it != headers.end()) {
            size_t content_length = std::stoull(content_length_it->second);
            size_t remaining = content_length - body_stream.str().size();
            
            if (remaining > 0) {
                std::vector<char> buffer(remaining);
                co_await asio::async_read(
                    socket, asio::buffer(buffer), asio::use_awaitable
                );
                body_stream.write(buffer.data(), buffer.size());
            }
        } else {
            std::error_code ec;
            while (true) {
                std::size_t bytes_transferred = co_await asio::async_read(
                    socket, response_buf, asio::transfer_at_least(1),
                    asio::redirect_error(asio::use_awaitable, ec)
                );
                
                if (ec == asio::error::eof) break;
                if (ec) {
                    co_return Response{0, "", {}, true, 
                        fmt::format("Error reading body: {}", ec.message())};
                }
                
                body_stream << &response_buf;
            }
        }

        std::error_code shutdown_ec;
        co_await socket.async_shutdown(
            asio::redirect_error(asio::use_awaitable, shutdown_ec)
        );

        co_return Response{status_code, body_stream.str(), headers, false, ""};

    } catch (const std::exception& e) {
        co_return Response{0, "", {}, true, 
            fmt::format("HTTPS request failed: {}", e.what())};
    }
}

asio::awaitable<Response> Client::async_http_request(const Request& req) {
    try {
        auto executor = co_await asio::this_coro::executor;
        asio::ip::tcp::socket socket(executor);

        asio::ip::tcp::resolver resolver(executor);
        auto endpoints = co_await resolver.async_resolve(
            req.host, "80", asio::use_awaitable
        );

        co_await asio::async_connect(socket, endpoints, asio::use_awaitable);

        std::string request_str = build_request_string(req);
        co_await asio::async_write(
            socket, asio::buffer(request_str), asio::use_awaitable
        );

        asio::streambuf response_buf;
        co_await asio::async_read_until(
            socket, response_buf, "\r\n", asio::use_awaitable
        );

        std::istream response_stream(&response_buf);
        std::string http_version;
        int status_code;
        std::string status_message;
        
        response_stream >> http_version >> status_code;
        std::getline(response_stream, status_message);

        co_await asio::async_read_until(
            socket, response_buf, "\r\n\r\n", asio::use_awaitable
        );
        
        std::map<std::string, std::string> headers;
        std::string header_line;
        while (std::getline(response_stream, header_line) && header_line != "\r") {
            auto pos = header_line.find(':');
            if (pos != std::string::npos) {
                std::string key = header_line.substr(0, pos);
                std::string value = header_line.substr(pos + 2);
                if (!value.empty() && value.back() == '\r') {
                    value.pop_back();
                }
                headers[key] = value;
            }
        }

        std::ostringstream body_stream;
        if (response_buf.size() > 0) {
            body_stream << &response_buf;
        }

        std::error_code ec;
        while (true) {
            co_await asio::async_read(
                socket, response_buf, asio::transfer_at_least(1),
                asio::redirect_error(asio::use_awaitable, ec)
            );
            
            if (ec == asio::error::eof) break;
            if (ec) {
                co_return Response{0, "", {}, true, 
                    fmt::format("Error reading body: {}", ec.message())};
            }
            
            body_stream << &response_buf;
        }

        socket.close();
        co_return Response{status_code, body_stream.str(), headers, false, ""};

    } catch (const std::exception& e) {
        co_return Response{0, "", {}, true, 
            fmt::format("HTTP request failed: {}", e.what())};
    }
}

std::string Client::build_request_string(const Request& req) const {
    std::ostringstream request;
    request << req.method << " " << req.path << " HTTP/1.1\r\n";
    request << "Host: " << req.host << "\r\n";
    
    for (const auto& [key, value] : req.headers) {
        request << key << ": " << value << "\r\n";
    }
    
    if (!req.body.empty()) {
        request << "Content-Length: " << req.body.size() << "\r\n";
    }
    
    request << "Connection: close\r\n\r\n";
    
    if (!req.body.empty()) {
        request << req.body;
    }
    
    return request.str();
}

} // namespace openai::http