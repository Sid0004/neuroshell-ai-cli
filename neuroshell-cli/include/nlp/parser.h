#ifndef NEUROSHELL_PARSER_H
#define NEUROSHELL_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace neuroshell {
namespace nlp {

/**
 * @brief Represents parsed intent from natural language input
 */
struct ParsedIntent {
    std::string action;              // Primary action (list, create, delete, etc.)
    std::string target;              // Target object (file, folder, process, etc.)
    std::map<std::string, std::string> parameters;  // Additional parameters
    float confidence;                // Confidence score (0.0 - 1.0)
    std::string original_input;      // Original natural language input
};

/**
 * @brief Token representing a word with linguistic information
 */
struct Token {
    std::string word;
    std::string pos_tag;  // Part of speech tag (VERB, NOUN, ADJ, etc.)
    bool is_stop_word;
};

/**
 * @brief NLP Parser for converting natural language to structured intent
 */
class Parser {
public:
    Parser();
    ~Parser();

    /**
     * @brief Parse natural language input into structured intent
     * @param input Natural language command string
     * @return ParsedIntent structure with extracted information
     */
    ParsedIntent parse(const std::string& input);

    /**
     * @brief Tokenize input string into words
     * @param input Input string to tokenize
     * @return Vector of tokens
     */
    std::vector<Token> tokenize(const std::string& input);

    /**
     * @brief Extract action verbs from tokens
     * @param tokens Vector of tokens
     * @return Primary action verb
     */
    std::string extractAction(const std::vector<Token>& tokens);

    /**
     * @brief Extract target object from tokens
     * @param tokens Vector of tokens
     * @return Target object
     */
    std::string extractTarget(const std::vector<Token>& tokens);

    /**
     * @brief Extract parameters from tokens
     * @param tokens Vector of tokens
     * @return Map of parameter key-value pairs
     */
    std::map<std::string, std::string> extractParameters(const std::vector<Token>& tokens);

private:
    // Action verb mappings
    std::map<std::string, std::string> action_mappings_;
    
    // Target object mappings
    std::map<std::string, std::string> target_mappings_;
    
    // Stop words to filter out
    std::vector<std::string> stop_words_;

    /**
     * @brief Initialize action verb mappings
     */
    void initializeActionMappings();

    /**
     * @brief Initialize target object mappings
     */
    void initializeTargetMappings();

    /**
     * @brief Initialize stop words list
     */
    void initializeStopWords();

    /**
     * @brief Check if word is a stop word
     */
    bool isStopWord(const std::string& word) const;

    /**
     * @brief Convert string to lowercase
     */
    std::string toLower(const std::string& str) const;

    /**
     * @brief Calculate confidence score for parsed intent
     */
    float calculateConfidence(const ParsedIntent& intent) const;
};

} // namespace nlp
} // namespace neuroshell

#endif // NEUROSHELL_PARSER_H
