#include "Logger.h"
#include <chrono>
#include <iomanip>
#include <ctime>

std::unique_ptr<Logger> Logger::instance_ = nullptr;

Logger::Logger() : min_level_(LogLevel::INFO), console_output_(true), file_output_(false) {}

Logger& Logger::getInstance() {
    if (!instance_) {
        instance_ = std::unique_ptr<Logger>(new Logger());
    }
    return *instance_;
}

Logger::~Logger() {
    close();
}

bool Logger::initialize(const std::string& filename, LogLevel min_level, bool console_output) {
    min_level_ = min_level;
    console_output_ = console_output;
    
    if (!filename.empty()) {
        log_file_.open(filename, std::ios::out | std::ios::app);
        if (log_file_.is_open()) {
            file_output_ = true;
            log(LogLevel::INFO, "Logger initialized with file: " + filename);
            return true;
        } else {
            std::cerr << "Failed to open log file: " << filename << std::endl;
            file_output_ = false;
            return false;
        }
    }
    
    file_output_ = false;
    log(LogLevel::INFO, "Logger initialized (console only)");
    return true;
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < min_level_) {
        return;
    }
    
    std::string timestamp = getCurrentTimestamp();
    std::string level_str = levelToString(level);
    std::string formatted_message = "[" + timestamp + "] [" + level_str + "] " + message;
    
    if (console_output_) {
        if (level >= LogLevel::ERROR) {
            std::cerr << formatted_message << std::endl;
        } else {
            std::cout << formatted_message << std::endl;
        }
    }
    
    if (file_output_ && log_file_.is_open()) {
        log_file_ << formatted_message << std::endl;
    }
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

void Logger::flush() {
    if (console_output_) {
        std::cout.flush();
        std::cerr.flush();
    }
    
    if (file_output_ && log_file_.is_open()) {
        log_file_.flush();
    }
}

void Logger::close() {
    if (log_file_.is_open()) {
        log(LogLevel::INFO, "Closing log file");
        flush();
        log_file_.close();
        file_output_ = false;
    }
}

std::string Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO ";
        case LogLevel::WARNING: return "WARN ";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}
