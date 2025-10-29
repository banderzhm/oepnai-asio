// Base Client Module
// Provides HTTP communication and authentication foundation

export module openai.client.base;

import asio;
import fmt;
import openai.http_client;
import std;

export namespace openai::client {

// Base client class providing HTTP and authentication functionality
class BaseClient {
public:
    explicit BaseClient(std::string api_key, asio::io_context& io_context)
        : api_key_(std::move(api_key))
        , io_context_(io_context)
        , http_client_(io_context)
        , api_base_("https://api.openai.com/v1") {}

    virtual ~BaseClient() = default;

    // Configuration
    void set_api_base(std::string base_url) {
        api_base_ = std::move(base_url);
    }

    void set_organization(std::string org_id) {
        organization_id_ = std::move(org_id);
    }

protected:
    // Helper: Add authentication headers to request
    void add_auth_headers(http::Request& req, bool json_content = true) const {
        req.headers["Authorization"] = fmt::format("Bearer {}", api_key_);
        
        if (json_content) {
            req.headers["Content-Type"] = "application/json";
        }
        
        if (!organization_id_.empty()) {
            req.headers["OpenAI-Organization"] = organization_id_;
        }
    }
    
    // Helper: Generate multipart/form-data boundary
    std::string generate_boundary() const {
        return "----OpenAIFormBoundary" + std::to_string(
            std::chrono::system_clock::now().time_since_epoch().count()
        );
    }
    
    // Helper: Build multipart/form-data body
    std::string build_multipart_formdata(
        const std::map<std::string, std::string>& fields,
        const std::map<std::string, std::pair<std::string, std::string>>& files,
        const std::string& boundary
    ) const {
        std::string body;
        
        // Add text fields
        for (const auto& [key, value] : fields) {
            body += "--" + boundary + "\r\n";
            body += "Content-Disposition: form-data; name=\"" + key + "\"\r\n\r\n";
            body += value + "\r\n";
        }
        
        // Add file fields
        for (const auto& [key, file_data] : files) {
            const auto& [filename, content] = file_data;
            body += "--" + boundary + "\r\n";
            body += "Content-Disposition: form-data; name=\"" + key + "\"; filename=\"" + filename + "\"\r\n";
            body += "Content-Type: application/octet-stream\r\n\r\n";
            body += content + "\r\n";
        }
        
        // End boundary
        body += "--" + boundary + "--\r\n";
        
        return body;
    }

    // Helper: Build full API endpoint URL
    std::string build_endpoint(const std::string& path) const {
        return api_base_ + path;
    }
    
    // Helper: Read file content
    std::string read_file_content(const std::string& filepath) const {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + filepath);
        }
        
        std::ostringstream content;
        content << file.rdbuf();
        return content.str();
    }

    // Helper: Unescape JSON strings
    std::string unescape_json(const std::string& str) const {
        std::string result;
        result.reserve(str.size());
        
        for (std::size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '\\' && i + 1 < str.length()) {
                switch (str[i + 1]) {
                    case 'n': result += '\n'; ++i; break;
                    case 't': result += '\t'; ++i; break;
                    case 'r': result += '\r'; ++i; break;
                    case '\\': result += '\\'; ++i; break;
                    case '"': result += '"'; ++i; break;
                    default: result += str[i]; break;
                }
            } else {
                result += str[i];
            }
        }
        
        return result;
    }

    // Helper: Parse boolean field from JSON
    bool parse_bool_field(const std::string& json_str, const std::string& field_name) const {
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

    // Helper: Parse double field from JSON
    double parse_double_field(const std::string& json_str, const std::string& field_name) const {
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

    // Protected members accessible to derived clients
    std::string api_key_;
    std::string organization_id_;
    std::string api_base_;
    http::Client http_client_;
    asio::io_context& io_context_;
};

} // namespace openai::client

