#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <memory>

namespace urban_transport {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger {
public:
    static Logger& get_instance();
    
    void initialize(const std::string& filename = "");
    void shutdown();
    
    void log(LogLevel level, const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);

private:
    Logger() = default;
    ~Logger();
    
    std::ofstream log_file_;
    std::mutex log_mutex_;
    bool initialized_ = false;
    bool use_file_ = false;
    
    std::string level_to_string(LogLevel level);
    void write_log(const std::string& message);
    
    // Eliminar copia
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

} // namespace urban_transport

#endif // LOGGER_H