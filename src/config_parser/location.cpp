/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:56:12 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/19 17:46:40 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "location.hpp"

location::location()
{
	m_get = false;
	m_delete = false;
	m_post = false;
};
location::location(const location& src)
{
	*this = src;
};
location::~location()
{
	
};
location& location::operator=(const location& src)
{
	if (this != &src)
	{
		
	}
	return (*this)
};	
std::string location::get_destination()
{
	return(destination);
}
std::string location::get_path()
{
	return(path);	
}
int	location::get_index()
{
	return(index);
}
bool location::get_method(std::string flag)
{
	if(flag == "get")
		return (m_get);
	else if (flag == "delete")
		return(m_delete);
	else
		return(m_post);
}

void location::set_destination(std::string input)
{
	destination = input;
}
void location::set_path(std::string input)
{
	path = input;
}
void location::set_index(int input)
{
	index = input;
}
void location::set_method(std::string flag, bool status)
{
	if(flag == "get")
		m_get = status;
	else if (flag == "delete")
		m_delete = status;
	else
		m_post = status;
}