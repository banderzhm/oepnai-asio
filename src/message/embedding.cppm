// Embeddings API Types Module
// Embeddings related types

export module openai.types.embedding;

import std;
import fmt;
import openai.types.common;

export namespace openai {

// ============================================================================
// EMBEDDINGS API - Request & Response Types
// ============================================================================

// Embedding request
struct EmbeddingRequest {
    std::string model{"text-embedding-ada-002"};
    std::string input;
    std::optional<std::string> user;
    
    std::string to_json() const;
};

// Embedding data
struct EmbeddingData {
    std::string object;
    std::vector<float> embedding;
    int index{0};
};

// Embedding usage
struct EmbeddingUsage {
    int prompt_tokens{0};
    int total_tokens{0};
};

// Embedding response
struct EmbeddingResponse {
    std::string object;
    std::vector<EmbeddingData> data;
    std::string model;
    EmbeddingUsage usage;
};

} // namespace openai

// ============================================================================
// Implementation
// ============================================================================

namespace openai {

// EmbeddingRequest::to_json implementation
std::string EmbeddingRequest::to_json() const {
    std::ostringstream json;
    json << "{";
    json << fmt::format(R"("model":"{}")", model);
    json << fmt::format(R"(,"input":"{}")", escape_json(input));
    
    if (user) {
        json << fmt::format(R"(,"user":"{}")", *user);
    }
    
    json << "}";
    return json.str();
}

} // namespace openai
