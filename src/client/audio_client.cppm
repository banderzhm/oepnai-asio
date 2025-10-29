// Audio Client Module
// Handles OpenAI Audio API operations

export module openai.client.audio;

import asio;
import fmt;
import openai.client.base;
import openai.http_client;
import openai.types.audio;
import std;

export namespace openai::client {

// Audio API client
class AudioClient : public BaseClient {
public:
    using BaseClient::BaseClient;

    // Create transcription
    asio::awaitable<AudioResponse> create_transcription(const AudioTranscriptionRequest& request) {
        std::string boundary = generate_boundary();
        
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = "/v1/audio/transcriptions";
        req.use_ssl = true;
        
        // Build multipart/form-data
        std::map<std::string, std::string> fields;
        fields["model"] = request.model;
        if (request.language && !request.language->empty()) {
            fields["language"] = *request.language;
        }
        if (request.prompt && !request.prompt->empty()) {
            fields["prompt"] = *request.prompt;
        }
        if (request.response_format && !request.response_format->empty()) {
            fields["response_format"] = *request.response_format;
        }
        if (request.temperature && *request.temperature > 0.0) {
            fields["temperature"] = std::to_string(*request.temperature);
        }
        
        std::map<std::string, std::pair<std::string, std::string>> files;
        try {
            std::string audio_content = read_file_content(request.file_path);
            files["file"] = {request.file_path, audio_content};
        } catch (const std::exception& e) {
            AudioResponse response;
            response.is_error = true;
            response.error_message = std::string("Failed to read audio file: ") + e.what();
            co_return response;
        }
        
        req.body = build_multipart_formdata(fields, files, boundary);
        req.headers["Content-Type"] = "multipart/form-data; boundary=" + boundary;
        
        add_auth_headers(req, false);
        
        auto response = co_await http_client_.async_request(req);
        
        AudioResponse audio_response;
        
        if (response.is_error) {
            audio_response.is_error = true;
            audio_response.error_message = response.error_message;
            co_return audio_response;
        }
        
        // Parse response (usually just text)
        audio_response.text = response.body;
        co_return audio_response;
    }

    // Create translation
    asio::awaitable<AudioResponse> create_translation(const AudioTranslationRequest& request) {
        std::string boundary = generate_boundary();
        
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = "/v1/audio/translations";
        req.use_ssl = true;
        
        // Build multipart/form-data
        std::map<std::string, std::string> fields;
        fields["model"] = request.model;
        if (request.prompt && !request.prompt->empty()) {
            fields["prompt"] = *request.prompt;
        }
        if (request.response_format && !request.response_format->empty()) {
            fields["response_format"] = *request.response_format;
        }
        if (request.temperature && *request.temperature > 0.0) {
            fields["temperature"] = std::to_string(*request.temperature);
        }
        
        std::map<std::string, std::pair<std::string, std::string>> files;
        try {
            std::string audio_content = read_file_content(request.file_path);
            files["file"] = {request.file_path, audio_content};
        } catch (const std::exception& e) {
            AudioResponse response;
            response.is_error = true;
            response.error_message = std::string("Failed to read audio file: ") + e.what();
            co_return response;
        }
        
        req.body = build_multipart_formdata(fields, files, boundary);
        req.headers["Content-Type"] = "multipart/form-data; boundary=" + boundary;
        
        add_auth_headers(req, false);
        
        auto response = co_await http_client_.async_request(req);
        
        AudioResponse audio_response;
        
        if (response.is_error) {
            audio_response.is_error = true;
            audio_response.error_message = response.error_message;
            co_return audio_response;
        }
        
        // Parse response (usually just text)
        audio_response.text = response.body;
        co_return audio_response;
    }
};

} // namespace openai::client

