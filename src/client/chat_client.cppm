// Chat Client Module
// Handles OpenAI Chat Completions API operations

export module openai.client.chat;

import asio;
import fmt;
import openai.client.base;
import openai.http_client;
import openai.types.chat;
import std;

export namespace openai::client {

// Chat Completions API client
class ChatClient : public BaseClient {
public:
    using BaseClient::BaseClient;

    // Create chat completion (async)
    asio::awaitable<ChatCompletionResponse> create_chat_completion(
        const ChatCompletionRequest& request
    ) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = "/v1/chat/completions";
        req.use_ssl = true;
        req.body = request.to_json();
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        ChatCompletionResponse chat_response;
        
        if (response.is_error) {
            chat_response.is_error = true;
            chat_response.error_message = response.error_message;
            co_return chat_response;
        }
        
        chat_response = parse_chat_completion_response(response.body);
        co_return chat_response;
    }

    // Create chat completion (sync)
    ChatCompletionResponse create_chat_completion_sync(
        const ChatCompletionRequest& request
    ) {
        ChatCompletionResponse result;
        
        asio::co_spawn(
            io_context_,
            [this, request, &result]() -> asio::awaitable<void> {
                result = co_await create_chat_completion(request);
            },
            asio::detached
        );
        
        io_context_.run();
        io_context_.restart();
        
        return result;
    }

private:
    ChatCompletionResponse parse_chat_completion_response(const std::string& json_str) {
        ChatCompletionResponse response;
        
        try {
            // Parse id
            auto id_pos = json_str.find("\"id\":");
            if (id_pos != std::string::npos) {
                id_pos += 5;
                auto id_start = json_str.find("\"", id_pos) + 1;
                auto id_end = json_str.find("\"", id_start);
                response.id = json_str.substr(id_start, id_end - id_start);
            }
            
            // Parse model
            auto model_pos = json_str.find("\"model\":");
            if (model_pos != std::string::npos) {
                model_pos += 8;
                auto model_start = json_str.find("\"", model_pos) + 1;
                auto model_end = json_str.find("\"", model_start);
                response.model = json_str.substr(model_start, model_end - model_start);
            }
            
            // Parse created
            auto created_pos = json_str.find("\"created\":");
            if (created_pos != std::string::npos) {
                created_pos += 10;
                auto end_pos = json_str.find_first_of(",}", created_pos);
                response.created = std::stoll(json_str.substr(created_pos, end_pos - created_pos));
            }
            
            // Parse choices
            auto choices_pos = json_str.find("\"choices\":[");
            if (choices_pos != std::string::npos) {
                auto choices_start = choices_pos + 11;
                auto current_pos = choices_start;
                
                while (true) {
                    auto choice_start = json_str.find("{", current_pos);
                    if (choice_start == std::string::npos) break;
                    
                    int brace_count = 1;
                    auto choice_end = choice_start + 1;
                    
                    while (choice_end < json_str.length() && brace_count > 0) {
                        if (json_str[choice_end] == '{') brace_count++;
                        else if (json_str[choice_end] == '}') brace_count--;
                        choice_end++;
                    }
                    
                    auto choice_json = json_str.substr(choice_start, choice_end - choice_start);
                    
                    ChatChoice choice;
                    choice.index = static_cast<int>(response.choices.size());
                    
                    // Parse finish_reason
                    auto finish_pos = choice_json.find("\"finish_reason\":");
                    if (finish_pos != std::string::npos) {
                        finish_pos += 16;
                        auto finish_start = choice_json.find("\"", finish_pos);
                        if (finish_start != std::string::npos) {
                            finish_start++;
                            auto finish_end = choice_json.find("\"", finish_start);
                            choice.finish_reason = choice_json.substr(finish_start, finish_end - finish_start);
                        }
                    }
                    
                    // Parse message
                    auto message_pos = choice_json.find("\"message\":{");
                    if (message_pos != std::string::npos) {
                        auto msg_start = message_pos + 11;
                        int msg_brace_count = 1;
                        auto msg_end = msg_start;
                        
                        while (msg_end < choice_json.length() && msg_brace_count > 0) {
                            if (choice_json[msg_end] == '{') msg_brace_count++;
                            else if (choice_json[msg_end] == '}') msg_brace_count--;
                            msg_end++;
                        }
                        
                        auto message_json = choice_json.substr(message_pos, msg_end - message_pos);
                        
                        // Parse role
                        auto role_pos = message_json.find("\"role\":");
                        if (role_pos != std::string::npos) {
                            role_pos += 7;
                            auto role_start = message_json.find("\"", role_pos) + 1;
                            auto role_end = message_json.find("\"", role_start);
                            auto role_str = message_json.substr(role_start, role_end - role_start);
                            
                            if (role_str == "system") choice.message.role = MessageRole::System;
                            else if (role_str == "user") choice.message.role = MessageRole::User;
                            else if (role_str == "assistant") choice.message.role = MessageRole::Assistant;
                        }
                        
                        // Parse content
                        auto content_pos = message_json.find("\"content\":");
                        if (content_pos != std::string::npos) {
                            content_pos += 10;
                            auto content_start = message_json.find("\"", content_pos) + 1;
                            auto content_end = content_start;
                            
                            while (content_end < message_json.length()) {
                                if (message_json[content_end] == '"' && 
                                    (content_end == 0 || message_json[content_end - 1] != '\\')) {
                                    break;
                                }
                                content_end++;
                            }
                            
                            choice.message.content = unescape_json(
                                message_json.substr(content_start, content_end - content_start)
                            );
                        }
                    }
                    
                    response.choices.push_back(choice);
                    
                    current_pos = choice_end;
                    auto next_comma = json_str.find(",", current_pos);
                    auto array_end = json_str.find("]", current_pos);
                    
                    if (array_end != std::string::npos && 
                        (next_comma == std::string::npos || array_end < next_comma)) {
                        break;
                    }
                    current_pos = next_comma + 1;
                }
            }
            
        } catch (const std::exception& e) {
            response.is_error = true;
            response.error_message = fmt::format("Failed to parse chat completion response: {}", e.what());
        }
        
        return response;
    }
};

} // namespace openai::client

