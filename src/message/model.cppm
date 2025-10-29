// Models API Types Module
// Model information types

export module openai.types.model;

import std;

export namespace openai {

// ============================================================================
// MODELS API - Response Types
// ============================================================================

// Model information
struct Model {
    std::string id;
    std::string object;
    std::int64_t created{0};
    std::string owned_by;
};

// Model list response
struct ModelListResponse {
    std::string object;
    std::vector<Model> data;
};

// Model list alias (for compatibility)
using ModelList = ModelListResponse;

} // namespace openai

