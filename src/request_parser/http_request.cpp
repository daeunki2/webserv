/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_request.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 19:13:03 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/25 21:11:33 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http_request.hpp"

http_request::http_request()
: m_method(""), m_uri_path(""), m_query_string(""), m_version("HTTP/1.1"), m_headers(), m_body("")
{

};

// 복사 생성자
http_request::http_request(const http_request& src)
: m_method(src.m_method), m_uri_path(src.m_uri_path), m_query_string(src.m_query_string), m_version(src.m_version), m_headers(src.m_headers), m_body(src.m_body), m_content_length(src.m_content_length), m_transfer_encoding(src.m_transfer_encoding)
{
	
};

http_request& http_request::operator=(const http_request& src)
{
    if (this != &src) {
        m_method = src.m_method;
        m_uri_path = src.m_uri_path;
        m_query_string = src.m_query_string;
        m_version = src.m_version;
        m_headers = src.m_headers;
        m_body = src.m_body;
        m_content_length = src.m_content_length;
        m_transfer_encoding = src.m_transfer_encoding;
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
const std::string& http_request::get_uri_path() const
{
	return m_uri_path;
};

const std::string& http_request::get_query_string() const
{
	return m_query_string;
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
long long http_request::get_content_length() const
{
	return m_content_length;
};
const std::string& http_request::get_transfer_encoding() const
{
	return m_transfer_encoding;
};


// Setter
void http_request::set_method(const std::string &method)
{
	m_method = method;
};
void http_request::set_uri_path(const std::string &path)
{
	m_uri_path = path;
};

void http_request::set_query_string(const std::string &path)
{
	m_query_string = path;
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
void http_request::set_content_length(long long length)
{
	m_content_length = length;
};
void http_request::set_transfer_encoding(const std::string &encoding)
{
	m_transfer_encoding = encoding;
};


bool http_request::is_chunked() const
{
	return m_transfer_encoding.find("chunked") != std::string::npos;
}
