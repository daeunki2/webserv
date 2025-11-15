/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_control.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 10:54:46 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/05 11:12:45 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#ifndef ERROR_CONTROL_HPP
#define ERROR_CONTROL_HPP

#include <exception>
#include <string>

enum ErrorType
{
	BLUE,
	YELLO,
	RED
};

class error_control : public std::exception
{
	private:
	std::string _msg;

	static std::string getColorCode(ErrorType color);

	public:
	error_control::error_control(ErrorType color, const std::string& location, const std::string& msg)
	~error_control() throw();
	const char* what() const throw();
};

#endif
