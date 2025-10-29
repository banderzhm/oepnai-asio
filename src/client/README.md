# OpenAI Client Modular Architecture

This directory contains the modular implementation of the OpenAI API client, organized by API domains.

## Architecture Overview

The client uses a **composition pattern** where each API domain has its own specialized client class that inherits from `BaseClient`. The main `Client` class composes all these specialized clients.

```
┌─────────────────────────────┐
│   openai::Client            │  ← Main unified client
│  (src/client/unified_client)│
└──────────┬──────────────────┘
           │ Composes
           ├─────────────────────────────┐
           │                             │
   ┌───────▼────────┐          ┌────────▼────────┐
   │  ModelClient   │          │   ChatClient    │
   │                │          │                 │
   └───────┬────────┘          └────────┬────────┘
           │ Inherits                   │ Inherits
           │                             │
           └──────────┬──────────────────┘
                      │
            ┌─────────▼─────────┐
            │    BaseClient     │  ← Provides HTTP + Auth
            │                   │
            └───────────────────┘
```

## Module Structure

### Core Modules

| Module | File | Description |
|--------|------|-------------|
| `openai.client.base` | `base_client.cppm` | Base client with HTTP and authentication |
| `openai.client.unified` | `unified_client.cppm` | Main client composing all sub-clients |

### API-Specific Clients (Implemented)

| Module | File | Description | Status |
|--------|------|-------------|--------|
| `openai.client.model` | `model_client.cppm` | Models API | ✅ Complete |
| `openai.client.chat` | `chat_client.cppm` | Chat Completions API | ✅ Complete |
| `openai.client.image` | `image_client.cppm` | Images API | ⚠️ Partial |

### API-Specific Clients (To Be Created)

| Module | File | Description | Priority |
|--------|------|-------------|----------|
| `openai.client.completion` | `completion_client.cppm` | Text Completions (Legacy) | Medium |
| `openai.client.embedding` | `embedding_client.cppm` | Text Embeddings | High |
| `openai.client.file` | `file_client.cppm` | File Management | High |
| `openai.client.fine_tuning` | `fine_tuning_client.cppm` | Model Fine-tuning | Medium |
| `openai.client.audio` | `audio_client.cppm` | Audio Transcription/Translation | Medium |
| `openai.client.moderation` | `moderation_client.cppm` | Content Moderation | Low |
| `openai.client.assistant` | `assistant_client.cppm` | Assistants API (Beta) | Low |
| `openai.client.thread` | `thread_client.cppm` | Threads API (Beta) | Low |
| `openai.client.run` | `run_client.cppm` | Runs API (Beta) | Low |

## Design Benefits

### 1. **Separation of Concerns**
Each API domain is isolated in its own module, making the codebase easier to understand and maintain.

### 2. **Easier Testing**
Individual clients can be tested independently without needing the full client.

### 3. **Better Compilation Times**
Changes to one API client don't require recompiling unrelated APIs due to module boundaries.

### 4. **Parallel Development**
Multiple developers can work on different API clients simultaneously without conflicts.

### 5. **Clear Responsibilities**
- `BaseClient`: HTTP communication and authentication
- Specialized clients: API-specific logic and parsing
- `Client`: Composition and unified interface

## Usage Example

```cpp
import openai;
import std;
import asio;

int main() {
    asio::io_context io_context;
    openai::Client client("your-api-key", io_context);
    
    // All APIs available through the unified client
    auto models = client.list_models();
    auto chat_response = client.create_chat_completion(request);
    auto image = client.generate_image(img_request);
}
```

## Implementation Guidelines

### Creating a New API Client

1. **Create module file**: `src/client/{api}_client.cppm`

2. **Define client class**:
```cpp
export module openai.client.{api};

import openai.client.base;
import openai.types.{api};

export namespace openai::client {
    class {Api}Client : public BaseClient {
    public:
        using BaseClient::BaseClient;
        
        // API methods here
        
    private:
        // Parser methods here
    };
}
```

3. **Add to unified client**:
```cpp
// In unified_client.cppm:
import openai.client.{api};

class Client {
private:
    client::{Api}Client {api}_client_;
};
```

4. **Delegate methods**:
```cpp
asio::awaitable<Response> method() {
    co_return co_await {api}_client_.method();
}
```

## Migration Plan

### Phase 1: Core APIs (Current)
- ✅ BaseClient
- ✅ ModelClient  
- ✅ ChatClient
- ✅ ImageClient (partial)

### Phase 2: Essential APIs (Next)
- [ ] EmbeddingClient
- [ ] FileClient
- [ ] ModerationClient

### Phase 3: Advanced APIs
- [ ] CompletionClient
- [ ] FineTuningClient
- [ ] AudioClient

### Phase 4: Beta APIs
- [ ] AssistantClient
- [ ] ThreadClient
- [ ] RunClient

## Contributing

When adding a new API client:

1. Follow the existing pattern (inherit from `BaseClient`)
2. Keep parsing logic private
3. Use `co_await` for async operations
4. Add comprehensive error handling
5. Update this README

## Notes

- The old `openai-client.cppm` is deprecated
- All new development should use the modular architecture
- The `openai-client.cpp` file will be split corresponding to each client module

