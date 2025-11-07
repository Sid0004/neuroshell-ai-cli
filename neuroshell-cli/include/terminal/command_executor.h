#pragma once

#include "common/types.h"
#include <string>
#include <functional>

namespace NeuroShell {

class CommandExecutor {
public:
    CommandExecutor();
    ~CommandExecutor();
    
    // Execute a shell command and capture output
    CommandBlock Execute(const std::string& command, const std::string& workingDir = "");
    
    // Execute command asynchronously with callback
    void ExecuteAsync(const std::string& command, 
                     std::function<void(const CommandBlock&)> callback,
                     const std::string& workingDir = "");
    
    // Cancel running command
    void Cancel();
    
    // Check if command is currently running
    bool IsRunning() const;
    
    // Get current working directory
    std::string GetWorkingDirectory() const;
    
    // Change working directory
    bool SetWorkingDirectory(const std::string& path);
    
    // Built-in commands (cd, clear, etc.)
    bool IsBuiltInCommand(const std::string& command) const;
    CommandBlock ExecuteBuiltIn(const std::string& command);
    
private:
    std::string currentWorkingDir_;
    bool isRunning_;
    
    // Platform-specific implementation
#ifdef _WIN32
    void* processHandle_;
#else
    int processId_;
#endif
    
    // Helper methods
    std::string CaptureOutput(const std::string& command);
    bool ExecuteCD(const std::string& path);
    void InitializeWorkingDirectory();
};

} // namespace NeuroShell
