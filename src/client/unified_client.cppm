// Unified Client Module
// Main OpenAI API client that composes all sub-clients

export module openai.client.unified;

import asio;
import openai.client.base;
import openai.client.model;
import openai.client.chat;
import openai.client.image;
import openai.client.embedding;
import openai.client.completion;
import openai.client.moderation;
import openai.client.file;
import openai.client.fine_tuning;
import openai.client.audio;
import openai.client.assistant;
import openai.client.thread;
import openai.client.run;
import openai.types;
import openai.types.common;
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
        , embedding_client_(api_key, io_context)
        , completion_client_(api_key, io_context)
        , moderation_client_(api_key, io_context)
        , file_client_(api_key, io_context)
        , fine_tuning_client_(api_key, io_context)
        , audio_client_(api_key, io_context)
        , assistant_client_(api_key, io_context)
        , thread_client_(api_key, io_context)
        , run_client_(api_key, io_context)
        , api_key_(std::move(api_key))
        , io_context_(io_context) {}

    // Configuration methods
    void set_api_base(std::string base_url) {
        model_client_.set_api_base(base_url);
        chat_client_.set_api_base(base_url);
        image_client_.set_api_base(base_url);
        embedding_client_.set_api_base(base_url);
        completion_client_.set_api_base(base_url);
        moderation_client_.set_api_base(base_url);
        file_client_.set_api_base(base_url);
        fine_tuning_client_.set_api_base(base_url);
        audio_client_.set_api_base(base_url);
        assistant_client_.set_api_base(base_url);
        thread_client_.set_api_base(base_url);
        run_client_.set_api_base(base_url);
    }

    void set_organization(std::string org_id) {
        model_client_.set_organization(org_id);
        chat_client_.set_organization(org_id);
        image_client_.set_organization(org_id);
        embedding_client_.set_organization(org_id);
        completion_client_.set_organization(org_id);
        moderation_client_.set_organization(org_id);
        file_client_.set_organization(org_id);
        fine_tuning_client_.set_organization(org_id);
        audio_client_.set_organization(org_id);
        assistant_client_.set_organization(org_id);
        thread_client_.set_organization(org_id);
        run_client_.set_organization(org_id);
    }

    // ========================================================================
    // Models API - Delegated to ModelClient
    // ========================================================================
    
    asio::awaitable<std::expected<ModelList, ApiError>> list_models() {
        co_return co_await model_client_.list_models();
    }

    asio::awaitable<std::expected<Model, ApiError>> retrieve_model(const std::string& model_id) {
        co_return co_await model_client_.retrieve_model(model_id);
    }

    // ========================================================================
    // Chat Completions API - Delegated to ChatClient
    // ========================================================================
    
    asio::awaitable<std::expected<ChatCompletionResponse, ApiError>> create_chat_completion(
        const ChatCompletionRequest& request
    ) {
        co_return co_await chat_client_.create_chat_completion(request);
    }

    std::expected<ChatCompletionResponse, ApiError> create_chat_completion_sync(
        const ChatCompletionRequest& request
    ) {
        return chat_client_.create_chat_completion_sync(request);
    }

    // ========================================================================
    // Images API - Delegated to ImageClient
    // ========================================================================
    
    asio::awaitable<std::expected<ImageResponse, ApiError>> generate_image(const ImageGenerationRequest& request) {
        co_return co_await image_client_.generate_image(request);
    }

    asio::awaitable<std::expected<ImageResponse, ApiError>> edit_image(const ImageEditRequest& request) {
        co_return co_await image_client_.edit_image(request);
    }

    asio::awaitable<std::expected<ImageResponse, ApiError>> create_image_variation(const ImageVariationRequest& request) {
        co_return co_await image_client_.create_image_variation(request);
    }

    // ========================================================================
    // Completions API (Legacy) - Delegated to CompletionClient
    // ========================================================================
    
    asio::awaitable<std::expected<std::string, ApiError>> create_completion(const CompletionRequest& request) {
        co_return co_await completion_client_.create_completion(request);
    }

    // ========================================================================
    // Embeddings API - Delegated to EmbeddingClient
    // ========================================================================
    
    asio::awaitable<std::expected<EmbeddingResponse, ApiError>> create_embedding(const EmbeddingRequest& request) {
        co_return co_await embedding_client_.create_embedding(request);
    }

    // ========================================================================
    // Files API - Delegated to FileClient
    // ========================================================================
    
    asio::awaitable<std::expected<FileUploadResponse, ApiError>> upload_file(const FileUploadRequest& request) {
        co_return co_await file_client_.upload_file(request);
    }

    asio::awaitable<std::expected<FileListResponse, ApiError>> list_files() {
        co_return co_await file_client_.list_files();
    }

    asio::awaitable<std::expected<FileObject, ApiError>> retrieve_file(const std::string& file_id) {
        co_return co_await file_client_.retrieve_file(file_id);
    }

    asio::awaitable<std::expected<FileContentResponse, ApiError>> retrieve_file_content(const std::string& file_id) {
        co_return co_await file_client_.retrieve_file_content(file_id);
    }

    asio::awaitable<std::expected<bool, ApiError>> delete_file(const std::string& file_id) {
        co_return co_await file_client_.delete_file(file_id);
    }

    // ========================================================================
    // Fine-tuning API - Delegated to FineTuningClient
    // ========================================================================
    
    asio::awaitable<std::expected<FineTuningJobResponse, ApiError>> create_fine_tuning_job(const FineTuningRequest& request) {
        co_return co_await fine_tuning_client_.create_fine_tuning_job(request);
    }

    asio::awaitable<std::expected<FineTuningJobListResponse, ApiError>> list_fine_tuning_jobs(int limit = 20) {
        co_return co_await fine_tuning_client_.list_fine_tuning_jobs(limit);
    }

    asio::awaitable<std::expected<FineTuningJobResponse, ApiError>> retrieve_fine_tuning_job(const std::string& job_id) {
        co_return co_await fine_tuning_client_.retrieve_fine_tuning_job(job_id);
    }

    asio::awaitable<std::expected<FineTuningJobResponse, ApiError>> cancel_fine_tuning_job(const std::string& job_id) {
        co_return co_await fine_tuning_client_.cancel_fine_tuning_job(job_id);
    }

    // ========================================================================
    // Audio API - Delegated to AudioClient
    // ========================================================================
    
    asio::awaitable<std::expected<AudioResponse, ApiError>> create_transcription(const AudioTranscriptionRequest& request) {
        co_return co_await audio_client_.create_transcription(request);
    }

    asio::awaitable<std::expected<AudioResponse, ApiError>> create_translation(const AudioTranslationRequest& request) {
        co_return co_await audio_client_.create_translation(request);
    }

    // ========================================================================
    // Moderation API - Delegated to ModerationClient
    // ========================================================================
    
    asio::awaitable<std::expected<ModerationResponse, ApiError>> create_moderation(const ModerationRequest& request) {
        co_return co_await moderation_client_.create_moderation(request);
    }

    // ========================================================================
    // Assistants API (Beta) - Delegated to AssistantClient
    // ========================================================================
    
    asio::awaitable<std::expected<Assistant, ApiError>> create_assistant(const CreateAssistantRequest& request) {
        co_return co_await assistant_client_.create_assistant(request);
    }

    asio::awaitable<std::expected<AssistantListResponse, ApiError>> list_assistants(int limit = 20) {
        co_return co_await assistant_client_.list_assistants(limit);
    }

    asio::awaitable<std::expected<Assistant, ApiError>> retrieve_assistant(const std::string& assistant_id) {
        co_return co_await assistant_client_.retrieve_assistant(assistant_id);
    }

    asio::awaitable<std::expected<Assistant, ApiError>> modify_assistant(
        const std::string& assistant_id,
        const ModifyAssistantRequest& request
    ) {
        co_return co_await assistant_client_.modify_assistant(assistant_id, request);
    }

    asio::awaitable<std::expected<DeleteAssistantResponse, ApiError>> delete_assistant(const std::string& assistant_id) {
        co_return co_await assistant_client_.delete_assistant(assistant_id);
    }

    // ========================================================================
    // Threads API (Beta) - Delegated to ThreadClient
    // ========================================================================
    
    asio::awaitable<std::expected<Thread, ApiError>> create_thread(const CreateThreadRequest& request) {
        co_return co_await thread_client_.create_thread(request);
    }

    asio::awaitable<std::expected<Thread, ApiError>> retrieve_thread(const std::string& thread_id) {
        co_return co_await thread_client_.retrieve_thread(thread_id);
    }

    asio::awaitable<std::expected<Thread, ApiError>> modify_thread(
        const std::string& thread_id,
        const ModifyThreadRequest& request
    ) {
        co_return co_await thread_client_.modify_thread(thread_id, request);
    }

    asio::awaitable<std::expected<DeleteThreadResponse, ApiError>> delete_thread(const std::string& thread_id) {
        co_return co_await thread_client_.delete_thread(thread_id);
    }

    // ========================================================================
    // Messages API (Beta) - Delegated to ThreadClient
    // ========================================================================
    
    asio::awaitable<std::expected<ThreadMessage, ApiError>> create_message(
        const std::string& thread_id,
        const CreateMessageRequest& request
    ) {
        co_return co_await thread_client_.create_message(thread_id, request);
    }

    asio::awaitable<std::expected<ThreadMessageListResponse, ApiError>> list_messages(
        const std::string& thread_id,
        int limit = 20
    ) {
        co_return co_await thread_client_.list_messages(thread_id, limit);
    }

    asio::awaitable<std::expected<ThreadMessage, ApiError>> retrieve_message(
        const std::string& thread_id,
        const std::string& message_id
    ) {
        co_return co_await thread_client_.retrieve_message(thread_id, message_id);
    }

    asio::awaitable<std::expected<ThreadMessage, ApiError>> modify_message(
        const std::string& thread_id,
        const std::string& message_id,
        const ModifyMessageRequest& request
    ) {
        co_return co_await thread_client_.modify_message(thread_id, message_id, request);
    }

    // ========================================================================
    // Runs API (Beta) - Delegated to RunClient
    // ========================================================================
    
    asio::awaitable<std::expected<Run, ApiError>> create_run(
        const std::string& thread_id,
        const CreateRunRequest& request
    ) {
        co_return co_await run_client_.create_run(thread_id, request);
    }

    asio::awaitable<std::expected<RunListResponse, ApiError>> list_runs(
        const std::string& thread_id,
        int limit = 20
    ) {
        co_return co_await run_client_.list_runs(thread_id, limit);
    }

    asio::awaitable<std::expected<Run, ApiError>> retrieve_run(
        const std::string& thread_id,
        const std::string& run_id
    ) {
        co_return co_await run_client_.retrieve_run(thread_id, run_id);
    }

    asio::awaitable<std::expected<Run, ApiError>> modify_run(
        const std::string& thread_id,
        const std::string& run_id,
        const ModifyRunRequest& request
    ) {
        co_return co_await run_client_.modify_run(thread_id, run_id, request);
    }

    asio::awaitable<std::expected<Run, ApiError>> cancel_run(
        const std::string& thread_id,
        const std::string& run_id
    ) {
        co_return co_await run_client_.cancel_run(thread_id, run_id);
    }

    asio::awaitable<std::expected<Run, ApiError>> submit_tool_outputs(
        const std::string& thread_id,
        const std::string& run_id,
        const SubmitToolOutputsRequest& request
    ) {
        co_return co_await run_client_.submit_tool_outputs(thread_id, run_id, request);
    }

    asio::awaitable<std::expected<RunStepListResponse, ApiError>> list_run_steps(
        const std::string& thread_id,
        const std::string& run_id,
        int limit = 20
    ) {
        co_return co_await run_client_.list_run_steps(thread_id, run_id, limit);
    }

    asio::awaitable<std::expected<RunStep, ApiError>> retrieve_run_step(
        const std::string& thread_id,
        const std::string& run_id,
        const std::string& step_id
    ) {
        co_return co_await run_client_.retrieve_run_step(thread_id, run_id, step_id);
    }

private:
    // Composed specialized clients - Core APIs
    client::ModelClient model_client_;
    client::ChatClient chat_client_;
    client::ImageClient image_client_;
    client::EmbeddingClient embedding_client_;
    client::CompletionClient completion_client_;
    client::ModerationClient moderation_client_;
    
    // Composed specialized clients - Advanced APIs
    client::FileClient file_client_;
    client::FineTuningClient fine_tuning_client_;
    client::AudioClient audio_client_;
    
    // Composed specialized clients - Beta APIs
    client::AssistantClient assistant_client_;
    client::ThreadClient thread_client_;
    client::RunClient run_client_;
    
    std::string api_key_;
    asio::io_context& io_context_;
};

} // namespace openai

