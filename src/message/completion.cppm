// Completion API Types Module
// Legacy completions related types

export module openai.types.completion;

import std;
import fmt;
import openai.types.common;

export namespace openai {

// ============================================================================
// COMPLETION API (Legacy) - Request & Response Types
// ============================================================================

// Completion request
struct CompletionRequest {
    std::string model{"text-davinci-003"};
    std::string prompt;
    std::optional<int> max_tokens{16};
    std::optional<float> temperature{1.0f};
    std::optional<float> top_p{1.0f};
    std::optional<int> n{1};
    std::optional<bool> stream{false};
    std::optional<int> logprobs;
    std::optional<bool> echo{false};
    std::optional<std::vector<std::string>> stop;
    std::optional<float> presence_penalty{0.0f};
    std::optional<float> frequency_penalty{0.0f};
    std::optional<int> best_of{1};
    std::optional<std::string> user;
    
    std::string to_json() const;
};

// ============================================================================
// EDIT API (Deprecated) - Request Type
// ============================================================================

// Edit request
struct EditRequest {
    std::string model{"text-davinci-edit-001"};
    std::string input;
    std::string instruction;
    std::optional<int> n{1};
    std::optional<float> temperature{1.0f};
    std::optional<float> top_p{1.0f};
    
    std::string to_json() const;
};

} // namespace openai

// ============================================================================
// Implementation
// ============================================================================

namespace openai {

// CompletionRequest::to_json implementation
std::string CompletionRequest::to_json() const {
    std::ostringstream json;
    json << "{";
    json << fmt::format(R"("model":"{}")", model);
    json << fmt::format(R"(,"prompt":"{}")", escape_json(prompt));
    
    if (max_tokens) {
        json << fmt::format(R"(,"max_tokens":{})", *max_tokens);
    }
    if (temperature) {
        json << fmt::format(R"(,"temperature":{})", *temperature);
    }
    if (top_p) {
        json << fmt::format(R"(,"top_p":{})", *top_p);
    }
    if (n) {
        json << fmt::format(R"(,"n":{})", *n);
    }
    if (stream) {
        json << fmt::format(R"(,"stream":{})", *stream ? "true" : "false");
    }
    if (echo) {
        json << fmt::format(R"(,"echo":{})", *echo ? "true" : "false");
    }
    if (presence_penalty) {
        json << fmt::format(R"(,"presence_penalty":{})", *presence_penalty);
    }
    if (frequency_penalty) {
        json << fmt::format(R"(,"frequency_penalty":{})", *frequency_penalty);
    }
    if (best_of) {
        json << fmt::format(R"(,"best_of":{})", *best_of);
    }
    
    json << "}";
    return json.str();
}

// EditRequest::to_json implementation
std::string EditRequest::to_json() const {
    std::ostringstream json;
    json << "{";
    json << fmt::format(R"("model":"{}")", model);
    json << fmt::format(R"(,"input":"{}")", escape_json(input));
    json << fmt::format(R"(,"instruction":"{}")", escape_json(instruction));
    
    if (n) {
        json << fmt::format(R"(,"n":{})", *n);
    }
    if (temperature) {
        json << fmt::format(R"(,"temperature":{})", *temperature);
    }
    if (top_p) {
        json << fmt::format(R"(,"top_p":{})", *top_p);
    }
    
    json << "}";
    return json.str();
}

} // namespace openai
