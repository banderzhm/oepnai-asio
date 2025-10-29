// Example 02: Multi-turn Chat Conversation
// Demonstrates maintaining conversation context

import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> conversation_example(openai::Client& client) {
    fmt::print("=== Multi-turn Conversation Example ===\n\n");
    
    openai::ChatCompletionRequest request;
    request.model = "gpt-3.5-turbo";
    request.max_tokens = 150;
    request.temperature = 0.8f;
    
    // System message - set AI personality
    openai::Message system_msg;
    system_msg.role = openai::MessageRole::System;
    system_msg.content = "You are a helpful programming assistant who explains concepts clearly with examples.";
    request.messages.push_back(system_msg);
    
    // First turn
    fmt::print("User: What is a coroutine in C++?\n\n");
    openai::Message user_msg1;
    user_msg1.role = openai::MessageRole::User;
    user_msg1.content = "What is a coroutine in C++?";
    request.messages.push_back(user_msg1);
    
    try {
        auto response1 = co_await client.create_chat_completion(request);
        if (response1.is_error || response1.choices.empty()) {
            fmt::print("Error: {}\n", response1.error_message);
            co_return;
        }
        
        fmt::print("Assistant: {}\n\n", response1.choices[0].message.content);
        
        // Add assistant's response to history
        request.messages.push_back(response1.choices[0].message);
        
        // Second turn
        fmt::print("User: Can you show me a simple example?\n\n");
        openai::Message user_msg2;
        user_msg2.role = openai::MessageRole::User;
        user_msg2.content = "Can you show me a simple example?";
        request.messages.push_back(user_msg2);
        
        auto response2 = co_await client.create_chat_completion(request);
        if (!response2.is_error && !response2.choices.empty()) {
            fmt::print("Assistant: {}\n\n", response2.choices[0].message.content);
            
            fmt::print("Total conversation tokens: {}\n", 
                response1.usage.total_tokens + response2.usage.total_tokens);
        }
        
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
    
    asio::co_spawn(io_context, conversation_example(client), asio::detached);
    io_context.run();
    
    return 0;
}

