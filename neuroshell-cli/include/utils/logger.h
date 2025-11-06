#ifndef NEUROSHELL_LOGGER_H
#define NEUROSHELL_LOGGER_H

#include <string>
#include <fstream>
#include <memory>
#include <mutex>

namespace neuroshell {
namespace utils {

/**
 * @brief Log severity levels
 */
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

/**
 * @brief Simple logging utility
 */
class Logger {
public:
    /**
     * @brief Get singleton instance
     */
    static Logger& getInstance();

    /**
     * @brief Initialize logger with file path
     * @param log_file Path to log file
     * @param console_output Enable console output
     */
    void initialize(const std::string& log_file, bool console_output = true);

    /**
     * @brief Set minimum log level
     * @param level Minimum level to log
     */
    void setLevel(LogLevel level);

    /**
     * @brief Log a message
     * @param level Log level
     * @param message Message to log
     */
    void log(LogLevel level, const std::string& message);

    /**
     * @brief Convenience methods
     */
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);

    /**
     * @brief Close log file
     */
    void close();

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream log_file_;
    bool console_output_;
    LogLevel min_level_;
    std::mutex mutex_;

    /**
     * @brief Get current timestamp string
     */
    std::string getCurrentTimestamp() const;

    /**
     * @brief Convert log level to string
     */
    std::string levelToString(LogLevel level) const;

    /**
     * @brief Write log entry
     */
    void writeLog(LogLevel level, const std::string& message);
};

} // namespace utils
} // namespace neuroshell

// Convenience macros
#define LOG_DEBUG(msg) neuroshell::utils::Logger::getInstance().debug(msg)
#define LOG_INFO(msg) neuroshell::utils::Logger::getInstance().info(msg)
#define LOG_WARNING(msg) neuroshell::utils::Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) neuroshell::utils::Logger::getInstance().error(msg)
#define LOG_CRITICAL(msg) neuroshell::utils::Logger::getInstance().critical(msg)

#endif // NEUROSHELL_LOGGER_H
