/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_request.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 19:13:03 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/20 19:30:05 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http_request.hpp"
#include <cctype>   // std::tolower
#include <cstdlib>  // std::strtoll (C 함수지만 C++98에서 사용 가능)


// ******************************************************
//          Canonical form
// ******************************************************

http_request::http_request()
: m_method()
, m_uri()
, m_path()
, m_query()
, m_version("HTTP/1.1")
, m_headers()
, m_body()
, m_content_length(0)
, m_has_content_length(false)
, m_is_chunked(false)
, m_keep_alive(true)
{
}

http_request::http_request(const http_request& other)
: m_method(other.m_method)
, m_uri(other.m_uri)
, m_path(other.m_path)
, m_query(other.m_query)
, m_version(other.m_version)
, m_headers(other.m_headers)
, m_body(other.m_body)
, m_content_length(other.m_content_length)
, m_has_content_length(other.m_has_content_length)
, m_is_chunked(other.m_is_chunked)
, m_keep_alive(other.m_keep_alive)
{
}

http_request& http_request::operator=(const http_request& other)
{
    if (this != &other)
    {
        m_method           = other.m_method;
        m_uri              = other.m_uri;
        m_path             = other.m_path;
        m_query            = other.m_query;
        m_version          = other.m_version;
        m_headers          = other.m_headers;
        m_body             = other.m_body;
        m_content_length   = other.m_content_length;
        m_has_content_length = other.m_has_content_length;
        m_is_chunked       = other.m_is_chunked;
        m_keep_alive       = other.m_keep_alive;
    }
    return *this;
}

http_request::~http_request()
{
}

void http_request::reset()
{
    m_method.clear();
    m_uri.clear();
    m_path.clear();
    m_query.clear();
    m_version = "HTTP/1.1";

    m_headers.clear();
    m_body.clear();

    m_content_length     = 0;
    m_has_content_length = false;
    m_is_chunked         = false;
    m_keep_alive         = true;
}

// ******************************************************
//                  Setters
// ******************************************************

void http_request::set_method(const std::string& method)
{
    m_method = method;
}

void http_request::set_uri(const std::string& uri)
{
    m_uri = uri;
    m_path.clear();
    m_query.clear();

    std::string::size_type pos = uri.find('?');
    if (pos == std::string::npos)
    {
        m_path = uri;
        m_query = "";
    }
    else
    {
        m_path  = uri.substr(0, pos);
        m_query = uri.substr(pos + 1);
    }

    if (m_path.empty())
        m_path = "/";
}

void http_request::set_version(const std::string& version)
{
    m_version = version;
}

void http_request::set_body(const std::string& body)
{
    m_body = body;
}

void http_request::append_body(const std::string& chunk)
{
    m_body.append(chunk);
}

void http_request::add_header(const std::string& name, const std::string& value)
{
    std::string lower = to_lower(name);
    m_headers[lower] = value;
}

void http_request::set_content_length(long long len)
{
    m_content_length     = len;
    m_has_content_length = true;
}

void http_request::set_chunked(bool value)
{
    m_is_chunked = value;
}

void http_request::set_keep_alive(bool value)
{
    m_keep_alive = value;
}

// ******************************************************
//                  Getters
// ******************************************************

const std::string& http_request::get_method() const
{
    return m_method;
}

const std::string& http_request::get_uri() const
{
    return m_uri;
}

const std::string& http_request::get_path() const
{
    return m_path;
}

const std::string& http_request::get_query() const
{
    return m_query;
}

const std::string& http_request::get_version() const
{
    return m_version;
}

const std::map<std::string, std::string>& http_request::get_headers() const
{
    return m_headers;
}

bool http_request::has_header(const std::string& name) const
{
    std::string lower = to_lower(name);
    std::map<std::string, std::string>::const_iterator it = m_headers.find(lower);
    return (it != m_headers.end());
}

std::string http_request::get_header(const std::string& name) const
{
    std::string lower = to_lower(name);
    std::map<std::string, std::string>::const_iterator it = m_headers.find(lower);
    if (it != m_headers.end())
        return it->second;
    return "";
}

const std::string& http_request::get_body() const
{
    return m_body;
}

long long http_request::get_content_length() const
{
    return m_content_length;
}

bool http_request::has_content_length() const
{
    return m_has_content_length;
}

bool http_request::is_chunked() const
{
    return m_is_chunked;
}

bool http_request::keep_alive() const
{
    return m_keep_alive;
}
