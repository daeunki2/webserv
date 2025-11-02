/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 12:35:55 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/30 12:57:10 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"

// ******************************************************
//              Constructors & Destructor
// ******************************************************
response::response() 
: m_status_code(0), m_bytes_sent(0)
{

};

response::response(const response& src)
: m_status_code(src.m_status_code), m_headers(src.m_headers), m_body(src.m_body), m_raw_response(src.m_raw_response), m_bytes_sent(src.m_bytes_sent)
{

};

response& response::operator=(const response& src)
{
	if (this != &src)
	{
		m_status_code = src.m_status_code;
		m_headers = src.m_headers;
		m_body = src.m_body;
		m_raw_response = src.m_raw_response;
		m_bytes_sent = src.m_bytes_sent;
	}
	return *this;
}

response::~response()
{

}

// ******************************************************
//                        Getter
// ******************************************************

// Getter
int		response::get_status_code() const
{
	return m_status_code;
};

const std::map<std::string, std::string>&	response::get_headers() const
{
	return m_headers;
};

const std::vector<char>&					response::get_body() const
{
	return m_body;
};

const std::vector<char>&					response::get_raw_response() const
{
	return m_raw_response;
};

size_t										response::get_bytes_sent() const
{
	return m_bytes_sent;
};
	

// ******************************************************
//                        Setter
// ******************************************************
void		response::set_status_code(int code)const
{
	m_status_code = code;
};

void		response::add_header(const std::string& key, const std::string& value)const
{
	m_headers[key] = value;
};
void		response::set_body(const std::vector<char>& body)const
{
	m_body = body;
};
void		response::append_body(const std::vector<char>& data)const
{
	m_body.insert(m_body.end(), data.begin(), data.end());
};
void		response::set_bytes_sent(size_t sent_count)const
{
	m_bytes_sent = sent_count;
};

// ******************************************************
//                        utile
// ******************************************************

void 		response::reset()const
{
		m_status_code = 0;
		m_headers.clear();
		m_body.clear();
		m_raw_response.clear(); 
		m_bytes_sent = 0;
};

void	response::clear_raw_response()const
{
	m_raw_response.clear();
};

void		response::finalize_response()const
{

}; 