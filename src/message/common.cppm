// Common utilities for types
// Internal helper functions

module;

export module openai.types.common;

import std;
import fmt;

export namespace openai {

// API Error type
struct ApiError {
    int status_code{0};
    std::string message;
    std::string type;
    
    // Constructor for simple error message
    explicit ApiError(std::string msg) : message(std::move(msg)) {}
    
    // Constructor with status code
    ApiError(int code, std::string msg) : status_code(code), message(std::move(msg)) {}
    
    // Constructor with full details
    ApiError(int code, std::string msg, std::string err_type) 
        : status_code(code), message(std::move(msg)), type(std::move(err_type)) {}
    
    // Convert to string for display
    std::string to_string() const {
        if (status_code > 0) {
            return fmt::format("[{}] {}", status_code, message);
        }
        return message;
    }
};

// Helper function to escape JSON strings
std::string escape_json(const std::string& str);

} // namespace openai

// ============================================================================
// Implementation
// ============================================================================

namespace openai {

// Helper function to escape JSON strings
std::string escape_json(const std::string& str) {
    std::ostringstream escaped;
    for (char c : str) {
        switch (c) {
            case '"': escaped << "\\\""; break;
            case '\\': escaped << "\\\\"; break;
            case '\b': escaped << "\\b"; break;
            case '\f': escaped << "\\f"; break;
            case '\n': escaped << "\\n"; break;
            case '\r': escaped << "\\r"; break;
            case '\t': escaped << "\\t"; break;
            default:
                if (c < 0x20) {
                    escaped << fmt::format("\\u{:04x}", static_cast<unsigned char>(c));
                } else {
                    escaped << c;
                }
        }
    }
    return escaped.str();
}

} // namespace openai
