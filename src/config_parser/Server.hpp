/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:56:01 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/01 16:48:12 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*                  store the information in server scoope                    */
/*============================================================================*/

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include "Location.hpp"

class Server
{
private:
    std::string _host;
    int _port;
    std::string _serverName;
    std::string _root;
    long long _clientMaxBodySize;
	bool        _hasCgi;
	std::string _cgiExtension;
	std::string _cgiPath;

	std::vector<Location> _locations;
    std::vector<std::pair<int, std::string> > _errorPages; // code → file

public:
    /* Canonical form */
    Server();
    Server(const Server &o);
    Server &operator=(const Server &o);
    ~Server();

    /* Setters */
    void setPort(int p);
    void setHost(const std::string &h);
    void setServerName(const std::string &n);
    void setRoot(const std::string &r);
    void setClientMaxBodySize(long long size);

    void addLocation(const Location &loc);
    void addErrorPage(int code, const std::string &file);

    /* Getters */
    const std::string &getHost() const;
    int getPort() const;
    const std::string &getServerName() const;
    const std::string &getRoot() const;
    long long getClientMaxBodySize() const;

    const std::vector<Location> &getLocations() const;
    const std::vector<std::pair<int, std::string> > &getErrorPages() const;

    const Location *findLocation(const std::string &path) const;
};

#endif
