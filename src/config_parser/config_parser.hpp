/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:13:32 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/14 11:42:15 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*    take the config file, parse it, and store in server or location class   */
/*============================================================================*/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "server.hpp"
#include "location.hpp"
#include "../libft/libft.hpp"

class config_parser
{
private:
    std::vector<server>      _servers;
    std::vector<std::string> tokens;
    std::string              file_name;

    // 내부 유틸리티
    std::vector<std::string> read_file_lines() const;
    void                     tokenize(const std::vector<std::string> &lines);

    void parse_server_block(size_t &i);
    void parse_location_block(server &sv, size_t &i);

    void expect(const std::string &token, const std::string &expected) const;

public:
    config_parser();
	config_parser(const std::string filename);
    config_parser(const config_parser &other);
    config_parser& operator=(const config_parser &other);
    ~config_parser();

    // ✔ 메인 파싱 함수
    void parse(const std::string &filename);

    const std::vector<server>& get_servers() const;
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