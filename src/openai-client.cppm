// OpenAI Client Module
// High-level OpenAI API client with C++23 coroutines

export module openai.client;

import asio;
import fmt;
import openai.http_client;
import openai.types;
import std;

export namespace openai {

// OpenAI API Client
class Client {
public:
    explicit Client(std::string api_key, asio::io_context& io_context);

    void set_api_base(std::string base_url);
    void set_organization(std::string org_id);

    // Models API
    asio::awaitable<ModelList> list_models();
    asio::awaitable<Model> retrieve_model(const std::string& model_id);

    // Chat Completions API
    asio::awaitable<ChatCompletionResponse> create_chat_completion(
        const ChatCompletionRequest& request
    );
    ChatCompletionResponse create_chat_completion_sync(
        const ChatCompletionRequest& request
    );

    // Completions API (Legacy)
    asio::awaitable<std::string> create_completion(const CompletionRequest& request);

    // Image Generation API
    asio::awaitable<ImageResponse> generate_image(const ImageGenerationRequest& request);

    // Embeddings API
    asio::awaitable<EmbeddingResponse> create_embedding(const EmbeddingRequest& request);

    // Files API
    asio::awaitable<FileListResponse> list_files();
    asio::awaitable<FileObject> retrieve_file(const std::string& file_id);
    asio::awaitable<bool> delete_file(const std::string& file_id);

    // Fine-tuning API
    asio::awaitable<FineTuningJobResponse> create_fine_tuning_job(const FineTuningRequest& request);
    asio::awaitable<FineTuningJobListResponse> list_fine_tuning_jobs(int limit = 20);
    asio::awaitable<FineTuningJobResponse> retrieve_fine_tuning_job(const std::string& job_id);
    asio::awaitable<FineTuningJobResponse> cancel_fine_tuning_job(const std::string& job_id);

    // Audio API
    asio::awaitable<AudioResponse> create_transcription(const AudioTranscriptionRequest& request);
    asio::awaitable<AudioResponse> create_translation(const AudioTranslationRequest& request);

    // Moderation API
    asio::awaitable<ModerationResponse> create_moderation(const ModerationRequest& request);

private:
    void add_auth_headers(http::Request& req);
    ChatCompletionResponse parse_chat_completion_response(const std::string& json_str);
    std::string unescape_json(const std::string& str);
    ImageResponse parse_image_response(const std::string& json_str);
    EmbeddingResponse parse_embedding_response(const std::string& json_str);
    FileObject parse_file_object(const std::string& json_str);
    FileListResponse parse_file_list_response(const std::string& json_str);
    FineTuningJob parse_fine_tuning_job(const std::string& json_str);
    FineTuningJobListResponse parse_fine_tuning_job_list_response(const std::string& json_str);
    ModerationResponse parse_moderation_response(const std::string& json_str);
    ModerationResult parse_moderation_result(const std::string& json_str);
    bool parse_bool_field(const std::string& json_str, const std::string& field_name);
    double parse_double_field(const std::string& json_str, const std::string& field_name);

    std::string api_key_;
    std::string organization_id_;
    std::string api_base_;
    http::Client http_client_;
    asio::io_context& io_context_;
};

} // namespace openai
