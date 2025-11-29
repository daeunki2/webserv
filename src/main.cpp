/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 10:03:10 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/29 17:42:58 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <iostream>
#include <string>
#include "config_parser.hpp"
#include "Server_Manager.hpp"
#include "Logger.hpp"
#include "Error.hpp"


extern volatile sig_atomic_t g_running;
extern void signal_handler(int);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        Logger::error(Logger::TAG_CORE, "Usage : ./webserv config.txt");
        return 1;
    }
    try
    {
		signal(SIGINT, signal_handler);
		signal(SIGTERM, signal_handler);
        std::string configFile = argv[1];

        ConfigParser parser(configFile);
        const std::vector<Server> &servers = parser.getServers();

        Server_Manager manager(servers);
        manager.run();
    }
    catch (const Error &e)
    {
        Logger::error(Logger::TAG_CORE, "Fatal error: " + std::string(e.what()));
        return 1;
    }
    catch (const std::exception &e)
    {
        Logger::error(Logger::TAG_CORE,"Unhandled std::exception: " + std::string(e.what()));
        return 1;
    }
    catch (...)
    {
        Logger::error(Logger::TAG_CORE, "Unknown fatal error occurred.");
        return 1;
    }

    return 0;
}

