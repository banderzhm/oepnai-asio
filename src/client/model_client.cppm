// Model Client Module
// Handles OpenAI Models API operations

export module openai.client.model;

import asio;
import fmt;
import openai.client.base;
import openai.http_client;
import openai.types.model;
import std;

export namespace openai::client {

// Models API client
class ModelClient : public BaseClient {
public:
    using BaseClient::BaseClient;

    // List all available models
    asio::awaitable<ModelList> list_models() {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = "/v1/models";
        req.use_ssl = true;
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        ModelList model_list;
        
        if (response.is_error) {
            model_list.is_error = true;
            model_list.error_message = response.error_message;
            co_return model_list;
        }
        
        model_list = parse_model_list(response.body);
        co_return model_list;
    }

    // Retrieve specific model information
    asio::awaitable<Model> retrieve_model(const std::string& model_id) {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/models/{}", model_id);
        req.use_ssl = true;
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        Model model;
        
        if (response.is_error) {
            model.is_error = true;
            model.error_message = response.error_message;
            co_return model;
        }
        
        model = parse_model(response.body);
        co_return model;
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
        auto current_pos = models_start;
        
        while (true) {
            auto model_start = json_str.find("{", current_pos);
            if (model_start == std::string::npos) break;
            
            auto model_end = json_str.find("}", model_start);
            if (model_end == std::string::npos) break;
            
            auto model_json = json_str.substr(model_start, model_end - model_start + 1);
            list.data.push_back(parse_model(model_json));
            
            current_pos = model_end + 1;
            auto next_comma = json_str.find(",", current_pos);
            auto array_end = json_str.find("]", current_pos);
            
            if (array_end != std::string::npos && 
                (next_comma == std::string::npos || array_end < next_comma)) {
                break;
            }
        }
        
        return list;
    }

    Model parse_model(const std::string& json_str) {
        Model model;
        
        // Parse id
        auto id_pos = json_str.find("\"id\":");
        if (id_pos != std::string::npos) {
            id_pos += 5;
            auto id_start = json_str.find("\"", id_pos) + 1;
            auto id_end = json_str.find("\"", id_start);
            model.id = json_str.substr(id_start, id_end - id_start);
        }
        
        // Parse created
        auto created_pos = json_str.find("\"created\":");
        if (created_pos != std::string::npos) {
            created_pos += 10;
            auto end_pos = json_str.find_first_of(",}", created_pos);
            model.created = std::stoll(json_str.substr(created_pos, end_pos - created_pos));
        }
        
        // Parse owned_by
        auto owned_pos = json_str.find("\"owned_by\":");
        if (owned_pos != std::string::npos) {
            owned_pos += 11;
            auto owned_start = json_str.find("\"", owned_pos) + 1;
            auto owned_end = json_str.find("\"", owned_start);
            model.owned_by = json_str.substr(owned_start, owned_end - owned_start);
        }
        
        return model;
    }
};

} // namespace openai::client

