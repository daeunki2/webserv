/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:56:01 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/23 19:13:32 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*                  store the information in server scoope                    */
/*============================================================================*/

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

    std::vector<int>			ports;
    std::string					server_name;
    std::string					root;
    std::vector<std::string>	index_files;
    size_t						client_max_body_size;
    std::vector<std::string>	error_pages;
    std::vector<location>		locations; //locations!!

	public:
	
};

#endif