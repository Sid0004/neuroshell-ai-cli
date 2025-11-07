#pragma once

#include "common/types.h"
#include <string>
#include <map>
#include <vector>

namespace NeuroShell {

// Simple rule-based AI for common commands (no network needed)
class SimpleAI {
public:
    SimpleAI();
    
    // Translate natural language to command
    std::string TranslateToCommand(const std::string& input);
    
    // Check if this looks like a natural language query
    bool IsNaturalLanguage(const std::string& input);
    
private:
    std::map<std::string, std::string> patterns_;
    
    void InitializePatterns();
    std::string MatchPattern(const std::string& input);
    std::vector<std::string> Tokenize(const std::string& input);
};

} // namespace NeuroShell
