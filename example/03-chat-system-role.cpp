// Example 03: Using System Role
// Demonstrates how system messages shape AI behavior

import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> system_role_example(openai::Client& client) {
    fmt::print("=== System Role Example ===\n\n");
    
    // Example 1: Pirate assistant
    fmt::print("--- Pirate Mode ---\n");
    {
        openai::ChatCompletionRequest request;
        request.model = "gpt-3.5-turbo";
        request.max_tokens = 100;
        
        openai::Message system_msg;
        system_msg.role = openai::MessageRole::System;
        system_msg.content = "You are a helpful assistant that speaks like a pirate.";
        request.messages.push_back(system_msg);
        
        openai::Message user_msg;
        user_msg.role = openai::MessageRole::User;
        user_msg.content = "Tell me about the ocean.";
        request.messages.push_back(user_msg);
        
        try {
            auto response = co_await client.create_chat_completion(request);
            if (response.has_value() && !response.value().choices.empty()) {
                fmt::print("Pirate: {}\n\n", response.value().choices[0].message.content);
            }
        } catch (const std::exception& e) {
            fmt::print("Exception: {}\n", e.what());
        }
    }
    
    // Example 2: JSON-only assistant
    fmt::print("--- JSON Mode ---\n");
    {
        openai::ChatCompletionRequest request;
        request.model = "gpt-3.5-turbo";
        request.max_tokens = 100;
        
        openai::Message system_msg;
        system_msg.role = openai::MessageRole::System;
        system_msg.content = "You respond only in valid JSON format with no additional text.";
        request.messages.push_back(system_msg);
        
        openai::Message user_msg;
        user_msg.role = openai::MessageRole::User;
        user_msg.content = "Give me information about Paris: name, country, population.";
        request.messages.push_back(user_msg);
        
        try {
            auto response = co_await client.create_chat_completion(request);
            if (response.has_value() && !response.value().choices.empty()) {
                fmt::print("JSON Response:\n{}\n\n", response.value().choices[0].message.content);
            }
        } catch (const std::exception& e) {
            fmt::print("Exception: {}\n", e.what());
        }
    }
    
    // Example 3: Expert mode
    fmt::print("--- Expert Mode ---\n");
    {
        openai::ChatCompletionRequest request;
        request.model = "gpt-3.5-turbo";
        request.max_tokens = 150;
        
        openai::Message system_msg;
        system_msg.role = openai::MessageRole::System;
        system_msg.content = "You are an expert C++ programmer. Give concise, technical answers with code examples.";
        request.messages.push_back(system_msg);
        
        openai::Message user_msg;
        user_msg.role = openai::MessageRole::User;
        user_msg.content = "How do I use std::optional?";
        request.messages.push_back(user_msg);
        
        try {
            auto response = co_await client.create_chat_completion(request);
            if (response.has_value() && !response.value().choices.empty()) {
                fmt::print("Expert: {}\n", response.value().choices[0].message.content);
            }
        } catch (const std::exception& e) {
            fmt::print("Exception: {}\n", e.what());
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
    
    asio::co_spawn(io_context, system_role_example(client), asio::detached);
    io_context.run();
    
    return 0;
}

