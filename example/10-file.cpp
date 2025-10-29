// Example 10: Files API
// Upload and manage files for fine-tuning

import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> files_example(openai::Client& client) {
    fmt::print("=== Files API Example ===\n\n");
    
    fmt::print("The Files API allows you to:\n");
    fmt::print("  - Upload files for fine-tuning or assistants\n");
    fmt::print("  - List uploaded files\n");
    fmt::print("  - Retrieve file information\n");
    fmt::print("  - Delete files\n\n");
    
    // List files
    fmt::print("--- Listing Files ---\n");
    
    try {
        auto file_list = co_await client.list_files();
        
        if (file_list.is_error) {
            fmt::print("Error: {}\n\n", file_list.error_message);
        } else {
            if (file_list.data.empty()) {
                fmt::print("No files found.\n\n");
            } else {
                fmt::print("Files ({} total):\n", file_list.data.size());
                for (const auto& file : file_list.data) {
                    fmt::print("  ID: {}\n", file.id);
                    fmt::print("  Filename: {}\n", file.filename);
                    fmt::print("  Purpose: {}\n", file.purpose);
                    fmt::print("  Size: {} bytes\n", file.bytes);
                    fmt::print("  Status: {}\n", file.status);
                    fmt::print("  ---\n");
                }
                fmt::print("\n");
            }
        }
        
        fmt::print(R"(File Upload Example:
  openai::FileUploadRequest upload_req;
  upload_req.file_path = "training_data.jsonl";
  upload_req.purpose = "fine-tune";
  auto upload_resp = co_await client.upload_file(upload_req);

File Format (JSONL for fine-tuning):
  {"prompt": "<prompt>", "completion": "<completion>"}
  {"prompt": "<prompt>", "completion": "<completion>"}
  ...

Supported purposes:
  - fine-tune: For creating fine-tuned models
  - assistants: For use with Assistants API

File size limits:
  - Maximum file size: 512 MB
  - Supported formats: .jsonl, .json, .txt, .pdf, etc.

Operations available:
  - client.upload_file(request)    // Upload a file
  - client.list_files()            // List all files
  - client.retrieve_file(file_id)  // Get file info
  - client.delete_file(file_id)    // Delete a file
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
    
    asio::co_spawn(io_context, files_example(client), asio::detached);
    io_context.run();
    
    return 0;
}

