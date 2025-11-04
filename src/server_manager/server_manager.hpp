/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_manager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:16:11 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/03 12:06:22 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*              store important information from raw request                  */
/*============================================================================*/

#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

/*
    if there is an error,  throw an exception. server manager will catch it. 
*/
#include "socket_builder.hpp"


class ServerManager
{
	private:
    // 1(Configuration)
    std::vector<Server>         _servers;
	// 2(sockets)
    std::vector<socket_builder>  _listening_sockets; 
	// 3 all the fds for the poll function
    std::vector<struct pollfd>  _poll_fds;
    // 4. client
    std::vector<Client>         _clients;

    // init
    void    initializeServers(const char* config_path); 
    void    setupListeningSockets();

    // --- loop ---
    void    startPollLoop();
    void    checkNewConnections();
    void    processClientIO(int client_index);
    void    closeClientConnection(int client_index);

    ServerManager(const ServerManager& other);
    ServerManager& operator=(const ServerManager& other);

public:
    ServerManager(const char* config_path);

    ~ServerManager();

    void    run(); // the heart of the webservev
};

#endif