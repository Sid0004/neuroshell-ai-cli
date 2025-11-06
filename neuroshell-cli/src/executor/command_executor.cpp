#include "executor/command_executor.h"
#include "utils/logger.h"
#include "utils/safety.h"
#include <iostream>
#include <chrono>
#include <array>
#include <memory>
#include <cstdio>

#ifdef _WIN32
    #define popen _popen
    #define pclose _pclose
#endif

namespace neuroshell {
namespace executor {

CommandExecutor::CommandExecutor()
    : dry_run_mode_(false)
    , verbose_mode_(false)
    , last_command_("")
{
    // Default confirmation callback
    confirmation_callback_ = [](const std::string& cmd) {
        std::cout << "\n⚠️  About to execute: " << cmd << std::endl;
        std::cout << "Continue? (y/n): ";
        char response;
        std::cin >> response;
        return (response == 'y' || response == 'Y');
    };
}

CommandExecutor::~CommandExecutor() {}

ExecutionResult CommandExecutor::execute(const nlp::MappedCommand& mapped_cmd, bool force) {
    ExecutionResult result;
    result.success = false;
    result.exit_code = -1;
    
    // Check if command is valid
    if (mapped_cmd.command.empty()) {
        result.error = "Invalid or unknown command";
        LOG_ERROR("Attempted to execute empty command");
        return result;
    }
    
    // Safety check
    if (!validateSafety(mapped_cmd)) {
        result.error = "Command failed safety validation";
        LOG_ERROR("Command blocked by safety checker: " + mapped_cmd.command);
        return result;
    }
    
    // Build full command
    std::string full_command = buildCommandString(mapped_cmd);
    last_command_ = full_command;
    
    // Verbose output
    if (verbose_mode_) {
        std::cout << "Original: " << mapped_cmd.description << std::endl;
        std::cout << "Mapped to: " << full_command << std::endl;
        std::cout << "Confidence: " << (mapped_cmd.confidence * 100) << "%" << std::endl;
    }
    
    // Dry run mode
    if (dry_run_mode_) {
        std::cout << "[DRY RUN] Would execute: " << full_command << std::endl;
        result.success = true;
        result.exit_code = 0;
        result.output = "[Dry run - no actual execution]";
        return result;
    }
    
    // Request confirmation for dangerous commands
    if (mapped_cmd.requires_confirmation && !force) {
        if (!requestConfirmation(full_command)) {
            result.error = "User cancelled operation";
            LOG_INFO("User cancelled command: " + full_command);
            return result;
        }
    }
    
    // Execute command
    LOG_INFO("Executing: " + full_command);
    result = executeSystemCommand(full_command);
    
    if (result.success) {
        LOG_INFO("Command completed successfully");
    } else {
        LOG_ERROR("Command failed with exit code: " + std::to_string(result.exit_code));
    }
    
    return result;
}

void CommandExecutor::setConfirmationCallback(std::function<bool(const std::string&)> callback) {
    confirmation_callback_ = callback;
}

void CommandExecutor::setDryRun(bool enabled) {
    dry_run_mode_ = enabled;
}

void CommandExecutor::setVerbose(bool enabled) {
    verbose_mode_ = enabled;
}

std::string CommandExecutor::getLastCommand() const {
    return last_command_;
}

ExecutionResult CommandExecutor::executeSystemCommand(const std::string& command) {
    ExecutionResult result;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Execute command and capture output
    std::array<char, 128> buffer;
    std::string output;
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        result.error = "Failed to execute command";
        result.success = false;
        return result;
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        output += buffer.data();
    }
    
    int exit_code = pclose(pipe);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    
    result.output = output;
    result.exit_code = exit_code;
    result.success = (exit_code == 0);
    result.execution_time = elapsed.count();
    
    if (!result.success) {
        result.error = "Command exited with code " + std::to_string(exit_code);
    }
    
    return result;
}

std::string CommandExecutor::buildCommandString(const nlp::MappedCommand& mapped_cmd) const {
    std::string result = mapped_cmd.command;
    
    // Add arguments
    for (const auto& arg : mapped_cmd.args) {
        result += " " + arg;
    }
    
    return sanitizeCommand(result);
}

bool CommandExecutor::requestConfirmation(const std::string& command) {
    if (confirmation_callback_) {
        return confirmation_callback_(command);
    }
    return false;
}

bool CommandExecutor::validateSafety(const nlp::MappedCommand& mapped_cmd) {
    utils::SafetyChecker checker;
    
    // Check if command is blacklisted
    if (checker.isBlacklisted(mapped_cmd.command)) {
        return false;
    }
    
    // Check for injection risks
    std::string full_cmd = buildCommandString(mapped_cmd);
    if (checker.hasInjectionRisk(full_cmd)) {
        return false;
    }
    
    return true;
}

std::string CommandExecutor::sanitizeCommand(const std::string& command) const {
    std::string result = command;
    
    // Remove dangerous characters (basic sanitization)
    // In a production system, this would be more sophisticated
    
    // Trim whitespace
    size_t start = result.find_first_not_of(" \t\n\r");
    size_t end = result.find_last_not_of(" \t\n\r");
    
    if (start != std::string::npos && end != std::string::npos) {
        result = result.substr(start, end - start + 1);
    }
    
    return result;
}

} // namespace executor
} // namespace neuroshell
