# OpenAI Asio - Modern C++23 OpenAI API Client

[![MSVC Build](https://github.com/banderzhm/oepnai-asio/actions/workflows/msvc.yml/badge.svg)](https://github.com/banderzhm/oepnai-asio/actions/workflows/msvc.yml)
[![Clang Build](https://github.com/banderzhm/oepnai-asio/actions/workflows/clang.yml/badge.svg)](https://github.com/banderzhm/oepnai-asio/actions/workflows/clang.yml)

A modern OpenAI API client library built with **C++23 coroutines** and **Asio**. Fully modular design with type-safe, high-performance asynchronous API capabilities.

[English](README.md) | [简体中文](README-CN.md)

## ✨ Key Features

- ✅ **C++23 Coroutines** - Elegant asynchronous programming with `co_await`
- ✅ **Modular Architecture** - Full C++23 modules, zero header dependencies
- ✅ **Asio Powered** - High-performance async I/O based on Asio
- ✅ **Native HTTPS Support** - Built-in SSL/TLS encryption
- ✅ **Type Safety** - Strong-typed request and response structures
- ✅ **Simple API** - Single import point, easy to use

## 📦 Requirements

- **Compilers**
  - MSVC 14.44+ (Visual Studio 2022 17.10+)
  - Clang 18+ with libc++
  - GCC 14+ (experimental support)
  
- **CMake** 4.0+
- **OpenSSL** (for HTTPS support)

## 🚀 Quick Start

### Building from Source

#### Windows (PowerShell)

```powershell
# 1. Clone the repository
git clone https://github.com/banderzhm/oepnai-asio
cd openai-asio

# 2. Install OpenSSL (if not already installed)
# Download from: https://slproweb.com/products/Win32OpenSSL.html

# 3. Build
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

#### Linux (Clang with libc++)

```bash
# 1. Install dependencies
sudo apt install clang-18 libc++-18-dev libssl-dev cmake

# 2. Clone the repository
git clone https://github.com/banderzhm/oepnai-asio
cd openai-asio

# 3. Build
mkdir build && cd build
cmake .. -DCMAKE_CXX_COMPILER=clang++-18
cmake --build . -j$(nproc)
```

## 💡 Usage Examples

This project provides rich example programs. All examples are located in the [`example/`](example/) directory:

| Example | Description | Source File |
|---------|-------------|-------------|
| 00 | Feature Showcase | [`00-showcase.cpp`](example/00-showcase.cpp) |
| 01 | Basic Chat Completion | [`01-chat.cpp`](example/01-chat.cpp) |
| 02 | Multi-turn Conversation | [`02-chat-conversation.cpp`](example/02-chat-conversation.cpp) |
| 03 | System Role Setup | [`03-chat-system-role.cpp`](example/03-chat-system-role.cpp) |
| 04 | HTTP Client Test | [`04-http-test.cpp`](example/04-http-test.cpp) |
| 05 | Model List | [`05-model.cpp`](example/05-model.cpp) |
| 06 | Text Completion | [`06-completion.cpp`](example/06-completion.cpp) |
| 07 | Text Edit | [`07-edit.cpp`](example/07-edit.cpp) |
| 08 | Image Generation | [`08-image.cpp`](example/08-image.cpp) |
| 09 | Text Embeddings | [`09-embedding.cpp`](example/09-embedding.cpp) |
| 10 | File Management | [`10-file.cpp`](example/10-file.cpp) |
| 11 | Fine-tuning | [`11-fine-tune.cpp`](example/11-fine-tune.cpp) |
| 12 | Audio Transcription | [`12-audio.cpp`](example/12-audio.cpp) |
| 13 | Content Moderation | [`13-moderation.cpp`](example/13-moderation.cpp) |

### Quick Example

```cpp
import openai;
import std;
import asio;

int main() {
    asio::io_context io_context;
    openai::Client client("your-api-key", io_context);
    
    openai::ChatCompletionRequest req;
    req.model = "gpt-3.5-turbo";
    req.messages.push_back({
        openai::MessageRole::User, 
        "Hello, how are you?"
    });
    
    auto response = client.create_chat_completion_sync(req);
    
    if (!response.is_error && !response.choices.empty()) {
        std::println("{}", response.choices[0].message.content);
    }
    
    return 0;
}
```

For more detailed examples, please check the source files in the [`example/`](example/) directory.

## 🏗️ Project Structure

```
openai-asio/
├── src/
│   ├── openai.cppm                 # Main module (single import point)
│   ├── openai-types.cppm           # Type definitions module
│   ├── openai-http_client.cppm/.cpp # HTTP client module
│   ├── client/                     # API client modules
│   │   ├── base_client.cppm        # Base client with common functionality
│   │   ├── unified_client.cppm     # Unified client (composition pattern)
│   │   ├── chat_client.cppm        # Chat Completions API
│   │   ├── completion_client.cppm  # Completions API (legacy)
│   │   ├── model_client.cppm       # Models API
│   │   ├── image_client.cppm       # Images API
│   │   ├── embedding_client.cppm   # Embeddings API
│   │   ├── file_client.cppm        # Files API
│   │   ├── fine_tuning_client.cppm # Fine-tuning API
│   │   ├── audio_client.cppm       # Audio API (Whisper)
│   │   ├── moderation_client.cppm  # Moderation API
│   │   ├── assistant_client.cppm   # Assistants API (Beta)
│   │   ├── thread_client.cppm      # Threads API (Beta)
│   │   └── run_client.cppm         # Runs API (Beta)
│   └── message/                    # Type definition sub-modules
│       ├── common.cppm             # Common types (ApiError, std::expected)
│       ├── chat.cppm               # Chat-related types
│       ├── completion.cppm         # Completion-related types
│       ├── model.cppm              # Model-related types
│       ├── image.cppm              # Image-related types
│       ├── embedding.cppm          # Embedding-related types
│       ├── file.cppm               # File-related types
│       ├── fine_tuning.cppm        # Fine-tuning-related types
│       ├── audio.cppm              # Audio-related types
│       ├── moderation.cppm         # Moderation-related types
│       ├── assistant.cppm          # Assistant-related types (Beta)
│       ├── thread.cppm             # Thread-related types (Beta)
│       └── run.cppm                # Run-related types (Beta)
├── example/                        # Example programs
│   └── CMakeLists.txt              # all_examples target for batch compilation
├── 3rdparty/                       # Third-party libraries
│   ├── asio/                       # Asio async I/O
│   ├── fmt/                        # Formatting library
│   └── nlohmann_json/              # JSON library
├── CMakeLists.txt                  # CMake configuration
└── LICENSE                         # MIT License
```

## 🔧 Modular Architecture

```
import openai;  ← Single import point
    ↓
openai (main module)
    ├─→ openai.http_client         (HTTP/HTTPS client)
    ├─→ openai.types               (Type definitions)
    │    ├─→ openai.types.chat
    │    ├─→ openai.types.completion
    │    ├─→ openai.types.model
    │    ├─→ openai.types.image
    │    ├─→ openai.types.embedding
    │    ├─→ openai.types.file
    │    ├─→ openai.types.fine_tuning
    │    ├─→ openai.types.audio
    │    ├─→ openai.types.moderation
    │    ├─→ openai.types.assistant  (Beta)
    │    ├─→ openai.types.thread     (Beta)
    │    └─→ openai.types.run        (Beta)
    └─→ openai.client.unified      (Unified client)
         ├─→ openai.client.base    (Base client)
         ├─→ openai.client.model   (Models API)
         ├─→ openai.client.chat    (Chat API)
         ├─→ openai.client.image   (Images API)
         ├─→ openai.client.embedding
         ├─→ openai.client.completion
         ├─→ openai.client.moderation
         ├─→ openai.client.file
         ├─→ openai.client.fine_tuning
         ├─→ openai.client.audio
         ├─→ openai.client.assistant  (Beta)
         ├─→ openai.client.thread     (Beta)
         └─→ openai.client.run        (Beta)
```

### Architecture Benefits

- 🔹 **Modular Design** - Each API in separate module, compile on demand
- 🔹 **Fast Compilation** - Module caching, faster incremental builds
- 🔹 **Easy Maintenance** - Separation of concerns, clear code organization
- 🔹 **Composition Pattern** - Unified client composes all specialized clients

## 📚 API Support

### Core APIs
| API | Status | Description |
|-----|--------|-------------|
| Chat Completions | ✅ | Chat completions (GPT-3.5/GPT-4) |
| Completions | ✅ | Text completions (legacy) |
| Models | ✅ | Model list and information |
| Images | ✅ | Image generation/editing/variation |
| Embeddings | ✅ | Text embedding vectors |
| Moderations | ✅ | Content moderation |

### Advanced APIs
| API | Status | Description |
|-----|--------|-------------|
| Files | ✅ | File upload/list/retrieve/delete |
| Fine-tuning | ✅ | Fine-tuning job management |
| Audio | ✅ | Audio transcription/translation (Whisper) |

### Beta APIs
| API | Status | Description |
|-----|--------|-------------|
| Assistants | ✅ | AI assistant creation and management |
| Threads | ✅ | Conversation thread management |
| Messages | ✅ | Message CRUD operations |
| Runs | ✅ | Run management and tool outputs |

**All OpenAI APIs fully implemented!**

## 🔧 Troubleshooting

### Windows: OpenSSL Not Found

```bash
cmake .. -DOPENSSL_ROOT_DIR="C:/Program Files/OpenSSL-Win64"
```

### Linux: libc++ Modules Not Found

```bash
cmake .. \
  -DLIBCXX_MODULE_DIRS=/usr/lib/llvm-18/share/libc++/v1 \
  -DLIBCXX_INCLUDE_DIRS=/usr/lib/llvm-18/include/c++/v1
```

### Compilation Error: Modules Not Supported

Ensure you're using a compiler and CMake version that supports C++23 modules:
- MSVC: Visual Studio 2022 17.10+
- Clang: 18.0+
- CMake: 4.0+

## 📄 License

[MIT License](LICENSE)

## 🤝 Contributing

Issues and Pull Requests are welcome!

## 🔗 Resources

### API & Language Features
- [OpenAI API Documentation](https://platform.openai.com/docs/api-reference)
- [C++23 Coroutines](https://en.cppreference.com/w/cpp/language/coroutines)
- [C++23 Modules](https://en.cppreference.com/w/cpp/language/modules)
- [Asio Documentation](https://think-async.com/Asio/)

### Build Tools
- [CMake Modules Guide](https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html)

## ⭐ Acknowledgments

- [Asio](https://github.com/banderzhm/asio) - Asynchronous I/O library (C++ modules branch)
- [fmt](https://github.com/fmtlib/fmt) - Formatting library
- [nlohmann/json](https://github.com/nlohmann/json) - JSON library
