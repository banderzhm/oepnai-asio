// Example 09: Embeddings API
// Convert text into vector embeddings for semantic search

import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> embedding_example(openai::Client& client, asio::io_context& io_context) {
    fmt::print("=== Embeddings API Example ===\n\n");
    
    fmt::print("Embeddings convert text into numerical vectors that capture semantic meaning.\n");
    fmt::print("Use cases: semantic search, clustering, recommendations, anomaly detection.\n\n");
    
    // Simple embedding request
    fmt::print("--- Creating Embedding ---\n");
    fmt::print("Text: \"The food was delicious and the waiter was friendly.\"\n");
    fmt::print("Model: text-embedding-ada-002\n\n");
    
    // Create embedding request manually using HTTP client
    openai::http::Request req;
    req.method = "POST";
    req.path = "/v1/embeddings";
    req.host = "api.openai.com";
    req.use_ssl = true;
    
    // Build JSON request body
    req.body = R"({
        "model": "text-embedding-ada-002",
        "input": "The food was delicious and the waiter was friendly."
    })";
    
    req.headers["Content-Type"] = "application/json";
    req.headers["Authorization"] = fmt::format("Bearer {}", std::getenv("OPENAI_API_KEY"));
    
    try {
        // Use the http_client from openai to make request
        openai::http::Client http_client(io_context);
        auto response = co_await http_client.async_request(req);
        
        if (response.is_error) {
            fmt::print("Error: {}\n", response.error_message);
            co_return;
        }
        
        if (response.status_code != 200) {
            fmt::print("API Error ({}): {}\n", response.status_code, response.body);
            co_return;
        }
        
        // Parse response to show embedding info
        const auto& body = response.body;
        
        // Extract embedding dimension (ada-002 has 1536 dimensions)
        auto data_pos = body.find("\"data\"");
        if (data_pos != std::string::npos) {
            fmt::print("✓ Embedding created successfully\n\n");
            
            // Extract some sample values
            auto embedding_pos = body.find("\"embedding\":[", data_pos);
            if (embedding_pos != std::string::npos) {
                auto values_start = embedding_pos + 13;
                auto values_end = body.find("]", values_start);
                auto sample_values = body.substr(values_start, std::min(size_t(100), values_end - values_start));
                
                fmt::print("Embedding vector (first few dimensions):\n");
                fmt::print("[{}...]\n\n", sample_values);
                fmt::print("Total dimensions: 1536 (text-embedding-ada-002)\n\n");
            }
            
            // Extract token usage
            auto usage_pos = body.find("\"total_tokens\":");
            if (usage_pos != std::string::npos) {
                auto token_start = usage_pos + 15;
                auto token_end = body.find_first_of(",}", token_start);
                auto tokens = body.substr(token_start, token_end - token_start);
                fmt::print("Tokens used: {}\n", tokens);
            }
        }
        
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
    
    asio::co_spawn(io_context, embedding_example(client, io_context), asio::detached);
    io_context.run();
    
    return 0;
}

