// Example 10: Files API
// Upload and manage files for fine-tuning

import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> files_example(openai::Client& client, asio::io_context& io_context) {
    fmt::print("=== Files API Example ===\n\n");
    
    fmt::print("The Files API allows you to:\n");
    fmt::print("  - Upload files for fine-tuning\n");
    fmt::print("  - List uploaded files\n");
    fmt::print("  - Retrieve file information\n");
    fmt::print("  - Delete files\n\n");
    
    // List files
    fmt::print("--- Listing Files ---\n");
    
    openai::http::Request req;
    req.method = "GET";
    req.path = "/v1/files";
    req.host = "api.openai.com";
    req.use_ssl = true;
    req.headers["Authorization"] = fmt::format("Bearer {}", std::getenv("OPENAI_API_KEY"));
    
    try {
        openai::http::Client http_client(io_context);
        auto response = co_await http_client.async_request(req);
        
        if (response.is_error) {
            fmt::print("Error: {}\n", response.error_message);
            co_return;
        }
        
        if (response.status_code != 200) {
            fmt::print("API Error ({}): {}\n", response.status_code, response.body);
            co_return;
        }
        
        fmt::print("Files API Response:\n");
        
        // Parse and display file list
        const auto& body = response.body;
        auto data_pos = body.find("\"data\":[");
        
        if (data_pos != std::string::npos) {
            auto array_start = data_pos + 8;
            auto array_end = body.find("]", array_start);
            
            if (array_end != std::string::npos && array_end - array_start < 5) {
                fmt::print("  No files found.\n\n");
            } else {
                fmt::print("{}\n\n", body.substr(0, std::min(size_t(500), body.size())));
            }
        }
        
        fmt::print(R"(File Upload Format:
  Files must be in JSONL format for fine-tuning:
  {{"prompt": "<prompt>", "completion": "<completion>"}}
  {{"prompt": "<prompt>", "completion": "<completion>"}}
  ...

To upload a file:
  POST /v1/files
  Content-Type: multipart/form-data
  Body: file=@training_data.jsonl, purpose=fine-tune

Supported purposes:
  - fine-tune: For creating fine-tuned models
  - assistants: For use with Assistants API

File size limits:
  - Maximum file size: 512 MB
  - Supported formats: .jsonl
)");
        
    } catch (const std::exception& e) {
        fmt::print("Exception: {}\n", e.what());
    }
}

int main() {
    const char* api_key = std::getenv("OPENAI_API_KEY");
    if (!api_key) {
        fmt::print("Error: OPENAI_API_KEY environment variable not set\n");
        return 1;
    }
    
    asio::io_context io_context;
    openai::Client client(api_key, io_context);
    
    asio::co_spawn(io_context, files_example(client, io_context), asio::detached);
    io_context.run();
    
    return 0;
}

