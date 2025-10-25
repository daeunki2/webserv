/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:56:12 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/25 15:15:45 by daeunki2         ###   ########.fr       */
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
		destination = src.destination;
		path = src.path;
		index = src.index;
		root= src.root;
		upload_store = src.upload_store;
		cgi_ext =cgi_ext;
		cgi_pass = cgi_pass;
		auto_user_file = src.auto_user_file;
		auto_basic = src.auto_basic;
		autoindex = src.autoindex;
		upload_enable = src.upload_enable;
		m_get = src.m_get; 
		m_post = src.m_post;
		m_delete = src.m_delete;
	}
	return (*this);
};




//getters
std::string location::get_destination()
{
	return(destination);
};
std::string location::get_path()
{
	return(path);	
};

std::string location::get_root()
{
	return(root);
};
std::string location::get_upload_store()
{
	return(upload_store);
};
std::string location::get_cgi_ext()
{
	return(cgi_ext);
};
std::string location::get_cgi_pass()
{
	return(cgi_pass);
};
std::string location::get_auto_user_file()
{
	return(auto_user_file);
};

std::vector<std::string>	location::get_index()
{
	return(index);
}

bool	location::get_on_or_off(std::string flag)
{
	if(flag == "auto_basic")
		return (auto_basic);
	else if (flag == "autoindex")
		return(autoindex);
	else
		return(upload_enable);
}


bool location::get_method(std::string flag)
{
	if(flag == "get")
		return (m_get);
	else if (flag == "delete")
		return(m_delete);
	else
		return(m_post);
};



//setters
void location::set_destination(std::string input)
{
	destination = input;
};
void location::set_path(std::string input)
{
	path = input;
};

void	location::set_root(std::string input)
{
	root = input;
};
void	location::set_upload_store(std::string input)
{
	upload_store = input;
};
void	location::set_cgi_ext(std::string input)
{
	cgi_ext = input;
};
void	location::set_cgi_pass(std::string input)
{
	cgi_pass = input;
};
void	location::set_auto_user_file(std::string input)
{
	auto_user_file = input;
};
void	location::set_index(std::vector<std::string> input)
{
	index = input;
};
void	location::set_on_or_off(std::string flag, bool status)
{
	if(flag == "auto_basic")
		auto_basic = status;
	else if (flag == "autoindex")
		autoindex= status;
	else
		upload_enable = status;
};

void	location::set_method(std::string flag, bool status)
{
	if(flag == "get")
		m_get = status;
	else if (flag == "delete")
		m_delete = status;
	else
		m_post = status;
};