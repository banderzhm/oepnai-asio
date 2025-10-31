// Runs API Types Module
// Assistant run execution (Beta)

export module openai.types.run;

import std;
import openai.types.assistant;

export namespace openai {

// ============================================================================
// RUNS API - Request & Response Types (Beta)
// ============================================================================

// Run status
enum class RunStatus {
    Queued,
    InProgress,
    RequiresAction,
    Cancelling,
    Cancelled,
    Failed,
    Completed,
    Expired
};

// Helper function to convert string to RunStatus
inline RunStatus string_to_run_status(const std::string& status_str) {
    if (status_str == "queued") return RunStatus::Queued;
    if (status_str == "in_progress") return RunStatus::InProgress;
    if (status_str == "requires_action") return RunStatus::RequiresAction;
    if (status_str == "cancelling") return RunStatus::Cancelling;
    if (status_str == "cancelled") return RunStatus::Cancelled;
    if (status_str == "failed") return RunStatus::Failed;
    if (status_str == "completed") return RunStatus::Completed;
    if (status_str == "expired") return RunStatus::Expired;
    return RunStatus::Queued;  // Default
}

// Run object
struct Run {
    std::string id;
    std::string object{"thread.run"};
    std::int64_t created_at{0};
    std::string thread_id;
    std::string assistant_id;
    RunStatus status;
    std::optional<std::string> required_action;
    std::optional<std::string> last_error;
    std::optional<std::int64_t> expires_at;
    std::optional<std::int64_t> started_at;
    std::optional<std::int64_t> cancelled_at;
    std::optional<std::int64_t> failed_at;
    std::optional<std::int64_t> completed_at;
    std::string model;
    std::optional<std::string> instructions;
    std::vector<AssistantTool> tools;
    std::vector<std::string> file_ids;
    std::map<std::string, std::string> metadata;
};

// Create run request
struct CreateRunRequest {
    std::string assistant_id;
    std::optional<std::string> model;
    std::optional<std::string> instructions;
    std::optional<std::vector<AssistantTool>> tools;
    std::optional<std::map<std::string, std::string>> metadata;
    
    std::string to_json() const;
};

// Modify run request
struct ModifyRunRequest {
    std::optional<std::map<std::string, std::string>> metadata;
    
    std::string to_json() const;
};

// Tool output for submit
struct ToolOutput {
    std::string tool_call_id;
    std::string output;
};

// Submit tool outputs request
struct SubmitToolOutputsRequest {
    std::vector<ToolOutput> tool_outputs;
    
    std::string to_json() const;
};

// Run list response
struct RunListResponse {
    std::string object;
    std::vector<Run> data;
    std::string first_id;
    std::string last_id;
    bool has_more{false};
};

// Run step (for detailed execution tracking)
struct RunStep {
    std::string id;
    std::string object{"thread.run.step"};
    std::int64_t created_at{0};
    std::string run_id;
    std::string assistant_id;
    std::string thread_id;
    std::string type;  // "message_creation" or "tool_calls"
    std::string status;
    std::optional<std::int64_t> completed_at;
    std::optional<std::int64_t> cancelled_at;
    std::optional<std::int64_t> failed_at;
    std::optional<std::string> last_error;
    std::map<std::string, std::string> step_details;
};

// Run step list response
struct RunStepListResponse {
    std::string object;
    std::vector<RunStep> data;
    std::string first_id;
    std::string last_id;
    bool has_more{false};
};

} // namespace openai

// ============================================================================
// Implementation
// ============================================================================

namespace openai {

// CreateRunRequest::to_json implementation
std::string CreateRunRequest::to_json() const {
    std::ostringstream json;
    json << "{";
    json << "\"assistant_id\":\"" << assistant_id << "\"";
    
    if (model) {
        json << ",\"model\":\"" << *model << "\"";
    }
    if (instructions) {
        json << ",\"instructions\":\"" << *instructions << "\"";
    }
    
    json << "}";
    return json.str();
}

// ModifyRunRequest::to_json implementation
std::string ModifyRunRequest::to_json() const {
    return "{}";  // Empty object for now
}

// SubmitToolOutputsRequest::to_json implementation
std::string SubmitToolOutputsRequest::to_json() const {
    std::ostringstream json;
    json << "{\"tool_outputs\":[";
    
    for (std::size_t i = 0; i < tool_outputs.size(); ++i) {
        if (i > 0) json << ",";
        json << "{";
        json << "\"tool_call_id\":\"" << tool_outputs[i].tool_call_id << "\",";
        json << "\"output\":\"" << tool_outputs[i].output << "\"";
        json << "}";
    }
    
    json << "]}";
    return json.str();
}

} // namespace openai