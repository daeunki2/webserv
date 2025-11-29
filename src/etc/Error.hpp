/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 17:41:25 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/29 17:41:28 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef ERROR_HPP
#define ERROR_HPP

#include <exception>
#include <string>

class Error : public std::exception
{
private:
    std::string _msg;

public:
    /* Canonical */
    Error();
    Error(const std::string &msg, const char *file, int line);
    Error(const Error &o);
    Error &operator=(const Error &o);
    ~Error() throw();

    /* override */
    const char *what() const throw();

private:
    std::string formatMsg(const std::string &msg, const char *file, int line) const;
    std::string toStringLine(int line) const;
};

#endif
