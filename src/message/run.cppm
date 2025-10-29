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

// Run list response
struct RunListResponse {
    std::string object;
    std::vector<Run> data;
    std::string first_id;
    std::string last_id;
    bool has_more{false};
    bool is_error{false};
    std::string error_message;
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
    bool is_error{false};
    std::string error_message;
};

} // namespace openai

