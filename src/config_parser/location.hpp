/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:56:05 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/19 17:45:19 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <string>
/*
ps. if you don't like the name, you cna change! 
*/
class location
{
	private : 
	std::string destination;
	std::string path;
	int index; 
	bool m_get;
	bool m_post;
	bool m_delete;

	public :
	location();
	location(const location& src);
	~location();
	location& operator=(const location& src);
	
	//getters
	std::string get_destination();
	std::string get_path();
	int			get_index();
	bool		get_method(std::string flag);

	//setters
	void		set_destination(std::string input);
	void		set_path(std::string input);
	void		set_index(int input);
	void		set_method(std::string flag, bool status);
};

#endif