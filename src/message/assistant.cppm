// Assistants API Types Module
// AI Assistants related types (Beta)

export module openai.types.assistant;

import std;

export namespace openai {

// ============================================================================
// ASSISTANTS API - Request & Response Types (Beta)
// ============================================================================

// Tool types
enum class AssistantToolType {
    CodeInterpreter,
    Retrieval,
    Function
};

// Function definition for tools
struct FunctionDefinition {
    std::string name;
    std::string description;
    std::string parameters;  // JSON schema
};

// Assistant tool
struct AssistantTool {
    AssistantToolType type;
    std::optional<FunctionDefinition> function;
};

// Assistant object
struct Assistant {
    std::string id;
    std::string object{"assistant"};
    std::int64_t created_at{0};
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::string model;
    std::optional<std::string> instructions;
    std::vector<AssistantTool> tools;
    std::vector<std::string> file_ids;
    std::map<std::string, std::string> metadata;
};

// Create assistant request
struct CreateAssistantRequest {
    std::string model;  // e.g., "gpt-4-turbo-preview"
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<std::string> instructions;
    std::optional<std::vector<AssistantTool>> tools;
    std::optional<std::vector<std::string>> file_ids;
    std::optional<std::map<std::string, std::string>> metadata;
    
    std::string to_json() const;
};

// Modify assistant request
struct ModifyAssistantRequest {
    std::optional<std::string> model;
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<std::string> instructions;
    std::optional<std::vector<AssistantTool>> tools;
    std::optional<std::vector<std::string>> file_ids;
    std::optional<std::map<std::string, std::string>> metadata;
    
    std::string to_json() const;
};

// Assistant list response
struct AssistantListResponse {
    std::string object;
    std::vector<Assistant> data;
    std::string first_id;
    std::string last_id;
    bool has_more{false};
};

// Delete assistant response
struct DeleteAssistantResponse {
    std::string id;
    std::string object;
    bool deleted{false};
};

} // namespace openai

// ============================================================================
// Implementation
// ============================================================================

namespace openai {

// CreateAssistantRequest::to_json implementation
std::string CreateAssistantRequest::to_json() const {
    std::ostringstream json;
    json << "{";
    json << "\"model\":\"" << model << "\"";
    
    if (name) {
        json << ",\"name\":\"" << *name << "\"";
    }
    if (description) {
        json << ",\"description\":\"" << *description << "\"";
    }
    if (instructions) {
        json << ",\"instructions\":\"" << *instructions << "\"";
    }
    
    json << "}";
    return json.str();
}

// ModifyAssistantRequest::to_json implementation
std::string ModifyAssistantRequest::to_json() const {
    std::ostringstream json;
    json << "{";
    bool first = true;
    
    if (model) {
        json << "\"model\":\"" << *model << "\"";
        first = false;
    }
    if (name) {
        if (!first) json << ",";
        json << "\"name\":\"" << *name << "\"";
        first = false;
    }
    if (description) {
        if (!first) json << ",";
        json << "\"description\":\"" << *description << "\"";
        first = false;
    }
    if (instructions) {
        if (!first) json << ",";
        json << "\"instructions\":\"" << *instructions << "\"";
    }
    
    json << "}";
    return json.str();
}

} // namespace openai

