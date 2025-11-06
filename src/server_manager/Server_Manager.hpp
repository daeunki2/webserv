/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Manager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:16:11 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/06 10:53:30 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*              store important information from raw request                  */
/*============================================================================*/

#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

#include <vector>
#include <map>
#include <poll.h>
#include <fcntl.h>
#include <cerrno>
#include <iostream>
#include <unistd.h>
#include <exception>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <algorithm>
#include <ctime>
#include "Server.hpp" 
#include "Client.hpp" 
#include "error_control.hpp" 

#define RECV_BUFFER_SIZE 65536
#define IDLE_TIMEOUT_SECONDS 60

class Server_Manager
{
private:
    std::vector<Server> servers;                 
    std::vector<int> listening_fds;              
    std::map<int, Server*> fd_to_server;         
    std::map<int, Client> clients;               
    std::vector<struct pollfd> poll_fds;         

    void init_sockets();                           
    void set_fd_non_blocking(int fd);
    void check_idle_clients();
    void accept_new_client(int server_fd);
    
    bool receive_request(int client_fd);
    bool send_response(int client_fd);
    
    void close_connection(int client_fd);

    bool is_listening_fd(int fd) const;
    Server* get_server_by_fd(int fd);

public:
    Server_Manager();
    explicit Server_Manager(const std::vector<Server>& servers);
    ~Server_Manager();

    void run();
};

#endif