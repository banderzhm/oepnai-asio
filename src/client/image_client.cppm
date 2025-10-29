// Image Client Module
// Handles OpenAI Images API operations

export module openai.client.image;

import asio;
import fmt;
import openai.client.base;
import openai.http_client;
import openai.types.image;
import std;

export namespace openai::client {

// Images API client
class ImageClient : public BaseClient {
public:
    using BaseClient::BaseClient;

    // Generate image from prompt
    asio::awaitable<ImageResponse> generate_image(const ImageGenerationRequest& request) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = "/v1/images/generations";
        req.use_ssl = true;
        req.body = request.to_json();
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        ImageResponse img_response;
        
        if (response.is_error) {
            img_response.is_error = true;
            img_response.error_message = response.error_message;
            co_return img_response;
        }
        
        img_response = parse_image_response(response.body);
        co_return img_response;
    }

    // Edit image (to be implemented with multipart/form-data)
    asio::awaitable<ImageResponse> edit_image(const ImageEditRequest& request) {
        // TODO: Implement multipart/form-data upload
        ImageResponse img_response;
        img_response.is_error = true;
        img_response.error_message = "Image edit not yet implemented";
        co_return img_response;
    }

    // Create image variation (to be implemented with multipart/form-data)
    asio::awaitable<ImageResponse> create_image_variation(const ImageVariationRequest& request) {
        // TODO: Implement multipart/form-data upload
        ImageResponse img_response;
        img_response.is_error = true;
        img_response.error_message = "Image variation not yet implemented";
        co_return img_response;
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

