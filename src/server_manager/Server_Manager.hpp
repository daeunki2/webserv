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
# include <netinet/in.h>
# include <sys/socket.h>
# include <netdb.h>
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

# ifndef IDLE_TIMEOUT_MS_VALUE
#  define IDLE_TIMEOUT_MS_VALUE ((unsigned long long)(IDLE_TIMEOUT_SECONDS) * 1000ULL)
# endif

extern volatile sig_atomic_t g_running;
void signal_handler(int); //ctrl + c

class Server_Manager
{
private:
    std::vector<Server>				_servers;
	std::vector<int>				_listening_fds;
	struct ListenSocketInfo
	{
		Server*     server;
		std::string host;
		int         port;
		ListenSocketInfo() : server(0), host(), port(0) {}
		ListenSocketInfo(Server *s, const std::string &h, int p)
		: server(s), host(h), port(p) {}
	};
	std::map<int, ListenSocketInfo>			_fd_to_server;		// listen fd -> info
	std::map<int, Client>			_clients;			// client fd -> Client
	std::vector<struct pollfd>		_poll_fds;
	struct CgiFdInfo
	{
		Client* client;
		bool    is_stdout;
		CgiFdInfo() : client(0), is_stdout(false) {}
		CgiFdInfo(Client* c, bool out) : client(c), is_stdout(out) {}
	};
	std::map<int, CgiFdInfo>       _cgi_fd_map;
	unsigned long long             _tick_counter;

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
    void add_poll_fd(int fd, short events);
    void remove_poll_fd(int fd);

	/* utile */
	bool   is_listening_fd(int fd) const;
	const ListenSocketInfo *get_listen_info(int fd) const;
	void   update_poll_events(int fd, short events);
	void   register_cgi_fds(Client &client);
	void   unregister_cgi_fd(int fd);
	bool   handle_cgi_poll_event(struct pollfd &pfd);

	/* client  */
	void check_idle_clients();
	void accept_new_client(int server_fd);
	void close_connection(int client_fd);

	/* I/O */
	bool receive_request(int client_fd);  // true → quit connection
	bool send_response(int client_fd);    // true → quit connection
};

#endif
