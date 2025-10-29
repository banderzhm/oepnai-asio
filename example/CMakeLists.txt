# OpenAI Asio Examples

# Helper function to create example executables
function(add_openai_example name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE openai_asio_core)
    set_property(TARGET ${name} PROPERTY
        CXX_MODULE_GENERATION_MODE "SEPARATE"
    )
endfunction()

# Add all examples
add_openai_example(00-showcase)
add_openai_example(01-chat)
add_openai_example(02-chat-conversation)
add_openai_example(03-chat-system-role)
add_openai_example(04-http-test)
add_openai_example(05-model)
add_openai_example(06-completion)
add_openai_example(07-edit)
add_openai_example(08-image)
add_openai_example(09-embedding)
add_openai_example(10-file)
add_openai_example(11-fine-tune)
add_openai_example(12-audio)
add_openai_example(13-moderation)

message(STATUS "==========================================")
message(STATUS "Examples configured:")
message(STATUS "  - 00-showcase          : API showcase overview")
message(STATUS "  - 01-chat              : Basic chat completion")
message(STATUS "  - 02-chat-conversation : Multi-turn conversation")
message(STATUS "  - 03-chat-system-role  : System role examples")
message(STATUS "  - 04-http-test         : HTTP client test (no API key needed)")
message(STATUS "  - 05-model             : List and retrieve models")
message(STATUS "  - 06-completion        : Text completion (legacy)")
message(STATUS "  - 07-edit              : Edit API (deprecated)")
message(STATUS "  - 08-image             : Image generation")
message(STATUS "  - 09-embedding         : Text embeddings")
message(STATUS "  - 10-file              : File management")
message(STATUS "  - 11-fine-tune         : Fine-tuning jobs")
message(STATUS "  - 12-audio             : Audio transcription/translation")
message(STATUS "  - 13-moderation        : Content moderation")
message(STATUS "==========================================")

