/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_control.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 10:51:43 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/05 11:11:31 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error_control.hpp"

#include "error_control.hpp"

error_control::error_control(ErrorType type, const std::string& location, const std::string& msg)
{
_msg = getColorCode(type) + "[ " + location + " ] : " + msg + "\033[0m";
}

error_control::~error_control() throw() {}

const char* error_control::what() const throw()
{
return _msg.c_str();
}

std::string error_control::getColorCode(ErrorType type)
{
switch(type)
{
case ErrorType::INFO:    return "\033[34m"; // blue
case ErrorType::WARNING: return "\033[33m"; // yello
case ErrorType::ERROR:   return "\033[31m"; // red
default:                 return "\033[0m";  // reset
}
}
