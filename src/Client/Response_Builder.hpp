/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_Builder.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 11:31:43 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/06 12:10:37 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_BUILDER_HPP
#define RESPONSE_BUILDER_HPP

#include <string>
#include <ctime>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include "http_request.hpp" 
#include "Server.hpp" 

class Response_Builder
{
	private:
	const http_request&	m_request;
	const Server*		m_config;
	std::string         m_response_content;
	
	void    build_status_line(int status_code, const std::string& reason_phrase);
	void    add_header(const std::string& key, const std::string& value);
	void    add_body(const std::string& body_content);

	std::string handle_get();
	std::string handle_post();
	std::string handle_delete();
	std::string get_mime_type(const std::string& path) const;
	std::string get_error_page_content(int status_code) const;

	public:
	Response_Builder(const http_request& request, const Server* config);
	~Response_Builder();
	
	const std::string& build_response();
};

#endif