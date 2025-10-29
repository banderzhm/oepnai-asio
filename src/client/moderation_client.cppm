// Moderation Client Module
// Handles OpenAI Moderation API operations

export module openai.client.moderation;

import asio;
import fmt;
import openai.client.base;
import openai.http_client;
import openai.types.moderation;
import std;

export namespace openai::client {

// Moderation API client
class ModerationClient : public BaseClient {
public:
    using BaseClient::BaseClient;

    // Create moderation check
    asio::awaitable<ModerationResponse> create_moderation(const ModerationRequest& request) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = "/v1/moderations";
        req.use_ssl = true;
        req.body = request.to_json();
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        ModerationResponse mod_response;
        
        if (response.is_error) {
            mod_response.is_error = true;
            mod_response.error_message = response.error_message;
            co_return mod_response;
        }
        
        mod_response = parse_moderation_response(response.body);
        co_return mod_response;
    }

private:
    ModerationResponse parse_moderation_response(const std::string& json_str) {
        ModerationResponse response;
        
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
            
            // Parse results array
            auto results_pos = json_str.find("\"results\":[");
            if (results_pos != std::string::npos) {
                auto results_start = results_pos + 11;
                auto current_pos = results_start;
                
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
                    
                    auto result_json = json_str.substr(obj_start, obj_end - obj_start);
                    response.results.push_back(parse_moderation_result(result_json));
                    
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
            
        } catch (const std::exception& e) {
            response.is_error = true;
            response.error_message = fmt::format("Failed to parse moderation response: {}", e.what());
        }
        
        return response;
    }

    ModerationResult parse_moderation_result(const std::string& json_str) {
        ModerationResult result;
        
        // Parse flagged
        result.flagged = parse_bool_field(json_str, "flagged");
        
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
};

} // namespace openai::client

