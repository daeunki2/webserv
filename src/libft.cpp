/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 16:46:11 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/28 13:05:44 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.hpp"

std::vector<std::string> split(const std::string &str, char delimiter)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end;

    while ((end = str.find(delimiter, start)) != std::string::npos)
    {
        if (end != start)
            result.push_back(str.substr(start, end - start));
        start = end + 1;
    }

    if (start < str.size())
        result.push_back(str.substr(start));

    return result;
}

std::string line_extracter(std::string separator)
{
    size_t position = m_buffer.find(separator);
    
    if (position == std::string::npos)
        return "";

    std::string line = m_buffer.substr(0, position);
    
    m_buffer.erase(0, position + 2);
    
    return line;
}