// Example 09: Embeddings API
// Convert text into vector embeddings for semantic search

import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> embedding_example(openai::Client& client) {
    fmt::print("=== Embeddings API Example ===\n\n");
    
    fmt::print("Embeddings convert text into numerical vectors that capture semantic meaning.\n");
    fmt::print("Use cases: semantic search, clustering, recommendations, anomaly detection.\n\n");
    
    // Simple embedding request
    fmt::print("--- Creating Embedding ---\n");
    fmt::print("Text: \"The food was delicious and the waiter was friendly.\"\n");
    fmt::print("Model: text-embedding-ada-002\n\n");
    
    // Create embedding request
    openai::EmbeddingRequest request;
    request.model = "text-embedding-ada-002";
    request.input = "The food was delicious and the waiter was friendly.";
    
    try {
        auto response = co_await client.create_embedding(request);
        
        if (!response.has_value()) {
            fmt::print("Error: {}\n\n", response.error().to_string());
            co_return;
        }
        
        fmt::print("✓ Embedding created successfully\n\n");
        
        if (!response.value().data.empty()) {
            const auto& embedding = response.value().data[0].embedding;
            
            fmt::print("Embedding vector:\n");
            fmt::print("  Total dimensions: {}\n", embedding.size());
            
            // Show first few dimensions
            fmt::print("  First 10 dimensions: [");
            for (std::size_t i = 0; i < std::min(std::size_t(10), embedding.size()); ++i) {
                fmt::print("{:.6f}", embedding[i]);
                if (i < 9 && i < embedding.size() - 1) fmt::print(", ");
            }
            fmt::print("...]\n\n");
        }
        
        fmt::print("Usage:\n");
        fmt::print("  Total tokens: {}\n\n", response.value().usage.total_tokens);
        
        fmt::print(R"(
Use cases:
  1. Semantic Search: Find similar documents
  2. Clustering: Group related items
  3. Recommendations: Suggest similar content
  4. Classification: Categorize text
  5. Anomaly Detection: Find outliers

Similarity calculation:
  Use cosine similarity between vectors:
  similarity = dot(v1, v2) / (norm(v1) * norm(v2))
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
    
    asio::co_spawn(io_context, embedding_example(client), asio::detached);
    io_context.run();
    
    return 0;
}

