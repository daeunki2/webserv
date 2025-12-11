/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:56:08 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/11 15:25:04 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* Canonical */

Server::Server()
: _listenTargets(), _serverName(""), _root(""),_clientMaxBodySize(0), _hasCgi(false)
{}

Server::Server(const Server &o)
{
    *this = o;
}

Server &Server::operator=(const Server &o)
{
    if (this != &o)
    {
        _listenTargets = o._listenTargets;
        _serverName = o._serverName;
        _root = o._root;
        _clientMaxBodySize = o._clientMaxBodySize;
        _locations = o._locations;
        _errorPages = o._errorPages;
		_hasCgi = o._hasCgi;
    }
    return *this;
}

Server::~Server() {}

/* Setters */

void Server::addListenTarget(const std::string &host, int port)
{
	ListenTarget tmp;
	tmp.host = host;
	tmp.port = port;
	_listenTargets.push_back(tmp);
}

bool Server::hasListenTarget(const std::string &host, int port) const
{
	for (size_t i = 0; i < _listenTargets.size(); ++i)
		if (_listenTargets[i].host == host && _listenTargets[i].port == port)
			return true;
	return false;
}

void Server::setServerName(const std::string &n) { _serverName = n; }
void Server::setRoot(const std::string &r) { _root = r; }
void Server::setClientMaxBodySize(long long size) { _clientMaxBodySize = size; }

void Server::addLocation(const Location &loc) { _locations.push_back(loc); }

void Server::addErrorPage(int code, const std::string &file)
{
    _errorPages.push_back(std::make_pair(code, file));
}

/* Getters */

const std::string &Server::getServerName() const { return _serverName; }
const std::string &Server::getRoot() const { return _root; }
long long Server::getClientMaxBodySize() const { return _clientMaxBodySize; }
const std::vector<Location> &Server::getLocations() const { return _locations; }
const std::vector<std::pair<int, std::string> > &Server::getErrorPages() const { return _errorPages; }
const std::vector<Server::ListenTarget> &Server::getListenTargets() const { return _listenTargets; }

const Location *Server::findLocation(const std::string &path) const
{
    const Location *best = 0;
    size_t bestLen = 0;

    for (size_t i = 0; i < _locations.size(); ++i)
    {
        const std::string &lp = _locations[i].getPath();
        if (lp.empty())
            continue;

        if (path.compare(0, lp.size(), lp) == 0 && lp.size() > bestLen)
        {
            best = &_locations[i];
            bestLen = lp.size();
        }
    }
    return best;
}
