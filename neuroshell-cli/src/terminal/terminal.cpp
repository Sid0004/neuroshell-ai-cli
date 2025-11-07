#include "terminal/terminal.h"
#include <algorithm>

namespace NeuroShell {

Terminal::Terminal()
    : executor_(nullptr)
    , historyNavigationIndex_(-1)
    , screenCleared_(false)
{
}

Terminal::~Terminal() = default;

void Terminal::Initialize() {
    executor_ = std::make_unique<CommandExecutor>();
    InitializeCompletions();
}

void Terminal::ExecuteCommand(const std::string& command) {
    if (command.empty()) return;
    
    CommandBlock block = executor_->Execute(command);
    history_.push_back(block);
    
    historyNavigationIndex_ = -1;
}

void Terminal::ExecuteAICommand(const std::string& command, const std::string& nlpPrompt) {
    if (command.empty()) return;
    
    CommandBlock block = executor_->Execute(command);
    block.isAIGenerated = true;
    block.aiPrompt = nlpPrompt;
    
    history_.push_back(block);
    historyNavigationIndex_ = -1;
}

void Terminal::ClearHistory() {
    history_.clear();
    historyNavigationIndex_ = -1;
}

void Terminal::ClearScreen() {
    screenCleared_ = true;
    // Don't actually clear history, just signal to UI to reset scroll
}

std::string Terminal::GetWorkingDirectory() const {
    if (executor_) {
        return executor_->GetWorkingDirectory();
    }
    return "";
}

const CommandBlock* Terminal::GetLastCommand() const {
    if (history_.empty()) return nullptr;
    return &history_.back();
}

std::vector<CommandBlock> Terminal::SearchHistory(const std::string& query) const {
    std::vector<CommandBlock> results;
    
    for (const auto& block : history_) {
        if (block.input.find(query) != std::string::npos ||
            block.output.find(query) != std::string::npos) {
            results.push_back(block);
        }
    }
    
    return results;
}

std::string Terminal::GetPreviousCommand() {
    if (history_.empty()) return "";
    
    if (historyNavigationIndex_ == -1) {
        historyNavigationIndex_ = static_cast<int>(history_.size()) - 1;
    } else if (historyNavigationIndex_ > 0) {
        historyNavigationIndex_--;
    }
    
    if (historyNavigationIndex_ >= 0 && historyNavigationIndex_ < static_cast<int>(history_.size())) {
        return history_[historyNavigationIndex_].input;
    }
    
    return "";
}

std::string Terminal::GetNextCommand() {
    if (historyNavigationIndex_ == -1 || history_.empty()) return "";
    
    historyNavigationIndex_++;
    
    if (historyNavigationIndex_ >= static_cast<int>(history_.size())) {
        historyNavigationIndex_ = -1;
        return "";
    }
    
    return history_[historyNavigationIndex_].input;
}

void Terminal::ResetHistoryNavigation() {
    historyNavigationIndex_ = -1;
}

std::vector<std::string> Terminal::GetCompletions(const std::string& partial) const {
    std::vector<std::string> completions;
    
    for (const auto& cmd : commonCommands_) {
        if (cmd.find(partial) == 0) {
            completions.push_back(cmd);
        }
    }
    
    return completions;
}

void Terminal::InitializeCompletions() {
    // Common commands for auto-completion
    commonCommands_ = {
        // Windows commands
        "dir", "cd", "cls", "copy", "move", "del", "mkdir", "rmdir",
        "type", "echo", "set", "path", "where", "whoami", "hostname",
        "ipconfig", "ping", "tracert", "netstat", "tasklist", "taskkill",
        
        // Unix/Linux commands (for WSL or cross-platform)
        "ls", "pwd", "cat", "grep", "find", "cp", "mv", "rm", "touch",
        "chmod", "chown", "ps", "top", "kill", "df", "du", "free",
        "uname", "date", "cal", "history", "clear",
        
        // Git commands
        "git status", "git add", "git commit", "git push", "git pull",
        "git clone", "git branch", "git checkout", "git merge", "git log",
        
        // Common tools
        "python", "node", "npm", "pip", "code", "notepad", "vim"
    };
}

bool Terminal::IsBuiltInCommand(const std::string& command) const {
    return executor_->IsBuiltInCommand(command);
}

void Terminal::HandleBuiltInCommand(const std::string& command) {
    CommandBlock block = executor_->ExecuteBuiltIn(command);
    history_.push_back(block);
}

} // namespace NeuroShell
