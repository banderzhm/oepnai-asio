// Files API Types Module
// File management related types

export module openai.types.file;

import std;

export namespace openai {

// ============================================================================
// FILES API - Response Types
// ============================================================================

// File object
struct FileObject {
    std::string id;
    std::string object;
    int bytes{0};
    std::int64_t created_at{0};
    std::string filename;
    std::string purpose;
    std::string status;
};

// File list response
struct FileListResponse {
    std::string object;
    std::vector<FileObject> data;
    bool is_error{false};
    std::string error_message;
};

} // namespace openai

