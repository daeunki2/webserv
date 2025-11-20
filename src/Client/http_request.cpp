/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_request.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 19:13:03 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/20 10:18:08 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "http_request.hpp"

HttpRequest::HttpRequest()
: _method(""), _uri(""), _path(""), _query(""),
  _version("HTTP/1.1"), _body(""), _keepAlive(false)
{}

HttpRequest::HttpRequest(const HttpRequest &o)
{
    *this = o;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &o)
{
    if (this != &o)
    {
        _method    = o._method;
        _uri       = o._uri;
        _path      = o._path;
        _query     = o._query;
        _version   = o._version;
        _headers   = o._headers;
        _body      = o._body;
        _keepAlive = o._keepAlive;
    }
    return *this;
}

HttpRequest::~HttpRequest() {}

/* setters */

void HttpRequest::setMethod(const std::string &m) { _method = m; }
void HttpRequest::setUri(const std::string &u) { _uri = u; }
void HttpRequest::setPath(const std::string &p) { _path = p; }
void HttpRequest::setQuery(const std::string &q) { _query = q; }
void HttpRequest::setVersion(const std::string &v) { _version = v; }
void HttpRequest::addHeader(const std::string &key, const std::string &value)
{
    _headers[key] = value;
}
void HttpRequest::setBody(const std::string &b) { _body = b; }
void HttpRequest::appendBody(const std::string &b) { _body += b; }
void HttpRequest::setKeepAlive(bool k) { _keepAlive = k; }

/* getters */

const std::string &HttpRequest::getMethod() const { return _method; }
const std::string &HttpRequest::getUri() const { return _uri; }
const std::string &HttpRequest::getPath() const { return _path; }
const std::string &HttpRequest::getQuery() const { return _query; }
const std::string &HttpRequest::getVersion() const { return _version; }
const std::map<std::string, std::string> &HttpRequest::getHeaders() const { return _headers; }
const std::string &HttpRequest::getBody() const { return _body; }
bool HttpRequest::getKeepAlive() const { return _keepAlive; }

bool HttpRequest::hasHeader(const std::string &key) const
{
    return _headers.find(key) != _headers.end();
}

std::string HttpRequest::getHeader(const std::string &key) const
{
    std::map<std::string, std::string>::const_iterator it = _headers.find(key);
    if (it == _headers.end())
        return "";
    return it->second;
}
