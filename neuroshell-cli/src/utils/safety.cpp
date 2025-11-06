#include "utils/safety.h"
#include <algorithm>
#include <regex>

namespace neuroshell {
namespace utils {

SafetyChecker::SafetyChecker() {
    initializeWhitelist();
    initializeBlacklist();
    initializeDangerousPatterns();
    initializeInjectionPatterns();
}

SafetyChecker::~SafetyChecker() {}

bool SafetyChecker::isSafe(const std::string& command) const {
    // Check if command is blacklisted
    if (isBlacklisted(command)) {
        return false;
    }
    
    // Check for injection risks
    if (hasInjectionRisk(command)) {
        return false;
    }
    
    // Extract command name
    std::string cmd_name = extractCommandName(command);
    
    // If in whitelist, it's safe
    if (whitelisted_commands_.find(cmd_name) != whitelisted_commands_.end()) {
        return true;
    }
    
    // If not in whitelist and not in blacklist, consider it moderately safe
    // but should require confirmation
    return !isDangerous(command);
}

bool SafetyChecker::isDangerous(const std::string& command) const {
    return matchesAnyPattern(command, dangerous_patterns_);
}

bool SafetyChecker::isBlacklisted(const std::string& command) const {
    std::string cmd_name = extractCommandName(command);
    return blacklisted_commands_.find(cmd_name) != blacklisted_commands_.end();
}

void SafetyChecker::addToWhitelist(const std::string& command) {
    whitelisted_commands_.insert(command);
}

void SafetyChecker::addToBlacklist(const std::string& command) {
    blacklisted_commands_.insert(command);
}

bool SafetyChecker::hasInjectionRisk(const std::string& input) const {
    return matchesAnyPattern(input, injection_patterns_);
}

void SafetyChecker::initializeWhitelist() {
    // Safe read-only commands
    whitelisted_commands_.insert("ls");
    whitelisted_commands_.insert("dir");
    whitelisted_commands_.insert("pwd");
    whitelisted_commands_.insert("cd");
    whitelisted_commands_.insert("echo");
    whitelisted_commands_.insert("cat");
    whitelisted_commands_.insert("type");
    whitelisted_commands_.insert("more");
    whitelisted_commands_.insert("less");
    whitelisted_commands_.insert("head");
    whitelisted_commands_.insert("tail");
    whitelisted_commands_.insert("find");
    whitelisted_commands_.insert("grep");
    whitelisted_commands_.insert("ps");
    whitelisted_commands_.insert("tasklist");
    whitelisted_commands_.insert("ipconfig");
    whitelisted_commands_.insert("ifconfig");
    whitelisted_commands_.insert("ping");
    whitelisted_commands_.insert("date");
    whitelisted_commands_.insert("time");
    whitelisted_commands_.insert("whoami");
    whitelisted_commands_.insert("hostname");
    whitelisted_commands_.insert("uname");
    whitelisted_commands_.insert("systeminfo");
}

void SafetyChecker::initializeBlacklist() {
    // Extremely dangerous commands that should never be auto-executed
    blacklisted_commands_.insert("format");
    blacklisted_commands_.insert("fdisk");
    blacklisted_commands_.insert("mkfs");
    blacklisted_commands_.insert("dd");
    blacklisted_commands_.insert("rm -rf /");
    blacklisted_commands_.insert(":(){ :|:& };:");  // Fork bomb
    blacklisted_commands_.insert("chmod -R 777 /");
    blacklisted_commands_.insert("chown -R");
    
    // Potentially harmful system commands
    blacklisted_commands_.insert("sudo su");
    blacklisted_commands_.insert("su root");
}

void SafetyChecker::initializeDangerousPatterns() {
    // Patterns that indicate dangerous operations
    dangerous_patterns_.push_back("rm.*-rf");
    dangerous_patterns_.push_back("del.*\\/s");
    dangerous_patterns_.push_back("rmdir.*\\/s");
    dangerous_patterns_.push_back("format.*:");
    dangerous_patterns_.push_back("taskkill.*\\/f");
    dangerous_patterns_.push_back("kill -9");
    dangerous_patterns_.push_back("killall");
    dangerous_patterns_.push_back("shutdown");
    dangerous_patterns_.push_back("reboot");
    dangerous_patterns_.push_back("halt");
    dangerous_patterns_.push_back("poweroff");
    dangerous_patterns_.push_back("> \\/dev\\/");
    dangerous_patterns_.push_back("dd if=");
}

void SafetyChecker::initializeInjectionPatterns() {
    // Patterns that might indicate command injection attempts
    injection_patterns_.push_back(";.*rm");
    injection_patterns_.push_back("&&.*del");
    injection_patterns_.push_back("\\|.*sh");
    injection_patterns_.push_back("`.*`");
    injection_patterns_.push_back("\\$\\(.*\\)");
    injection_patterns_.push_back(">.*\\/");
    injection_patterns_.push_back("<<");
    injection_patterns_.push_back("\\.\\.\\/");  // Path traversal
}

std::string SafetyChecker::extractCommandName(const std::string& command) const {
    // Extract first word (command name)
    size_t pos = command.find_first_of(" \t");
    if (pos != std::string::npos) {
        return command.substr(0, pos);
    }
    return command;
}

bool SafetyChecker::matchesAnyPattern(const std::string& str, 
                                      const std::vector<std::string>& patterns) const {
    for (const auto& pattern : patterns) {
        try {
            std::regex rgx(pattern, std::regex::icase);
            if (std::regex_search(str, rgx)) {
                return true;
            }
        } catch (const std::regex_error&) {
            // Invalid regex pattern, skip
            continue;
        }
    }
    return false;
}

} // namespace utils
} // namespace neuroshell
