#include "terminal/command_executor.h"
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define getcwd _getcwd
#define chdir _chdir
#else
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#endif

namespace NeuroShell {

CommandExecutor::CommandExecutor()
    : isRunning_(false)
#ifdef _WIN32
    , processHandle_(nullptr)
#else
    , processId_(-1)
#endif
{
    InitializeWorkingDirectory();
}

CommandExecutor::~CommandExecutor() {
    Cancel();
}

void CommandExecutor::InitializeWorkingDirectory() {
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer)) != nullptr) {
        currentWorkingDir_ = buffer;
    }
}

std::string CommandExecutor::GetWorkingDirectory() const {
    return currentWorkingDir_;
}

bool CommandExecutor::SetWorkingDirectory(const std::string& path) {
    if (chdir(path.c_str()) == 0) {
        InitializeWorkingDirectory();
        return true;
    }
    return false;
}

bool CommandExecutor::IsRunning() const {
    return isRunning_;
}

bool CommandExecutor::IsBuiltInCommand(const std::string& command) const {
    if (command.empty()) return false;
    
    std::string cmd = command;
    auto spacePos = cmd.find(' ');
    if (spacePos != std::string::npos) {
        cmd = cmd.substr(0, spacePos);
    }
    
    return cmd == "cd" || cmd == "clear" || cmd == "exit" || cmd == "pwd";
}

CommandBlock CommandExecutor::ExecuteBuiltIn(const std::string& command) {
    CommandBlock block;
    block.input = command;
    block.workingDirectory = currentWorkingDir_;
    
    std::string cmd = command;
    auto spacePos = cmd.find(' ');
    std::string arg;
    
    if (spacePos != std::string::npos) {
        arg = cmd.substr(spacePos + 1);
        cmd = cmd.substr(0, spacePos);
    }
    
    if (cmd == "cd") {
        if (arg.empty()) {
            arg = getenv("HOME") ? getenv("HOME") : getenv("USERPROFILE");
        }
        
        if (ExecuteCD(arg)) {
            block.output = "Changed directory to: " + currentWorkingDir_;
            block.status = CommandStatus::Success;
            block.exitCode = 0;
        } else {
            block.output = "Failed to change directory: " + arg;
            block.status = CommandStatus::Failed;
            block.exitCode = 1;
        }
    }
    else if (cmd == "pwd") {
        block.output = currentWorkingDir_;
        block.status = CommandStatus::Success;
        block.exitCode = 0;
    }
    else if (cmd == "clear") {
        block.output = "";
        block.status = CommandStatus::Success;
        block.exitCode = 0;
    }
    else if (cmd == "exit") {
        block.output = "Use Ctrl+Q or close window to exit.";
        block.status = CommandStatus::Success;
        block.exitCode = 0;
    }
    
    return block;
}

bool CommandExecutor::ExecuteCD(const std::string& path) {
    return SetWorkingDirectory(path);
}

CommandBlock CommandExecutor::Execute(const std::string& command, const std::string& workingDir) {
    CommandBlock block;
    block.input = command;
    block.workingDirectory = workingDir.empty() ? currentWorkingDir_ : workingDir;
    
    // Check if built-in
    if (IsBuiltInCommand(command)) {
        return ExecuteBuiltIn(command);
    }
    
    isRunning_ = true;
    
    try {
        block.output = CaptureOutput(command);
        block.status = CommandStatus::Success;
        block.exitCode = 0;
    }
    catch (const std::exception& e) {
        block.output = std::string("Error: ") + e.what();
        block.status = CommandStatus::Failed;
        block.exitCode = 1;
    }
    
    isRunning_ = false;
    return block;
}

std::string CommandExecutor::CaptureOutput(const std::string& command) {
#ifdef _WIN32
    // Windows implementation using _popen
    std::string fullCommand = "cd /d \"" + currentWorkingDir_ + "\" && " + command + " 2>&1";
    FILE* pipe = _popen(fullCommand.c_str(), "r");
    
    if (!pipe) {
        throw std::runtime_error("Failed to execute command");
    }
    
    std::stringstream result;
    char buffer[256];
    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result << buffer;
    }
    
    int exitCode = _pclose(pipe);
    return result.str();
#else
    // Unix/Linux implementation
    std::string fullCommand = "cd \"" + currentWorkingDir_ + "\" && " + command + " 2>&1";
    FILE* pipe = popen(fullCommand.c_str(), "r");
    
    if (!pipe) {
        throw std::runtime_error("Failed to execute command");
    }
    
    std::stringstream result;
    char buffer[256];
    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result << buffer;
    }
    
    int exitCode = pclose(pipe);
    return result.str();
#endif
}

void CommandExecutor::ExecuteAsync(const std::string& command, 
                                   std::function<void(const CommandBlock&)> callback,
                                   const std::string& workingDir) {
    // Launch in separate thread
    std::thread([this, command, callback, workingDir]() {
        CommandBlock result = Execute(command, workingDir);
        callback(result);
    }).detach();
}

void CommandExecutor::Cancel() {
    if (isRunning_) {
#ifdef _WIN32
        if (processHandle_) {
            TerminateProcess(processHandle_, 1);
            CloseHandle(processHandle_);
            processHandle_ = nullptr;
        }
#else
        if (processId_ > 0) {
            kill(processId_, SIGTERM);
            processId_ = -1;
        }
#endif
        isRunning_ = false;
    }
}

} // namespace NeuroShell
