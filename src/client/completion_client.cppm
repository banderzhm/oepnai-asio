// Completion Client Module
// Handles OpenAI Completions API operations (Legacy)

export module openai.client.completion;

import asio;
import fmt;
import openai.client.base;
import openai.http_client;
import openai.types.completion;
import std;

export namespace openai::client {

// Completions API client (Legacy)
class CompletionClient : public BaseClient {
public:
    using BaseClient::BaseClient;

    // Create text completion
    asio::awaitable<std::string> create_completion(const CompletionRequest& request) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = "/v1/completions";
        req.use_ssl = true;
        req.body = request.to_json();
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return fmt::format("Error: {}", response.error_message);
        }
        
        // Parse the first choice text
        auto choices_pos = response.body.find("\"choices\":[");
        if (choices_pos != std::string::npos) {
            auto text_pos = response.body.find("\"text\":", choices_pos);
            if (text_pos != std::string::npos) {
                text_pos += 7;
                auto text_start = response.body.find("\"", text_pos) + 1;
                auto text_end = text_start;
                
                while (text_end < response.body.length()) {
                    if (response.body[text_end] == '"' && 
                        (text_end == 0 || response.body[text_end - 1] != '\\')) {
                        break;
                    }
                    text_end++;
                }
                
                co_return unescape_json(response.body.substr(text_start, text_end - text_start));
            }
        }
        
        co_return "";
    }
};

} // namespace openai::client

