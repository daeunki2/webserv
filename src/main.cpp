/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 20:09:10 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/05 09:43:25 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_parser.hpp"
#include "server_manager.hpp"

int main(int ac, char **av)
{
	if (ac == 1 || ac == 2)
	{
		try
		{
			std::string config_filename;
			config_parser config;
			server_manager webserv;

			if (ac == 1)
				config_filename = "default.config";
			else
				config_filename = av[1];
			config(config_filename);
			wenserv(config.get_servers());
			webserv.run();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			return 1;
		}
		
	}
	else
	{
		std::cout << "Error: wrong arguments" << std::endl;
		return 1;
	}
	return 0;
}