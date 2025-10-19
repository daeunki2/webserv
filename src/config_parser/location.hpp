/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:56:05 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/19 18:25:27 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <string>
#include <vector>
/*
ps. if you don't like the name, you cna change! 
*/
class location
{
	private : 
	std::string destination;// location /destination {
	std::string path;
	std::vector<int> index; // fd of index files
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
	std::vector<int> 	get_index();
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
	void			set_index(std::vector<int> input);
	void			set_method(std::string flag, bool status);
	void			set_on_or_off(std::string flag, bool status);
};

#endif