/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 17:41:47 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/05 17:56:18 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"
#include <sstream>
#include <cctype>
#include <cstdlib>

std::string trim(const std::string &s)
{
    size_t start = 0;
    size_t end = s.size();

    while (start < end && std::isspace(s[start]))
        start++;
    while (end > start && std::isspace(s[end - 1]))
        end--;

    return s.substr(start, end - start);
}



std::string to_lower(const std::string& s)
{
    std::string result;
    result.reserve(s.size());
    for (std::string::size_type i = 0; i < s.size(); ++i)
        result.push_back(static_cast<char>(std::tolower(static_cast<int>(s[i]))));
    return result;
}

std::vector<std::string> split(const std::string &s)
{
    std::vector<std::string> res;
    std::istringstream iss(s);
    std::string token;

    while (iss >> token)
        res.push_back(token);

    return res;
}

char *ft_strdup(const char *str)
{
    if (!str)
        return NULL;

    int len = 0;
    while (str[len] != '\0')
        ++len;

    char *dup = new char[len + 1];
    for (int i = 0; i < len; ++i)
        dup[i] = str[i];

    dup[len] = '\0';
    return dup;
}




std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> res;
    std::string token;
    std::istringstream iss(s);

    while (std::getline(iss, token, delim))
        res.push_back(token);

    return res;
}

bool isNumber(const std::string &s)
{
    if (s.empty()) return false;

    for (size_t i = 0; i < s.size(); i++)
        if (!std::isdigit(s[i]))
            return false;

    return true;
}

int toInt(const std::string &s)
{
    return std::atoi(s.c_str());
}

long long toLLong(const std::string &s)
{
    return std::atoi(s.c_str());
}

std::string toString(int n)
{
    std::ostringstream ss;
    ss << n;
    return ss.str();
}
