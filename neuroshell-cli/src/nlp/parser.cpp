#include "nlp/parser.h"
#include <algorithm>
#include <sstream>
#include <cctype>

namespace neuroshell {
namespace nlp {

Parser::Parser() {
    initializeActionMappings();
    initializeTargetMappings();
    initializeStopWords();
}

Parser::~Parser() {}

ParsedIntent Parser::parse(const std::string& input) {
    ParsedIntent intent;
    intent.original_input = input;
    
    // Tokenize input
    auto tokens = tokenize(input);
    
    // Extract components
    intent.action = extractAction(tokens);
    intent.target = extractTarget(tokens);
    intent.parameters = extractParameters(tokens);
    
    // Calculate confidence
    intent.confidence = calculateConfidence(intent);
    
    return intent;
}

std::vector<Token> Parser::tokenize(const std::string& input) {
    std::vector<Token> tokens;
    std::istringstream iss(toLower(input));
    std::string word;
    
    while (iss >> word) {
        // Remove punctuation
        word.erase(std::remove_if(word.begin(), word.end(), ::ispunct), word.end());
        
        if (!word.empty()) {
            Token token;
            token.word = word;
            token.is_stop_word = isStopWord(word);
            // Simple POS tagging would go here
            token.pos_tag = "UNKNOWN";
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

std::string Parser::extractAction(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        if (token.is_stop_word) continue;
        
        // Check if word maps to an action
        auto it = action_mappings_.find(token.word);
        if (it != action_mappings_.end()) {
            return it->second;
        }
    }
    
    return "unknown";
}

std::string Parser::extractTarget(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        if (token.is_stop_word) continue;
        
        // Check if word maps to a target
        auto it = target_mappings_.find(token.word);
        if (it != target_mappings_.end()) {
            return it->second;
        }
    }
    
    return "unknown";
}

std::map<std::string, std::string> Parser::extractParameters(const std::vector<Token>& tokens) {
    std::map<std::string, std::string> params;
    
    // Extract file extensions
    for (const auto& token : tokens) {
        if (token.word.find(".") != std::string::npos) {
            params["extension"] = token.word;
        }
        
        // Extract paths
        if (token.word.find("/") != std::string::npos || 
            token.word.find("\\") != std::string::npos) {
            params["path"] = token.word;
        }
        
        // Extract names (words after certain keywords)
        if (token.word == "called" || token.word == "named") {
            // Next token would be the name
            params["name_follows"] = "true";
        }
    }
    
    // Look for "all" keyword for recursive operations
    for (const auto& token : tokens) {
        if (token.word == "all" || token.word == "recursive") {
            params["recursive"] = "true";
        }
    }
    
    return params;
}

void Parser::initializeActionMappings() {
    // List/Show actions
    action_mappings_["list"] = "list";
    action_mappings_["show"] = "list";
    action_mappings_["display"] = "list";
    action_mappings_["find"] = "list";
    action_mappings_["search"] = "list";
    action_mappings_["get"] = "list";
    
    // Create actions
    action_mappings_["create"] = "create";
    action_mappings_["make"] = "create";
    action_mappings_["add"] = "create";
    action_mappings_["new"] = "create";
    
    // Delete actions
    action_mappings_["delete"] = "delete";
    action_mappings_["remove"] = "delete";
    action_mappings_["erase"] = "delete";
    action_mappings_["destroy"] = "delete";
    
    // Copy actions
    action_mappings_["copy"] = "copy";
    action_mappings_["duplicate"] = "copy";
    
    // Move actions
    action_mappings_["move"] = "move";
    action_mappings_["rename"] = "move";
    
    // Edit actions
    action_mappings_["edit"] = "edit";
    action_mappings_["modify"] = "edit";
    action_mappings_["change"] = "edit";
    
    // Run actions
    action_mappings_["run"] = "execute";
    action_mappings_["execute"] = "execute";
    action_mappings_["start"] = "execute";
    action_mappings_["launch"] = "execute";
    
    // Stop actions
    action_mappings_["stop"] = "stop";
    action_mappings_["kill"] = "stop";
    action_mappings_["terminate"] = "stop";
}

void Parser::initializeTargetMappings() {
    // File targets
    target_mappings_["file"] = "file";
    target_mappings_["files"] = "file";
    target_mappings_["document"] = "file";
    target_mappings_["documents"] = "file";
    
    // Folder targets
    target_mappings_["folder"] = "folder";
    target_mappings_["folders"] = "folder";
    target_mappings_["directory"] = "folder";
    target_mappings_["directories"] = "folder";
    target_mappings_["dir"] = "folder";
    
    // Process targets
    target_mappings_["process"] = "process";
    target_mappings_["processes"] = "process";
    target_mappings_["program"] = "process";
    target_mappings_["application"] = "process";
    target_mappings_["app"] = "process";
    
    // Network targets
    target_mappings_["network"] = "network";
    target_mappings_["ip"] = "network";
    target_mappings_["address"] = "network";
    target_mappings_["connection"] = "network";
    
    // System targets
    target_mappings_["system"] = "system";
    target_mappings_["info"] = "system";
    target_mappings_["information"] = "system";
}

void Parser::initializeStopWords() {
    stop_words_ = {
        "a", "an", "the", "in", "on", "at", "to", "for", "of", "with",
        "from", "by", "about", "as", "into", "through", "during", "before",
        "after", "above", "below", "up", "down", "out", "off", "over",
        "under", "again", "further", "then", "once", "here", "there",
        "when", "where", "why", "how", "all", "both", "each", "few",
        "more", "most", "other", "some", "such", "no", "nor", "not",
        "only", "own", "same", "so", "than", "too", "very", "can",
        "will", "just", "should", "now", "me", "my", "this", "that"
    };
}

bool Parser::isStopWord(const std::string& word) const {
    return std::find(stop_words_.begin(), stop_words_.end(), word) != stop_words_.end();
}

std::string Parser::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

float Parser::calculateConfidence(const ParsedIntent& intent) const {
    float confidence = 0.0f;
    
    // Base confidence on what we could extract
    if (intent.action != "unknown") confidence += 0.5f;
    if (intent.target != "unknown") confidence += 0.3f;
    if (!intent.parameters.empty()) confidence += 0.2f;
    
    return confidence;
}

} // namespace nlp
} // namespace neuroshell
