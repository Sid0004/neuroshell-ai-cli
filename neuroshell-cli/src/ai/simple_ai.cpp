#include "ai/simple_ai.h"
#include <algorithm>
#include <sstream>
#include <cctype>

namespace NeuroShell {

SimpleAI::SimpleAI() {
    InitializePatterns();
}

void SimpleAI::InitializePatterns() {
    // File operations
    patterns_["list files"] = "dir";
    patterns_["show files"] = "dir";
    patterns_["list directory"] = "dir";
    patterns_["show directory"] = "dir";
    patterns_["list folder"] = "dir";
    patterns_["show folder"] = "dir";
    
    // Navigation
    patterns_["go back"] = "cd ..";
    patterns_["go up"] = "cd ..";
    patterns_["back directory"] = "cd ..";
    patterns_["parent folder"] = "cd ..";
    
    // System info
    patterns_["show system info"] = "systeminfo";
    patterns_["system information"] = "systeminfo";
    patterns_["computer info"] = "systeminfo";
    patterns_["show ip"] = "ipconfig";
    patterns_["ip address"] = "ipconfig";
    patterns_["network info"] = "ipconfig /all";
    
    // Process management
    patterns_["list processes"] = "tasklist";
    patterns_["show processes"] = "tasklist";
    patterns_["running programs"] = "tasklist";
    patterns_["show tasks"] = "tasklist";
    
    // Disk operations
    patterns_["show drives"] = "wmic logicaldisk get name,size,freespace";
    patterns_["list drives"] = "wmic logicaldisk get name,size,freespace";
    patterns_["disk space"] = "wmic logicaldisk get name,size,freespace";
    
    // Clear screen
    patterns_["clear screen"] = "cls";
    patterns_["clear terminal"] = "cls";
    patterns_["clean screen"] = "cls";
    
    // User info
    patterns_["who am i"] = "whoami";
    patterns_["current user"] = "whoami";
    patterns_["show user"] = "whoami";
    
    // Time/Date
    patterns_["show time"] = "time /t";
    patterns_["current time"] = "time /t";
    patterns_["show date"] = "date /t";
    patterns_["current date"] = "date /t";
    
    // Environment
    patterns_["show environment"] = "set";
    patterns_["environment variables"] = "set";
    patterns_["show path"] = "echo %PATH%";
}

std::string SimpleAI::TranslateToCommand(const std::string& input) {
    // Convert to lowercase for matching
    std::string lower = input;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    // Remove extra whitespace
    std::stringstream ss(lower);
    std::string cleaned;
    std::string word;
    while (ss >> word) {
        if (!cleaned.empty()) cleaned += " ";
        cleaned += word;
    }
    
    // Direct pattern match
    auto it = patterns_.find(cleaned);
    if (it != patterns_.end()) {
        return it->second;
    }
    
    // Partial matching
    std::string result = MatchPattern(cleaned);
    if (!result.empty()) {
        return result;
    }
    
    // Smart matching for common patterns
    if (cleaned.find("create") != std::string::npos && cleaned.find("folder") != std::string::npos) {
        // Extract folder name if provided
        size_t pos = cleaned.find("folder");
        if (pos + 7 < cleaned.length()) {
            std::string rest = input.substr(input.find("folder") + 7);
            std::stringstream restss(rest);
            std::string foldername;
            restss >> foldername;
            if (!foldername.empty()) {
                return "mkdir " + foldername;
            }
        }
        return "mkdir newfolder";
    }
    
    if (cleaned.find("create") != std::string::npos && cleaned.find("file") != std::string::npos) {
        size_t pos = cleaned.find("file");
        if (pos + 5 < cleaned.length()) {
            std::string rest = input.substr(input.find("file") + 5);
            std::stringstream restss(rest);
            std::string filename;
            restss >> filename;
            if (!filename.empty()) {
                return "type nul > " + filename;
            }
        }
        return "type nul > newfile.txt";
    }
    
    if (cleaned.find("delete") != std::string::npos || cleaned.find("remove") != std::string::npos) {
        std::vector<std::string> tokens = Tokenize(input);
        if (tokens.size() >= 2) {
            std::string target = tokens[tokens.size() - 1];
            return "del " + target;
        }
    }
    
    if (cleaned.find("copy") != std::string::npos) {
        std::vector<std::string> tokens = Tokenize(input);
        if (tokens.size() >= 3) {
            return "copy " + tokens[tokens.size() - 2] + " " + tokens[tokens.size() - 1];
        }
    }
    
    if (cleaned.find("move") != std::string::npos) {
        std::vector<std::string> tokens = Tokenize(input);
        if (tokens.size() >= 3) {
            return "move " + tokens[tokens.size() - 2] + " " + tokens[tokens.size() - 1];
        }
    }
    
    if (cleaned.find("go to") != std::string::npos || cleaned.find("change to") != std::string::npos) {
        std::vector<std::string> tokens = Tokenize(input);
        if (!tokens.empty()) {
            return "cd " + tokens[tokens.size() - 1];
        }
    }
    
    // Return empty if no match
    return "";
}

bool SimpleAI::IsNaturalLanguage(const std::string& input) {
    std::string lower = input;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    // Check for question words
    std::vector<std::string> questionWords = {
        "what", "how", "show", "list", "get", "find", "where",
        "which", "who", "when", "why", "create", "make", "display"
    };
    
    for (const auto& qword : questionWords) {
        if (lower.find(qword) != std::string::npos) {
            return true;
        }
    }
    
    // Check if it contains spaces (natural language tends to)
    if (lower.find(' ') != std::string::npos && lower.find('/') == std::string::npos) {
        return true;
    }
    
    return false;
}

std::string SimpleAI::MatchPattern(const std::string& input) {
    // Find best partial match
    int bestScore = 0;
    std::string bestMatch;
    
    for (const auto& pair : patterns_) {
        int score = 0;
        std::vector<std::string> patternWords = Tokenize(pair.first);
        std::vector<std::string> inputWords = Tokenize(input);
        
        for (const auto& pw : patternWords) {
            for (const auto& iw : inputWords) {
                if (pw == iw) {
                    score++;
                }
            }
        }
        
        if (score > bestScore && score >= 2) {
            bestScore = score;
            bestMatch = pair.second;
        }
    }
    
    return bestMatch;
}

std::vector<std::string> SimpleAI::Tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    
    while (ss >> token) {
        // Convert to lowercase
        std::transform(token.begin(), token.end(), token.begin(), ::tolower);
        // Remove punctuation
        token.erase(std::remove_if(token.begin(), token.end(), ::ispunct), token.end());
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

} // namespace NeuroShell
