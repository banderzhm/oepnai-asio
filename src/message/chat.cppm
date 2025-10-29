// Chat API Types Module
// Chat completions related types

export module openai.types.chat;

import std;
import fmt;
import openai.types.common;

export namespace openai {

// ============================================================================
// ENUMERATIONS
// ============================================================================

// Chat message role
enum class MessageRole {
    System,
    User,
    Assistant,
    Function
};

// Convert MessageRole to string
std::string to_string(MessageRole role);

// ============================================================================
// CHAT API - Request & Response Types
// ============================================================================

// Chat message
struct Message {
    MessageRole role{MessageRole::User};
    std::string content;
    std::optional<std::string> name;
    std::optional<std::string> function_call;
    
    std::string to_json() const;
};

// Chat completion request
struct ChatCompletionRequest {
    std::string model{"gpt-3.5-turbo"};
    std::vector<Message> messages;
    std::optional<float> temperature;
    std::optional<int> max_tokens;
    std::optional<float> top_p;
    std::optional<float> frequency_penalty;
    std::optional<float> presence_penalty;
    std::optional<int> n;
    std::optional<bool> stream;
    std::optional<std::vector<std::string>> stop;
    std::optional<std::string> user;
    
    std::string to_json() const;
};

// Chat completion choice
struct ChatCompletionChoice {
    int index{0};
    Message message;
    std::string finish_reason;
};

// Chat completion usage statistics
struct ChatCompletionUsage {
    int prompt_tokens{0};
    int completion_tokens{0};
    int total_tokens{0};
};

// Chat completion response
struct ChatCompletionResponse {
    std::string id;
    std::string object;
    std::int64_t created{0};
    std::string model;
    std::vector<ChatCompletionChoice> choices;
    ChatCompletionUsage usage;
    bool is_error{false};
    std::string error_message;
};

} // namespace openai

// ============================================================================
// Implementation
// ============================================================================

namespace openai {

// Convert MessageRole to string
std::string to_string(MessageRole role) {
    switch (role) {
        case MessageRole::System: return "system";
        case MessageRole::User: return "user";
        case MessageRole::Assistant: return "assistant";
        case MessageRole::Function: return "function";
        default: return "user";
    }
}

// Message::to_json implementation
std::string Message::to_json() const {
    std::ostringstream json;
    json << "{";
    json << fmt::format(R"("role":"{}")", to_string(role));
    json << fmt::format(R"(,"content":"{}")", escape_json(content));
    
    if (name) {
        json << fmt::format(R"(,"name":"{}")", *name);
    }
    if (function_call) {
        json << fmt::format(R"(,"function_call":{})", *function_call);
    }
    
    json << "}";
    return json.str();
}

// ChatCompletionRequest::to_json implementation
std::string ChatCompletionRequest::to_json() const {
    std::ostringstream json;
    json << "{";
    json << fmt::format(R"("model":"{}")", model);
    
    json << R"(,"messages":[)";
    for (std::size_t i = 0; i < messages.size(); ++i) {
        if (i > 0) json << ",";
        json << messages[i].to_json();
    }
    json << "]";
    
    if (temperature) {
        json << fmt::format(R"(,"temperature":{})", *temperature);
    }
    if (max_tokens) {
        json << fmt::format(R"(,"max_tokens":{})", *max_tokens);
    }
    if (top_p) {
        json << fmt::format(R"(,"top_p":{})", *top_p);
    }
    if (frequency_penalty) {
        json << fmt::format(R"(,"frequency_penalty":{})", *frequency_penalty);
    }
    if (presence_penalty) {
        json << fmt::format(R"(,"presence_penalty":{})", *presence_penalty);
    }
    if (n) {
        json << fmt::format(R"(,"n":{})", *n);
    }
    if (stream) {
        json << fmt::format(R"(,"stream":{})", *stream ? "true" : "false");
    }
    if (stop) {
        json << R"(,"stop":[)";
        for (std::size_t i = 0; i < stop->size(); ++i) {
            if (i > 0) json << ",";
            json << fmt::format(R"("{}")", (*stop)[i]);
        }
        json << "]";
    }
    if (user) {
        json << fmt::format(R"(,"user":"{}")", *user);
    }
    
    json << "}";
    return json.str();
}

} // namespace openai