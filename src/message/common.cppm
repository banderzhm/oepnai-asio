// Common utilities for types
// Internal helper functions

module;

export module openai.types.common;

import std;
import fmt;

export namespace openai {

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
