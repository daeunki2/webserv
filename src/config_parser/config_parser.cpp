/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locherif <locherif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:13:38 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/02 01:14:33 by locherif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "config_parser.hpp"
#include "parsing.hpp"
#include <iostream>

Config::Config()
{
    std::cout << "[Config] Constructed ✅" << std::endl;
}

Config::~Config()
{
    std::cout << "[Config] Destroyed 🧹" << std::endl;
}
//std::vector<std::string> readFileLines(const std::string &filename);

Config parser(int ac, char **av)
{
    Config config;
    if (ac != 2 || !av) {
        std::cerr << "Usage: ./webserv <config_file>" << std::endl;
        return Config();
    }
    std::vector<std::string> file = readFileLines((std::string)av[1]);
    std::vector<std::vector<std::string> > tokens = tokenizeLines(file);
    if(make_checks(tokens) == 1)
        return Config();
    
    // std::cout << "\n--- File Content ---" << std::endl;
    // for (size_t i = 0; i < file.size(); ++i) {
    //     std::cout << i << ": " << file[i] << std::endl;}
    std::cout << "Parsing OK!" << std::endl;
    return config;
}
