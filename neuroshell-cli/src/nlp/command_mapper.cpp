#include "nlp/command_mapper.h"
#include <algorithm>
#include <sstream>

namespace neuroshell {
namespace nlp {

CommandMapper::CommandMapper() {
    initializeDefaultMappings();
}

CommandMapper::~CommandMapper() {}

MappedCommand CommandMapper::mapToCommand(const ParsedIntent& intent) {
    MappedCommand result;
    result.confidence = intent.confidence;
    
    // Create key from action and target
    std::string key = createKey(intent.action, intent.target);
    
    // Find matching template
    auto it = command_templates_.find(key);
    if (it != command_templates_.end()) {
        const auto& tmpl = it->second;
        
        // Get platform-specific command
        std::string base_cmd = getPlatformCommand(tmpl);
        
        // Replace parameters
        result.command = replaceParameters(base_cmd, intent.parameters);
        
        result.requires_confirmation = tmpl.requires_confirmation;
        result.is_dangerous = tmpl.is_dangerous;
        result.description = intent.original_input;
    } else {
        // Unknown command
        result.command = "";
        result.requires_confirmation = true;
        result.is_dangerous = false;
        result.description = "Unknown command: " + intent.original_input;
        result.confidence = 0.0f;
    }
    
    return result;
}

bool CommandMapper::loadMappings(const std::string& config_path) {
    // TODO: Implement JSON config loading
    // For now, use default mappings
    return true;
}

std::vector<std::string> CommandMapper::getAvailablePatterns() const {
    std::vector<std::string> patterns;
    
    for (const auto& pair : command_templates_) {
        patterns.push_back(pair.first);
    }
    
    return patterns;
}

void CommandMapper::initializeDefaultMappings() {
    // List files
    command_templates_["list:file"] = {
        "dir",                    // Windows
        "ls -la",                 // Linux
        "ls -la",                 // macOS
        false, false, {}
    };
    
    // List files recursively
    command_templates_["list:file_recursive"] = {
        "dir /s",
        "ls -laR",
        "ls -laR",
        false, false, {}
    };
    
    // Create folder
    command_templates_["create:folder"] = {
        "mkdir {name}",
        "mkdir -p {name}",
        "mkdir -p {name}",
        false, false, {"name"}
    };
    
    // Delete file
    command_templates_["delete:file"] = {
        "del /q {name}",
        "rm -f {name}",
        "rm -f {name}",
        true, true, {"name"}
    };
    
    // Delete folder
    command_templates_["delete:folder"] = {
        "rmdir /s /q {name}",
        "rm -rf {name}",
        "rm -rf {name}",
        true, true, {"name"}
    };
    
    // Copy file
    command_templates_["copy:file"] = {
        "copy {source} {dest}",
        "cp {source} {dest}",
        "cp {source} {dest}",
        false, false, {"source", "dest"}
    };
    
    // Move file
    command_templates_["move:file"] = {
        "move {source} {dest}",
        "mv {source} {dest}",
        "mv {source} {dest}",
        false, false, {"source", "dest"}
    };
    
    // List processes
    command_templates_["list:process"] = {
        "tasklist",
        "ps aux",
        "ps aux",
        false, false, {}
    };
    
    // Stop process
    command_templates_["stop:process"] = {
        "taskkill /F /IM {name}",
        "killall {name}",
        "killall {name}",
        true, true, {"name"}
    };
    
    // Network info
    command_templates_["list:network"] = {
        "ipconfig",
        "ifconfig",
        "ifconfig",
        false, false, {}
    };
    
    // System info
    command_templates_["list:system"] = {
        "systeminfo",
        "uname -a",
        "uname -a",
        false, false, {}
    };
    
    // Current directory
    command_templates_["list:folder"] = {
        "cd",
        "pwd",
        "pwd",
        false, false, {}
    };
}

std::string CommandMapper::getPlatformCommand(const CommandTemplate& tmpl) const {
    std::string platform = detectPlatform();
    
    if (platform == "windows") {
        return tmpl.windows_cmd;
    } else if (platform == "linux") {
        return tmpl.linux_cmd;
    } else if (platform == "macos") {
        return tmpl.macos_cmd;
    }
    
    return tmpl.windows_cmd; // Default to Windows
}

std::string CommandMapper::replaceParameters(
    const std::string& cmd_template,
    const std::map<std::string, std::string>& params) const {
    
    std::string result = cmd_template;
    
    // Replace {name}, {source}, {dest}, etc.
    for (const auto& param : params) {
        std::string placeholder = "{" + param.first + "}";
        size_t pos = result.find(placeholder);
        
        if (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), param.second);
        }
    }
    
    return result;
}

std::string CommandMapper::detectPlatform() const {
    #ifdef PLATFORM_WINDOWS
        return "windows";
    #elif defined(PLATFORM_LINUX)
        return "linux";
    #elif defined(PLATFORM_MACOS)
        return "macos";
    #else
        return "windows"; // Default
    #endif
}

std::string CommandMapper::createKey(const std::string& action, const std::string& target) const {
    return action + ":" + target;
}

} // namespace nlp
} // namespace neuroshell
