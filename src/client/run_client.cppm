// Run Client Module
// Handles OpenAI Runs API operations (Beta)

export module openai.client.run;

import asio;
import fmt;
import openai.client.base;
import openai.http_client;
import openai.types.run;
import std;

export namespace openai::client {

// Runs API client (Beta)
class RunClient : public BaseClient {
public:
    using BaseClient::BaseClient;

    // Create run
    asio::awaitable<Run> create_run(
        const std::string& thread_id,
        const CreateRunRequest& request
    ) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/threads/{}/runs", thread_id);
        req.use_ssl = true;
        req.body = request.to_json();
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        Run run;
        
        if (response.is_error) {
            run.is_error = true;
            run.error_message = response.error_message;
            co_return run;
        }
        
        run = parse_run(response.body);
        co_return run;
    }

    // List runs
    asio::awaitable<RunListResponse> list_runs(
        const std::string& thread_id,
        int limit = 20
    ) {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/threads/{}/runs?limit={}", thread_id, limit);
        req.use_ssl = true;
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        RunListResponse list_response;
        
        if (response.is_error) {
            list_response.is_error = true;
            list_response.error_message = response.error_message;
            co_return list_response;
        }
        
        list_response = parse_run_list_response(response.body);
        co_return list_response;
    }

    // Retrieve run
    asio::awaitable<Run> retrieve_run(
        const std::string& thread_id,
        const std::string& run_id
    ) {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/threads/{}/runs/{}", thread_id, run_id);
        req.use_ssl = true;
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        Run run;
        
        if (response.is_error) {
            run.is_error = true;
            run.error_message = response.error_message;
            co_return run;
        }
        
        run = parse_run(response.body);
        co_return run;
    }

    // Modify run
    asio::awaitable<Run> modify_run(
        const std::string& thread_id,
        const std::string& run_id,
        const ModifyRunRequest& request
    ) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/threads/{}/runs/{}", thread_id, run_id);
        req.use_ssl = true;
        req.body = request.to_json();
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        Run run;
        
        if (response.is_error) {
            run.is_error = true;
            run.error_message = response.error_message;
            co_return run;
        }
        
        run = parse_run(response.body);
        co_return run;
    }

    // Cancel run
    asio::awaitable<Run> cancel_run(
        const std::string& thread_id,
        const std::string& run_id
    ) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/threads/{}/runs/{}/cancel", thread_id, run_id);
        req.use_ssl = true;
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        Run run;
        
        if (response.is_error) {
            run.is_error = true;
            run.error_message = response.error_message;
            co_return run;
        }
        
        run = parse_run(response.body);
        co_return run;
    }

    // Submit tool outputs to run
    asio::awaitable<Run> submit_tool_outputs(
        const std::string& thread_id,
        const std::string& run_id,
        const SubmitToolOutputsRequest& request
    ) {
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/threads/{}/runs/{}/submit_tool_outputs", thread_id, run_id);
        req.use_ssl = true;
        req.body = request.to_json();
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        Run run;
        
        if (response.is_error) {
            run.is_error = true;
            run.error_message = response.error_message;
            co_return run;
        }
        
        run = parse_run(response.body);
        co_return run;
    }

    // List run steps
    asio::awaitable<RunStepListResponse> list_run_steps(
        const std::string& thread_id,
        const std::string& run_id,
        int limit = 20
    ) {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/threads/{}/runs/{}/steps?limit={}", thread_id, run_id, limit);
        req.use_ssl = true;
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        RunStepListResponse list_response;
        
        if (response.is_error) {
            list_response.is_error = true;
            list_response.error_message = response.error_message;
            co_return list_response;
        }
        
        list_response = parse_run_step_list_response(response.body);
        co_return list_response;
    }

    // Retrieve run step
    asio::awaitable<RunStep> retrieve_run_step(
        const std::string& thread_id,
        const std::string& run_id,
        const std::string& step_id
    ) {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/threads/{}/runs/{}/steps/{}", thread_id, run_id, step_id);
        req.use_ssl = true;
        req.headers["OpenAI-Beta"] = "assistants=v2";
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        RunStep step;
        
        if (response.is_error) {
            step.is_error = true;
            step.error_message = response.error_message;
            co_return step;
        }
        
        step = parse_run_step(response.body);
        co_return step;
    }

private:
    Run parse_run(const std::string& json_str) {
        Run run;
        
        // Parse id
        auto id_pos = json_str.find("\"id\":");
        if (id_pos != std::string::npos) {
            id_pos += 5;
            auto id_start = json_str.find("\"", id_pos) + 1;
            auto id_end = json_str.find("\"", id_start);
            run.id = json_str.substr(id_start, id_end - id_start);
        }
        
        // Parse thread_id
        auto thread_pos = json_str.find("\"thread_id\":");
        if (thread_pos != std::string::npos) {
            thread_pos += 12;
            auto thread_start = json_str.find("\"", thread_pos) + 1;
            auto thread_end = json_str.find("\"", thread_start);
            run.thread_id = json_str.substr(thread_start, thread_end - thread_start);
        }
        
        // Parse assistant_id
        auto assistant_pos = json_str.find("\"assistant_id\":");
        if (assistant_pos != std::string::npos) {
            assistant_pos += 15;
            auto assistant_start = json_str.find("\"", assistant_pos) + 1;
            auto assistant_end = json_str.find("\"", assistant_start);
            run.assistant_id = json_str.substr(assistant_start, assistant_end - assistant_start);
        }
        
        // Parse status
        auto status_pos = json_str.find("\"status\":");
        if (status_pos != std::string::npos) {
            status_pos += 9;
            auto status_start = json_str.find("\"", status_pos) + 1;
            auto status_end = json_str.find("\"", status_start);
            run.status = json_str.substr(status_start, status_end - status_start);
        }
        
        // Parse model
        auto model_pos = json_str.find("\"model\":");
        if (model_pos != std::string::npos) {
            model_pos += 8;
            auto model_start = json_str.find("\"", model_pos) + 1;
            auto model_end = json_str.find("\"", model_start);
            run.model = json_str.substr(model_start, model_end - model_start);
        }
        
        // Parse created_at
        auto created_pos = json_str.find("\"created_at\":");
        if (created_pos != std::string::npos) {
            created_pos += 13;
            auto end_pos = json_str.find_first_of(",}", created_pos);
            run.created_at = std::stoll(json_str.substr(created_pos, end_pos - created_pos));
        }
        
        return run;
    }

    RunStep parse_run_step(const std::string& json_str) {
        RunStep step;
        
        // Parse id
        auto id_pos = json_str.find("\"id\":");
        if (id_pos != std::string::npos) {
            id_pos += 5;
            auto id_start = json_str.find("\"", id_pos) + 1;
            auto id_end = json_str.find("\"", id_start);
            step.id = json_str.substr(id_start, id_end - id_start);
        }
        
        // Parse run_id
        auto run_pos = json_str.find("\"run_id\":");
        if (run_pos != std::string::npos) {
            run_pos += 9;
            auto run_start = json_str.find("\"", run_pos) + 1;
            auto run_end = json_str.find("\"", run_start);
            step.run_id = json_str.substr(run_start, run_end - run_start);
        }
        
        // Parse thread_id
        auto thread_pos = json_str.find("\"thread_id\":");
        if (thread_pos != std::string::npos) {
            thread_pos += 12;
            auto thread_start = json_str.find("\"", thread_pos) + 1;
            auto thread_end = json_str.find("\"", thread_start);
            step.thread_id = json_str.substr(thread_start, thread_end - thread_start);
        }
        
        // Parse assistant_id
        auto assistant_pos = json_str.find("\"assistant_id\":");
        if (assistant_pos != std::string::npos) {
            assistant_pos += 15;
            auto assistant_start = json_str.find("\"", assistant_pos) + 1;
            auto assistant_end = json_str.find("\"", assistant_start);
            step.assistant_id = json_str.substr(assistant_start, assistant_end - assistant_start);
        }
        
        // Parse type
        auto type_pos = json_str.find("\"type\":");
        if (type_pos != std::string::npos) {
            type_pos += 7;
            auto type_start = json_str.find("\"", type_pos) + 1;
            auto type_end = json_str.find("\"", type_start);
            step.type = json_str.substr(type_start, type_end - type_start);
        }
        
        // Parse status
        auto status_pos = json_str.find("\"status\":");
        if (status_pos != std::string::npos) {
            status_pos += 9;
            auto status_start = json_str.find("\"", status_pos) + 1;
            auto status_end = json_str.find("\"", status_start);
            step.status = json_str.substr(status_start, status_end - status_start);
        }
        
        // Parse created_at
        auto created_pos = json_str.find("\"created_at\":");
        if (created_pos != std::string::npos) {
            created_pos += 13;
            auto end_pos = json_str.find_first_of(",}", created_pos);
            step.created_at = std::stoll(json_str.substr(created_pos, end_pos - created_pos));
        }
        
        return step;
    }

    RunListResponse parse_run_list_response(const std::string& json_str) {
        RunListResponse response;
        
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
                
                auto run_json = json_str.substr(obj_start, obj_end - obj_start);
                response.data.push_back(parse_run(run_json));
                
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

    RunStepListResponse parse_run_step_list_response(const std::string& json_str) {
        RunStepListResponse response;
        
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
                
                auto step_json = json_str.substr(obj_start, obj_end - obj_start);
                response.data.push_back(parse_run_step(step_json));
                
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

