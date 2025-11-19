/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 20:09:10 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/19 13:17:12 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "../src/config_parser/config_parser.hpp"
#include "../src/config_parser/Server.hpp"
#include "../src/config_parser/Location.hpp"
#include "../src/libft/Logger.hpp"
#include "../src/libft/Error.hpp"


static void printLocationInfo(const Location &loc)
{
    std::cout << "  [LOCATION] " << loc.getPath() << std::endl;
    std::cout << "    root: " << loc.getRoot() << std::endl;
    std::cout << "    index: " << loc.getIndex() << std::endl;
    std::cout << "    autoindex: " << (loc.getAutoindex() ? "on" : "off") << std::endl;
    std::cout << "    upload path: " << loc.getUploadPath() << std::endl;

    // Methods
    const std::vector<std::string> &m = loc.getMethods();
    if (!m.empty())
    {
        std::cout << "    allowed methods: ";
        for (size_t i = 0; i < m.size(); i++)
            std::cout << m[i] << " ";
        std::cout << std::endl;
    }

    // Redirect
    if (loc.isRedirect())
    {
        std::cout << "    redirect: " << loc.getRedirectCode()
                << " -> " << loc.getRedirectUrl() << std::endl;
    }

    // CGI
    if (!loc.getCgiExtension().empty())
    {
        std::cout << "    cgi: (" << loc.getCgiExtension()
                  << ") " << loc.getCgiPath() << std::endl;
    }
}

static void printServerInfo(const Server &srv)
{
    std::cout << "==============================\n";
    std::cout << "SERVER" << std::endl;
    std::cout << "------------------------------\n";
    std::cout << "  port: " << srv.getPort() << std::endl;
    std::cout << "  server_name: " << srv.getServerName() << std::endl;
    std::cout << "  root: " << srv.getRoot() << std::endl;
    std::cout << "  client_max_body_size: " << srv.getClientMaxBodySize() << std::endl;

    // error pages
    const std::vector<std::pair<int,std::string> > &errs = srv.getErrorPages();
    if (!errs.empty())
    {
        std::cout << "  error_pages:" << std::endl;
        for (size_t i = 0; i < errs.size(); i++)
        {
            std::cout << "    " << errs[i].first << " -> " << errs[i].second << std::endl;
        }
    }

    // locations
    const std::vector<Location> &locs = srv.getLocations();
    for (size_t i = 0; i < locs.size(); i++)
        printLocationInfo(locs[i]);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./test_config <config_file>" << std::endl;
        return 1;
    }

    try
    {
        ConfigParser parser;
        std::vector<Server> servers = parser.parse(argv[1]);

        std::cout << "=== Parsed " << servers.size() << " server blocks ===\n\n";

        for (size_t i = 0; i < servers.size(); i++)
            printServerInfo(servers[i]);
    }
    catch (const Error &e)
    {
        Logger::error(e.what());
    }
    catch (const std::exception &e)
    {
        Logger::error(e.what());
    }

    return 0;
}

// #include "config_parser.hpp"
// #include "server_manager.hpp"

// int main(int ac, char **av)
// {
// 	if (ac == 1 || ac == 2)
// 	{
// 		try
// 		{
// 			std::string config_filename;
// 			config_parser config;
// 			server_manager webserv;
// 			if (ac == 1)
// 				config_filename = "default";
// 			else
// 				config_filename = av[1];
// 			config(config_filename);
	
// 			wenserv(config.get_servers());
// 			webserv.run();
// 		}
// 		catch(const std::exception& e)
// 		{
// 			std::cerr << e.what() << '\n';
// 			return 1;
// 		}
		
// 	}
// 	else
// 	{
// 		std::cout << "Error: wrong arguments" << std::endl;
// 		return 1;
// 	}
// 	return 0;
// }