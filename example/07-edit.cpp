// Example 07: Edit API (Deprecated)
// The Edit API has been deprecated by OpenAI

import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> edit_example() {
    fmt::print("=== Edit API Example ===\n\n");
    
    fmt::print(R"(⚠️  DEPRECATED: The Edit API has been deprecated by OpenAI.

The Edit endpoint was previously available at:
  POST https://api.openai.com/v1/edits

Instead, use Chat Completion with appropriate instructions:

Example:
  openai::ChatCompletionRequest req;
  req.model = "gpt-3.5-turbo";
  req.messages.push_back({{
      openai::MessageRole::System,
      "You are a helpful editor. Fix grammar and spelling."
  }});
  req.messages.push_back({{
      openai::MessageRole::User,
      "What day of the wek is it?"
  }});

This approach provides more flexibility and better results.
)");
    
    co_return;
}

int main() {
    fmt::print("The Edit API has been deprecated.\n");
    fmt::print("Please use Chat Completion for text editing tasks.\n\n");
    
    asio::io_context io_context;
    asio::co_spawn(io_context, edit_example(), asio::detached);
    io_context.run();
    
    return 0;
}

