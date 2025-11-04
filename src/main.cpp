/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 20:09:10 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/03 11:55:07 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "server_manager"

int main (int ac, char **av, char **envp)
{
	int code = 0;
	const char *config;
	if (ac == 2 || ac == 1)
	{
		if (ac == 1)
			config = "";
		else
			config = av[1];
		servermanager engine(config);
	}
	else
	{
		std::cout << "USAGE : ./webserv configfile or ./webserv"
		code = 1;
	}
	return code;
};