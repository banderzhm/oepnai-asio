# OpenAI Asio - Modern C++23 OpenAI API Client

[![MSVC Build](https://github.com/banderzhm/oepnai-asio/actions/workflows/msvc.yml/badge.svg)](https://github.com/banderzhm/oepnai-asio/actions/workflows/msvc.yml)
[![Clang Build](https://github.com/banderzhm/oepnai-asio/actions/workflows/clang.yml/badge.svg)](https://github.com/banderzhm/oepnai-asio/actions/workflows/clang.yml)

一个使用 C++23 协程和 Asio 实现的现代化 OpenAI API 客户端库。完全采用 C++23 模块系统，提供类型安全、高性能的异步 API 调用能力。

[English](README.md) | 简体中文

## ✨ 核心特性

- ✅ **C++23 协程** - 使用 `co_await` 进行优雅的异步编程
- ✅ **模块化架构** - 完全使用 C++23 模块，零头文件依赖
- ✅ **Asio 驱动** - 基于 Asio 的高性能异步 I/O
- ✅ **HTTPS 原生支持** - 内置 SSL/TLS 加密通信
- ✅ **类型安全** - 强类型的请求和响应结构
- ✅ **简洁 API** - 单一导入点，易于使用

## 📦 环境要求

- **编译器**
  - MSVC 14.44+ (Visual Studio 2022 17.10+)
  - Clang 18+ with libc++
  - GCC 14+ (实验性支持)
  
- **CMake** 4.0+
- **OpenSSL** (for HTTPS support)

## 🚀 快速开始

### 构建项目

#### Windows (PowerShell)

```powershell
# 1. 克隆仓库
git clone <your-repo-url>
cd openai-asio

# 2. 安装 OpenSSL (如果未安装)
# 下载: https://slproweb.com/products/Win32OpenSSL.html

# 3. 构建
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

#### Linux (Clang with libc++)

```bash
# 1. 安装依赖
sudo apt install clang-18 libc++-18-dev libssl-dev cmake

# 2. 克隆仓库
git clone <your-repo-url>
cd openai-asio

# 3. 构建
mkdir build && cd build
cmake .. -DCMAKE_CXX_COMPILER=clang++-18
cmake --build . -j$(nproc)
```

## 💡 使用示例

本项目提供了丰富的示例程序，所有示例位于 [`example/`](example/) 目录：

| 示例 | 说明 | 源文件 |
|------|------|--------|
| 00 | 功能展示 | [`00-showcase.cpp`](example/00-showcase.cpp) |
| 01 | 基础聊天补全 | [`01-chat.cpp`](example/01-chat.cpp) |
| 02 | 多轮对话 | [`02-chat-conversation.cpp`](example/02-chat-conversation.cpp) |
| 03 | 系统角色设定 | [`03-chat-system-role.cpp`](example/03-chat-system-role.cpp) |
| 04 | HTTP 客户端测试 | [`04-http-test.cpp`](example/04-http-test.cpp) |
| 05 | 模型列表 | [`05-model.cpp`](example/05-model.cpp) |
| 06 | 文本补全 | [`06-completion.cpp`](example/06-completion.cpp) |
| 07 | 文本编辑 | [`07-edit.cpp`](example/07-edit.cpp) |
| 08 | 图像生成 | [`08-image.cpp`](example/08-image.cpp) |
| 09 | 文本嵌入 | [`09-embedding.cpp`](example/09-embedding.cpp) |
| 10 | 文件管理 | [`10-file.cpp`](example/10-file.cpp) |
| 11 | 微调模型 | [`11-fine-tune.cpp`](example/11-fine-tune.cpp) |
| 12 | 音频转录 | [`12-audio.cpp`](example/12-audio.cpp) |
| 13 | 内容审核 | [`13-moderation.cpp`](example/13-moderation.cpp) |

### 快速示例

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

更多详细示例请查看 [`example/`](example/) 目录中的源文件。

## 🏗️ 项目结构

```
openai-asio/
├── src/
│   ├── openai.cppm                 # 主模块（单一导入点）
│   ├── openai-types.cppm           # 类型定义模块
│   ├── openai-http_client.cppm/.cpp # HTTP 客户端模块
│   ├── openai-client.cppm/.cpp     # OpenAI API 客户端模块
│   └── message/                    # 类型定义子模块
│       ├── chat.cppm               # 聊天相关类型
│       ├── completion.cppm         # 补全相关类型
│       ├── model.cppm              # 模型相关类型
│       ├── image.cppm              # 图像相关类型
│       ├── embedding.cppm          # 嵌入相关类型
│       ├── file.cppm               # 文件相关类型
│       ├── fine_tuning.cppm        # 微调相关类型
│       ├── audio.cppm              # 音频相关类型
│       ├── moderation.cppm         # 审核相关类型
│       └── common.cppm             # 公共工具函数
├── example/                        # 示例程序
├── 3rdparty/                       # 第三方库
│   ├── asio/                       # Asio 异步 I/O
│   ├── fmt/                        # 格式化库
│   └── nlohmann_json/              # JSON 库
└── CMakeLists.txt                  # CMake 配置
```

## 🔧 模块架构

```
import openai;  ← 单一导入点
    ↓
openai (主模块)
    ├─→ openai.http_client   (HTTP/HTTPS 客户端)
    ├─→ openai.types         (类型定义)
    │    ├─→ openai.types.chat
    │    ├─→ openai.types.completion
    │    ├─→ openai.types.model
    │    ├─→ openai.types.image
    │    ├─→ openai.types.embedding
    │    ├─→ openai.types.file
    │    ├─→ openai.types.fine_tuning
    │    ├─→ openai.types.audio
    │    ├─→ openai.types.moderation
    │    └─→ openai.types.common
    └─→ openai.client        (API 客户端)
```

## 📚 API 支持

| API | 状态 | 说明 |
|-----|------|------|
| Chat Completions | ✅ | 聊天补全 (GPT-3.5/GPT-4) |
| Completions | ✅ | 文本补全 (legacy) |
| Edits | ✅ | 文本编辑 (已弃用) |
| Models | ✅ | 模型列表和信息 |
| Images | ✅ | 图像生成 (DALL-E) |
| Embeddings | ✅ | 文本嵌入 |
| Files | ✅ | 文件管理 |
| Fine-tuning | ✅ | 模型微调 |
| Audio | ✅ | 音频转录/翻译 |
| Moderations | ✅ | 内容审核 |

## 🔧 故障排除

### Windows: 找不到 OpenSSL

```bash
cmake .. -DOPENSSL_ROOT_DIR="C:/Program Files/OpenSSL-Win64"
```

### Linux: 找不到 libc++ 模块

```bash
cmake .. \
  -DLIBCXX_MODULE_DIRS=/usr/lib/llvm-18/share/libc++/v1 \
  -DLIBCXX_INCLUDE_DIRS=/usr/lib/llvm-18/include/c++/v1
```

### 编译错误: 不支持模块

确保使用支持 C++23 模块的编译器和 CMake 版本：
- MSVC: Visual Studio 2022 17.10+
- Clang: 18.0+
- CMake: 4.0+

## 📄 许可证

MIT License

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

## 🔗 相关资源

- [OpenAI API 文档](https://platform.openai.com/docs/api-reference)
- [C++23 协程](https://en.cppreference.com/w/cpp/language/coroutines)
- [Asio 文档](https://think-async.com/Asio/)
- [C++23 模块](https://en.cppreference.com/w/cpp/language/modules)

## ⭐ 致谢

- [Asio](https://github.com/banderzhm/asio) - 异步 I/O 库（C++ 模块分支）
- [fmt](https://github.com/fmtlib/fmt) - 格式化库
- [nlohmann/json](https://github.com/nlohmann/json) - JSON 库
