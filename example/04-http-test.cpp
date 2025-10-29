// Example 04: Direct HTTP Client Test
// Demonstrates the underlying HTTP client

import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> http_test_example(asio::io_context& io_context) {
    fmt::print("=== HTTP Client Test Example ===\n\n");
    
    openai::http::Client http_client(io_context);
    
    // Test 1: Simple GET request
    fmt::print("--- Test 1: GET Request ---\n");
    {
        openai::http::Request request;
        request.method = "GET";
        request.host = "httpbin.org";
        request.path = "/get";
        request.use_ssl = true;
        
        try {
            auto response = co_await http_client.async_request(request);
            
            if (response.is_error) {
                fmt::print("Error: {}\n", response.error_message);
            } else {
                fmt::print("Status: {}\n", response.status_code);
                fmt::print("Headers:\n");
                for (const auto& [key, value] : response.headers) {
                    fmt::print("  {}: {}\n", key, value);
                }
                fmt::print("\nBody (first 300 chars):\n{}\n", 
                    response.body.substr(0, std::min(size_t(300), response.body.size())));
                if (response.body.size() > 300) {
                    fmt::print("... (truncated)\n");
                }
            }
        } catch (const std::exception& e) {
            fmt::print("Exception: {}\n", e.what());
        }
    }
    
    fmt::print("\n--- Test 2: POST Request ---\n");
    {
        openai::http::Request request;
        request.method = "POST";
        request.host = "httpbin.org";
        request.path = "/post";
        request.use_ssl = true;
        request.body = R"({"message": "Hello from C++23 coroutines!"})";
        request.headers["Content-Type"] = "application/json";
        
        try {
            auto response = co_await http_client.async_request(request);
            
            if (response.is_error) {
                fmt::print("Error: {}\n", response.error_message);
            } else {
                fmt::print("Status: {}\n", response.status_code);
                fmt::print("Body (first 300 chars):\n{}\n", 
                    response.body.substr(0, std::min(size_t(300), response.body.size())));
            }
        } catch (const std::exception& e) {
            fmt::print("Exception: {}\n", e.what());
        }
    }
}

int main() {
    fmt::print("This example does not require an API key.\n\n");
    
    asio::io_context io_context;
    
    asio::co_spawn(io_context, http_test_example(io_context), asio::detached);
    io_context.run();
    
    return 0;
}

