/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locherif <locherif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 03:14:27 by locherif          #+#    #+#             */
/*   Updated: 2025/11/02 02:12:38 by locherif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
//#include "location.hpp"
#include <map>

class Location {

    public:
        std::string path;                       // ex: /, /images, /upload, ...
        std::string root;                       // ex: ./www/images
        std::map<std::string, std::string> cgi; // ex: ".py" -> "/usr/bin/python3"
        std::string upload_store;               // ex: ./www/uploads/files
        int autoindex;                          // 0 = non mentionné, 1 = off, 2 = on
        int upload_enable;                      // 0 = non mentionné, 1 = off, 2 = on
        std::vector<std::string> allow_methods; // ex: [GET, HEAD]
        int auth_basic;                         // 0 = non mentionné, 1 = off, 2 = on
        std::string auth_user_file;             // ex: ./conf/users.txt
        std::string cgi_pass;                   // ex: ./scripts/api.py
};

class Server {

    public:
        int listen;                             // ex: 8080
        std::string server_name;                // ex: localhost
        std::string root;                       // ex: ./www/default
        std::vector<std::string> index;         // ex: [index.html, index.htm]
        std::map<int, std::string> error_pages; // ex: 404 -> /errors/404.html
        size_t client_max_body_size;            // en octets (converti depuis "5M")
        std::vector<Location> locations;        // sous-blocs location
};

class Config {
    public:
        Config();
        ~Config();
        std::vector<Server> servers; //holds all servers
};

std::vector<std::string> readFileLines(const std::string &filename);
std::vector<std::vector<std::string> > tokenizeLines(const std::vector<std::string> &lines);
int check_scopes_logic(std::vector<std::vector<std::string> > tokens);
int make_checks(std::vector<std::vector<std::string> > tokens);
int check_double_semicolon(const std::vector<std::vector<std::string> > &tokens);
int check_forbidden_comments(const std::vector<std::vector<std::string> > &tokens);



#endif