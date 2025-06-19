#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <sstream>

/**
 * @brief Logging levels
 */
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3
};

/**
 * @brief Simple logging utility class
 */
class Logger {
private:
    static std::unique_ptr<Logger> instance_;
    std::ofstream log_file_;
    LogLevel min_level_;
    bool console_output_;
    bool file_output_;

    /**
     * @brief Private constructor for singleton pattern
     */
    Logger();

public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the Logger instance
     */
    static Logger& getInstance();

    /**
     * @brief Destructor
     */
    ~Logger();

    /**
     * @brief Initialize logger with file output
     * @param filename Log file name
     * @param min_level Minimum logging level
     * @param console_output Enable console output
     * @return True if successful, false otherwise
     */
    bool initialize(const std::string& filename, LogLevel min_level = LogLevel::INFO, 
                   bool console_output = true);

    /**
     * @brief Set minimum logging level
     * @param level Minimum level
     */
    void setMinLevel(LogLevel level) { min_level_ = level; }

    /**
     * @brief Enable/disable console output
     * @param enable Enable console output
     */
    void setConsoleOutput(bool enable) { console_output_ = enable; }

    /**
     * @brief Enable/disable file output
     * @param enable Enable file output
     */
    void setFileOutput(bool enable) { file_output_ = enable; }

    /**
     * @brief Log a message
     * @param level Log level
     * @param message Message to log
     */
    void log(LogLevel level, const std::string& message);

    /**
     * @brief Log debug message
     * @param message Message to log
     */
    void debug(const std::string& message);

    /**
     * @brief Log info message
     * @param message Message to log
     */
    void info(const std::string& message);

    /**
     * @brief Log warning message
     * @param message Message to log
     */
    void warning(const std::string& message);

    /**
     * @brief Log error message
     * @param message Message to log
     */
    void error(const std::string& message);

    /**
     * @brief Flush log buffers
     */
    void flush();

    /**
     * @brief Close log file
     */
    void close();

private:
    /**
     * @brief Get current timestamp string
     * @return Formatted timestamp
     */
    std::string getCurrentTimestamp() const;

    /**
     * @brief Convert log level to string
     * @param level Log level
     * @return Level string
     */
    std::string levelToString(LogLevel level) const;
};

// Convenience macros
#define LOG_DEBUG(msg) Logger::getInstance().debug(msg)
#define LOG_INFO(msg) Logger::getInstance().info(msg)
#define LOG_WARNING(msg) Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) Logger::getInstance().error(msg)

// Stream-like logging macros
#define LOG_STREAM(level) LogStream(level)

/**
 * @brief Stream-like logging helper class
 */
class LogStream {
private:
    std::ostringstream stream_;
    LogLevel level_;

public:
    explicit LogStream(LogLevel level) : level_(level) {}
    
    ~LogStream() {
        Logger::getInstance().log(level_, stream_.str());
    }
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        stream_ << value;
        return *this;
    }
};

#endif // LOGGER_H
