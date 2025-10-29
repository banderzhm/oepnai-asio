// Audio API Example - Transcription and Translation using Whisper
import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> audio_example(openai::Client& client) {
    fmt::print(R"(=== Audio API Example ===

The Audio API provides two capabilities using the Whisper model:
  1. Transcription: Convert audio to text in the same language
  2. Translation: Convert audio to English text

Supported audio formats: mp3, mp4, mpeg, mpga, m4a, wav, webm
Max file size: 25 MB

)");

    // Transcription example
    fmt::print("--- Audio Transcription ---\n");
    fmt::print("Converting audio to text in the same language...\n\n");
    
    openai::AudioTranscriptionRequest transcription_req;
    transcription_req.file_path = "audio.mp3";  // Change this to your audio file
    transcription_req.model = "whisper-1";
    transcription_req.response_format = "json";
    
    try {
        auto transcription = co_await client.create_transcription(transcription_req);
        
        if (transcription.is_error) {
            fmt::print("Note: {}\n\n", transcription.error_message);
            fmt::print(R"(To use this feature, you would need to:
  1. Implement multipart/form-data file upload in the HTTP client
  2. Read the audio file from disk
  3. Send it as part of the request body

Example request would be:
  POST https://api.openai.com/v1/audio/transcriptions
  Content-Type: multipart/form-data; boundary=----WebKitFormBoundary
  
  ------WebKitFormBoundary
  Content-Disposition: form-data; name="file"; filename="audio.mp3"
  Content-Type: audio/mpeg
  
  [binary audio data]
  ------WebKitFormBoundary
  Content-Disposition: form-data; name="model"
  
  whisper-1
  ------WebKitFormBoundary--

)");
        } else {
            fmt::print("Transcription:\n{}\n\n", transcription.text);
        }
    } catch (const std::exception& e) {
        fmt::print("Error: {}\n\n", e.what());
    }
    
    // Translation example
    fmt::print("--- Audio Translation ---\n");
    fmt::print("Converting audio to English text...\n\n");
    
    openai::AudioTranslationRequest translation_req;
    translation_req.file_path = "german.m4a";  // Change this to your audio file
    translation_req.model = "whisper-1";
    translation_req.response_format = "json";
    
    try {
        auto translation = co_await client.create_translation(translation_req);
        
        if (translation.is_error) {
            fmt::print("Note: {}\n\n", translation.error_message);
        } else {
            fmt::print("Translation:\n{}\n\n", translation.text);
        }
    } catch (const std::exception& e) {
        fmt::print("Error: {}\n\n", e.what());
    }
    
    fmt::print(R"(Use cases for Audio API:
  - Meeting transcriptions
  - Podcast subtitles
  - Voice command processing
  - Multilingual content translation
  - Accessibility features

)");
}

int main() {
    const char* api_key = std::getenv("OPENAI_API_KEY");
    if (!api_key) {
        fmt::print("Error: OPENAI_API_KEY environment variable not set\n");
        return 1;
    }
    
    asio::io_context io_context;
    openai::Client client(api_key, io_context);
    
    asio::co_spawn(io_context, audio_example(client), asio::detached);
    io_context.run();
    
    return 0;
}

