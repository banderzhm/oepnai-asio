// Fine-tuning API Types Module
// Fine-tuning related types

export module openai.types.fine_tuning;

import std;
import fmt;

export namespace openai {

// ============================================================================
// FINE-TUNING API - Request & Response Types
// ============================================================================

// Fine-tuning hyperparameters
struct FineTuningHyperparameters {
    std::optional<int> n_epochs;
    std::optional<float> learning_rate_multiplier;
    std::optional<int> batch_size;
};

// Fine-tuning request
struct FineTuningRequest {
    std::string training_file;
    std::string model{"gpt-3.5-turbo"};
    std::optional<std::string> validation_file;
    std::optional<FineTuningHyperparameters> hyperparameters;
    std::optional<std::string> suffix;
    
    std::string to_json() const;
};

// Fine-tuning error
struct FineTuningError {
    std::string code;
    std::string message;
    std::string param;
};

// Fine-tuning job
struct FineTuningJob {
    std::string id;
    std::string object;
    std::int64_t created_at{0};
    std::int64_t finished_at{0};
    std::string model;
    std::string fine_tuned_model;
    std::string organization_id;
    std::string status;
    FineTuningHyperparameters hyperparameters;
    std::string training_file;
    std::string validation_file;
    std::vector<std::string> result_files;
    int trained_tokens{0};
    std::optional<FineTuningError> error;
};

// Fine-tuning job list response
struct FineTuningJobListResponse {
    std::string object;
    std::vector<FineTuningJob> data;
    bool has_more{false};
};

// Fine-tuning job response (alias)
using FineTuningJobResponse = FineTuningJob;

} // namespace openai

// ============================================================================
// Implementation
// ============================================================================

namespace openai {

// FineTuningRequest::to_json implementation
std::string FineTuningRequest::to_json() const {
    std::ostringstream json;
    json << "{";
    json << fmt::format(R"("training_file":"{}")", training_file);
    json << fmt::format(R"(,"model":"{}")", model);
    
    if (validation_file) {
        json << fmt::format(R"(,"validation_file":"{}")", *validation_file);
    }
    if (suffix) {
        json << fmt::format(R"(,"suffix":"{}")", *suffix);
    }
    if (hyperparameters) {
        json << R"(,"hyperparameters":{)";
        bool first = true;
        if (hyperparameters->n_epochs) {
            json << fmt::format(R"("n_epochs":{})", *hyperparameters->n_epochs);
            first = false;
        }
        if (hyperparameters->learning_rate_multiplier) {
            if (!first) json << ",";
            json << fmt::format(R"("learning_rate_multiplier":{})", *hyperparameters->learning_rate_multiplier);
            first = false;
        }
        if (hyperparameters->batch_size) {
            if (!first) json << ",";
            json << fmt::format(R"("batch_size":{})", *hyperparameters->batch_size);
        }
        json << "}";
    }
    
    json << "}";
    return json.str();
}

} // namespace openai
