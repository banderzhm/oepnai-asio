// Unified Client Module
// Main OpenAI API client that composes all sub-clients

export module openai.client.unified;

import asio;
import openai.client.base;
import openai.client.model;
import openai.client.chat;
import openai.client.image;
import openai.types;
import std;

export namespace openai {

// Unified OpenAI API Client
// Composes all specialized API clients into a single interface
class Client {
public:
    explicit Client(std::string api_key, asio::io_context& io_context)
        : model_client_(api_key, io_context)
        , chat_client_(api_key, io_context)
        , image_client_(api_key, io_context)
        , api_key_(std::move(api_key))
        , io_context_(io_context) {}

    // Configuration methods
    void set_api_base(std::string base_url) {
        model_client_.set_api_base(base_url);
        chat_client_.set_api_base(base_url);
        image_client_.set_api_base(base_url);
    }

    void set_organization(std::string org_id) {
        model_client_.set_organization(org_id);
        chat_client_.set_organization(org_id);
        image_client_.set_organization(org_id);
    }

    // ========================================================================
    // Models API - Delegated to ModelClient
    // ========================================================================
    
    asio::awaitable<ModelList> list_models() {
        co_return co_await model_client_.list_models();
    }

    asio::awaitable<Model> retrieve_model(const std::string& model_id) {
        co_return co_await model_client_.retrieve_model(model_id);
    }

    // ========================================================================
    // Chat Completions API - Delegated to ChatClient
    // ========================================================================
    
    asio::awaitable<ChatCompletionResponse> create_chat_completion(
        const ChatCompletionRequest& request
    ) {
        co_return co_await chat_client_.create_chat_completion(request);
    }

    ChatCompletionResponse create_chat_completion_sync(
        const ChatCompletionRequest& request
    ) {
        return chat_client_.create_chat_completion_sync(request);
    }

    // ========================================================================
    // Images API - Delegated to ImageClient
    // ========================================================================
    
    asio::awaitable<ImageResponse> generate_image(const ImageGenerationRequest& request) {
        co_return co_await image_client_.generate_image(request);
    }

    asio::awaitable<ImageResponse> edit_image(const ImageEditRequest& request) {
        co_return co_await image_client_.edit_image(request);
    }

    asio::awaitable<ImageResponse> create_image_variation(const ImageVariationRequest& request) {
        co_return co_await image_client_.create_image_variation(request);
    }

    // ========================================================================
    // Legacy APIs - To be refactored into separate clients
    // ========================================================================
    
    // Completions API (Legacy) - To be moved to CompletionClient
    asio::awaitable<std::string> create_completion(const CompletionRequest& request) {
        // Placeholder - needs implementation
        co_return "Not implemented";
    }

    // Embeddings API - To be moved to EmbeddingClient
    asio::awaitable<EmbeddingResponse> create_embedding(const EmbeddingRequest& request) {
        // Placeholder - needs implementation
        EmbeddingResponse response;
        response.is_error = true;
        response.error_message = "Not implemented";
        co_return response;
    }

    // Files API - To be moved to FileClient
    asio::awaitable<FileUploadResponse> upload_file(const FileUploadRequest& request) {
        // Placeholder - needs implementation
        FileUploadResponse response;
        response.is_error = true;
        response.error_message = "Not implemented";
        co_return response;
    }

    asio::awaitable<FileListResponse> list_files() {
        // Placeholder - needs implementation
        FileListResponse response;
        response.is_error = true;
        response.error_message = "Not implemented";
        co_return response;
    }

    asio::awaitable<FileObject> retrieve_file(const std::string& file_id) {
        // Placeholder - needs implementation
        FileObject response;
        response.is_error = true;
        response.error_message = "Not implemented";
        co_return response;
    }

    asio::awaitable<FileContentResponse> retrieve_file_content(const std::string& file_id) {
        // Placeholder - needs implementation
        FileContentResponse response;
        response.is_error = true;
        response.error_message = "Not implemented";
        co_return response;
    }

    asio::awaitable<bool> delete_file(const std::string& file_id) {
        // Placeholder - needs implementation
        co_return false;
    }

    // Fine-tuning API - To be moved to FineTuningClient
    asio::awaitable<FineTuningJobResponse> create_fine_tuning_job(const FineTuningRequest& request) {
        // Placeholder - needs implementation
        FineTuningJobResponse response;
        response.is_error = true;
        response.error_message = "Not implemented";
        co_return response;
    }

    asio::awaitable<FineTuningJobListResponse> list_fine_tuning_jobs(int limit = 20) {
        // Placeholder - needs implementation
        FineTuningJobListResponse response;
        response.is_error = true;
        response.error_message = "Not implemented";
        co_return response;
    }

    asio::awaitable<FineTuningJobResponse> retrieve_fine_tuning_job(const std::string& job_id) {
        // Placeholder - needs implementation
        FineTuningJobResponse response;
        response.is_error = true;
        response.error_message = "Not implemented";
        co_return response;
    }

    asio::awaitable<FineTuningJobResponse> cancel_fine_tuning_job(const std::string& job_id) {
        // Placeholder - needs implementation
        FineTuningJobResponse response;
        response.is_error = true;
        response.error_message = "Not implemented";
        co_return response;
    }

    // Audio API - To be moved to AudioClient
    asio::awaitable<AudioResponse> create_transcription(const AudioTranscriptionRequest& request) {
        // Placeholder - needs implementation
        AudioResponse response;
        response.is_error = true;
        response.error_message = "Not implemented";
        co_return response;
    }

    asio::awaitable<AudioResponse> create_translation(const AudioTranslationRequest& request) {
        // Placeholder - needs implementation
        AudioResponse response;
        response.is_error = true;
        response.error_message = "Not implemented";
        co_return response;
    }

    // Moderation API - To be moved to ModerationClient
    asio::awaitable<ModerationResponse> create_moderation(const ModerationRequest& request) {
        // Placeholder - needs implementation
        ModerationResponse response;
        response.is_error = true;
        response.error_message = "Not implemented";
        co_return response;
    }

private:
    // Composed specialized clients
    client::ModelClient model_client_;
    client::ChatClient chat_client_;
    client::ImageClient image_client_;
    
    // TODO: Add more specialized clients:
    // client::CompletionClient completion_client_;
    // client::EmbeddingClient embedding_client_;
    // client::FileClient file_client_;
    // client::FineTuningClient fine_tuning_client_;
    // client::AudioClient audio_client_;
    // client::ModerationClient moderation_client_;
    // client::AssistantClient assistant_client_;
    // client::ThreadClient thread_client_;
    // client::RunClient run_client_;
    
    std::string api_key_;
    asio::io_context& io_context_;
};

} // namespace openai

