/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 17:41:19 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/29 17:41:22 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Error.hpp"
#include <sstream>

/* Canonical */

Error::Error()
: _msg("Unknown error")
{}

Error::Error(const std::string &msg, const char *file, int line)
{
    _msg = formatMsg(msg, file, line);
}

Error::Error(const Error &o)
: _msg(o._msg)
{}

Error &Error::operator=(const Error &o)
{
    if (this != &o)
        _msg = o._msg;
    return *this;
}

Error::~Error() throw() {}

const char *Error::what() const throw()
{
    return _msg.c_str();
}

/**********************************
 * helpers
 **********************************/
std::string Error::formatMsg(const std::string &msg, const char *file, int line) const
{
    std::ostringstream ss;
    ss << msg << " (" << file << ":" << toStringLine(line) << ")";
    return ss.str();
}

std::string Error::toStringLine(int line) const
{
    std::ostringstream ss;
    ss << line;
    return ss.str();
}
