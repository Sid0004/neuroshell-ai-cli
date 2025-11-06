#include "utils/config_loader.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace neuroshell {
namespace utils {

ConfigLoader::ConfigLoader() {}

ConfigLoader::~ConfigLoader() {}

bool ConfigLoader::load(const std::string& file_path) {
    config_file_path_ = file_path;
    config_data_.clear();
    
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    parseSimpleConfig(buffer.str());
    
    return true;
}

std::string ConfigLoader::getString(const std::string& key, const std::string& default_value) const {
    auto it = config_data_.find(key);
    if (it != config_data_.end()) {
        return it->second;
    }
    return default_value;
}

int ConfigLoader::getInt(const std::string& key, int default_value) const {
    auto it = config_data_.find(key);
    if (it != config_data_.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {
            return default_value;
        }
    }
    return default_value;
}

bool ConfigLoader::getBool(const std::string& key, bool default_value) const {
    auto it = config_data_.find(key);
    if (it != config_data_.end()) {
        std::string value = it->second;
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        return (value == "true" || value == "1" || value == "yes");
    }
    return default_value;
}

bool ConfigLoader::hasKey(const std::string& key) const {
    return config_data_.find(key) != config_data_.end();
}

std::vector<std::string> ConfigLoader::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& pair : config_data_) {
        keys.push_back(pair.first);
    }
    return keys;
}

void ConfigLoader::parseSimpleConfig(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Find '=' separator
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Remove quotes if present
            if (value.length() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.length() - 2);
            }
            
            config_data_[key] = value;
        }
    }
}

} // namespace utils
} // namespace neuroshell
