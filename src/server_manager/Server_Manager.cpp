/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Manager.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 13:40:10 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/28 17:43:16 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_Manager.hpp"
/* ************************************************************************** */
/*                           Canonical form                                   */
/* ************************************************************************** */
volatile sig_atomic_t g_running = 1;

void signal_handler(int)
{
    g_running = 0;
}

/* ************************************************************************** */
/*                           Canonical form                                   */
/* ************************************************************************** */

Server_Manager::Server_Manager()
{}

Server_Manager::Server_Manager(const std::vector<Server> &servers)
: _servers(servers)
{
    init_sockets();
}

Server_Manager::Server_Manager(const Server_Manager &o)
: _servers(o._servers), _listening_fds(o._listening_fds), _fd_to_server(o._fd_to_server), _clients(o._clients), _poll_fds(o._poll_fds)
{}

Server_Manager &Server_Manager::operator=(const Server_Manager &o)
{
    if (this != &o)
    {
        _servers       = o._servers;
        _listening_fds = o._listening_fds;
        _fd_to_server  = o._fd_to_server;
        _clients       = o._clients;
        _poll_fds      = o._poll_fds;
    }
    return *this;
}

Server_Manager::~Server_Manager()
{
    for (size_t i = 0; i < _poll_fds.size(); ++i)
    {
        if (_poll_fds[i].fd >= 0)
            close(_poll_fds[i].fd);
    }
}


/* ************************************************************************** */
/*                             init_sockets                                   */
/* ************************************************************************** */

void Server_Manager::set_fd_non_blocking(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
        throw Error("fcntl(F_SETFL, O_NONBLOCK) failed: " + std::string(strerror(errno)), __FILE__, __LINE__);
}


void Server_Manager::init_sockets()
{
    Logger::info("Initializing listening sockets...");

    for (size_t i = 0; i < _servers.size(); ++i)
    {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0)
            throw Error("socket() failed: " + std::string(strerror(errno)),
                        __FILE__, __LINE__);

        int optval = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        {
            close(fd);
            throw Error("setsockopt(SO_REUSEADDR) failed: " + std::string(strerror(errno)),
                        __FILE__, __LINE__);
        }

        set_fd_non_blocking(fd);

        struct sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port        = htons(_servers[i].getPort());

        if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            std::string msg = "bind() failed on port " + toString(_servers[i].getPort()) + ": " + std::string(strerror(errno));
            close(fd);
            throw Error(msg, __FILE__, __LINE__);
        }

        if (listen(fd, SOMAXCONN) < 0)
        {
            std::string msg = "listen() failed on port " + toString(_servers[i].getPort()) + ": " + std::string(strerror(errno));
            close(fd);
            throw Error(msg, __FILE__, __LINE__);
        }

        _listening_fds.push_back(fd);
        _fd_to_server[fd] = &_servers[i];

        struct pollfd pfd;
        pfd.fd      = fd;
        pfd.events  = POLLIN;
        pfd.revents = 0;
        _poll_fds.push_back(pfd);

        Logger::info("Listening on port " + toString(_servers[i].getPort()));
    }
}

/* ************************************************************************** */
/*                               Utilities                                    */
/* ************************************************************************** */

bool Server_Manager::is_listening_fd(int fd) const
{
    std::vector<int>::const_iterator it =
        std::find(_listening_fds.begin(), _listening_fds.end(), fd);
    return (it != _listening_fds.end());
}

Server *Server_Manager::get_server_by_fd(int fd)
{
    std::map<int, Server*>::iterator it = _fd_to_server.find(fd);
    if (it == _fd_to_server.end())
        return 0;
    return it->second;
}

void Server_Manager::update_poll_events(int fd, short events)
{
    for (size_t i = 0; i < _poll_fds.size(); ++i)
    {
        if (_poll_fds[i].fd == fd)
        {
            _poll_fds[i].events  = events;
            _poll_fds[i].revents = 0;
            return;
        }
    }
}

/* ************************************************************************** */
/*                            Client management                               */
/* ************************************************************************** */

void Server_Manager::close_connection(int client_fd)
{
    Logger::info("Closing client FD " + toString(client_fd));

    _clients.erase(client_fd);

    for (size_t i = 0; i < _poll_fds.size(); ++i)
    {
        if (_poll_fds[i].fd == client_fd)
        {
            _poll_fds.erase(_poll_fds.begin() + i);
            break;
        }
    }

    if (close(client_fd) < 0)
        Logger::error("close() failed for FD " + toString(client_fd) + ": " + std::string(strerror(errno)));
}


void Server_Manager::check_idle_clients()
{
    time_t now = time(NULL);

    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); )
    {
        int fd = it->first;
        time_t last = it->second.last_active_time;

        if (now - last > IDLE_TIMEOUT_SECONDS)
        {
            Logger::warn("Client FD " + toString(fd) + " idle timeout.");

            ++it;
            close_connection(fd);
        }
        else
        {
            ++it;
        }
    }
}


void Server_Manager::accept_new_client(int server_fd)
{
    struct sockaddr_storage client_addr;
    socklen_t addr_size;

    while (true)
    {
        addr_size = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_size);
        if (client_fd < 0)
        {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                return;
            }
            Logger::error("accept() failed: " + std::string(strerror(errno)));
            return;
        }

        set_fd_non_blocking(client_fd);

        Server *config = get_server_by_fd(server_fd);
        if (!config)
        {
            Logger::error("No server config for FD " + toString(server_fd));
            close(client_fd);
            continue;
        }

        Client c(client_fd, config);
        c.last_active_time = time(NULL);
        _clients.insert(std::make_pair(client_fd, c));

        struct pollfd pfd;
        pfd.fd = client_fd;
        pfd.events = POLLIN;
        pfd.revents = 0;
        _poll_fds.push_back(pfd);

        Logger::info("Accepted new client FD " + toString(client_fd));
    }
}


/* ************************************************************************** */
/*                         Request / Response                                 */
/* ************************************************************************** */

bool Server_Manager::receive_request(int fd)
{
    Client &client = _clients[fd];

    char buf[RECV_BUFFER_SIZE];
    ssize_t n = recv(fd, buf, RECV_BUFFER_SIZE, 0);

    if (n <= 0)
    {
		if(n < 0)
		Logger::error("recv failed");
		close_connection(fd);
        return true;
    }

    client.last_active_time = time(NULL);

    Client::ParsingState st = client.handle_recv_data(buf, n);

    if (st == Client::PARSING_COMPLETED)
    {
		Logger::info("got request from FD " + toString(fd));
        client.update_state(Client::REQUEST_COMPLETE);
    }

    return false;
}

bool Server_Manager::send_response(int client_fd)
{
    Client &client = _clients[client_fd];
    const std::string &buf = client.get_response_buffer();
    size_t &sent = client.get_sent_bytes();

    size_t total = buf.size();
    if (sent >= total)
    {
        client.update_state(Client::CONNECTION_CLOSE);
        close_connection(client_fd);
        return true;
    }

    ssize_t bytes_sent = send(client_fd, buf.c_str() + sent, total - sent, 0);

    if (bytes_sent < 0)
    {
		Logger::error("send() failed");
        close_connection(client_fd);
        return true;
    }

    sent += bytes_sent;

	if (sent >= total)
	{
    	if (client.get_request().keep_alive())
    	{
        	client.reset();
        	update_poll_events(client_fd, POLLIN);
        	client.update_state(Client::RECVING_REQUEST);
    	}
    	else
    	{
        	client.update_state(Client::CONNECTION_CLOSE);
        	close_connection(client_fd);
    	}
		Logger::info("send response to FD " + toString(client_fd));
    	return true;
	}
    return false;
}


/* ************************************************************************** */
/*                              Main loop                                     */
/* ************************************************************************** */

void Server_Manager::run()
{
	Logger::info("ServerManager main loop started.");

	while (g_running)
	{
		check_idle_clients();
		if (_poll_fds.empty())
		{
			Logger::error("No poll fds left. Exiting loop.");
			break;
		}
		int ret = poll(&_poll_fds[0], _poll_fds.size(), TIMEOUT_MS);
		if (ret < 0)
		{
			if (errno == EINTR)
			{
				if (!g_running)
				{
					Logger::info("ctrl + c situation. Exiting loop."); 
					break;
				}
				continue;
			}
			Logger::error("poll() failed: " + std::string(strerror(errno)));
		break;
		}

        if (ret == 0)
            continue;

        for (size_t i = 0; i < _poll_fds.size(); ++i)
        {
            struct pollfd &pfd = _poll_fds[i];
            int fd = pfd.fd;

            if (pfd.revents == 0)
                continue;

            bool closed = false;

            if (is_listening_fd(fd))
            {
                if (pfd.revents & POLLIN)
                    accept_new_client(fd);
                continue;
            }

            if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                Logger::warn("poll error/hup on client FD " + toString(fd));
                close_connection(fd);
                closed = true;
            }


            if (!closed && (pfd.revents & POLLIN))
            {
                if (receive_request(fd))
                    closed = true;
            }
            if (!closed)
            {
                std::map<int, Client>::iterator it = _clients.find(fd);
                if (it != _clients.end())
                {
                    Client &client = it->second;

                    if (client.get_state() == Client::REQUEST_COMPLETE)
                    {
                        client.build_response();
                        client.update_state(Client::SENDING_RESPONSE);
                        update_poll_events(fd, POLLOUT);
                    }
                }
            }
            if (!closed && (pfd.revents & POLLOUT))
            {
                if (send_response(fd))
                    closed = true;
            }
            if (closed && i > 0)
                --i;
        }
    }
}
