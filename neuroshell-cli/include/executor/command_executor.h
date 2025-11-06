#ifndef NEUROSHELL_COMMAND_EXECUTOR_H
#define NEUROSHELL_COMMAND_EXECUTOR_H

#include "nlp/command_mapper.h"
#include <string>
#include <functional>

namespace neuroshell {
namespace executor {

/**
 * @brief Result of command execution
 */
struct ExecutionResult {
    bool success;                // Whether execution was successful
    int exit_code;              // Command exit code
    std::string output;         // Standard output
    std::string error;          // Standard error
    double execution_time;      // Execution time in seconds
};

/**
 * @brief Executes mapped CLI commands safely
 */
class CommandExecutor {
public:
    CommandExecutor();
    ~CommandExecutor();

    /**
     * @brief Execute a mapped command
     * @param mapped_cmd The command to execute
     * @param force Skip confirmation prompts if true
     * @return ExecutionResult with command output and status
     */
    ExecutionResult execute(const nlp::MappedCommand& mapped_cmd, bool force = false);

    /**
     * @brief Set callback for user confirmation
     * @param callback Function that returns true if user confirms
     */
    void setConfirmationCallback(std::function<bool(const std::string&)> callback);

    /**
     * @brief Enable or disable dry run mode (show command without executing)
     * @param enabled True to enable dry run mode
     */
    void setDryRun(bool enabled);

    /**
     * @brief Enable or disable verbose output
     * @param enabled True to enable verbose mode
     */
    void setVerbose(bool enabled);

    /**
     * @brief Get the last executed command
     * @return Last command string
     */
    std::string getLastCommand() const;

private:
    bool dry_run_mode_;
    bool verbose_mode_;
    std::string last_command_;
    std::function<bool(const std::string&)> confirmation_callback_;

    /**
     * @brief Execute system command and capture output
     * @param command Full command string
     * @return ExecutionResult
     */
    ExecutionResult executeSystemCommand(const std::string& command);

    /**
     * @brief Build full command string from MappedCommand
     * @param mapped_cmd Mapped command structure
     * @return Full command string
     */
    std::string buildCommandString(const nlp::MappedCommand& mapped_cmd) const;

    /**
     * @brief Request user confirmation
     * @param command Command to confirm
     * @return True if user confirms
     */
    bool requestConfirmation(const std::string& command);

    /**
     * @brief Validate command safety
     * @param mapped_cmd Command to validate
     * @return True if command is safe to execute
     */
    bool validateSafety(const nlp::MappedCommand& mapped_cmd);

    /**
     * @brief Sanitize command input
     * @param command Command to sanitize
     * @return Sanitized command
     */
    std::string sanitizeCommand(const std::string& command) const;
};

} // namespace executor
} // namespace neuroshell

#endif // NEUROSHELL_COMMAND_EXECUTOR_H
