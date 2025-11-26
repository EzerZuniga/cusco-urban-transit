#include "infra/logger.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <ctime>

using namespace urban_transport;

Logger& Logger::get_instance() {
    static Logger instance;
    return instance;
}

void Logger::initialize(const std::string& filename) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    if (initialized_) return;
    if (!filename.empty()) {
        log_file_.open(filename, std::ios::app);
        if (log_file_.is_open()) use_file_ = true;
    }
    initialized_ = true;
    log(LogLevel::INFO, "Logger initialized");
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(log_mutex_);
    if (!initialized_) return;
    log(LogLevel::INFO, "Logger shutdown");
    if (log_file_.is_open()) log_file_.close();
    initialized_ = false;
    use_file_ = false;
}

Logger::~Logger() {
    shutdown();
}

void Logger::log(LogLevel level, const std::string& message) {
    if (!initialized_) return;
    std::lock_guard<std::mutex> lock(log_mutex_);

    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();

    std::string level_str = level_to_string(level);
    std::string log_entry = "[" + ss.str() + "] [" + level_str + "] " + message;
    write_log(log_entry);
}

void Logger::debug(const std::string& message) { log(LogLevel::DEBUG, message); }
void Logger::info(const std::string& message) { log(LogLevel::INFO, message); }
void Logger::warning(const std::string& message) { log(LogLevel::WARNING, message); }
void Logger::error(const std::string& message) { log(LogLevel::ERROR, message); }
void Logger::critical(const std::string& message) { log(LogLevel::CRITICAL, message); }

std::string Logger::level_to_string(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

void Logger::write_log(const std::string& message) {
    std::cout << message << std::endl;
    if (use_file_ && log_file_.is_open()) {
        log_file_ << message << std::endl;
        log_file_.flush();
    }
}