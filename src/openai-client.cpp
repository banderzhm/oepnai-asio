// OpenAI Client Module - Implementation

module openai.client;

import asio;
import fmt;
import openai.http_client;
import openai.types;
import std;

namespace openai {

Client::Client(std::string api_key, asio::io_context& io_context)
    : api_key_(std::move(api_key))
    , http_client_(io_context)
    , api_base_("api.openai.com")
    , io_context_(io_context) {}

void Client::set_api_base(std::string base_url) {
    api_base_ = std::move(base_url);
}

void Client::set_organization(std::string org_id) {
    organization_id_ = std::move(org_id);
}

asio::awaitable<ModelList> Client::list_models() {
    http::Request req;
    req.method = "GET";
    req.path = "/v1/models";
    req.host = api_base_;
    req.use_ssl = true;
    add_auth_headers(req);

    auto response = co_await http_client_.async_request(req);
    
    if (response.is_error) {
        throw std::runtime_error(
            fmt::format("Failed to list models: {}", response.error_message)
        );
    }

    if (response.status_code != 200) {
        throw std::runtime_error(
            fmt::format("API returned status {}: {}", 
                response.status_code, response.body)
        );
    }

    ModelList models;
    models.object = "list";
    
    // Parse JSON response to extract model data
    const auto& body = response.body;
    auto data_pos = body.find("\"data\":[");
    if (data_pos != std::string::npos) {
        auto array_start = data_pos + 8;
        auto current_pos = array_start;
        
        while (current_pos < body.size()) {
            auto obj_start = body.find("{", current_pos);
            if (obj_start == std::string::npos || obj_start > body.find("]", current_pos)) break;
            
            Model model;
            
            // Extract id
            auto id_pos = body.find("\"id\":\"", obj_start);
            if (id_pos != std::string::npos && id_pos < body.find("}", obj_start)) {
                id_pos += 6;
                auto id_end = body.find("\"", id_pos);
                model.id = body.substr(id_pos, id_end - id_pos);
            }
            
            // Extract owned_by
            auto owned_pos = body.find("\"owned_by\":\"", obj_start);
            if (owned_pos != std::string::npos && owned_pos < body.find("}", obj_start)) {
                owned_pos += 12;
                auto owned_end = body.find("\"", owned_pos);
                model.owned_by = body.substr(owned_pos, owned_end - owned_pos);
            }
            
            models.data.push_back(model);
            current_pos = body.find("}", obj_start) + 1;
        }
    }
    
    co_return models;
}

asio::awaitable<Model> Client::retrieve_model(const std::string& model_id) {
    http::Request req;
    req.method = "GET";
    req.path = fmt::format("/v1/models/{}", model_id);
    req.host = api_base_;
    req.use_ssl = true;
    add_auth_headers(req);

    auto response = co_await http_client_.async_request(req);
    
    if (response.is_error) {
        throw std::runtime_error(
            fmt::format("Failed to retrieve model: {}", response.error_message)
        );
    }

    if (response.status_code != 200) {
        throw std::runtime_error(
            fmt::format("API returned status {}: {}", 
                response.status_code, response.body)
        );
    }

    Model model;
    const auto& body = response.body;
    
    // Extract id
    auto id_pos = body.find("\"id\":\"");
    if (id_pos != std::string::npos) {
        id_pos += 6;
        auto id_end = body.find("\"", id_pos);
        model.id = body.substr(id_pos, id_end - id_pos);
    }
    
    // Extract owned_by
    auto owned_pos = body.find("\"owned_by\":\"");
    if (owned_pos != std::string::npos) {
        owned_pos += 12;
        auto owned_end = body.find("\"", owned_pos);
        model.owned_by = body.substr(owned_pos, owned_end - owned_pos);
    }
    
    co_return model;
}

asio::awaitable<ChatCompletionResponse> Client::create_chat_completion(
    const ChatCompletionRequest& request
) {
    http::Request req;
    req.method = "POST";
    req.path = "/v1/chat/completions";
    req.host = api_base_;
    req.use_ssl = true;
    req.body = request.to_json();
    add_auth_headers(req);
    req.headers["Content-Type"] = "application/json";

    auto response = co_await http_client_.async_request(req);
    
    if (response.is_error) {
        ChatCompletionResponse error_response;
        error_response.is_error = true;
        error_response.error_message = response.error_message;
        co_return error_response;
    }

    if (response.status_code != 200) {
        ChatCompletionResponse error_response;
        error_response.is_error = true;
        error_response.error_message = fmt::format(
            "API returned status {}: {}", 
            response.status_code, 
            response.body
        );
        co_return error_response;
    }

    auto result = parse_chat_completion_response(response.body);
    co_return result;
}

asio::awaitable<std::string> Client::create_completion(const CompletionRequest& request) {
    http::Request req;
    req.method = "POST";
    req.path = "/v1/completions";
    req.host = api_base_;
    req.use_ssl = true;
    req.body = request.to_json();
    add_auth_headers(req);
    req.headers["Content-Type"] = "application/json";

    auto response = co_await http_client_.async_request(req);
    
    if (response.is_error) {
        throw std::runtime_error(
            fmt::format("Failed to create completion: {}", response.error_message)
        );
    }

    if (response.status_code != 200) {
        throw std::runtime_error(
            fmt::format("API returned status {}: {}", 
                response.status_code, response.body)
        );
    }

    co_return response.body;
}

ChatCompletionResponse Client::create_chat_completion_sync(
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

asio::awaitable<ImageResponse> Client::generate_image(const ImageGenerationRequest& request) {
    http::Request req;
    req.method = "POST";
    req.path = "/v1/images/generations";
    req.host = api_base_;
    req.use_ssl = true;
    req.body = request.to_json();
    add_auth_headers(req);
    req.headers["Content-Type"] = "application/json";

    auto response = co_await http_client_.async_request(req);
    
    if (response.is_error) {
        ImageResponse error_response;
        error_response.is_error = true;
        error_response.error_message = response.error_message;
        co_return error_response;
    }

    if (response.status_code != 200) {
        ImageResponse error_response;
        error_response.is_error = true;
        error_response.error_message = fmt::format(
            "API returned status {}: {}", 
            response.status_code, 
            response.body
        );
        co_return error_response;
    }

    auto result = parse_image_response(response.body);
    co_return result;
}

asio::awaitable<EmbeddingResponse> Client::create_embedding(const EmbeddingRequest& request) {
    http::Request req;
    req.method = "POST";
    req.path = "/v1/embeddings";
    req.host = api_base_;
    req.use_ssl = true;
    req.body = request.to_json();
    add_auth_headers(req);
    req.headers["Content-Type"] = "application/json";

    auto response = co_await http_client_.async_request(req);
    
    if (response.is_error) {
        EmbeddingResponse error_response;
        error_response.is_error = true;
        error_response.error_message = response.error_message;
        co_return error_response;
    }

    if (response.status_code != 200) {
        EmbeddingResponse error_response;
        error_response.is_error = true;
        error_response.error_message = fmt::format(
            "API returned status {}: {}", 
            response.status_code, 
            response.body
        );
        co_return error_response;
    }

    auto result = parse_embedding_response(response.body);
    co_return result;
}

asio::awaitable<FileListResponse> Client::list_files() {
    http::Request req;
    req.method = "GET";
    req.path = "/v1/files";
    req.host = api_base_;
    req.use_ssl = true;
    add_auth_headers(req);

    auto response = co_await http_client_.async_request(req);
    
    if (response.is_error) {
        FileListResponse error_response;
        error_response.is_error = true;
        error_response.error_message = response.error_message;
        co_return error_response;
    }

    if (response.status_code != 200) {
        FileListResponse error_response;
        error_response.is_error = true;
        error_response.error_message = fmt::format(
            "API returned status {}: {}", 
            response.status_code, 
            response.body
        );
        co_return error_response;
    }

    auto result = parse_file_list_response(response.body);
    co_return result;
}

asio::awaitable<FileObject> Client::retrieve_file(const std::string& file_id) {
    http::Request req;
    req.method = "GET";
    req.path = fmt::format("/v1/files/{}", file_id);
    req.host = api_base_;
    req.use_ssl = true;
    add_auth_headers(req);

    auto response = co_await http_client_.async_request(req);
    
    if (response.is_error) {
        throw std::runtime_error(
            fmt::format("Failed to retrieve file: {}", response.error_message)
        );
    }

    if (response.status_code != 200) {
        throw std::runtime_error(
            fmt::format("API returned status {}: {}", 
                response.status_code, response.body)
        );
    }

    auto result = parse_file_object(response.body);
    co_return result;
}

asio::awaitable<bool> Client::delete_file(const std::string& file_id) {
    http::Request req;
    req.method = "DELETE";
    req.path = fmt::format("/v1/files/{}", file_id);
    req.host = api_base_;
    req.use_ssl = true;
    add_auth_headers(req);

    auto response = co_await http_client_.async_request(req);
    
    if (response.is_error) {
        throw std::runtime_error(
            fmt::format("Failed to delete file: {}", response.error_message)
        );
    }

    if (response.status_code != 200) {
        throw std::runtime_error(
            fmt::format("API returned status {}: {}", 
                response.status_code, response.body)
        );
    }

    // Check if deletion was successful
    auto deleted_pos = response.body.find("\"deleted\":true");
    co_return (deleted_pos != std::string::npos);
}

asio::awaitable<FineTuningJobResponse> Client::create_fine_tuning_job(const FineTuningRequest& request) {
    http::Request req;
    req.method = "POST";
    req.path = "/v1/fine_tuning/jobs";
    req.host = api_base_;
    req.use_ssl = true;
    req.body = request.to_json();
    add_auth_headers(req);
    req.headers["Content-Type"] = "application/json";

    auto response = co_await http_client_.async_request(req);
    
    if (response.is_error) {
        FineTuningJobResponse error_response;
        error_response.is_error = true;
        error_response.error_message = response.error_message;
        co_return error_response;
    }

    if (response.status_code != 200) {
        FineTuningJobResponse error_response;
        error_response.is_error = true;
        error_response.error_message = fmt::format(
            "API returned status {}: {}", 
            response.status_code, 
            response.body
        );
        co_return error_response;
    }

    FineTuningJobResponse result;
    result.job = parse_fine_tuning_job(response.body);
    co_return result;
}

asio::awaitable<FineTuningJobListResponse> Client::list_fine_tuning_jobs(int limit) {
    http::Request req;
    req.method = "GET";
    req.path = fmt::format("/v1/fine_tuning/jobs?limit={}", limit);
    req.host = api_base_;
    req.use_ssl = true;
    add_auth_headers(req);

    auto response = co_await http_client_.async_request(req);
    
    if (response.is_error) {
        FineTuningJobListResponse error_response;
        error_response.is_error = true;
        error_response.error_message = response.error_message;
        co_return error_response;
    }

    if (response.status_code != 200) {
        FineTuningJobListResponse error_response;
        error_response.is_error = true;
        error_response.error_message = fmt::format(
            "API returned status {}: {}", 
            response.status_code, 
            response.body
        );
        co_return error_response;
    }

    auto result = parse_fine_tuning_job_list_response(response.body);
    co_return result;
}

asio::awaitable<FineTuningJobResponse> Client::retrieve_fine_tuning_job(const std::string& job_id) {
    http::Request req;
    req.method = "GET";
    req.path = fmt::format("/v1/fine_tuning/jobs/{}", job_id);
    req.host = api_base_;
    req.use_ssl = true;
    add_auth_headers(req);

    auto response = co_await http_client_.async_request(req);
    
    if (response.is_error) {
        FineTuningJobResponse error_response;
        error_response.is_error = true;
        error_response.error_message = response.error_message;
        co_return error_response;
    }

    if (response.status_code != 200) {
        FineTuningJobResponse error_response;
        error_response.is_error = true;
        error_response.error_message = fmt::format(
            "API returned status {}: {}", 
            response.status_code, 
            response.body
        );
        co_return error_response;
    }

    FineTuningJobResponse result;
    result.job = parse_fine_tuning_job(response.body);
    co_return result;
}

asio::awaitable<FineTuningJobResponse> Client::cancel_fine_tuning_job(const std::string& job_id) {
    http::Request req;
    req.method = "POST";
    req.path = fmt::format("/v1/fine_tuning/jobs/{}/cancel", job_id);
    req.host = api_base_;
    req.use_ssl = true;
    add_auth_headers(req);

    auto response = co_await http_client_.async_request(req);
    
    if (response.is_error) {
        FineTuningJobResponse error_response;
        error_response.is_error = true;
        error_response.error_message = response.error_message;
        co_return error_response;
    }

    if (response.status_code != 200) {
        FineTuningJobResponse error_response;
        error_response.is_error = true;
        error_response.error_message = fmt::format(
            "API returned status {}: {}", 
            response.status_code, 
            response.body
        );
        co_return error_response;
    }

    FineTuningJobResponse result;
    result.job = parse_fine_tuning_job(response.body);
    co_return result;
}

asio::awaitable<AudioResponse> Client::create_transcription(const AudioTranscriptionRequest& request) {
    // Note: File upload requires multipart/form-data which is complex
    // For now, returning a placeholder that indicates the feature requires file handling
    AudioResponse response;
    response.is_error = true;
    response.error_message = "Audio transcription requires multipart/form-data file upload support. "
                              "This feature needs additional HTTP client implementation for file uploads.";
    co_return response;
}

asio::awaitable<AudioResponse> Client::create_translation(const AudioTranslationRequest& request) {
    // Note: File upload requires multipart/form-data which is complex
    // For now, returning a placeholder that indicates the feature requires file handling
    AudioResponse response;
    response.is_error = true;
    response.error_message = "Audio translation requires multipart/form-data file upload support. "
                              "This feature needs additional HTTP client implementation for file uploads.";
    co_return response;
}

asio::awaitable<ModerationResponse> Client::create_moderation(const ModerationRequest& request) {
    http::Request req;
    req.method = "POST";
    req.path = "/v1/moderations";
    req.host = api_base_;
    req.use_ssl = true;
    req.body = request.to_json();
    add_auth_headers(req);
    req.headers["Content-Type"] = "application/json";

    auto response = co_await http_client_.async_request(req);
    
    if (response.is_error) {
        ModerationResponse error_response;
        error_response.is_error = true;
        error_response.error_message = response.error_message;
        co_return error_response;
    }

    if (response.status_code != 200) {
        ModerationResponse error_response;
        error_response.is_error = true;
        error_response.error_message = fmt::format(
            "API returned status {}: {}", 
            response.status_code, 
            response.body
        );
        co_return error_response;
    }

    auto result = parse_moderation_response(response.body);
    co_return result;
}

void Client::add_auth_headers(http::Request& req) {
    req.headers["Authorization"] = fmt::format("Bearer {}", api_key_);
    
    if (!organization_id_.empty()) {
        req.headers["OpenAI-Organization"] = organization_id_;
    }
    
    req.headers["User-Agent"] = "openai-asio-cpp/1.0";
}

ChatCompletionResponse Client::parse_chat_completion_response(const std::string& json_str) {
    ChatCompletionResponse response;
    
    try {
        auto id_pos = json_str.find(R"("id":")");
        if (id_pos != std::string::npos) {
            id_pos += 6;
            auto end_pos = json_str.find('"', id_pos);
            response.id = json_str.substr(id_pos, end_pos - id_pos);
        }
        
        auto model_pos = json_str.find(R"("model":")");
        if (model_pos != std::string::npos) {
            model_pos += 9;
            auto end_pos = json_str.find('"', model_pos);
            response.model = json_str.substr(model_pos, end_pos - model_pos);
        }
        
        auto content_pos = json_str.find(R"("content":")");
        if (content_pos != std::string::npos) {
            content_pos += 11;
            auto end_pos = json_str.find('"', content_pos);
            
            while (end_pos != std::string::npos && 
                   json_str[end_pos - 1] == '\\') {
                end_pos = json_str.find('"', end_pos + 1);
            }
            
            std::string content = json_str.substr(content_pos, end_pos - content_pos);
            
            ChatCompletionChoice choice;
            choice.index = 0;
            choice.message.role = MessageRole::Assistant;
            choice.message.content = unescape_json(content);
            
            response.choices.push_back(choice);
        }
        
        auto prompt_tokens_pos = json_str.find(R"("prompt_tokens":)");
        if (prompt_tokens_pos != std::string::npos) {
            prompt_tokens_pos += 16;
            auto end_pos = json_str.find_first_of(",}", prompt_tokens_pos);
            response.usage.prompt_tokens = std::stoi(
                json_str.substr(prompt_tokens_pos, end_pos - prompt_tokens_pos)
            );
        }
        
        auto completion_tokens_pos = json_str.find(R"("completion_tokens":)");
        if (completion_tokens_pos != std::string::npos) {
            completion_tokens_pos += 20;
            auto end_pos = json_str.find_first_of(",}", completion_tokens_pos);
            response.usage.completion_tokens = std::stoi(
                json_str.substr(completion_tokens_pos, end_pos - completion_tokens_pos)
            );
        }
        
        auto total_tokens_pos = json_str.find(R"("total_tokens":)");
        if (total_tokens_pos != std::string::npos) {
            total_tokens_pos += 15;
            auto end_pos = json_str.find_first_of(",}", total_tokens_pos);
            response.usage.total_tokens = std::stoi(
                json_str.substr(total_tokens_pos, end_pos - total_tokens_pos)
            );
        }
        
    } catch (const std::exception& e) {
        response.is_error = true;
        response.error_message = fmt::format("Failed to parse response: {}", e.what());
    }
    
    return response;
}

std::string Client::unescape_json(const std::string& str) {
    std::ostringstream unescaped;
    for (std::size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case '"': unescaped << '"'; ++i; break;
                case '\\': unescaped << '\\'; ++i; break;
                case 'b': unescaped << '\b'; ++i; break;
                case 'f': unescaped << '\f'; ++i; break;
                case 'n': unescaped << '\n'; ++i; break;
                case 'r': unescaped << '\r'; ++i; break;
                case 't': unescaped << '\t'; ++i; break;
                default: unescaped << str[i];
            }
        } else {
            unescaped << str[i];
        }
    }
    return unescaped.str();
}

ImageResponse Client::parse_image_response(const std::string& json_str) {
    ImageResponse response;
    
    try {
        auto created_pos = json_str.find("\"created\":");
        if (created_pos != std::string::npos) {
            created_pos += 10;
            auto end_pos = json_str.find_first_of(",}", created_pos);
            response.created = std::stoll(json_str.substr(created_pos, end_pos - created_pos));
        }
        
        auto data_pos = json_str.find("\"data\":[");
        if (data_pos != std::string::npos) {
            auto current_pos = data_pos + 8;
            
            while (current_pos < json_str.size()) {
                auto obj_start = json_str.find("{", current_pos);
                if (obj_start == std::string::npos || obj_start > json_str.find("]", current_pos)) break;
                
                ImageData img_data;
                
                auto url_pos = json_str.find("\"url\":\"", obj_start);
                if (url_pos != std::string::npos && url_pos < json_str.find("}", obj_start)) {
                    url_pos += 7;
                    auto url_end = json_str.find("\"", url_pos);
                    img_data.url = json_str.substr(url_pos, url_end - url_pos);
                }
                
                auto revised_prompt_pos = json_str.find("\"revised_prompt\":\"", obj_start);
                if (revised_prompt_pos != std::string::npos && revised_prompt_pos < json_str.find("}", obj_start)) {
                    revised_prompt_pos += 18;
                    auto prompt_end = json_str.find("\"", revised_prompt_pos);
                    img_data.revised_prompt = unescape_json(json_str.substr(revised_prompt_pos, prompt_end - revised_prompt_pos));
                }
                
                response.data.push_back(img_data);
                current_pos = json_str.find("}", obj_start) + 1;
            }
        }
    } catch (const std::exception& e) {
        response.is_error = true;
        response.error_message = fmt::format("Failed to parse image response: {}", e.what());
    }
    
    return response;
}

EmbeddingResponse Client::parse_embedding_response(const std::string& json_str) {
    EmbeddingResponse response;
    
    try {
        auto model_pos = json_str.find("\"model\":\"");
        if (model_pos != std::string::npos) {
            model_pos += 9;
            auto end_pos = json_str.find("\"", model_pos);
            response.model = json_str.substr(model_pos, end_pos - model_pos);
        }
        
        auto object_pos = json_str.find("\"object\":\"");
        if (object_pos != std::string::npos) {
            object_pos += 10;
            auto end_pos = json_str.find("\"", object_pos);
            response.object = json_str.substr(object_pos, end_pos - object_pos);
        }
        
        auto data_pos = json_str.find("\"data\":[");
        if (data_pos != std::string::npos) {
            auto current_pos = data_pos + 8;
            
            while (current_pos < json_str.size()) {
                auto obj_start = json_str.find("{", current_pos);
                if (obj_start == std::string::npos || obj_start > json_str.find("]", current_pos)) break;
                
                EmbeddingData emb_data;
                
                auto index_pos = json_str.find("\"index\":", obj_start);
                if (index_pos != std::string::npos && index_pos < json_str.find("}", obj_start)) {
                    index_pos += 8;
                    auto end_pos = json_str.find_first_of(",}", index_pos);
                    emb_data.index = std::stoi(json_str.substr(index_pos, end_pos - index_pos));
                }
                
                auto embedding_pos = json_str.find("\"embedding\":[", obj_start);
                if (embedding_pos != std::string::npos && embedding_pos < json_str.find("}", obj_start)) {
                    auto array_start = embedding_pos + 13;
                    auto array_end = json_str.find("]", array_start);
                    
                    auto values_str = json_str.substr(array_start, array_end - array_start);
                    std::istringstream values_stream(values_str);
                    std::string value;
                    
                    while (std::getline(values_stream, value, ',')) {
                        emb_data.embedding.push_back(std::stof(value));
                    }
                }
                
                response.data.push_back(emb_data);
                current_pos = json_str.find("}", obj_start) + 1;
            }
        }
        
        auto prompt_tokens_pos = json_str.find("\"prompt_tokens\":");
        if (prompt_tokens_pos != std::string::npos) {
            prompt_tokens_pos += 16;
            auto end_pos = json_str.find_first_of(",}", prompt_tokens_pos);
            response.usage.prompt_tokens = std::stoi(json_str.substr(prompt_tokens_pos, end_pos - prompt_tokens_pos));
        }
        
        auto total_tokens_pos = json_str.find("\"total_tokens\":");
        if (total_tokens_pos != std::string::npos) {
            total_tokens_pos += 15;
            auto end_pos = json_str.find_first_of(",}", total_tokens_pos);
            response.usage.total_tokens = std::stoi(json_str.substr(total_tokens_pos, end_pos - total_tokens_pos));
        }
        
    } catch (const std::exception& e) {
        response.is_error = true;
        response.error_message = fmt::format("Failed to parse embedding response: {}", e.what());
    }
    
    return response;
}

FileObject Client::parse_file_object(const std::string& json_str) {
    FileObject file_obj;
    
    try {
        auto id_pos = json_str.find("\"id\":\"");
        if (id_pos != std::string::npos) {
            id_pos += 6;
            auto end_pos = json_str.find("\"", id_pos);
            file_obj.id = json_str.substr(id_pos, end_pos - id_pos);
        }
        
        auto filename_pos = json_str.find("\"filename\":\"");
        if (filename_pos != std::string::npos) {
            filename_pos += 12;
            auto end_pos = json_str.find("\"", filename_pos);
            file_obj.filename = json_str.substr(filename_pos, end_pos - filename_pos);
        }
        
        auto purpose_pos = json_str.find("\"purpose\":\"");
        if (purpose_pos != std::string::npos) {
            purpose_pos += 11;
            auto end_pos = json_str.find("\"", purpose_pos);
            file_obj.purpose = json_str.substr(purpose_pos, end_pos - purpose_pos);
        }
        
        auto status_pos = json_str.find("\"status\":\"");
        if (status_pos != std::string::npos) {
            status_pos += 10;
            auto end_pos = json_str.find("\"", status_pos);
            file_obj.status = json_str.substr(status_pos, end_pos - status_pos);
        }
        
        auto bytes_pos = json_str.find("\"bytes\":");
        if (bytes_pos != std::string::npos) {
            bytes_pos += 8;
            auto end_pos = json_str.find_first_of(",}", bytes_pos);
            file_obj.bytes = std::stoll(json_str.substr(bytes_pos, end_pos - bytes_pos));
        }
        
        auto created_pos = json_str.find("\"created_at\":");
        if (created_pos != std::string::npos) {
            created_pos += 13;
            auto end_pos = json_str.find_first_of(",}", created_pos);
            file_obj.created_at = std::stoll(json_str.substr(created_pos, end_pos - created_pos));
        }
        
    } catch (const std::exception&) {
        // Return partially populated object
    }
    
    return file_obj;
}

FileListResponse Client::parse_file_list_response(const std::string& json_str) {
    FileListResponse response;
    
    try {
        auto object_pos = json_str.find("\"object\":\"");
        if (object_pos != std::string::npos) {
            object_pos += 10;
            auto end_pos = json_str.find("\"", object_pos);
            response.object = json_str.substr(object_pos, end_pos - object_pos);
        }
        
        auto data_pos = json_str.find("\"data\":[");
        if (data_pos != std::string::npos) {
            auto current_pos = data_pos + 8;
            
            while (current_pos < json_str.size()) {
                auto obj_start = json_str.find("{", current_pos);
                if (obj_start == std::string::npos || obj_start > json_str.find("]", current_pos)) break;
                
                auto obj_end = json_str.find("}", obj_start);
                auto file_json = json_str.substr(obj_start, obj_end - obj_start + 1);
                
                response.data.push_back(parse_file_object(file_json));
                current_pos = obj_end + 1;
            }
        }
    } catch (const std::exception& e) {
        response.is_error = true;
        response.error_message = fmt::format("Failed to parse file list response: {}", e.what());
    }
    
    return response;
}

FineTuningJob Client::parse_fine_tuning_job(const std::string& json_str) {
    FineTuningJob job;
    
    try {
        auto id_pos = json_str.find("\"id\":\"");
        if (id_pos != std::string::npos) {
            id_pos += 6;
            auto end_pos = json_str.find("\"", id_pos);
            job.id = json_str.substr(id_pos, end_pos - id_pos);
        }
        
        auto model_pos = json_str.find("\"model\":\"");
        if (model_pos != std::string::npos) {
            model_pos += 9;
            auto end_pos = json_str.find("\"", model_pos);
            job.model = json_str.substr(model_pos, end_pos - model_pos);
        }
        
        auto status_pos = json_str.find("\"status\":\"");
        if (status_pos != std::string::npos) {
            status_pos += 10;
            auto end_pos = json_str.find("\"", status_pos);
            job.status = json_str.substr(status_pos, end_pos - status_pos);
        }
        
        auto training_file_pos = json_str.find("\"training_file\":\"");
        if (training_file_pos != std::string::npos) {
            training_file_pos += 17;
            auto end_pos = json_str.find("\"", training_file_pos);
            job.training_file = json_str.substr(training_file_pos, end_pos - training_file_pos);
        }
        
        auto org_id_pos = json_str.find("\"organization_id\":\"");
        if (org_id_pos != std::string::npos) {
            org_id_pos += 19;
            auto end_pos = json_str.find("\"", org_id_pos);
            job.organization_id = json_str.substr(org_id_pos, end_pos - org_id_pos);
        }
        
        auto fine_tuned_model_pos = json_str.find("\"fine_tuned_model\":\"");
        if (fine_tuned_model_pos != std::string::npos) {
            fine_tuned_model_pos += 20;
            auto end_pos = json_str.find("\"", fine_tuned_model_pos);
            if (end_pos > fine_tuned_model_pos) {
                job.fine_tuned_model = json_str.substr(fine_tuned_model_pos, end_pos - fine_tuned_model_pos);
            }
        }
        
        auto created_pos = json_str.find("\"created_at\":");
        if (created_pos != std::string::npos) {
            created_pos += 13;
            auto end_pos = json_str.find_first_of(",}", created_pos);
            job.created_at = std::stoll(json_str.substr(created_pos, end_pos - created_pos));
        }
        
        auto finished_pos = json_str.find("\"finished_at\":");
        if (finished_pos != std::string::npos) {
            finished_pos += 14;
            auto end_pos = json_str.find_first_of(",}", finished_pos);
            auto value_str = json_str.substr(finished_pos, end_pos - finished_pos);
            if (value_str != "null") {
                job.finished_at = std::stoll(value_str);
            }
        }
        
        auto trained_tokens_pos = json_str.find("\"trained_tokens\":");
        if (trained_tokens_pos != std::string::npos) {
            trained_tokens_pos += 17;
            auto end_pos = json_str.find_first_of(",}", trained_tokens_pos);
            auto value_str = json_str.substr(trained_tokens_pos, end_pos - trained_tokens_pos);
            if (value_str != "null") {
                job.trained_tokens = std::stoi(value_str);
            }
        }
        
    } catch (const std::exception&) {
        // Return partially populated object
    }
    
    return job;
}

FineTuningJobListResponse Client::parse_fine_tuning_job_list_response(const std::string& json_str) {
    FineTuningJobListResponse response;
    
    try {
        auto object_pos = json_str.find("\"object\":\"");
        if (object_pos != std::string::npos) {
            object_pos += 10;
            auto end_pos = json_str.find("\"", object_pos);
            response.object = json_str.substr(object_pos, end_pos - object_pos);
        }
        
        auto has_more_pos = json_str.find("\"has_more\":");
        if (has_more_pos != std::string::npos) {
            has_more_pos += 11;
            auto end_pos = json_str.find_first_of(",}", has_more_pos);
            auto value = json_str.substr(has_more_pos, end_pos - has_more_pos);
            response.has_more = (value == "true");
        }
        
        auto data_pos = json_str.find("\"data\":[");
        if (data_pos != std::string::npos) {
            auto current_pos = data_pos + 8;
            
            while (current_pos < json_str.size()) {
                auto obj_start = json_str.find("{", current_pos);
                if (obj_start == std::string::npos || obj_start > json_str.find("]", current_pos)) break;
                
                int depth = 0;
                auto obj_end = obj_start;
                for (; obj_end < json_str.size(); ++obj_end) {
                    if (json_str[obj_end] == '{') depth++;
                    else if (json_str[obj_end] == '}') {
                        depth--;
                        if (depth == 0) break;
                    }
                }
                
                auto job_json = json_str.substr(obj_start, obj_end - obj_start + 1);
                response.data.push_back(parse_fine_tuning_job(job_json));
                current_pos = obj_end + 1;
            }
        }
    } catch (const std::exception& e) {
        response.is_error = true;
        response.error_message = fmt::format("Failed to parse fine-tuning job list response: {}", e.what());
    }
    
    return response;
}

ModerationResponse Client::parse_moderation_response(const std::string& json_str) {
    ModerationResponse response;
    
    try {
        // Parse id
        auto id_pos = json_str.find("\"id\":\"");
        if (id_pos != std::string::npos) {
            id_pos += 6;
            auto end_pos = json_str.find("\"", id_pos);
            response.id = json_str.substr(id_pos, end_pos - id_pos);
        }
        
        // Parse model
        auto model_pos = json_str.find("\"model\":\"");
        if (model_pos != std::string::npos) {
            model_pos += 9;
            auto end_pos = json_str.find("\"", model_pos);
            response.model = json_str.substr(model_pos, end_pos - model_pos);
        }
        
        // Parse results array
        auto results_pos = json_str.find("\"results\":[");
        if (results_pos != std::string::npos) {
            auto current_pos = results_pos + 11;
            
            while (current_pos < json_str.size()) {
                auto obj_start = json_str.find("{", current_pos);
                if (obj_start == std::string::npos || obj_start > json_str.find("]", current_pos)) break;
                
                int depth = 0;
                auto obj_end = obj_start;
                for (; obj_end < json_str.size(); ++obj_end) {
                    if (json_str[obj_end] == '{') depth++;
                    else if (json_str[obj_end] == '}') {
                        depth--;
                        if (depth == 0) break;
                    }
                }
                
                auto result_json = json_str.substr(obj_start, obj_end - obj_start + 1);
                ModerationResult result = parse_moderation_result(result_json);
                response.results.push_back(result);
                current_pos = obj_end + 1;
            }
        }
    } catch (const std::exception& e) {
        response.is_error = true;
        response.error_message = fmt::format("Failed to parse moderation response: {}", e.what());
    }
    
    return response;
}

ModerationResult Client::parse_moderation_result(const std::string& json_str) {
    ModerationResult result;
    
    // Parse flagged
    auto flagged_pos = json_str.find("\"flagged\":");
    if (flagged_pos != std::string::npos) {
        flagged_pos += 10;
        auto end_pos = json_str.find_first_of(",}", flagged_pos);
        auto value = json_str.substr(flagged_pos, end_pos - flagged_pos);
        result.flagged = (value == "true");
    }
    
    // Parse categories
    auto categories_pos = json_str.find("\"categories\":{");
    if (categories_pos != std::string::npos) {
        auto cats_section = json_str.substr(categories_pos);
        auto cats_end = cats_section.find("}");
        cats_section = cats_section.substr(0, cats_end + 1);
        
        result.categories.hate = parse_bool_field(cats_section, "hate");
        result.categories.hate_threatening = parse_bool_field(cats_section, "hate/threatening");
        result.categories.harassment = parse_bool_field(cats_section, "harassment");
        result.categories.harassment_threatening = parse_bool_field(cats_section, "harassment/threatening");
        result.categories.self_harm = parse_bool_field(cats_section, "self-harm");
        result.categories.self_harm_intent = parse_bool_field(cats_section, "self-harm/intent");
        result.categories.self_harm_instructions = parse_bool_field(cats_section, "self-harm/instructions");
        result.categories.sexual = parse_bool_field(cats_section, "sexual");
        result.categories.sexual_minors = parse_bool_field(cats_section, "sexual/minors");
        result.categories.violence = parse_bool_field(cats_section, "violence");
        result.categories.violence_graphic = parse_bool_field(cats_section, "violence/graphic");
    }
    
    // Parse category_scores
    auto scores_pos = json_str.find("\"category_scores\":{");
    if (scores_pos != std::string::npos) {
        auto scores_section = json_str.substr(scores_pos);
        auto scores_end = scores_section.find("}");
        scores_section = scores_section.substr(0, scores_end + 1);
        
        result.category_scores.hate = parse_double_field(scores_section, "hate");
        result.category_scores.hate_threatening = parse_double_field(scores_section, "hate/threatening");
        result.category_scores.harassment = parse_double_field(scores_section, "harassment");
        result.category_scores.harassment_threatening = parse_double_field(scores_section, "harassment/threatening");
        result.category_scores.self_harm = parse_double_field(scores_section, "self-harm");
        result.category_scores.self_harm_intent = parse_double_field(scores_section, "self-harm/intent");
        result.category_scores.self_harm_instructions = parse_double_field(scores_section, "self-harm/instructions");
        result.category_scores.sexual = parse_double_field(scores_section, "sexual");
        result.category_scores.sexual_minors = parse_double_field(scores_section, "sexual/minors");
        result.category_scores.violence = parse_double_field(scores_section, "violence");
        result.category_scores.violence_graphic = parse_double_field(scores_section, "violence/graphic");
    }
    
    return result;
}

bool Client::parse_bool_field(const std::string& json_str, const std::string& field_name) {
    auto field_key = fmt::format("\"{}\":", field_name);
    auto pos = json_str.find(field_key);
    if (pos != std::string::npos) {
        pos += field_key.length();
        auto end_pos = json_str.find_first_of(",}", pos);
        auto value = json_str.substr(pos, end_pos - pos);
        return (value == "true");
    }
    return false;
}

double Client::parse_double_field(const std::string& json_str, const std::string& field_name) {
    auto field_key = fmt::format("\"{}\":", field_name);
    auto pos = json_str.find(field_key);
    if (pos != std::string::npos) {
        pos += field_key.length();
        auto end_pos = json_str.find_first_of(",}", pos);
        auto value_str = json_str.substr(pos, end_pos - pos);
        try {
            return std::stod(value_str);
        } catch (...) {
            return 0.0;
        }
    }
    return 0.0;
}

} // namespace openai