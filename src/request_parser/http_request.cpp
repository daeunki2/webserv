/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_request.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 19:13:03 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/23 19:52:25 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http_request.hpp"

http_request::http_request()
: m_method(""), m_path(""), m_version("HTTP/1.1"), m_headers(), m_body("")
{

};

// 복사 생성자
http_request::http_request(const http_request &src)
: m_method(src.m_method), m_path(src.m_path), m_version(src.m_version), m_headers(src.m_headers), m_body(src.m_body)
{

};

// 대입 연산자
http_request& http_request::operator=(const http_request &src)
{
	if (this != &src)
	{
		m_method = src.m_method;
		m_path = src.m_path;
		m_version = src.m_version;
		m_headers = src.m_headers;
		m_body = src.m_body;
	}
	return *this;
}

// 소멸자
http_request::~http_request()
{

}




// Getter
const std::string& http_request::get_method() const
{
	return m_method;
};
const std::string& http_request::get_path() const
{
	return m_path;
};
const std::string& http_request::get_version() const
{
	return m_version;
};
const std::map<std::string, std::string>& http_request::get_headers() const
{
	return m_headers;
};
const std::string& http_request::get_body() const
{
	return m_body;
};



// Setter
void http_request::set_method(const std::string &method)
{
	m_method = method;
};
void http_request::set_path(const std::string &path)
{
	m_path = path;
};
void http_request::set_version(const std::string &version)
{
	m_version = version;
};
void http_request::add_header(const std::string &key, const std::string &value)
{
	m_headers[key] =  value;
};
void http_request::set_body(const std::string &body)
{
	m_body = body;
};
