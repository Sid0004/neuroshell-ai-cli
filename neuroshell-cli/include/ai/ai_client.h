#pragma once

#include "common/types.h"
#include "ai/http_client.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace NeuroShell {

struct AIResponse {
    std::string text;              // AI's response text
    std::vector<std::string> commands;  // Extracted shell commands
    bool success;
    std::string error;
    
    AIResponse() : success(false) {}
};

class AIClient {
public:
    AIClient();
    ~AIClient();
    
    // Initialize with configuration
    void Initialize(const AIConfig& config);
    
    // Convert natural language to shell command
    AIResponse TranslateToCommand(const std::string& naturalLanguage);
    
    // Async version with callback
    void TranslateToCommandAsync(const std::string& naturalLanguage,
                                 std::function<void(const AIResponse&)> callback);
    
    // Get AI explanation for a command
    AIResponse ExplainCommand(const std::string& shellCommand);
    
    // Chat with AI assistant
    AIResponse Chat(const std::string& message, const std::vector<std::string>& context = {});
    
    // Check if AI is configured and available
    bool IsAvailable() const;
    
    // Get current configuration
    const AIConfig& GetConfig() const { return config_; }
    
    // Update configuration
    void UpdateConfig(const AIConfig& config);
    
private:
    AIConfig config_;
    std::unique_ptr<HttpClient> httpClient_;
    
    // Provider-specific implementations
    AIResponse CallOpenAI(const std::string& prompt);
    AIResponse CallGroq(const std::string& prompt);
    AIResponse CallGemini(const std::string& prompt);
    AIResponse CallOllama(const std::string& prompt);
    
    // Build request JSON for each provider
    std::string BuildOpenAIRequest(const std::string& prompt);
    std::string BuildGroqRequest(const std::string& prompt);
    std::string BuildGeminiRequest(const std::string& prompt);
    std::string BuildOllamaRequest(const std::string& prompt);
    
    // Parse responses
    AIResponse ParseOpenAIResponse(const std::string& json);
    AIResponse ParseGroqResponse(const std::string& json);
    AIResponse ParseGeminiResponse(const std::string& json);
    AIResponse ParseOllamaResponse(const std::string& json);
    
    // Extract shell commands from AI response
    std::vector<std::string> ExtractCommands(const std::string& text);
    
    // System prompt for command translation
    std::string GetCommandTranslationPrompt(const std::string& nlp);
};

} // namespace NeuroShell
