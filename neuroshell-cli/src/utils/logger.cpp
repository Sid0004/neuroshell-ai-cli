#include "utils/logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <chrono>

namespace neuroshell {
namespace utils {

Logger::Logger()
    : console_output_(true)
    , min_level_(LogLevel::INFO)
{}

Logger::~Logger() {
    close();
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::initialize(const std::string& log_file, bool console_output) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    console_output_ = console_output;
    
    if (!log_file.empty()) {
        log_file_.open(log_file, std::ios::app);
        if (!log_file_.is_open()) {
            std::cerr << "Failed to open log file: " << log_file << std::endl;
        }
    }
}

void Logger::setLevel(LogLevel level) {
    min_level_ = level;
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < min_level_) {
        return;
    }
    
    writeLog(level, message);
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::critical(const std::string& message) {
    log(LogLevel::CRITICAL, message);
}

void Logger::close() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

std::string Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO";
        case LogLevel::WARNING:  return "WARNING";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

void Logger::writeLog(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string timestamp = getCurrentTimestamp();
    std::string level_str = levelToString(level);
    
    std::stringstream log_entry;
    log_entry << "[" << timestamp << "] "
              << "[" << level_str << "] "
              << message;
    
    std::string entry = log_entry.str();
    
    // Write to console
    if (console_output_) {
        if (level >= LogLevel::ERROR) {
            std::cerr << entry << std::endl;
        } else {
            std::cout << entry << std::endl;
        }
    }
    
    // Write to file
    if (log_file_.is_open()) {
        log_file_ << entry << std::endl;
        log_file_.flush();
    }
}

} // namespace utils
} // namespace neuroshell
