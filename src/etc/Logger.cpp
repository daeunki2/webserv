#include "Logger.hpp"

/* ANSI codes */
#define C_RESET   "\033[0m"
#define C_RED     "\033[31m"
#define C_GREEN   "\033[32m"
#define C_YELLOW  "\033[33m"
#define C_CYAN    "\033[36m"

/* Canonical */

Logger::Logger() {}
Logger::Logger(const Logger &o) { (void)o; }
Logger &Logger::operator=(const Logger &o)
{
    (void)o;
    return *this;
}
Logger::~Logger() {}

std::string Logger::color(const std::string &msg, const std::string &code)
{
    return code + msg + C_RESET;
}

void Logger::info(const std::string &msg)
{
    std::cout << color("[INFO] ", C_GREEN) << msg << std::endl;
}

void Logger::warn(const std::string &msg)
{
    std::cout << color("[WARN] ", C_YELLOW) << msg << std::endl;
}

void Logger::error(const std::string &msg)
{
    std::cout << color("[ERROR] ", C_RED) << msg << std::endl;
}

void Logger::debug(const std::string &msg)
{
    std::cout << color("[DEBUG] ", C_CYAN) << msg << std::endl;
}
