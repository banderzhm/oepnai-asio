// Example 05: Models API
// List and retrieve information about available models

import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> models_example(openai::Client& client) {
    fmt::print("=== Models API Example ===\n\n");
    
    fmt::print("This example demonstrates the Models API.\n");
    fmt::print("The Models API allows you to:\n");
    fmt::print("  - List all available models\n");
    fmt::print("  - Retrieve details about specific models\n\n");
    
    // Example 1: List all models
    fmt::print("--- Example 1: List All Models ---\n");
    try {
        auto models = co_await client.list_models();
        if (models.has_value()) {
            fmt::print("Found {} models:\n\n", models.value().data.size());
            
            // Show first 10 models
            int count = 0;
            for (const auto& model : models.value().data) {
                if (count++ >= 10) {
                    fmt::print("... and {} more models\n\n", models.value().data.size() - 10);
                    break;
                }
                fmt::print("  • {} (owned by: {})\n", model.id, model.owned_by);
            }
        } else {
            fmt::print("Error listing models: {}\n\n", models.error().to_string());
        }
    } catch (const std::exception& e) {
        fmt::print("Error listing models: {}\n\n", e.what());
    }
    
    // Example 2: Retrieve specific model details
    fmt::print("--- Example 2: Retrieve Specific Model ---\n");
    try {
        auto model = co_await client.retrieve_model("gpt-3.5-turbo");
        if (model.has_value()) {
            fmt::print("Model ID: {}\n", model.value().id);
            fmt::print("Owned by: {}\n", model.value().owned_by);
            fmt::print("Object type: {}\n\n", model.value().object);
        } else {
            fmt::print("Error retrieving model: {}\n\n", model.error().to_string());
        }
    } catch (const std::exception& e) {
        fmt::print("Error retrieving model: {}\n\n", e.what());
    }
    
    fmt::print(R"(Common model families:
  GPT-4 Models:
    - gpt-4                  : Most capable model
    - gpt-4-turbo           : Latest GPT-4 with vision
    - gpt-4-turbo-preview   : Preview of latest improvements

  GPT-3.5 Models:
    - gpt-3.5-turbo         : Fast and cost-effective
    - gpt-3.5-turbo-16k     : Extended context window

  Embedding Models:
    - text-embedding-ada-002     : Most capable embedding
    - text-embedding-3-small     : New smaller model
    - text-embedding-3-large     : New larger model

  Image Models:
    - dall-e-2              : Standard image generation
    - dall-e-3              : Latest and highest quality

  Audio Models:
    - whisper-1             : Speech to text
    - tts-1                 : Text to speech
)");
}

int main() {
    const char* api_key = std::getenv("OPENAI_API_KEY");
    if (!api_key) {
        fmt::print("Error: OPENAI_API_KEY environment variable not set\n");
        return 1;
    }
    
    asio::io_context io_context;
    openai::Client client(api_key, io_context);
    
    asio::co_spawn(io_context, models_example(client), asio::detached);
    io_context.run();
    
    return 0;
}

