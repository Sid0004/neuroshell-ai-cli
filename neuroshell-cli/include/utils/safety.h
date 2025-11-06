#ifndef NEUROSHELL_SAFETY_H
#define NEUROSHELL_SAFETY_H

#include <string>
#include <vector>
#include <set>

namespace neuroshell {
namespace utils {

/**
 * @brief Safety checker for command execution
 */
class SafetyChecker {
public:
    SafetyChecker();
    ~SafetyChecker();

    /**
     * @brief Check if a command is safe to execute
     * @param command Command to check
     * @return True if command is considered safe
     */
    bool isSafe(const std::string& command) const;

    /**
     * @brief Check if command is dangerous and requires confirmation
     * @param command Command to check
     * @return True if command is dangerous
     */
    bool isDangerous(const std::string& command) const;

    /**
     * @brief Check if command is in blacklist
     * @param command Command to check
     * @return True if command is blacklisted
     */
    bool isBlacklisted(const std::string& command) const;

    /**
     * @brief Add command to whitelist
     * @param command Command to whitelist
     */
    void addToWhitelist(const std::string& command);

    /**
     * @brief Add command to blacklist
     * @param command Command to blacklist
     */
    void addToBlacklist(const std::string& command);

    /**
     * @brief Check for potential command injection
     * @param input Input to check
     * @return True if potential injection detected
     */
    bool hasInjectionRisk(const std::string& input) const;

private:
    std::set<std::string> whitelisted_commands_;
    std::set<std::string> blacklisted_commands_;
    std::vector<std::string> dangerous_patterns_;
    std::vector<std::string> injection_patterns_;

    /**
     * @brief Initialize default whitelisted commands
     */
    void initializeWhitelist();

    /**
     * @brief Initialize default blacklisted commands
     */
    void initializeBlacklist();

    /**
     * @brief Initialize dangerous command patterns
     */
    void initializeDangerousPatterns();

    /**
     * @brief Initialize injection patterns
     */
    void initializeInjectionPatterns();

    /**
     * @brief Extract command name from full command string
     */
    std::string extractCommandName(const std::string& command) const;

    /**
     * @brief Check if string matches any pattern
     */
    bool matchesAnyPattern(const std::string& str, 
                          const std::vector<std::string>& patterns) const;
};

} // namespace utils
} // namespace neuroshell

#endif // NEUROSHELL_SAFETY_H
