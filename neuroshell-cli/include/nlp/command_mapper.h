#ifndef NEUROSHELL_COMMAND_MAPPER_H
#define NEUROSHELL_COMMAND_MAPPER_H

#include "nlp/parser.h"
#include <string>
#include <vector>
#include <map>

namespace neuroshell {
namespace nlp {

/**
 * @brief Represents a mapped CLI command
 */
struct MappedCommand {
    std::string command;              // Actual CLI command to execute
    std::vector<std::string> args;    // Command arguments
    bool requires_confirmation;       // Whether to ask for confirmation
    bool is_dangerous;                // Whether command is potentially dangerous
    std::string description;          // Human-readable description
    float confidence;                 // Mapping confidence score
};

/**
 * @brief Maps parsed intents to actual CLI commands
 */
class CommandMapper {
public:
    CommandMapper();
    ~CommandMapper();

    /**
     * @brief Map parsed intent to CLI command
     * @param intent Parsed intent from Parser
     * @return MappedCommand structure with CLI command details
     */
    MappedCommand mapToCommand(const ParsedIntent& intent);

    /**
     * @brief Load command mappings from JSON configuration
     * @param config_path Path to configuration file
     * @return True if loaded successfully
     */
    bool loadMappings(const std::string& config_path);

    /**
     * @brief Get all available command patterns
     * @return Vector of command pattern descriptions
     */
    std::vector<std::string> getAvailablePatterns() const;

private:
    // Command templates for different platforms
    struct CommandTemplate {
        std::string windows_cmd;
        std::string linux_cmd;
        std::string macos_cmd;
        bool requires_confirmation;
        bool is_dangerous;
        std::vector<std::string> required_params;
    };

    // Map of action+target combinations to command templates
    std::map<std::string, CommandTemplate> command_templates_;

    /**
     * @brief Initialize default command mappings
     */
    void initializeDefaultMappings();

    /**
     * @brief Get command for current platform
     * @param tmpl Command template
     * @return Platform-specific command
     */
    std::string getPlatformCommand(const CommandTemplate& tmpl) const;

    /**
     * @brief Replace parameters in command template
     * @param cmd_template Template string with placeholders
     * @param params Parameter map
     * @return Command with parameters filled in
     */
    std::string replaceParameters(const std::string& cmd_template,
                                   const std::map<std::string, std::string>& params) const;

    /**
     * @brief Detect current operating system
     * @return Platform identifier (windows, linux, macos)
     */
    std::string detectPlatform() const;

    /**
     * @brief Create composite key from action and target
     */
    std::string createKey(const std::string& action, const std::string& target) const;
};

} // namespace nlp
} // namespace neuroshell

#endif // NEUROSHELL_COMMAND_MAPPER_H
