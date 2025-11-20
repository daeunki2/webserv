/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:56:08 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/19 13:11:12 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* Canonical */

Server::Server()
: _port(0), _serverName(""), _root(""),
  _clientMaxBodySize(1000000) // 1MB default
{}

Server::Server(const Server &o)
{
    *this = o;
}

Server &Server::operator=(const Server &o)
{
    if (this != &o)
    {
        _port = o._port;
        _serverName = o._serverName;
        _root = o._root;
        _clientMaxBodySize = o._clientMaxBodySize;
        _locations = o._locations;
        _errorPages = o._errorPages;
    }
    return *this;
}

Server::~Server() {}

/* Setters */

void Server::setPort(int p) { _port = p; }
void Server::setServerName(const std::string &n) { _serverName = n; }
void Server::setRoot(const std::string &r) { _root = r; }
void Server::setClientMaxBodySize(size_t size) { _clientMaxBodySize = size; }

void Server::addLocation(const Location &loc) { _locations.push_back(loc); }

void Server::addErrorPage(int code, const std::string &file)
{
    _errorPages.push_back(std::make_pair(code, file));
}

/* Getters */

int Server::getPort() const { return _port; }
const std::string &Server::getServerName() const { return _serverName; }
const std::string &Server::getRoot() const { return _root; }
size_t Server::getClientMaxBodySize() const { return _clientMaxBodySize; }
const std::vector<Location> &Server::getLocations() const { return _locations; }
const std::vector<std::pair<int, std::string> > &Server::getErrorPages() const { return _errorPages; }
