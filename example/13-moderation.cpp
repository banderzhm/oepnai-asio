// Moderation API Example - Content filtering and safety
import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> moderation_example(openai::Client& client) {
    fmt::print(R"(=== Moderation API Example ===

The Moderation API classifies text for potentially harmful content.
It's free to use and helps ensure content complies with OpenAI usage policies.

Categories checked:
  - hate: Content promoting hatred based on identity
  - hate/threatening: Hateful content with violence/harm
  - harassment: Content intended to harass/bully
  - harassment/threatening: Harassment with violence/harm
  - self-harm: Content promoting self-harm
  - self-harm/intent: Intent to engage in self-harm
  - self-harm/instructions: Instructions for self-harm
  - sexual: Sexual content
  - sexual/minors: Sexual content involving minors
  - violence: Content depicting violence
  - violence/graphic: Graphic violent content

)");

    // Test different types of content
    std::vector<std::string> test_inputs = {
        "I want to kill them.",
        "How do I bake a delicious chocolate cake?",
        "This product is terrible and waste of money.",
        "You are doing great work! Keep it up!"
    };
    
    for (std::size_t i = 0; i < test_inputs.size(); ++i) {
        fmt::print("--- Test {} ---\n", i + 1);
        fmt::print("Input: \"{}\"\n\n", test_inputs[i]);
        
        openai::ModerationRequest request;
        request.input = test_inputs[i];
        request.model = "text-moderation-latest";
        
        try {
            auto moderation = co_await client.create_moderation(request);
            
            if (!moderation.has_value()) {
                fmt::print("Error: {}\n\n", moderation.error().to_string());
                continue;
            }
            
            fmt::print("Model: {}\n", moderation.value().model);
            fmt::print("Results:\n");
            
            for (const auto& result : moderation.value().results) {
                fmt::print("  Flagged: {}\n\n", result.flagged ? "YES" : "NO");
                
                if (result.flagged) {
                    fmt::print("  Flagged Categories:\n");
                    if (result.categories.hate) fmt::print("    - hate\n");
                    if (result.categories.hate_threatening) fmt::print("    - hate/threatening\n");
                    if (result.categories.harassment) fmt::print("    - harassment\n");
                    if (result.categories.harassment_threatening) fmt::print("    - harassment/threatening\n");
                    if (result.categories.self_harm) fmt::print("    - self-harm\n");
                    if (result.categories.self_harm_intent) fmt::print("    - self-harm/intent\n");
                    if (result.categories.self_harm_instructions) fmt::print("    - self-harm/instructions\n");
                    if (result.categories.sexual) fmt::print("    - sexual\n");
                    if (result.categories.sexual_minors) fmt::print("    - sexual/minors\n");
                    if (result.categories.violence) fmt::print("    - violence\n");
                    if (result.categories.violence_graphic) fmt::print("    - violence/graphic\n");
                    fmt::print("\n");
                }
                
                fmt::print("  Top Category Scores:\n");
                if (result.category_scores.hate > 0.01)
                    fmt::print("    hate: {:.6f}\n", result.category_scores.hate);
                if (result.category_scores.harassment > 0.01)
                    fmt::print("    harassment: {:.6f}\n", result.category_scores.harassment);
                if (result.category_scores.self_harm > 0.01)
                    fmt::print("    self-harm: {:.6f}\n", result.category_scores.self_harm);
                if (result.category_scores.sexual > 0.01)
                    fmt::print("    sexual: {:.6f}\n", result.category_scores.sexual);
                if (result.category_scores.violence > 0.01)
                    fmt::print("    violence: {:.6f}\n", result.category_scores.violence);
                if (result.category_scores.violence_graphic > 0.01)
                    fmt::print("    violence/graphic: {:.6f}\n", result.category_scores.violence_graphic);
            }
            
            fmt::print("\n");
            
        } catch (const std::exception& e) {
            fmt::print("Error: {}\n\n", e.what());
        }
    }
    
    fmt::print(R"(Best Practices:
  - Use moderation before showing user-generated content
  - Combine with your own content filters
  - Review flagged content manually if needed
  - Free to use - no cost for moderation calls

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
    
    asio::co_spawn(io_context, moderation_example(client), asio::detached);
    io_context.run();
    
    return 0;
}

