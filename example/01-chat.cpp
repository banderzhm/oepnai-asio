// Example 01: Chat Completion
// Demonstrates basic chat completion with GPT models

import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> chat_example(openai::Client& client) {
    fmt::print("=== Chat Completion Example ===\n\n");
    
    // Create chat completion request
    openai::ChatCompletionRequest request;
    request.model = "gpt-3.5-turbo";
    request.max_tokens = 100;
    request.temperature = 0.7f;
    
    // Add user message
    openai::Message msg;
    msg.role = openai::MessageRole::User;
    msg.content = "Explain quantum computing in simple terms";
    request.messages.push_back(msg);
    
    fmt::print("Sending request to OpenAI...\n");
    
    try {
        auto response = co_await client.create_chat_completion(request);
        
        if (response.is_error) {
            fmt::print("Error: {}\n", response.error_message);
            co_return;
        }
        
        fmt::print("\nResponse:\n");
        fmt::print("  ID: {}\n", response.id);
        fmt::print("  Model: {}\n", response.model);
        fmt::print("\n");
        
        if (!response.choices.empty()) {
            fmt::print("Assistant: {}\n\n", response.choices[0].message.content);
        }
        
        fmt::print("Usage:\n");
        fmt::print("  Prompt tokens: {}\n", response.usage.prompt_tokens);
        fmt::print("  Completion tokens: {}\n", response.usage.completion_tokens);
        fmt::print("  Total tokens: {}\n", response.usage.total_tokens);
        
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
    
    asio::co_spawn(io_context, chat_example(client), asio::detached);
    io_context.run();
    
    return 0;
}

