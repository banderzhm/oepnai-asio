// Example 06: Text Completion (Legacy)
// Demonstrates the legacy completion API

import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> completion_example(openai::Client& client) {
    fmt::print("=== Text Completion Example (Legacy) ===\n\n");
    
    fmt::print("Note: The Completion API is legacy. Consider using Chat Completion instead.\n\n");
    
    // Example 1: Simple completion
    fmt::print("--- Example 1: Simple Completion ---\n");
    {
        openai::CompletionRequest req;
        req.model = "text-davinci-003";
        req.prompt = "Say this is a test";
        req.max_tokens = 7;
        req.temperature = 0.0f;
        
        fmt::print("Prompt: {}\n", req.prompt);
        
        try {
            auto response = co_await client.create_completion(req);
            fmt::print("Completion: {}\n\n", response);
        } catch (const std::exception& e) {
            fmt::print("Error: {}\n\n", e.what());
        }
    }
    
    // Example 2: Creative completion
    fmt::print("--- Example 2: Creative Story Completion ---\n");
    {
        openai::CompletionRequest req;
        req.model = "text-davinci-003";
        req.prompt = "Once upon a time in a land far away,";
        req.max_tokens = 50;
        req.temperature = 0.8f;
        
        fmt::print("Prompt: {}\n", req.prompt);
        
        try {
            auto response = co_await client.create_completion(req);
            fmt::print("Story: {}\n\n", response);
        } catch (const std::exception& e) {
            fmt::print("Error: {}\n\n", e.what());
        }
    }
    
    // Example 3: Code completion
    fmt::print("--- Example 3: Code Completion ---\n");
    {
        openai::CompletionRequest req;
        req.model = "text-davinci-003";
        req.prompt = "// Function to calculate fibonacci\ndef fibonacci(n):";
        req.max_tokens = 100;
        req.temperature = 0.0f;
        
        fmt::print("Prompt:\n{}\n", req.prompt);
        
        try {
            auto response = co_await client.create_completion(req);
            fmt::print("Completed code:\n{}\n", response);
        } catch (const std::exception& e) {
            fmt::print("Error: {}\n", e.what());
        }
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
    
    asio::co_spawn(io_context, completion_example(client), asio::detached);
    io_context.run();
    
    return 0;
}

