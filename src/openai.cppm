// OpenAI Module - Main Module Interface
// Import this single module to use all OpenAI functionality
//
// Usage:
//   import openai;
//   import std;
//   #include <asio.hpp>
//
//   int main() {
//       asio::io_context io_context;
//       openai::Client client("your-api-key", io_context);
//       
//       openai::ChatCompletionRequest req;
//       req.model = "gpt-3.5-turbo";
//       req.messages.push_back({openai::MessageRole::User, "Hello!"});
//       
//       auto response = client.create_chat_completion_sync(req);
//   }

export module openai;

// Re-export all sub-modules
export import openai.http_client;
export import openai.types;
export import openai.client;

// Re-export commonly used namespaces
export namespace openai {
    // All types and classes from sub-modules are automatically available
}
