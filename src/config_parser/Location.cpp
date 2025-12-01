/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:56:12 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/01 18:36:20 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

/* Canonical form */

Location::Location()
: _path(""), _root(""), _index("index.html"), _autoindex(false), _uploadPath(""), _isRedirect(false), _redirectCode(0),  _clientMaxBodySize(0),  _hasClientMaxBodySize(false), _hasCgi(false) 
{}

Location::Location(const std::string &path)
: _path(path), _root(""), _index("index.html"), _autoindex(false), _uploadPath(""), _isRedirect(false), _redirectCode(0), _clientMaxBodySize(0),  _hasClientMaxBodySize(false), _hasCgi(false) 
{}

Location::Location(const Location &o)
{
    *this = o;
}

Location &Location::operator=(const Location &o)
{
    if (this != &o)
    {
        _path = o._path;
        _root = o._root;
        _methods = o._methods;
        _index = o._index;
        _autoindex = o._autoindex;
        _uploadPath = o._uploadPath;

        _isRedirect = o._isRedirect;
        _redirectCode = o._redirectCode;
        _redirectUrl = o._redirectUrl;

        _cgiExtension = o._cgiExtension;
        _cgiPath = o._cgiPath;

		_clientMaxBodySize = o._clientMaxBodySize;
		_hasClientMaxBodySize = o._hasClientMaxBodySize;
		_hasCgi = o._hasCgi;
    }
    return *this;
}

Location::~Location() {}

/* Setters */

void Location::setRoot(const std::string &r) { _root = r; }
void Location::addMethod(const std::string &m) { _methods.push_back(m); }
void Location::setIndex(const std::string &idx) { _index = idx; }
void Location::setAutoindex(bool e) { _autoindex = e; }
void Location::setUploadPath(const std::string &p) { _uploadPath = p; }
void Location::setClientMaxBodySize(size_t size)
{
    _clientMaxBodySize = size;
    _hasClientMaxBodySize = true;
}
void Location::setRedirect(int code, const std::string &url)
{
    _isRedirect = true;
    _redirectCode = code;
    _redirectUrl = url;
}

void Location::setCgi(const std::string &ext, const std::string &path)
{
	_hasCgi = true;
    _cgiExtension = ext;
    _cgiPath = path;
}

/* Getters */

const std::string &Location::getPath() const { return _path; }
const std::string &Location::getRoot() const { return _root; }
const std::vector<std::string> &Location::getMethods() const { return _methods; }
const std::string &Location::getIndex() const { return _index; }
bool Location::getAutoindex() const { return _autoindex; }
const std::string &Location::getUploadPath() const { return _uploadPath; }

bool Location::isRedirect() const { return _isRedirect; }
int Location::getRedirectCode() const { return _redirectCode; }
const std::string &Location::getRedirectUrl() const { return _redirectUrl; }

const std::string &Location::getCgiExtension() const { return _cgiExtension; }
const std::string &Location::getCgiPath() const { return _cgiPath; }
bool Location::hasClientMaxBodySize() const
{
    return _hasClientMaxBodySize;
}

size_t Location::getClientMaxBodySize() const
{
    return _clientMaxBodySize;
}

bool Location::hasCgi() const
{
	return _hasCgi;
}
