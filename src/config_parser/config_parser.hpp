/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:13:32 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/30 13:58:25 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*    take the config file, parse it, and store in server or location class   */
/*============================================================================*/


#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "ServerConfig.hpp"
# include <string>
# include <vector>
# include <fstream>
# include <sstream>
# include <stdexcept>
# include <iostream>

class ConfigParser
{
	private:
	std::vector<server>   _servers;
	std::vector<std::string>    _tokens;
	size_t                      _current_index;

	void                        _tokenize(const std::string& filename);
	std::string                 _getNextToken();
	std::string                 _peekNextToken() const;
	void                        _expectToken(const std::string& expected);
	long                        _parseSize(const std::string& size_str); // 5M -> bytes 변환

	ServerConfig                _parseServerBlock();
	LocationConfig              _parseLocationBlock(ServerConfig& current_server);

	void                        _parseServerDirective(ServerConfig& server);
	void                        _parseLocationDirective(LocationConfig& location);

	void                        _parseIndex(std::vector<std::string>& index_files);

	public:
	ConfigParser();
	~ConfigParser();

	void parse(const std::string& filename);
	const std::vector<ServerConfig>& getServers() const;
};

#endif

/*
logic
0. there is 3 status. in_server, in_location, none
1. read from the start to eod.
2. when I find "server {", change status_in_server and start a small function to store server info.
2-1. when I find "location {" start small funtion for store locaion info.
2-2. store informations.
2-3. when i find"}" stop a function for location and chage status to in_server
2-4. when I find "}", stop a function for server and change status to none
3. reapat this till i see eof.

idea.
1. i need a big class parser, and store the value in server and location 
2. in server class, i have location as a list or vector(advanced array).
*/