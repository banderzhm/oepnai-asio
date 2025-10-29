// Example 11: Fine-tuning API
// Create and manage fine-tuned models

import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> fine_tune_example(openai::Client& client, asio::io_context& io_context) {
    fmt::print("=== Fine-tuning API Example ===\n\n");
    
    fmt::print("Fine-tuning allows you to customize models on your data.\n");
    fmt::print("Steps:\n");
    fmt::print("  1. Prepare training data (JSONL format)\n");
    fmt::print("  2. Upload file using Files API\n");
    fmt::print("  3. Create fine-tuning job\n");
    fmt::print("  4. Monitor job progress\n");
    fmt::print("  5. Use fine-tuned model\n\n");
    
    // List fine-tuning jobs
    fmt::print("--- Listing Fine-tuning Jobs ---\n");
    
    openai::http::Request req;
    req.method = "GET";
    req.path = "/v1/fine_tuning/jobs";
    req.host = "api.openai.com";
    req.use_ssl = true;
    req.headers["Authorization"] = fmt::format("Bearer {}", std::getenv("OPENAI_API_KEY"));
    
    try {
        openai::http::Client http_client(io_context);
        auto response = co_await http_client.async_request(req);
        
        if (response.is_error) {
            fmt::print("Error: {}\n", response.error_message);
            co_return;
        }
        
        if (response.status_code != 200) {
            fmt::print("API Error ({}): {}\n", response.status_code, response.body);
            co_return;
        }
        
        const auto& body = response.body;
        auto data_pos = body.find("\"data\":[");
        
        if (data_pos != std::string::npos) {
            auto array_start = data_pos + 8;
            auto array_end = body.find("]", array_start);
            
            if (array_end != std::string::npos && array_end - array_start < 5) {
                fmt::print("  No fine-tuning jobs found.\n\n");
            } else {
                fmt::print("Jobs found:\n");
                fmt::print("{}\n\n", body.substr(0, std::min(std::size_t(800), body.size())));
            }
        }
        
        fmt::print(R"(Training Data Format:
  {{"messages": [{{"role": "system", "content": "..."}}, {{"role": "user", "content": "..."}}, {{"role": "assistant", "content": "..."}}]}}
  {{"messages": [{{"role": "user", "content": "..."}}, {{"role": "assistant", "content": "..."}}]}}

Create Fine-tuning Job:
  POST /v1/fine_tuning/jobs
  {{
    "training_file": "file-abc123",
    "model": "gpt-3.5-turbo",
    "hyperparameters": {{
      "n_epochs": 3
    }}
  }}

Supported base models:
  - gpt-3.5-turbo (recommended)
  - babbage-002
  - davinci-002

Hyperparameters:
  - n_epochs: Number of training epochs (default: auto)
  - batch_size: Batch size (default: auto)
  - learning_rate_multiplier: Learning rate multiplier (default: auto)

Job lifecycle:
  validating_files → running → succeeded/failed

Cost:
  Fine-tuning costs vary by model and training tokens.
  Check pricing at: https://openai.com/pricing
)");
        
    } catch (const std::exception& e) {
        fmt::print("Exception: {}\n", e.what());
    }
}

int main() {
    const char* api_key = std::getenv("OPENAI_API_KEY");
    if (!api_key) {
        fmt::print("Error: OPENAI_API_KEY environment variable not set\n");
        return 1;
    }
    
    asio::io_context io_context;
    openai::Client client(api_key, io_context);
    
    asio::co_spawn(io_context, fine_tune_example(client, io_context), asio::detached);
    io_context.run();
    
    return 0;
}

