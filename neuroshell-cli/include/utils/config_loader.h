#ifndef NEUROSHELL_CONFIG_LOADER_H
#define NEUROSHELL_CONFIG_LOADER_H

#include <string>
#include <map>
#include <vector>

namespace neuroshell {
namespace utils {

/**
 * @brief Simple JSON-like configuration loader
 * For a real implementation, use nlohmann/json or similar
 */
class ConfigLoader {
public:
    ConfigLoader();
    ~ConfigLoader();

    /**
     * @brief Load configuration from file
     * @param file_path Path to config file
     * @return True if loaded successfully
     */
    bool load(const std::string& file_path);

    /**
     * @brief Get string value
     * @param key Configuration key
     * @param default_value Default value if key not found
     * @return Configuration value
     */
    std::string getString(const std::string& key, const std::string& default_value = "") const;

    /**
     * @brief Get integer value
     */
    int getInt(const std::string& key, int default_value = 0) const;

    /**
     * @brief Get boolean value
     */
    bool getBool(const std::string& key, bool default_value = false) const;

    /**
     * @brief Check if key exists
     */
    bool hasKey(const std::string& key) const;

    /**
     * @brief Get all keys
     */
    std::vector<std::string> getKeys() const;

private:
    std::map<std::string, std::string> config_data_;
    std::string config_file_path_;

    /**
     * @brief Parse simple key=value configuration
     */
    void parseSimpleConfig(const std::string& content);
};

} // namespace utils
} // namespace neuroshell

#endif // NEUROSHELL_CONFIG_LOADER_H
