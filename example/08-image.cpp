// Example 08: Image Generation API
// Generate, edit, and create variations of images

import asio;
import fmt;
import openai;
import std;

asio::awaitable<void> image_example(openai::Client& client) {
    fmt::print("=== Image Generation API Example ===\n\n");
      
    fmt::print("The Images API allows you to:\n");
    fmt::print("  1. Generate images from text prompts\n");
    fmt::print("  2. Edit existing images with prompts (requires file upload)\n");
    fmt::print("  3. Create variations of images (requires file upload)\n\n");
    
    // Example 1: Generate image with DALL-E 2
    fmt::print("--- Example 1: Generate Image (DALL-E 2) ---\n");
    {
        openai::ImageGenerationRequest req;
        req.prompt = "A cute baby sea otter floating in the ocean";
        req.model = "dall-e-2";
        req.n = 1;
        req.size = "512x512";
        req.response_format = "url";
        
        fmt::print("Prompt: {}\n", req.prompt);
        fmt::print("Model: {}\n", *req.model);
        fmt::print("Size: {}\n\n", *req.size);
        
        try {
            auto response = co_await client.generate_image(req);
            
            if (response.is_error) {
                fmt::print("Error: {}\n\n", response.error_message);
            } else {
                fmt::print("✓ Image generated successfully!\n");
                fmt::print("Number of images: {}\n", response.data.size());
                for (size_t i = 0; i < response.data.size(); ++i) {
                    fmt::print("  Image {}: {}\n", i + 1, response.data[i].url);
                }
                fmt::print("\n");
            }
        } catch (const std::exception& e) {
            fmt::print("Exception: {}\n\n", e.what());
        }
    }
    
    // Example 2: Generate image with DALL-E 3
    fmt::print("--- Example 2: Generate Image (DALL-E 3) ---\n");
    {
        openai::ImageGenerationRequest req;
        req.prompt = "A futuristic cityscape at sunset with flying cars";
        req.model = "dall-e-3";
        req.n = 1;  // DALL-E 3 only supports n=1
        req.size = "1024x1024";
        req.quality = "standard";  // or "hd"
        req.style = "vivid";  // or "natural"
        
        fmt::print("Prompt: {}\n", req.prompt);
        fmt::print("Model: {}\n", *req.model);
        fmt::print("Size: {}\n", *req.size);
        fmt::print("Quality: {}\n", *req.quality);
        fmt::print("Style: {}\n\n", *req.style);
        
        try {
            auto response = co_await client.generate_image(req);
            
            if (response.is_error) {
                fmt::print("Error: {}\n\n", response.error_message);
            } else {
                fmt::print("✓ Image generated successfully!\n");
                for (const auto& img : response.data) {
                    fmt::print("  URL: {}\n", img.url);
                    if (!img.revised_prompt.empty()) {
                        fmt::print("  Revised prompt: {}\n", img.revised_prompt);
                    }
                }
                fmt::print("\n");
            }
        } catch (const std::exception& e) {
            fmt::print("Exception: {}\n\n", e.what());
        }
    }
    
    fmt::print(R"(Supported sizes:
  DALL-E 2:
    - 256x256
    - 512x512
    - 1024x1024

  DALL-E 3:
    - 1024x1024 (square)
    - 1024x1792 (portrait)
    - 1792x1024 (landscape)

Notes:
  • DALL-E 3 only supports generating 1 image at a time (n=1)
  • DALL-E 3 may revise your prompt for safety/quality
  • Image editing and variations require file upload (multipart/form-data)
  • Generated images are temporary and should be downloaded promptly
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
    
    asio::co_spawn(io_context, image_example(client), asio::detached);
    io_context.run();
    
    return 0;
}

