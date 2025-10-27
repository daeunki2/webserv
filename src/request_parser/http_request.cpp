/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_request.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 19:13:03 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/27 13:52:16 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http_request.hpp"

// ******************************************************
//             Private Static Helper Function
// ******************************************************

std::string http_request::to_lower(const std::string& str)
{
std::string result;
    // 결과 문자열의 크기를 미리 예약하여 성능 향상 (선택 사항)
    result.reserve(str.size()); 
    for(size_t i = 0; i < str.size(); ++i)
    {
        result.push_back(static_cast<char>(std::tolower(static_cast<int>(str[i]))));
    }
    return result;
}

// ******************************************************
//              Constructors & Destructor
// ******************************************************

http_request::http_request()
: m_method(""), m_uri_path(""), m_query_string(""), m_version("HTTP/1.1"), m_headers(), m_body(""), m_content_length(0), m_is_chunked(false)
{
}

http_request::http_request(const http_request& src)
: m_method(src.m_method), m_uri_path(src.m_uri_path), m_query_string(src.m_query_string), m_version(src.m_version), m_headers(src.m_headers), m_body(src.m_body), m_content_length(src.m_content_length), m_is_chunked(src.m_is_chunked)
{
    
}

http_request& http_request::operator=(const http_request& src)
{
    if (this != &src)
	{
        m_method = src.m_method;
        m_uri_path = src.m_uri_path;
        m_query_string = src.m_query_string;
        m_version = src.m_version;
        m_headers = src.m_headers;
        m_body = src.m_body;
        m_content_length = src.m_content_length;
        m_is_chunked = src.m_is_chunked;
    }
    return *this;
}

http_request::~http_request()
{
	
}

// ******************************************************
//                       Getter
// ******************************************************

const std::string& http_request::get_method() const
{
	return m_method;
}

const std::string& http_request::get_uri_path() const
{
	return m_uri_path;
}

const std::string& http_request::get_query_string() const
{
	return m_query_string;
}

const std::string& http_request::get_version() const
{
	return m_version;
}

const std::map<std::string, std::string>& http_request::get_headers() const
{
	return m_headers;
}

const std::string& http_request::get_body() const
{
	return m_body;
}

long long http_request::get_content_length() const
{
	return m_content_length;
}

bool http_request::is_chunked() const
{
	return m_is_chunked;
}

const std::string http_request::get_header_value(const std::string &key) const
{
    std::string lower_key = to_lower(key);
    
    std::map<std::string, std::string>::const_iterator it = m_headers.find(lower_key);
    
    if (it != m_headers.end())
	{
        return it->second;
	}
    return "";
}

// ******************************************************
//                       Setter
// ******************************************************

void http_request::set_method(const std::string &method)
{
	m_method = method;
}

void http_request::set_uri_path(const std::string &path)
{
	m_uri_path = path;
}

void http_request::set_query_string(const std::string &query)
{
	m_query_string = query;
}

void http_request::set_version(const std::string &version)
{
	m_version = version;
}

void http_request::set_content_length(long long length)
{
	m_content_length = length;
}

void http_request::set_is_chunked(bool chunked)
{
	m_is_chunked = chunked;
}

void http_request::add_header(const std::string &key, const std::string &value)
{
    std::string lower_key = to_lower(key);
    m_headers[lower_key] =  value;
}

void http_request::append_body(const std::string &data)
{
    m_body.append(data);
}

void http_request::reset()
{
    m_method.clear();
    m_uri_path.clear();
    m_query_string.clear();
    m_version = "HTTP/1.1";
    m_headers.clear();
    m_body.clear();
    m_content_length = 0;
    m_is_chunked = false;
}