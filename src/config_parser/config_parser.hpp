/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:13:32 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/19 13:18:27 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*    take the config file, parse it, and store in server or location class   */
/*============================================================================*/
#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <vector>
#include <string>
#include <fstream>
#include "Server.hpp"
#include "Location.hpp"
#include "../libft/Logger.hpp"
#include "../libft/Error.hpp"
#include "../libft/Utils.hpp"


class ConfigParser
{
private:
    std::vector<std::string> _lines;
    std::vector<std::string> _tokens;
    size_t _i;

public:
    // Canonical form
    ConfigParser();
    ConfigParser(const ConfigParser &o);
    ConfigParser &operator=(const ConfigParser &o);
    ~ConfigParser();

    std::vector<Server> parse(const std::string &path);

private:
    /* tokenizer */
    void tokenize();
    bool hasNext();
    const std::string &peek();
    const std::string &next();
    void expect(const std::string &t);
    bool consume(const std::string &t);

    /* blocks */
    void parseServerBlock(std::vector<Server> &servers);
    void parseLocationBlock(Server &srv);

    /* helpers */
    int toIntChecked(const std::string &s);
};

#endif
