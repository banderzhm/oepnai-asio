// Assistant Client Module
// Handles OpenAI Assistants API operations (Beta)

export module openai.client.assistant;

import asio;
import fmt;
import openai.client.base;
import openai.http_client;
import openai.types.assistant;
import std;

export namespace openai::client {

// Assistants API client (Beta)
class AssistantClient : public BaseClient {
public:
    using BaseClient::BaseClient;

    // Create assistant
    asio::awaitable<Assistant> create_assistant(const CreateAssistantRequest& request) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = "/v1/assistants";
        req.use_ssl = true;
        req.body = request.to_json();
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        Assistant assistant;
        
        if (response.is_error) {
            assistant.is_error = true;
            assistant.error_message = response.error_message;
            co_return assistant;
        }
        
        assistant = parse_assistant(response.body);
        co_return assistant;
    }

    // List assistants
    asio::awaitable<AssistantListResponse> list_assistants(int limit = 20) {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/assistants?limit={}", limit);
        req.use_ssl = true;
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        AssistantListResponse list_response;
        
        if (response.is_error) {
            list_response.is_error = true;
            list_response.error_message = response.error_message;
            co_return list_response;
        }
        
        list_response = parse_assistant_list_response(response.body);
        co_return list_response;
    }

    // Retrieve assistant
    asio::awaitable<Assistant> retrieve_assistant(const std::string& assistant_id) {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/assistants/{}", assistant_id);
        req.use_ssl = true;
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        Assistant assistant;
        
        if (response.is_error) {
            assistant.is_error = true;
            assistant.error_message = response.error_message;
            co_return assistant;
        }
        
        assistant = parse_assistant(response.body);
        co_return assistant;
    }

    // Modify assistant
    asio::awaitable<Assistant> modify_assistant(
        const std::string& assistant_id,
        const ModifyAssistantRequest& request
    ) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/assistants/{}", assistant_id);
        req.use_ssl = true;
        req.body = request.to_json();
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        Assistant assistant;
        
        if (response.is_error) {
            assistant.is_error = true;
            assistant.error_message = response.error_message;
            co_return assistant;
        }
        
        assistant = parse_assistant(response.body);
        co_return assistant;
    }

    // Delete assistant
    asio::awaitable<DeleteAssistantResponse> delete_assistant(const std::string& assistant_id) {
        http::Request req;
        req.method = "DELETE";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/assistants/{}", assistant_id);
        req.use_ssl = true;
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        DeleteAssistantResponse delete_response;
        
        if (response.is_error) {
            delete_response.is_error = true;
            delete_response.error_message = response.error_message;
            co_return delete_response;
        }
        
        // Parse id
        auto id_pos = response.body.find("\"id\":");
        if (id_pos != std::string::npos) {
            id_pos += 5;
            auto id_start = response.body.find("\"", id_pos) + 1;
            auto id_end = response.body.find("\"", id_start);
            delete_response.id = response.body.substr(id_start, id_end - id_start);
        }
        
        // Parse deleted
        delete_response.deleted = parse_bool_field(response.body, "deleted");
        
        co_return delete_response;
    }

private:
    Assistant parse_assistant(const std::string& json_str) {
        Assistant assistant;
        
        // Parse id
        auto id_pos = json_str.find("\"id\":");
        if (id_pos != std::string::npos) {
            id_pos += 5;
            auto id_start = json_str.find("\"", id_pos) + 1;
            auto id_end = json_str.find("\"", id_start);
            assistant.id = json_str.substr(id_start, id_end - id_start);
        }
        
        // Parse name
        auto name_pos = json_str.find("\"name\":");
        if (name_pos != std::string::npos) {
            name_pos += 7;
            auto name_start = json_str.find("\"", name_pos);
            if (name_start != std::string::npos) {
                name_start++;
                auto name_end = json_str.find("\"", name_start);
                assistant.name = json_str.substr(name_start, name_end - name_start);
            }
        }
        
        // Parse model
        auto model_pos = json_str.find("\"model\":");
        if (model_pos != std::string::npos) {
            model_pos += 8;
            auto model_start = json_str.find("\"", model_pos) + 1;
            auto model_end = json_str.find("\"", model_start);
            assistant.model = json_str.substr(model_start, model_end - model_start);
        }
        
        // Parse created_at
        auto created_pos = json_str.find("\"created_at\":");
        if (created_pos != std::string::npos) {
            created_pos += 13;
            auto end_pos = json_str.find_first_of(",}", created_pos);
            assistant.created_at = std::stoll(json_str.substr(created_pos, end_pos - created_pos));
        }
        
        // Parse instructions
        auto instr_pos = json_str.find("\"instructions\":");
        if (instr_pos != std::string::npos) {
            instr_pos += 15;
            auto instr_start = json_str.find("\"", instr_pos);
            if (instr_start != std::string::npos) {
                instr_start++;
                auto instr_end = json_str.find("\"", instr_start);
                assistant.instructions = json_str.substr(instr_start, instr_end - instr_start);
            }
        }
        
        return assistant;
    }

    AssistantListResponse parse_assistant_list_response(const std::string& json_str) {
        AssistantListResponse response;
        
        // Parse data array
        auto data_pos = json_str.find("\"data\":[");
        if (data_pos != std::string::npos) {
            auto data_start = data_pos + 8;
            auto current_pos = data_start;
            
            while (true) {
                auto obj_start = json_str.find("{", current_pos);
                if (obj_start == std::string::npos) break;
                
                int brace_count = 1;
                auto obj_end = obj_start + 1;
                
                while (obj_end < json_str.length() && brace_count > 0) {
                    if (json_str[obj_end] == '{') brace_count++;
                    else if (json_str[obj_end] == '}') brace_count--;
                    obj_end++;
                }
                
                auto assistant_json = json_str.substr(obj_start, obj_end - obj_start);
                response.data.push_back(parse_assistant(assistant_json));
                
                current_pos = obj_end;
                auto next_comma = json_str.find(",", current_pos);
                auto array_end = json_str.find("]", current_pos);
                
                if (array_end != std::string::npos && 
                    (next_comma == std::string::npos || array_end < next_comma)) {
                    break;
                }
                current_pos = next_comma + 1;
            }
        }
        
        // Parse has_more
        response.has_more = parse_bool_field(json_str, "has_more");
        
        return response;
    }
};

} // namespace openai::client

