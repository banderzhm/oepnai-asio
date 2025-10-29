// Files API Types Module
// File management related types

export module openai.types.file;

import std;

export namespace openai {

// ============================================================================
// FILES API - Request & Response Types
// ============================================================================

// File upload request
struct FileUploadRequest {
    std::string file_path;          // Local file path
    std::string purpose;             // "fine-tune", "assistants", etc.
    std::vector<char> file_content;  // File content (alternative to file_path)
    std::string filename;            // Filename (when using file_content)
};

// File object
struct FileObject {
    std::string id;
    std::string object;
    int bytes{0};
    std::int64_t created_at{0};
    std::string filename;
    std::string purpose;
    std::string status;
    std::string status_details;
};

// File list response
struct FileListResponse {
    std::string object;
    std::vector<FileObject> data;
    bool is_error{false};
    std::string error_message;
};

// File upload response
using FileUploadResponse = FileObject;

// File content response
struct FileContentResponse {
    std::string content;
    bool is_error{false};
    std::string error_message;
};

} // namespace openai

