// Image Client Module
// Handles OpenAI Images API operations

export module openai.client.image;

import asio;
import fmt;
import openai.client.base;
import openai.http_client;
import openai.types.image;
import openai.types.common;
import std;

export namespace openai::client {

// Images API client
class ImageClient : public BaseClient {
public:
    using BaseClient::BaseClient;

    // Generate image from prompt
    asio::awaitable<std::expected<ImageResponse, ApiError>> generate_image(const ImageGenerationRequest& request) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = "/v1/images/generations";
        req.use_ssl = true;
        req.body = request.to_json();
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        co_return parse_image_response(response.body);
    }

    // Edit image
    asio::awaitable<std::expected<ImageResponse, ApiError>> edit_image(const ImageEditRequest& request) {
        std::string boundary = generate_boundary();
        
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = "/v1/images/edits";
        req.use_ssl = true;
        
        // Build multipart/form-data
        std::map<std::string, std::string> fields;
        fields["prompt"] = request.prompt;
        if (request.n && *request.n > 0) {
            fields["n"] = std::to_string(*request.n);
        }
        if (request.size && !request.size->empty()) {
            fields["size"] = *request.size;
        }
        if (request.response_format && !request.response_format->empty()) {
            fields["response_format"] = *request.response_format;
        }
        
        std::map<std::string, std::pair<std::string, std::string>> files;
        try {
            std::string image_content = read_file_content(request.image_path);
            files["image"] = {request.image_path, image_content};
            
            if (request.mask && !request.mask->empty()) {
                std::string mask_content = read_file_content(*request.mask);
                files["mask"] = {*request.mask, mask_content};
            }
        } catch (const std::exception& e) {
            co_return std::unexpected(ApiError(std::string("Failed to read image file: ") + e.what()));
        }
        
        req.body = build_multipart_formdata(fields, files, boundary);
        req.headers["Content-Type"] = "multipart/form-data; boundary=" + boundary;
        
        add_auth_headers(req, false);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        co_return parse_image_response(response.body);
    }

    // Create image variation
    asio::awaitable<std::expected<ImageResponse, ApiError>> create_image_variation(const ImageVariationRequest& request) {
        std::string boundary = generate_boundary();
        
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = "/v1/images/variations";
        req.use_ssl = true;
        
        // Build multipart/form-data
        std::map<std::string, std::string> fields;
        if (request.n && *request.n > 0) {
            fields["n"] = std::to_string(*request.n);
        }
        if (request.size && !request.size->empty()) {
            fields["size"] = *request.size;
        }
        if (request.response_format && !request.response_format->empty()) {
            fields["response_format"] = *request.response_format;
        }
        
        std::map<std::string, std::pair<std::string, std::string>> files;
        try {
            std::string image_content = read_file_content(request.image_path);
            files["image"] = {request.image_path, image_content};
        } catch (const std::exception& e) {
            co_return std::unexpected(ApiError(std::string("Failed to read image file: ") + e.what()));
        }
        
        req.body = build_multipart_formdata(fields, files, boundary);
        req.headers["Content-Type"] = "multipart/form-data; boundary=" + boundary;
        
        add_auth_headers(req, false);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        co_return parse_image_response(response.body);
    }

private:
    ImageResponse parse_image_response(const std::string& json_str) {
        ImageResponse response;
        
        // Parse created timestamp
        auto created_pos = json_str.find("\"created\":");
        if (created_pos != std::string::npos) {
            created_pos += 10;
            auto end_pos = json_str.find_first_of(",}", created_pos);
            response.created = std::stoll(json_str.substr(created_pos, end_pos - created_pos));
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
                
                ImageData data;
                
                // Parse URL
                auto url_pos = obj_json.find("\"url\":");
                if (url_pos != std::string::npos) {
                    url_pos += 6;
                    auto url_start = obj_json.find("\"", url_pos) + 1;
                    auto url_end = obj_json.find("\"", url_start);
                    data.url = obj_json.substr(url_start, url_end - url_start);
                }
                
                // Parse revised_prompt (DALL-E 3)
                auto revised_pos = obj_json.find("\"revised_prompt\":");
                if (revised_pos != std::string::npos) {
                    revised_pos += 17;
                    auto revised_start = obj_json.find("\"", revised_pos) + 1;
                    auto revised_end = obj_json.find("\"", revised_start);
                    data.revised_prompt = obj_json.substr(revised_start, revised_end - revised_start);
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
        
        return response;
    }
};

} // namespace openai::client

