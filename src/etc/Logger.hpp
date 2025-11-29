/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 17:41:39 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/29 17:41:40 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>

class Logger
{
public:
    /* Log tag */
    enum Tag {
        TAG_EVENT,
        TAG_REQ,
        TAG_CGI,
        TAG_POLL,
        TAG_CONF,
        TAG_FD,
        TAG_TIMEOUT,
        TAG_CORE,
        TAG_UNKNOWN
    };

    /* Canonical */
    Logger();
    Logger(const Logger &o);
    Logger &operator=(const Logger &o);
    ~Logger();

    /* Static APIs */
    static void info(Tag tag, const std::string &msg);
    static void warn(Tag tag, const std::string &msg);
    static void error(Tag tag, const std::string &msg);
    static void debug(Tag tag, const std::string &msg);

private:
    static std::string color(const std::string &msg, const std::string &code);
    static std::string bracket(const std::string &s);
    static std::string tagToString(Tag tag);
};

#endif
