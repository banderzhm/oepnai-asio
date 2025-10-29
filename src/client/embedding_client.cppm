// Embedding Client Module
// Handles OpenAI Embeddings API operations

export module openai.client.embedding;

import asio;
import fmt;
import openai.client.base;
import openai.http_client;
import openai.types.embedding;
import std;

export namespace openai::client {

// Embeddings API client
class EmbeddingClient : public BaseClient {
public:
    using BaseClient::BaseClient;

    // Create embeddings for input text
    asio::awaitable<EmbeddingResponse> create_embedding(const EmbeddingRequest& request) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = "/v1/embeddings";
        req.use_ssl = true;
        req.body = request.to_json();
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        EmbeddingResponse emb_response;
        
        if (response.is_error) {
            emb_response.is_error = true;
            emb_response.error_message = response.error_message;
            co_return emb_response;
        }
        
        emb_response = parse_embedding_response(response.body);
        co_return emb_response;
    }

private:
    EmbeddingResponse parse_embedding_response(const std::string& json_str) {
        EmbeddingResponse response;
        
        // Parse model
        auto model_pos = json_str.find("\"model\":");
        if (model_pos != std::string::npos) {
            model_pos += 8;
            auto model_start = json_str.find("\"", model_pos) + 1;
            auto model_end = json_str.find("\"", model_start);
            response.model = json_str.substr(model_start, model_end - model_start);
        }
        
        // Parse data array
        auto data_pos = json_str.find("\"data\":[");
        if (data_pos != std::string::npos) {
            auto data_start = data_pos + 8;
            auto current_pos = data_start;
            
            while (true) {
                auto obj_start = json_str.find("{", current_pos);
                if (obj_start == std::string::npos) break;
                
                auto obj_end = json_str.find("}", obj_start);
                if (obj_end == std::string::npos) break;
                
                auto obj_json = json_str.substr(obj_start, obj_end - obj_start + 1);
                
                EmbeddingData data;
                
                // Parse index
                auto index_pos = obj_json.find("\"index\":");
                if (index_pos != std::string::npos) {
                    index_pos += 8;
                    auto end_pos = obj_json.find_first_of(",}", index_pos);
                    data.index = std::stoi(obj_json.substr(index_pos, end_pos - index_pos));
                }
                
                // Parse embedding vector
                auto embedding_pos = obj_json.find("\"embedding\":[");
                if (embedding_pos != std::string::npos) {
                    auto vec_start = embedding_pos + 13;
                    auto vec_end = obj_json.find("]", vec_start);
                    auto vec_str = obj_json.substr(vec_start, vec_end - vec_start);
                    
                    // Parse float values
                    std::size_t pos = 0;
                    while (pos < vec_str.length()) {
                        auto next_comma = vec_str.find(",", pos);
                        if (next_comma == std::string::npos) {
                            next_comma = vec_str.length();
                        }
                        
                        auto value_str = vec_str.substr(pos, next_comma - pos);
                        // Trim whitespace
                        value_str.erase(0, value_str.find_first_not_of(" \t\n\r"));
                        value_str.erase(value_str.find_last_not_of(" \t\n\r") + 1);
                        
                        if (!value_str.empty()) {
                            data.embedding.push_back(std::stod(value_str));
                        }
                        
                        pos = next_comma + 1;
                    }
                }
                
                response.data.push_back(data);
                
                current_pos = obj_end + 1;
                auto next_comma = json_str.find(",", current_pos);
                auto array_end = json_str.find("]", current_pos);
                
                if (array_end != std::string::npos && 
                    (next_comma == std::string::npos || array_end < next_comma)) {
                    break;
                }
            }
        }
        
        // Parse usage
        auto usage_pos = json_str.find("\"usage\":{");
        if (usage_pos != std::string::npos) {
            auto usage_section = json_str.substr(usage_pos);
            auto usage_end = usage_section.find("}");
            usage_section = usage_section.substr(0, usage_end + 1);
            
            auto prompt_tokens_pos = usage_section.find("\"prompt_tokens\":");
            if (prompt_tokens_pos != std::string::npos) {
                prompt_tokens_pos += 16;
                auto end_pos = usage_section.find_first_of(",}", prompt_tokens_pos);
                response.usage.prompt_tokens = std::stoi(
                    usage_section.substr(prompt_tokens_pos, end_pos - prompt_tokens_pos)
                );
            }
            
            auto total_tokens_pos = usage_section.find("\"total_tokens\":");
            if (total_tokens_pos != std::string::npos) {
                total_tokens_pos += 15;
                auto end_pos = usage_section.find_first_of(",}", total_tokens_pos);
                response.usage.total_tokens = std::stoi(
                    usage_section.substr(total_tokens_pos, end_pos - total_tokens_pos)
                );
            }
        }
        
        return response;
    }
};

} // namespace openai::client

