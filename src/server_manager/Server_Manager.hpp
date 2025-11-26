/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Manager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:16:11 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/24 15:29:27 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*              store important information from raw request                  */
/*============================================================================*/
#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

# include <vector>
# include <map>
# include <poll.h>
# include <fcntl.h>
# include <cerrno>
# include <iostream>
# include <unistd.h>
# include <string>
# include <cstring>
# include <algorithm>
# include <ctime>
# include <netinet/in.h>
# include <sys/socket.h>
#include <signal.h>

# include "Server.hpp"
# include "Client.hpp"
# include "Logger.hpp"
# include "Error.hpp"
# include "Utils.hpp"

# ifndef RECV_BUFFER_SIZE // tip : linux kernel is 200kb
#  define RECV_BUFFER_SIZE 65536 // our recv is 64kb 
# endif

# ifndef IDLE_TIMEOUT_SECONDS
#  define IDLE_TIMEOUT_SECONDS 60
# endif

# ifndef TIMEOUT_MS
#  define TIMEOUT_MS 1000   // poll timeout 1s
# endif

extern volatile sig_atomic_t g_running;
void signal_handler(int); //ctrl + c

class Server_Manager
{
private:
    std::vector<Server>				_servers;
    std::vector<int>				_listening_fds;
    std::map<int, Server*>			_fd_to_server;		// listen fd -> Server*
    std::map<int, Client>			_clients;			// client fd -> Client
    std::vector<struct pollfd>		_poll_fds;

public:
	Server_Manager();
	Server_Manager(const std::vector<Server> &servers);
	Server_Manager(const Server_Manager &o);
	Server_Manager &operator=(const Server_Manager &o);
	~Server_Manager();

	void run();

private:
	/* init */
	void init_sockets();
	void set_fd_non_blocking(int fd);

	/* utile */
	bool   is_listening_fd(int fd) const;
	Server *get_server_by_fd(int fd);
	void   update_poll_events(int fd, short events);

	/* client  */
	void check_idle_clients();
	void accept_new_client(int server_fd);
	void close_connection(int client_fd);

	/* I/O */
	bool receive_request(int client_fd);  // true → quit connection
	bool send_response(int client_fd);    // true → quit connection
};

#endif
