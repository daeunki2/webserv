/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:13:38 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/14 11:44:26 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_parser.hpp"

// ******************************************************
//            constructer 
// ******************************************************

config_parser::config_parser()
{
}

config_parser::config_parser(const std::string filename)
:file_name(filename);
{
}

config_parser::config_parser(const config_parser &other)
{
    *this = other;
}

config_parser& config_parser::operator=(const config_parser &other)
{
    if (this != &other)
    {
        this->_servers  = other._servers;
        this->tokens    = other.tokens;
        this->file_name = other.file_name;
    }
    return *this;
}

config_parser::~config_parser()
{
}


// ******************************************************
//            			helper
// ******************************************************

std::vector<std::string> config_parser::read_file_lines() const
{
    std::vector<std::string> lines;
    std::ifstream file(file_name.c_str());

    if (!file.is_open())
    {
        std::cerr << "Error: could not open file " << file_name << std::endl;
        return lines;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty())
            lines.push_back(line);
    }

    file.close();
    return lines;
}

void	config_parser::tokenize(const std::vector<std::string> &lines)
{
	
}

// ******************************************************
//            			main
// ******************************************************