// Audio API Types Module
// Audio transcription and translation types

export module openai.types.audio;

import std;

export namespace openai {

// ============================================================================
// AUDIO API - Request & Response Types
// ============================================================================

// Audio transcription request
struct AudioTranscriptionRequest {
    std::string file_path;
    std::string model{"whisper-1"};
    std::optional<std::string> language;
    std::optional<std::string> prompt;
    std::optional<std::string> response_format{"json"};
    std::optional<double> temperature{0.0};
};

// Audio translation request
struct AudioTranslationRequest {
    std::string file_path;
    std::string model{"whisper-1"};
    std::optional<std::string> prompt;
    std::optional<std::string> response_format{"json"};
    std::optional<double> temperature{0.0};
};

// Audio response
struct AudioResponse {
    std::string text;
    bool is_error{false};
    std::string error_message;
};

} // namespace openai

