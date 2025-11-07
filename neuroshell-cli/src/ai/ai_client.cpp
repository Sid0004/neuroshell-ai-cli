#include "ai/ai_client.h"
#include <regex>
#include <sstream>
#include <thread>

// For JSON parsing - using simple string manipulation for now
// In production, use nlohmann/json library

namespace NeuroShell {

AIClient::AIClient()
    : httpClient_(nullptr)
{
}

AIClient::~AIClient() = default;

void AIClient::Initialize(const AIConfig& config) {
    config_ = config;
    httpClient_ = std::make_unique<HttpClient>();
    httpClient_->SetTimeout(60); // 60 seconds for AI requests
}

bool AIClient::IsAvailable() const {
    return !config_.apiKey.empty() && config_.provider != AIProvider::None;
}

void AIClient::UpdateConfig(const AIConfig& config) {
    config_ = config;
}

AIResponse AIClient::TranslateToCommand(const std::string& naturalLanguage) {
    if (!IsAvailable()) {
        AIResponse response;
        response.error = "AI client not configured";
        return response;
    }
    
    std::string prompt = GetCommandTranslationPrompt(naturalLanguage);
    
    AIResponse response;
    switch (config_.provider) {
        case AIProvider::OpenAI:
            response = CallOpenAI(prompt);
            break;
        case AIProvider::Groq:
            response = CallGroq(prompt);
            break;
        case AIProvider::Gemini:
            response = CallGemini(prompt);
            break;
        case AIProvider::Ollama:
            response = CallOllama(prompt);
            break;
        default:
            response.error = "Unknown AI provider";
            return response;
    }
    
    if (response.success) {
        response.commands = ExtractCommands(response.text);
    }
    
    return response;
}

void AIClient::TranslateToCommandAsync(const std::string& naturalLanguage,
                                       std::function<void(const AIResponse&)> callback) {
    std::thread([this, naturalLanguage, callback]() {
        AIResponse response = TranslateToCommand(naturalLanguage);
        callback(response);
    }).detach();
}

AIResponse AIClient::ExplainCommand(const std::string& shellCommand) {
    if (!IsAvailable()) {
        AIResponse response;
        response.error = "AI client not configured";
        return response;
    }
    
    std::string prompt = "Explain this shell command in simple terms:\n\n" + shellCommand;
    
    switch (config_.provider) {
        case AIProvider::OpenAI:
            return CallOpenAI(prompt);
        case AIProvider::Groq:
            return CallGroq(prompt);
        case AIProvider::Gemini:
            return CallGemini(prompt);
        case AIProvider::Ollama:
            return CallOllama(prompt);
        default:
            AIResponse response;
            response.error = "Unknown AI provider";
            return response;
    }
}

AIResponse AIClient::Chat(const std::string& message, const std::vector<std::string>& context) {
    if (!IsAvailable()) {
        AIResponse response;
        response.error = "AI client not configured";
        return response;
    }
    
    std::string prompt = message;
    if (!context.empty()) {
        prompt = "Context:\n";
        for (const auto& ctx : context) {
            prompt += ctx + "\n";
        }
        prompt += "\nUser: " + message;
    }
    
    switch (config_.provider) {
        case AIProvider::OpenAI:
            return CallOpenAI(prompt);
        case AIProvider::Groq:
            return CallGroq(prompt);
        case AIProvider::Gemini:
            return CallGemini(prompt);
        case AIProvider::Ollama:
            return CallOllama(prompt);
        default:
            AIResponse response;
            response.error = "Unknown AI provider";
            return response;
    }
}

std::string AIClient::GetCommandTranslationPrompt(const std::string& nlp) {
    std::ostringstream oss;
    oss << "You are a shell command translator. Convert the following natural language request into shell commands.\n\n";
    oss << "Rules:\n";
    oss << "1. Output ONLY the shell commands, nothing else\n";
    oss << "2. Use Windows commands (cmd/PowerShell) if applicable\n";
    oss << "3. If multiple commands needed, separate with newlines\n";
    oss << "4. Make commands safe - avoid destructive operations without confirmation\n";
    oss << "5. Use cross-platform commands when possible\n\n";
    oss << "User request: " << nlp << "\n\n";
    oss << "Commands:";
    
    return oss.str();
}

// OpenAI API Implementation
std::string AIClient::BuildOpenAIRequest(const std::string& prompt) {
    std::ostringstream json;
    json << "{"
         << "\"model\":\"" << config_.model << "\","
         << "\"messages\":[{\"role\":\"user\",\"content\":\"" << prompt << "\"}],"
         << "\"temperature\":" << config_.temperature << ","
         << "\"max_tokens\":" << config_.maxTokens
         << "}";
    return json.str();
}

AIResponse AIClient::CallOpenAI(const std::string& prompt) {
    std::string url = config_.endpoint.empty() ? 
        "https://api.openai.com/v1/chat/completions" : config_.endpoint;
    
    std::string requestBody = BuildOpenAIRequest(prompt);
    
    std::map<std::string, std::string> headers;
    headers["Authorization"] = "Bearer " + config_.apiKey;
    
    HttpResponse httpResp = httpClient_->Post(url, requestBody, headers);
    
    if (!httpResp.success) {
        AIResponse response;
        response.error = "HTTP Error: " + httpResp.error;
        return response;
    }
    
    return ParseOpenAIResponse(httpResp.body);
}

AIResponse AIClient::ParseOpenAIResponse(const std::string& json) {
    AIResponse response;
    
    // Simple JSON parsing - find content field
    size_t contentPos = json.find("\"content\"");
    if (contentPos == std::string::npos) {
        response.error = "Invalid response format";
        return response;
    }
    
    size_t start = json.find("\"", contentPos + 10);
    size_t end = json.find("\"", start + 1);
    
    if (start != std::string::npos && end != std::string::npos) {
        response.text = json.substr(start + 1, end - start - 1);
        response.success = true;
    } else {
        response.error = "Failed to parse response";
    }
    
    return response;
}

// Groq API Implementation (similar to OpenAI)
std::string AIClient::BuildGroqRequest(const std::string& prompt) {
    return BuildOpenAIRequest(prompt); // Groq uses OpenAI-compatible API
}

AIResponse AIClient::CallGroq(const std::string& prompt) {
    std::string url = config_.endpoint.empty() ? 
        "https://api.groq.com/openai/v1/chat/completions" : config_.endpoint;
    
    std::string requestBody = BuildGroqRequest(prompt);
    
    std::map<std::string, std::string> headers;
    headers["Authorization"] = "Bearer " + config_.apiKey;
    
    HttpResponse httpResp = httpClient_->Post(url, requestBody, headers);
    
    if (!httpResp.success) {
        AIResponse response;
        response.error = "HTTP Error: " + httpResp.error;
        return response;
    }
    
    return ParseGroqResponse(httpResp.body);
}

AIResponse AIClient::ParseGroqResponse(const std::string& json) {
    return ParseOpenAIResponse(json); // Same format as OpenAI
}

// Gemini API Implementation
std::string AIClient::BuildGeminiRequest(const std::string& prompt) {
    std::ostringstream json;
    json << "{"
         << "\"contents\":[{\"parts\":[{\"text\":\"" << prompt << "\"}]}],"
         << "\"generationConfig\":{"
         << "\"temperature\":" << config_.temperature << ","
         << "\"maxOutputTokens\":" << config_.maxTokens
         << "}"
         << "}";
    return json.str();
}

AIResponse AIClient::CallGemini(const std::string& prompt) {
    std::string url = config_.endpoint.empty() ? 
        "https://generativelanguage.googleapis.com/v1/models/" + config_.model + ":generateContent?key=" + config_.apiKey
        : config_.endpoint + "?key=" + config_.apiKey;
    
    std::string requestBody = BuildGeminiRequest(prompt);
    
    HttpResponse httpResp = httpClient_->Post(url, requestBody);
    
    if (!httpResp.success) {
        AIResponse response;
        response.error = "HTTP Error: " + httpResp.error;
        return response;
    }
    
    return ParseGeminiResponse(httpResp.body);
}

AIResponse AIClient::ParseGeminiResponse(const std::string& json) {
    AIResponse response;
    
    // Find text field in Gemini response
    size_t textPos = json.find("\"text\"");
    if (textPos == std::string::npos) {
        response.error = "Invalid response format";
        return response;
    }
    
    size_t start = json.find("\"", textPos + 7);
    size_t end = json.find("\"", start + 1);
    
    if (start != std::string::npos && end != std::string::npos) {
        response.text = json.substr(start + 1, end - start - 1);
        response.success = true;
    } else {
        response.error = "Failed to parse response";
    }
    
    return response;
}

// Ollama API Implementation (local LLM)
std::string AIClient::BuildOllamaRequest(const std::string& prompt) {
    std::ostringstream json;
    json << "{"
         << "\"model\":\"" << config_.model << "\","
         << "\"prompt\":\"" << prompt << "\","
         << "\"stream\":false"
         << "}";
    return json.str();
}

AIResponse AIClient::CallOllama(const std::string& prompt) {
    std::string url = config_.endpoint.empty() ? 
        "http://localhost:11434/api/generate" : config_.endpoint;
    
    std::string requestBody = BuildOllamaRequest(prompt);
    
    HttpResponse httpResp = httpClient_->Post(url, requestBody);
    
    if (!httpResp.success) {
        AIResponse response;
        response.error = "HTTP Error: " + httpResp.error + " (Is Ollama running?)";
        return response;
    }
    
    return ParseOllamaResponse(httpResp.body);
}

AIResponse AIClient::ParseOllamaResponse(const std::string& json) {
    AIResponse response;
    
    // Find response field
    size_t responsePos = json.find("\"response\"");
    if (responsePos == std::string::npos) {
        response.error = "Invalid response format";
        return response;
    }
    
    size_t start = json.find("\"", responsePos + 11);
    size_t end = json.find("\"", start + 1);
    
    if (start != std::string::npos && end != std::string::npos) {
        response.text = json.substr(start + 1, end - start - 1);
        response.success = true;
    } else {
        response.error = "Failed to parse response";
    }
    
    return response;
}

// Extract shell commands from AI response
std::vector<std::string> AIClient::ExtractCommands(const std::string& text) {
    std::vector<std::string> commands;
    
    // Split by newlines
    std::istringstream stream(text);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Remove leading/trailing whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");
        
        if (start != std::string::npos && end != std::string::npos) {
            line = line.substr(start, end - start + 1);
            
            // Skip empty lines and comments
            if (!line.empty() && line[0] != '#' && line[0] != '/') {
                // Remove code block markers if present
                if (line.find("```") == std::string::npos && 
                    line.find("$ ") != 0) {
                    commands.push_back(line);
                }
            }
        }
    }
    
    return commands;
}

} // namespace NeuroShell
