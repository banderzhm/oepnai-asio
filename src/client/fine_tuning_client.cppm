// Fine-tuning Client Module
// Handles OpenAI Fine-tuning API operations

export module openai.client.fine_tuning;

import asio;
import fmt;
import openai.client.base;
import openai.http_client;
import openai.types.fine_tuning;
import openai.types.common;
import std;

export namespace openai::client {

// Fine-tuning API client
class FineTuningClient : public BaseClient {
public:
    using BaseClient::BaseClient;

    // Create fine-tuning job
    asio::awaitable<std::expected<FineTuningJob, ApiError>> create_fine_tuning_job(const FineTuningRequest& request) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = "/v1/fine_tuning/jobs";
        req.use_ssl = true;
        req.body = request.to_json();
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        FineTuningJobResponse job_response;
        
        if (response.is_error) {
            job_response.is_error = true;
            job_response.error_message = response.error_message;
            co_return job_response;
        }
        
        job_response.job = parse_fine_tuning_job(response.body);
        co_return job_response;
    }

    // List fine-tuning jobs
    asio::awaitable<std::expected<FineTuningJobListResponse, ApiError>> list_fine_tuning_jobs(int limit = 20) {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/fine_tuning/jobs?limit={}", limit);
        req.use_ssl = true;
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        FineTuningJobListResponse list_response;
        
        if (response.is_error) {
            list_response.is_error = true;
            list_response.error_message = response.error_message;
            co_return list_response;
        }
        
        list_response = parse_fine_tuning_job_list_response(response.body);
        co_return list_response;
    }

    // Retrieve fine-tuning job
    asio::awaitable<FineTuningJobResponse> retrieve_fine_tuning_job(const std::string& job_id) {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/fine_tuning/jobs/{}", job_id);
        req.use_ssl = true;
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        FineTuningJobResponse job_response;
        
        if (response.is_error) {
            job_response.is_error = true;
            job_response.error_message = response.error_message;
            co_return job_response;
        }
        
        job_response.job = parse_fine_tuning_job(response.body);
        co_return job_response;
    }

    // Cancel fine-tuning job
    asio::awaitable<FineTuningJobResponse> cancel_fine_tuning_job(const std::string& job_id) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/fine_tuning/jobs/{}/cancel", job_id);
        req.use_ssl = true;
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        FineTuningJobResponse job_response;
        
        if (response.is_error) {
            job_response.is_error = true;
            job_response.error_message = response.error_message;
            co_return job_response;
        }
        
        job_response.job = parse_fine_tuning_job(response.body);
        co_return job_response;
    }

private:
    FineTuningJob parse_fine_tuning_job(const std::string& json_str) {
        FineTuningJob job;
        
        // Parse id
        auto id_pos = json_str.find("\"id\":");
        if (id_pos != std::string::npos) {
            id_pos += 5;
            auto id_start = json_str.find("\"", id_pos) + 1;
            auto id_end = json_str.find("\"", id_start);
            job.id = json_str.substr(id_start, id_end - id_start);
        }
        
        // Parse model
        auto model_pos = json_str.find("\"model\":");
        if (model_pos != std::string::npos) {
            model_pos += 8;
            auto model_start = json_str.find("\"", model_pos) + 1;
            auto model_end = json_str.find("\"", model_start);
            job.model = json_str.substr(model_start, model_end - model_start);
        }
        
        // Parse created_at
        auto created_pos = json_str.find("\"created_at\":");
        if (created_pos != std::string::npos) {
            created_pos += 13;
            auto end_pos = json_str.find_first_of(",}", created_pos);
            job.created_at = std::stoll(json_str.substr(created_pos, end_pos - created_pos));
        }
        
        // Parse status
        auto status_pos = json_str.find("\"status\":");
        if (status_pos != std::string::npos) {
            status_pos += 9;
            auto status_start = json_str.find("\"", status_pos) + 1;
            auto status_end = json_str.find("\"", status_start);
            job.status = json_str.substr(status_start, status_end - status_start);
        }
        
        // Parse fine_tuned_model
        auto ft_model_pos = json_str.find("\"fine_tuned_model\":");
        if (ft_model_pos != std::string::npos) {
            ft_model_pos += 19;
            auto next_quote = json_str.find("\"", ft_model_pos);
            if (next_quote != std::string::npos) {
                auto ft_model_start = next_quote + 1;
                auto ft_model_end = json_str.find("\"", ft_model_start);
                if (ft_model_end != std::string::npos) {
                    job.fine_tuned_model = json_str.substr(ft_model_start, ft_model_end - ft_model_start);
                }
            }
        }
        
        // Parse training_file
        auto train_file_pos = json_str.find("\"training_file\":");
        if (train_file_pos != std::string::npos) {
            train_file_pos += 16;
            auto train_start = json_str.find("\"", train_file_pos) + 1;
            auto train_end = json_str.find("\"", train_start);
            job.training_file = json_str.substr(train_start, train_end - train_start);
        }
        
        return job;
    }

    FineTuningJobListResponse parse_fine_tuning_job_list_response(const std::string& json_str) {
        FineTuningJobListResponse response;
        
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
                
                auto job_json = json_str.substr(obj_start, obj_end - obj_start);
                response.jobs.push_back(parse_fine_tuning_job(job_json));
                
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

