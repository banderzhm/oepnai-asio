// File Client Module
// Handles OpenAI Files API operations

export module openai.client.file;

import asio;
import fmt;
import openai.client.base;
import openai.http_client;
import openai.types.file;
import openai.types.common;
import std;

export namespace openai::client {

// Files API client
class FileClient : public BaseClient {
public:
    using BaseClient::BaseClient;

    // Upload file
    asio::awaitable<std::expected<FileUploadResponse, ApiError>> upload_file(const FileUploadRequest& request) {
        std::string boundary = generate_boundary();
        
        http::Request req;
        req.method = "POST";
        req.host = "api.openai.com";
        req.path = "/v1/files";
        req.use_ssl = true;
        
        // Build multipart/form-data
        std::map<std::string, std::string> fields;
        fields["purpose"] = request.purpose;
        
        std::map<std::string, std::pair<std::string, std::string>> files;
        try {
            std::string file_content = read_file_content(request.file_path);
            files["file"] = {request.file_path, file_content};
        } catch (const std::exception& e) {
            co_return std::unexpected(ApiError(std::string("Failed to read file: ") + e.what()));
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
        
        co_return parse_file_upload_response(response.body);
    }

    // List files
    asio::awaitable<std::expected<FileListResponse, ApiError>> list_files() {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = "/v1/files";
        req.use_ssl = true;
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        co_return parse_file_list_response(response.body);
    }

    // Retrieve file metadata
    asio::awaitable<std::expected<FileObject, ApiError>> retrieve_file(const std::string& file_id) {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/files/{}", file_id);
        req.use_ssl = true;
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        co_return parse_file_object(response.body);
    }

    // Retrieve file content
    asio::awaitable<std::expected<FileContentResponse, ApiError>> retrieve_file_content(const std::string& file_id) {
        http::Request req;
        req.method = "GET";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/files/{}/content", file_id);
        req.use_ssl = true;
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        FileContentResponse content_response;
        content_response.content = response.body;
        co_return content_response;
    }

    // Delete file
    asio::awaitable<std::expected<bool, ApiError>> delete_file(const std::string& file_id) {
        http::Request req;
        req.method = "DELETE";
        req.host = "api.openai.com";
        req.path = fmt::format("/v1/files/{}", file_id);
        req.use_ssl = true;
        
        add_auth_headers(req);
        
        auto response = co_await http_client_.async_request(req);
        
        if (response.is_error) {
            co_return std::unexpected(ApiError(response.error_message));
        }
        
        if (response.status_code != 200) {
            co_return std::unexpected(ApiError(response.status_code,
                fmt::format("HTTP {}: {}", response.status_code, response.body)));
        }
        
        // Check for "deleted": true
        auto deleted_pos = response.body.find("\"deleted\":");
        if (deleted_pos != std::string::npos) {
            deleted_pos += 10;
            auto end_pos = response.body.find_first_of(",}", deleted_pos);
            auto value = response.body.substr(deleted_pos, end_pos - deleted_pos);
            co_return (value == "true");
        }
        
        co_return false;
    }

private:
    FileUploadResponse parse_file_upload_response(const std::string& json_str) {
        // FileUploadResponse is an alias for FileObject
        return parse_file_object(json_str);
    }

    FileObject parse_file_object(const std::string& json_str) {
        FileObject file;
        
        // Parse id
        auto id_pos = json_str.find("\"id\":");
        if (id_pos != std::string::npos) {
            id_pos += 5;
            auto id_start = json_str.find("\"", id_pos) + 1;
            auto id_end = json_str.find("\"", id_start);
            file.id = json_str.substr(id_start, id_end - id_start);
        }
        
        // Parse filename
        auto filename_pos = json_str.find("\"filename\":");
        if (filename_pos != std::string::npos) {
            filename_pos += 11;
            auto filename_start = json_str.find("\"", filename_pos) + 1;
            auto filename_end = json_str.find("\"", filename_start);
            file.filename = json_str.substr(filename_start, filename_end - filename_start);
        }
        
        // Parse purpose
        auto purpose_pos = json_str.find("\"purpose\":");
        if (purpose_pos != std::string::npos) {
            purpose_pos += 10;
            auto purpose_start = json_str.find("\"", purpose_pos) + 1;
            auto purpose_end = json_str.find("\"", purpose_start);
            file.purpose = json_str.substr(purpose_start, purpose_end - purpose_start);
        }
        
        // Parse bytes
        auto bytes_pos = json_str.find("\"bytes\":");
        if (bytes_pos != std::string::npos) {
            bytes_pos += 8;
            auto end_pos = json_str.find_first_of(",}", bytes_pos);
            file.bytes = std::stoi(json_str.substr(bytes_pos, end_pos - bytes_pos));
        }
        
        // Parse created_at
        auto created_pos = json_str.find("\"created_at\":");
        if (created_pos != std::string::npos) {
            created_pos += 13;
            auto end_pos = json_str.find_first_of(",}", created_pos);
            file.created_at = std::stoll(json_str.substr(created_pos, end_pos - created_pos));
        }
        
        // Parse status
        auto status_pos = json_str.find("\"status\":");
        if (status_pos != std::string::npos) {
            status_pos += 9;
            auto status_start = json_str.find("\"", status_pos) + 1;
            auto status_end = json_str.find("\"", status_start);
            file.status = json_str.substr(status_start, status_end - status_start);
        }
        
        return file;
    }

    FileListResponse parse_file_list_response(const std::string& json_str) {
        FileListResponse response;
        
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
                
                auto file_json = json_str.substr(obj_start, obj_end - obj_start);
                response.data.push_back(parse_file_object(file_json));
                
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
        
        return response;
    }
};

} // namespace openai::client

