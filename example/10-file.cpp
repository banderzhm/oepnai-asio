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
        
        if (!file_list.has_value()) {
            fmt::print("Error: {}\n\n", file_list.error().to_string());
        } else {
            if (file_list.value().data.empty()) {
                fmt::print("No files found.\n\n");
            } else {
                fmt::print("Files ({} total):\n", file_list.value().data.size());
                for (const auto& file : file_list.value().data) {
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
        
        fmt::print("File Upload Example:\n");
        fmt::print("  openai::FileUploadRequest upload_req;\n");
        fmt::print("  upload_req.file_path = \"training_data.jsonl\";\n");
        fmt::print("  upload_req.purpose = \"fine-tune\";\n");
        fmt::print("  auto upload_resp = co_await client.upload_file(upload_req);\n\n");
        
        fmt::print("File Format (JSONL for fine-tuning):\n");
        fmt::print("  {{\"prompt\": \"<prompt>\", \"completion\": \"<completion>\"}}\n");
        fmt::print("  {{\"prompt\": \"<prompt>\", \"completion\": \"<completion>\"}}\n");
        fmt::print("  ...\n\n");
        
        fmt::print("Supported purposes:\n");
        fmt::print("  - fine-tune: For creating fine-tuned models\n");
        fmt::print("  - assistants: For use with Assistants API\n\n");
        
        fmt::print("File size limits:\n");
        fmt::print("  - Maximum file size: 512 MB\n");
        fmt::print("  - Supported formats: .jsonl, .json, .txt, .pdf, etc.\n\n");
        
        fmt::print("Operations available:\n");
        fmt::print("  - client.upload_file(request)    // Upload a file\n");
        fmt::print("  - client.list_files()            // List all files\n");
        fmt::print("  - client.retrieve_file(file_id)  // Get file info\n");
        fmt::print("  - client.delete_file(file_id)    // Delete a file\n");
        
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

