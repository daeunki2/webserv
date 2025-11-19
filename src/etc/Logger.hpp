#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>

class Logger
{
public:
    /* Canonical */
    Logger();
    Logger(const Logger &o);
    Logger &operator=(const Logger &o);
    ~Logger();

    /* Static APIs */
    static void info(const std::string &msg);
    static void warn(const std::string &msg);
    static void error(const std::string &msg);
    static void debug(const std::string &msg);

private:
    static std::string color(const std::string &msg, const std::string &code);
};

#endif
