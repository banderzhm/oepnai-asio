// Threads and Messages API Types Module
// Conversation threads and messages (Beta)

export module openai.types.thread;

import std;

export namespace openai {

// ============================================================================
// THREADS API - Request & Response Types (Beta)
// ============================================================================

// Message role for threads
enum class ThreadMessageRole {
    User,
    Assistant
};

// Helper function to convert string to ThreadMessageRole
inline ThreadMessageRole string_to_thread_message_role(const std::string& role_str) {
    if (role_str == "assistant") {
        return ThreadMessageRole::Assistant;
    }
    return ThreadMessageRole::User;  // Default to User
}

// Message content (text)
struct MessageContent {
    std::string type{"text"};
    std::string text;
};

// Thread message
struct ThreadMessage {
    std::string id;
    std::string object{"thread.message"};
    std::int64_t created_at{0};
    std::string thread_id;
    ThreadMessageRole role;
    std::vector<MessageContent> content;
    std::vector<std::string> file_ids;
    std::optional<std::string> assistant_id;
    std::optional<std::string> run_id;
    std::map<std::string, std::string> metadata;
};

// Thread object
struct Thread {
    std::string id;
    std::string object{"thread"};
    std::int64_t created_at{0};
    std::map<std::string, std::string> metadata;
};

// Create thread request
struct CreateThreadRequest {
    std::optional<std::vector<ThreadMessage>> messages;
    std::optional<std::map<std::string, std::string>> metadata;
    
    std::string to_json() const;
};

// Create message request
struct CreateMessageRequest {
    ThreadMessageRole role;
    std::string content;
    std::optional<std::vector<std::string>> file_ids;
    std::optional<std::map<std::string, std::string>> metadata;
    
    std::string to_json() const;
};

// Modify thread request
struct ModifyThreadRequest {
    std::optional<std::map<std::string, std::string>> metadata;
    
    std::string to_json() const;
};

// Modify message request
struct ModifyMessageRequest {
    std::optional<std::map<std::string, std::string>> metadata;
    
    std::string to_json() const;
};

// Thread message list response
struct ThreadMessageListResponse {
    std::string object;
    std::vector<ThreadMessage> data;
    std::string first_id;
    std::string last_id;
    bool has_more{false};
};

// Delete thread response
struct DeleteThreadResponse {
    std::string id;
    std::string object;
    bool deleted{false};
};

} // namespace openai

// ============================================================================
// Implementation
// ============================================================================

namespace openai {

// CreateThreadRequest::to_json implementation
std::string CreateThreadRequest::to_json() const {
    return "{}";  // Empty object for now
}

// CreateMessageRequest::to_json implementation
std::string CreateMessageRequest::to_json() const {
    std::ostringstream json;
    json << "{";
    json << "\"role\":\"" << (role == ThreadMessageRole::User ? "user" : "assistant") << "\",";
    json << "\"content\":\"" << content << "\"";
    json << "}";
    return json.str();
}

// ModifyThreadRequest::to_json implementation
std::string ModifyThreadRequest::to_json() const {
    return "{}";  // Empty object for now
}

// ModifyMessageRequest::to_json implementation
std::string ModifyMessageRequest::to_json() const {
    return "{}";  // Empty object for now
}

} // namespace openai

