// Model Client Module
// Handles OpenAI Models API operations

export module openai.client.model;

import asio;
import fmt;
import openai.client.base;
import openai.http_client;
import openai.types;
import std;

export namespace openai::client {

// Models API client
class ModelClient : public BaseClient {
public:
    using BaseClient::BaseClient;

    // List all available models
    asio::awaitable<std::expected<ModelList, ApiError>> list_models() {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = "/v1/models";
        req.use_ssl = true;
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code, 
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        co_return parse_model_list(response.body);
    }

    // Retrieve specific model information
    asio::awaitable<std::expected<Model, ApiError>> retrieve_model(const std::string& model_id) {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/models/{}", model_id);
        req.use_ssl = true;
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        co_return parse_model(response.body);
    }

private:
    ModelList parse_model_list(const std::string& json_str) {
        ModelList list;
        list.object = "list";
        
        // Find "data" array
        auto data_pos = json_str.find("\"data\":[");
        if (data_pos == std::string::npos) {
            return list;
        }
        
        // Parse each model in array
        auto models_start = data_pos + 8;
        auto models_end = json_str.find("]", models_start);
        
        std::size_t pos = models_start;
        while (pos < models_end) {
            auto obj_start = json_str.find("{", pos);
            if (obj_start == std::string::npos || obj_start >= models_end) break;
            
            auto obj_end = json_str.find("}", obj_start);
            if (obj_end == std::string::npos) break;
            
            std::string model_json = json_str.substr(obj_start, obj_end - obj_start + 1);
            list.data.push_back(parse_model(model_json));
            
            pos = obj_end + 1;
        }
        
        return list;
    }
    
    Model parse_model(const std::string& json_str) {
        Model model;
        
        // Extract id
        auto id_pos = json_str.find("\"id\":");
        if (id_pos != std::string::npos) {
            auto id_start = json_str.find("\"", id_pos + 5) + 1;
            auto id_end = json_str.find("\"", id_start);
            model.id = json_str.substr(id_start, id_end - id_start);
        }
        
        // Extract object
        auto obj_pos = json_str.find("\"object\":");
        if (obj_pos != std::string::npos) {
            auto obj_start = json_str.find("\"", obj_pos + 9) + 1;
            auto obj_end = json_str.find("\"", obj_start);
            model.object = json_str.substr(obj_start, obj_end - obj_start);
        }
        
        // Extract created
        auto created_pos = json_str.find("\"created\":");
        if (created_pos != std::string::npos) {
            auto created_start = created_pos + 10;
            auto created_end = json_str.find_first_of(",}", created_start);
            std::string created_str = json_str.substr(created_start, created_end - created_start);
            model.created = std::stoll(created_str);
        }
        
        // Extract owned_by
        auto owner_pos = json_str.find("\"owned_by\":");
        if (owner_pos != std::string::npos) {
            auto owner_start = json_str.find("\"", owner_pos + 11) + 1;
            auto owner_end = json_str.find("\"", owner_start);
            model.owned_by = json_str.substr(owner_start, owner_end - owner_start);
        }
        
        return model;
    }
};

} // namespace openai::client
