/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:56:01 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/19 13:11:01 by daeunki2         ###   ########.fr       */
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
    int _port;
    std::string _serverName;
    std::string _root;
    size_t _clientMaxBodySize;

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
    void setServerName(const std::string &n);
    void setRoot(const std::string &r);
    void setClientMaxBodySize(size_t size);

    void addLocation(const Location &loc);
    void addErrorPage(int code, const std::string &file);

    /* Getters */
    int getPort() const;
    const std::string &getServerName() const;
    const std::string &getRoot() const;
    size_t getClientMaxBodySize() const;

    const std::vector<Location> &getLocations() const;
    const std::vector<std::pair<int, std::string> > &getErrorPages() const;
};

#endif
