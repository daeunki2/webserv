/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:13:32 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/20 10:49:18 by daeunki2         ###   ########.fr       */
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
#include "../etc/Logger.hpp"
#include "../etc/Error.hpp"
#include "../etc/Utils.hpp"


class ConfigParser
{
private:
    std::vector<std::string> _lines;
    std::vector<std::string> _tokens;
    size_t _i;
	std::vector<Server> _servers;
	
public:
    // Canonical form
    ConfigParser();
	ConfigParser(const std::string &file);
    ConfigParser(const ConfigParser &o);
    ConfigParser &operator=(const ConfigParser &o);
    ~ConfigParser();

	void	parse(const std::string &path);
	const std::vector<Server> &getServers() const;

	private:
    /* tokenizer */
    void tokenize();
    bool hasNext();
    const std::string &peek();
    const std::string &next();
    void expect(const std::string &t);
    bool consume(const std::string &t);

    /* blocks */
    void parseServerBlock();
    void parseLocationBlock(Server &srv);

};

#endif
