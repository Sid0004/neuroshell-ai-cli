#pragma once

#include "common/types.h"
#include "terminal/command_executor.h"
#include <vector>
#include <string>
#include <memory>

namespace NeuroShell {

class Terminal {
public:
    Terminal();
    ~Terminal();
    
    // Initialize terminal
    void Initialize();
    
    // Execute command and add to history
    void ExecuteCommand(const std::string& command);
    
    // Execute AI-generated command with NLP prompt tracking
    void ExecuteAICommand(const std::string& command, const std::string& nlpPrompt);
    
    // Get command history
    const std::vector<CommandBlock>& GetHistory() const { return history_; }
    
    // Clear history
    void ClearHistory();
    
    // Clear screen (keep history but reset display)
    void ClearScreen();
    
    // Get current working directory
    std::string GetWorkingDirectory() const;
    
    // Auto-completion suggestions
    std::vector<std::string> GetCompletions(const std::string& partial) const;
    
    // Command history navigation
    std::string GetPreviousCommand();
    std::string GetNextCommand();
    void ResetHistoryNavigation();
    
    // Get last command block
    const CommandBlock* GetLastCommand() const;
    
    // Search history
    std::vector<CommandBlock> SearchHistory(const std::string& query) const;
    
private:
    std::unique_ptr<CommandExecutor> executor_;
    std::vector<CommandBlock> history_;
    int historyNavigationIndex_;
    bool screenCleared_;
    
    // Built-in commands
    void HandleBuiltInCommand(const std::string& command);
    bool IsBuiltInCommand(const std::string& command) const;
    
    // Command completion data
    std::vector<std::string> commonCommands_;
    void InitializeCompletions();
};

} // namespace NeuroShell
