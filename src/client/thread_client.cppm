// Thread Client Module
// Handles OpenAI Threads and Messages API operations (Beta)

export module openai.client.thread;

import asio;
import fmt;
import openai.client.base;
import openai.http_client;
import openai.types.thread;
import openai.types.common;
import std;

export namespace openai::client {

// Threads and Messages API client (Beta)
class ThreadClient : public BaseClient {
public:
    using BaseClient::BaseClient;

    // ========================================================================
    // Threads API
    // ========================================================================

    // Create thread
    asio::awaitable<std::expected<Thread, ApiError>> create_thread(const CreateThreadRequest& request) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = "/v1/threads";
        req.use_ssl = true;
        req.body = request.to_json();
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        co_return parse_thread(response.body);
    }

    // Retrieve thread
    asio::awaitable<std::expected<Thread, ApiError>> retrieve_thread(const std::string& thread_id) {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/threads/{}", thread_id);
        req.use_ssl = true;
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        co_return parse_thread(response.body);
    }

    // Modify thread
    asio::awaitable<std::expected<Thread, ApiError>> modify_thread(
        const std::string& thread_id,
        const ModifyThreadRequest& request
    ) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/threads/{}", thread_id);
        req.use_ssl = true;
        req.body = request.to_json();
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        co_return parse_thread(response.body);
    }

    // Delete thread
    asio::awaitable<std::expected<DeleteThreadResponse, ApiError>> delete_thread(const std::string& thread_id) {
        http::Request req;
        req.method = "DELETE";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/threads/{}", thread_id);
        req.use_ssl = true;
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        DeleteThreadResponse delete_response;
        
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

    // ========================================================================
    // Messages API
    // ========================================================================

    // Create message
    asio::awaitable<std::expected<ThreadMessage, ApiError>> create_message(
        const std::string& thread_id,
        const CreateMessageRequest& request
    ) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/threads/{}/messages", thread_id);
        req.use_ssl = true;
        req.body = request.to_json();
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        co_return parse_message(response.body);
    }

    // List messages
    asio::awaitable<std::expected<ThreadMessageListResponse, ApiError>> list_messages(
        const std::string& thread_id,
        int limit = 20
    ) {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/threads/{}/messages?limit={}", thread_id, limit);
        req.use_ssl = true;
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        co_return parse_message_list_response(response.body);
    }

    // Retrieve message
    asio::awaitable<std::expected<ThreadMessage, ApiError>> retrieve_message(
        const std::string& thread_id,
        const std::string& message_id
    ) {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/threads/{}/messages/{}", thread_id, message_id);
        req.use_ssl = true;
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        co_return parse_message(response.body);
    }

    // Modify message
    asio::awaitable<std::expected<ThreadMessage, ApiError>> modify_message(
        const std::string& thread_id,
        const std::string& message_id,
        const ModifyMessageRequest& request
    ) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/threads/{}/messages/{}", thread_id, message_id);
        req.use_ssl = true;
        req.body = request.to_json();
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        co_return parse_message(response.body);
    }

private:
    Thread parse_thread(const std::string& json_str) {
        Thread thread;
        
        // Parse id
        auto id_pos = json_str.find("\"id\":");
        if (id_pos != std::string::npos) {
            id_pos += 5;
            auto id_start = json_str.find("\"", id_pos) + 1;
            auto id_end = json_str.find("\"", id_start);
            thread.id = json_str.substr(id_start, id_end - id_start);
        }
        
        // Parse created_at
        auto created_pos = json_str.find("\"created_at\":");
        if (created_pos != std::string::npos) {
            created_pos += 13;
            auto end_pos = json_str.find_first_of(",}", created_pos);
            thread.created_at = std::stoll(json_str.substr(created_pos, end_pos - created_pos));
        }
        
        return thread;
    }

    ThreadMessage parse_message(const std::string& json_str) {
        ThreadMessage message;
        
        // Parse id
        auto id_pos = json_str.find("\"id\":");
        if (id_pos != std::string::npos) {
            id_pos += 5;
            auto id_start = json_str.find("\"", id_pos) + 1;
            auto id_end = json_str.find("\"", id_start);
            message.id = json_str.substr(id_start, id_end - id_start);
        }
        
        // Parse thread_id
        auto thread_pos = json_str.find("\"thread_id\":");
        if (thread_pos != std::string::npos) {
            thread_pos += 12;
            auto thread_start = json_str.find("\"", thread_pos) + 1;
            auto thread_end = json_str.find("\"", thread_start);
            message.thread_id = json_str.substr(thread_start, thread_end - thread_start);
        }
        
        // Parse role
        auto role_pos = json_str.find("\"role\":");
        if (role_pos != std::string::npos) {
            role_pos += 7;
            auto role_start = json_str.find("\"", role_pos) + 1;
            auto role_end = json_str.find("\"", role_start);
            auto role_str = json_str.substr(role_start, role_end - role_start);
            message.role = string_to_thread_message_role(role_str);
        }
        
        // Parse created_at
        auto created_pos = json_str.find("\"created_at\":");
        if (created_pos != std::string::npos) {
            created_pos += 13;
            auto end_pos = json_str.find_first_of(",}", created_pos);
            message.created_at = std::stoll(json_str.substr(created_pos, end_pos - created_pos));
        }
        
        return message;
    }

    ThreadMessageListResponse parse_message_list_response(const std::string& json_str) {
        ThreadMessageListResponse response;
        
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
                
                auto message_json = json_str.substr(obj_start, obj_end - obj_start);
                response.data.push_back(parse_message(message_json));
                
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

