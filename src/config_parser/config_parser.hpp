/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locherif <locherif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:13:32 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/28 12:59:32 by locherif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*    take the config file, parse it, and store in server or location class   */
/*============================================================================*/


#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <iostream>
#include <string>
#include <vector>

class location
{
	private : 
	std::string destination;// location /destination {
	std::string path;
	std::vector<std::string> index; //  index files
	std::string root;
	std::string upload_store;
	std::string cgi_ext;
	std::string cgi_pass;
	std::string auto_user_file;
	bool auto_basic;
	bool autoindex;
	bool upload_enable;
	bool m_get; // allow_methood
	bool m_post; // allow_methood
	bool m_delete; // allow_methood

	public :
	location();
	location(const location& src);
	~location();
	location& operator=(const location& src);
	
	//getters
	std::string 		get_destination();
	std::string 		get_path();
	std::string 		get_root();
	std::string 		get_upload_store();
	std::string 		get_cgi_ext();
	std::string 		get_cgi_pass();
	std::string 		get_auto_user_file();
	std::vector<std::string> 	get_index();
	bool				get_on_or_off(std::string flag);
	bool				get_method(std::string flag);

	//setters
	void			set_destination(std::string input);
	void			set_path(std::string input);
	void			set_root(std::string input);
	void			set_upload_store(std::string input);
	void			set_cgi_ext(std::string input);
	void			set_cgi_pass(std::string input);
	void			set_auto_user_file(std::string input);
	void			set_index(std::vector<std::string> input);
	void			set_method(std::string flag, bool status);
	void			set_on_or_off(std::string flag, bool status);
};

#endif
/*
server {
    listen 8080;
    host 0.0.0.0;
    server_name localhost;
    root www/localhost;
    error 404 404.html;
    client_max_body_size 1M;

    location {
        path /;
        index index.html;
        listing true;
        method GET,POST;
    }
}

*/

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