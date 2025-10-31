// Example 00: Showcase - Overview of OpenAI API capabilities
// 展示 OpenAI API 的主要功能

import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> showcase_example(openai::Client& client) {
    fmt::print("=== OpenAI API Showcase ===\n\n");
    
    // 1. Chat Completion (已实现)
    fmt::print("1. Chat Completion\n");
    fmt::print("   Creating a chat with GPT-3.5-turbo...\n");
    {
        openai::ChatCompletionRequest req;
        req.model = "gpt-3.5-turbo";
        req.max_tokens = 50;
        req.messages.push_back({
            openai::MessageRole::User,
            "Explain AI in one sentence"
        });
        
        try {
            auto response = co_await client.create_chat_completion(req);
            if (response.has_value() && !response.value().choices.empty()) {
                fmt::print("   Response: {}\n\n", response.value().choices[0].message.content);
            } else if (!response.has_value()) {
                fmt::print("   Error: {}\n\n", response.error().to_string());
            }
        } catch (const std::exception& e) {
            fmt::print("   Error: {}\n\n", e.what());
        }
    }
    
    // 2. Text Completion (legacy)
    fmt::print("2. Text Completion (Legacy)\n");
    fmt::print("   Using completion API...\n");
    {
        openai::CompletionRequest req;
        req.model = "text-davinci-003";
        req.prompt = "Say this is a test";
        req.max_tokens = 7;
        req.temperature = 0.0f;
        
        try {
            auto response = co_await client.create_completion(req);
            if (response.has_value()) {
                fmt::print("   Response: {}\n\n", response.value().substr(0, 100));
            } else {
                fmt::print("   Error: {}\n\n", response.error().to_string());
            }
        } catch (const std::exception& e) {
            fmt::print("   Error: {}\n\n", e.what());
        }
    }
    
    // 3-8. Other APIs
    fmt::print(R"(3. Image Generation ✓
   - Generate images from text prompts
   - Edit existing images
   - Create variations

4. Embeddings ✓
   - Convert text to vector embeddings
   - Useful for semantic search and similarity

5. Models List ✓
   - List available models
   - Get model details

6. Files & Fine-tuning ✓
   - Upload training files
   - Create fine-tuned models
   - Monitor training progress

7. Audio ✓
   - Transcribe audio to text (requires multipart/form-data)
   - Translate audio to English (requires multipart/form-data)

8. Moderation ✓
   - Check content for policy violations
   - Free to use

All APIs are now implemented! See individual example files:
  01-04: Chat examples
  05: Models API
  06-07: Completion & Edit (legacy)
  08: Image Generation
  09: Embeddings
  10-11: Files & Fine-tuning
  12: Audio (framework ready, needs file upload)
  13: Moderation
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
    
    asio::co_spawn(io_context, showcase_example(client), asio::detached);
    io_context.run();
    
    return 0;
}

