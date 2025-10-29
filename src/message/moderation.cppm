// Moderation API Types Module
// Content moderation related types

export module openai.types.moderation;

import std;
import fmt;
import openai.types.common;

export namespace openai {

// ============================================================================
// MODERATION API - Request & Response Types
// ============================================================================

// Moderation category scores
struct ModerationCategoryScores {
    double hate{0.0};
    double hate_threatening{0.0};
    double harassment{0.0};
    double harassment_threatening{0.0};
    double self_harm{0.0};
    double self_harm_intent{0.0};
    double self_harm_instructions{0.0};
    double sexual{0.0};
    double sexual_minors{0.0};
    double violence{0.0};
    double violence_graphic{0.0};
};

// Moderation categories
struct ModerationCategories {
    bool hate{false};
    bool hate_threatening{false};
    bool harassment{false};
    bool harassment_threatening{false};
    bool self_harm{false};
    bool self_harm_intent{false};
    bool self_harm_instructions{false};
    bool sexual{false};
    bool sexual_minors{false};
    bool violence{false};
    bool violence_graphic{false};
};

// Moderation result
struct ModerationResult {
    bool flagged{false};
    ModerationCategories categories;
    ModerationCategoryScores category_scores;
};

// Moderation request
struct ModerationRequest {
    std::string input;
    std::optional<std::string> model{"text-moderation-latest"};
    
    std::string to_json() const;
};

// Moderation response
struct ModerationResponse {
    std::string id;
    std::string model;
    std::vector<ModerationResult> results;
};

} // namespace openai

// ============================================================================
// Implementation
// ============================================================================

namespace openai {

// ModerationRequest::to_json implementation
std::string ModerationRequest::to_json() const {
    std::ostringstream json;
    json << "{";
    json << fmt::format(R"("input":"{}")", escape_json(input));
    if (model) {
        json << fmt::format(R"(,"model":"{}")", *model);
    }
    json << "}";
    return json.str();
}

} // namespace openai
