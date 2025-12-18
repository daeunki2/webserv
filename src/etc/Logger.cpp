/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 17:41:32 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/03 16:38:09 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Logger.hpp"

/* ANSI codes */
#define C_RESET   "\033[0m"
#define C_RED     "\033[31m"
#define C_GREEN   "\033[32m"
#define C_YELLOW  "\033[33m"
#define C_CYAN    "\033[36m"
#define C_GRAY    "\033[90m"

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

std::string Logger::bracket(const std::string &s)
{
    return "[" + s + "] ";
}

std::string Logger::tagToString(Tag tag)
{
    switch (tag)
    {
        case TAG_EVENT:   return "EVENT";
        case TAG_REQ:     return "REQ";
        case TAG_CGI:     return "CGI";
        case TAG_POLL:    return "POLL";
        case TAG_CONF:    return "CONF";
        case TAG_FD:      return "FD";
        case TAG_TIMEOUT: return "TIMEOUT";
        case TAG_CORE:    return "CORE";
        default:          return "UNKNOWN";
    }
}

void Logger::info(Tag tag, const std::string &msg)
{
    std::cerr
        << color(bracket("INFO"), C_GREEN)
        << color(bracket(tagToString(tag)), C_GRAY)
        << msg << std::endl;
}

void Logger::warn(Tag tag, const std::string &msg)
{
    std::cerr
        << color(bracket("WARN"), C_YELLOW)
        << color(bracket(tagToString(tag)), C_GRAY)
        << msg << std::endl;
}

void Logger::error(Tag tag, const std::string &msg)
{
    std::cerr
        << color(bracket("ERROR"), C_RED)
        << color(bracket(tagToString(tag)), C_GRAY)
        << msg << std::endl;
}

void Logger::debug(Tag tag, const std::string &msg)
{
    std::cerr
        << color(bracket("DEBUG"), C_CYAN)
        << color(bracket(tagToString(tag)), C_GRAY)
        << msg << std::endl;
}
