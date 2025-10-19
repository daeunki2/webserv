/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:56:01 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/19 16:55:14 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <string>
#include <vector>
#include "location.hpp"

class location;  

class server
{
	private: 
	std::vector<location> locations;
	std::vector<int> port;
	std::string host;
	std::string root;

	public:
	
};

#endif