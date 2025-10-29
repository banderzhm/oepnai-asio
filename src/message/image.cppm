// Image Generation API Types Module
// Image generation related types

export module openai.types.image;

import std;
import fmt;
import openai.types.common;

export namespace openai {

// ============================================================================
// IMAGE GENERATION API - Request & Response Types
// ============================================================================

// Image generation request
struct ImageGenerationRequest {
    std::string prompt;
    std::optional<std::string> model{"dall-e-2"};
    std::optional<int> n{1};
    std::optional<std::string> quality;
    std::optional<std::string> response_format{"url"};
    std::optional<std::string> size{"1024x1024"};
    std::optional<std::string> style;
    std::optional<std::string> user;
    
    std::string to_json() const;
};

// Image edit request
struct ImageEditRequest {
    std::string image_path;         // Path to image to edit (PNG, < 4MB, square)
    std::string prompt;              // Text description of desired image
    std::optional<std::string> mask; // Path to mask image (optional)
    std::optional<std::string> model{"dall-e-2"};
    std::optional<int> n{1};
    std::optional<std::string> size{"1024x1024"};
    std::optional<std::string> response_format{"url"};
    std::optional<std::string> user;
    
    std::vector<char> image_content;  // Image content (alternative to path)
    std::vector<char> mask_content;   // Mask content (alternative to path)
};

// Image variation request  
struct ImageVariationRequest {
    std::string image_path;          // Path to image (PNG, < 4MB, square)
    std::optional<std::string> model{"dall-e-2"};
    std::optional<int> n{1};
    std::optional<std::string> response_format{"url"};
    std::optional<std::string> size{"1024x1024"};
    std::optional<std::string> user;
    
    std::vector<char> image_content;  // Image content (alternative to path)
};

// Image data
struct ImageData {
    std::string url;
    std::string b64_json;
    std::string revised_prompt;
};

// Image response
struct ImageResponse {
    std::int64_t created{0};
    std::vector<ImageData> data;
    bool is_error{false};
    std::string error_message;
};

} // namespace openai

// ============================================================================
// Implementation
// ============================================================================

namespace openai {

// ImageGenerationRequest::to_json implementation
std::string ImageGenerationRequest::to_json() const {
    std::ostringstream json;
    json << "{";
    json << fmt::format(R"("prompt":"{}")", escape_json(prompt));
    
    if (model) {
        json << fmt::format(R"(,"model":"{}")", *model);
    }
    if (n) {
        json << fmt::format(R"(,"n":{})", *n);
    }
    if (quality) {
        json << fmt::format(R"(,"quality":"{}")", *quality);
    }
    if (response_format) {
        json << fmt::format(R"(,"response_format":"{}")", *response_format);
    }
    if (size) {
        json << fmt::format(R"(,"size":"{}")", *size);
    }
    if (style) {
        json << fmt::format(R"(,"style":"{}")", *style);
    }
    if (user) {
        json << fmt::format(R"(,"user":"{}")", *user);
    }
    
    json << "}";
    return json.str();
}

} // namespace openai
