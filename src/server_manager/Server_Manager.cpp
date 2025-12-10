/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Manager.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 13:40:10 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/08 13:25:52 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_Manager.hpp"
#include <stdint.h>
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
: _tick_counter(0)
{}

Server_Manager::Server_Manager(const std::vector<Server> &servers)
: _servers(servers), _tick_counter(0)
{
    init_sockets();
}

Server_Manager::Server_Manager(const Server_Manager &o)
: _servers(o._servers), _listening_fds(o._listening_fds), _fd_to_server(o._fd_to_server), _clients(o._clients), _poll_fds(o._poll_fds), _tick_counter(o._tick_counter)
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
        _tick_counter  = o._tick_counter;
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
    Logger::info(Logger::TAG_CORE, "Initializing listening sockets...");

    for (size_t i = 0; i < _servers.size(); ++i)
    {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0)
            throw Error("socket() failed: " + std::string(strerror(errno)),__FILE__, __LINE__);

        int optval = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        {
            close(fd);
            throw Error("setsockopt(SO_REUSEADDR) failed: " + std::string(strerror(errno)), __FILE__, __LINE__);
        }

        set_fd_non_blocking(fd);

        struct sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port   = htons(_servers[i].getPort());

        const std::string &host = _servers[i].getHost();
        if (host.empty())
        {
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
        }
        else
        {
            struct addrinfo hints;
            std::memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;

            struct addrinfo *result = NULL;
            int gai_ret = getaddrinfo(host.c_str(), NULL, &hints, &result);
            if (gai_ret != 0 || !result)
            {
                close(fd);
                throw Error("getaddrinfo failed for host " + host + ": " + std::string(gai_strerror(gai_ret)), __FILE__, __LINE__);
            }
            addr.sin_addr = reinterpret_cast<struct sockaddr_in*>(result->ai_addr)->sin_addr;
            freeaddrinfo(result);
        }

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

        add_poll_fd(fd, POLLIN);

        std::string hostDesc = host.empty() ? std::string("*") : host;
        Logger::info(Logger::TAG_EVENT, "Listening on " + hostDesc + ":" + toString(_servers[i].getPort()));
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

void Server_Manager::add_poll_fd(int fd, short events)
{
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = events;
	pfd.revents = 0;
	_poll_fds.push_back(pfd);
}

void Server_Manager::remove_poll_fd(int fd)
{
	for (size_t i = 0; i < _poll_fds.size(); ++i)
	{
		if (_poll_fds[i].fd == fd)
		{
			_poll_fds.erase(_poll_fds.begin() + i);
			break;
		}
	}
}

void Server_Manager::register_cgi_fds(Client &client)
{
	if (!client.has_active_cgi())
		return;
	int in_fd = client.get_cgi_stdin_fd();
	if (in_fd >= 0 && _cgi_fd_map.find(in_fd) == _cgi_fd_map.end())
	{
		add_poll_fd(in_fd, POLLOUT);
		_cgi_fd_map[in_fd] = CgiFdInfo(&client, false);
	}
	int out_fd = client.get_cgi_stdout_fd();
	if (out_fd >= 0 && _cgi_fd_map.find(out_fd) == _cgi_fd_map.end())
	{
		add_poll_fd(out_fd, POLLIN);
		_cgi_fd_map[out_fd] = CgiFdInfo(&client, true);
	}
}

void Server_Manager::unregister_cgi_fd(int fd)
{
	std::map<int, CgiFdInfo>::iterator it = _cgi_fd_map.find(fd);
	if (it != _cgi_fd_map.end())
		_cgi_fd_map.erase(it);
	remove_poll_fd(fd);
}

bool Server_Manager::handle_cgi_poll_event(struct pollfd &pfd)
{
	std::map<int, CgiFdInfo>::iterator it = _cgi_fd_map.find(pfd.fd);
	if (it == _cgi_fd_map.end())
		return false;
	Client *client = it->second.client;
	bool is_stdout = it->second.is_stdout;
	bool ok = true;

	if (pfd.revents & (POLLERR | POLLNVAL))
		ok = false;
	else if (!is_stdout && (pfd.revents & POLLOUT))
		ok = client->handle_cgi_stdin_event();
	else if (is_stdout && (pfd.revents & (POLLIN | POLLHUP)))
		ok = client->handle_cgi_stdout_event();

	bool removed = false;
	if (!ok)
	{
		unregister_cgi_fd(pfd.fd);
		removed = true;
		if (client->get_state() == Client::SENDING_RESPONSE)
			update_poll_events(client->get_fd(), POLLOUT);
		return removed;
	}

	if (!is_stdout && client->is_cgi_body_complete())
	{
		unregister_cgi_fd(pfd.fd);
		removed = true;
	}
	else if (is_stdout && client->is_cgi_stdout_closed())
	{
		unregister_cgi_fd(pfd.fd);
		removed = true;
		client->handle_cgi_completion();
		if (client->get_state() == Client::SENDING_RESPONSE)
			update_poll_events(client->get_fd(), POLLOUT);
	}
	return removed;
}

/* ************************************************************************** */
/*                            Client management                               */
/* ************************************************************************** */

void Server_Manager::close_connection(int client_fd)
{
    Logger::info(Logger::TAG_EVENT, "Closing client FD " + toString(client_fd));
    std::map<int, Client>::iterator it = _clients.find(client_fd);
    if (it == _clients.end())
        return;
    Client &client = it->second;
	client.abort_cgi();
	for (std::map<int, CgiFdInfo>::iterator cgi_it = _cgi_fd_map.begin(); cgi_it != _cgi_fd_map.end(); )
	{
		if (cgi_it->second.client == &client)
		{
			remove_poll_fd(cgi_it->first);
			_cgi_fd_map.erase(cgi_it++);
		}
		else
			++cgi_it;
	}
    _clients.erase(it);

    remove_poll_fd(client_fd);

    if (close(client_fd) < 0)
        Logger::error(Logger::TAG_FD, "close() failed for FD " + toString(client_fd) + ": " + std::string(strerror(errno)));}


void Server_Manager::check_idle_clients()
{
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); )
    {
        int fd = it->first;
        size_t last = it->second.last_activity_tick;

        if (_tick_counter >= last && _tick_counter - last >= IDLE_TIMEOUT_TICKS)
        {
            Logger::warn(Logger::TAG_TIMEOUT, "Client FD " + toString(fd) + " idle timeout.");
            ++it;
            close_connection(fd);
        }
        else
        {
            ++it;
        }
    }
}


static std::string ipv4_to_string(uint32_t host_ip)
{
    std::string addr;
    addr += toString((host_ip >> 24) & 0xFF);
    addr += ".";
    addr += toString((host_ip >> 16) & 0xFF);
    addr += ".";
    addr += toString((host_ip >> 8) & 0xFF);
    addr += ".";
    addr += toString(host_ip & 0xFF);
    return addr;
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
				Logger::error(Logger::TAG_EVENT, "accept() failed: " + std::string(strerror(errno)));
				return;
        }

        set_fd_non_blocking(client_fd);

        Server *config = get_server_by_fd(server_fd);
        if (!config)
        {
			Logger::error(Logger::TAG_CONF, "No server config for FD " + toString(server_fd));
			close(client_fd);
            continue;
        }

        std::string remoteAddr;
        std::string remotePort;
        if (client_addr.ss_family == AF_INET)
        {
            struct sockaddr_in *sin = reinterpret_cast<struct sockaddr_in*>(&client_addr);
            uint32_t ip_host = ntohl(sin->sin_addr.s_addr);
            remoteAddr = ipv4_to_string(ip_host);
            remotePort = toString(ntohs(sin->sin_port));
        }

        Client c(client_fd, config, remoteAddr, remotePort);
        c.last_activity_tick = _tick_counter;
        _clients.insert(std::make_pair(client_fd, c));

        add_poll_fd(client_fd, POLLIN);

		Logger::info(Logger::TAG_EVENT, "Accepted new client FD " + toString(client_fd));
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
		Logger::error(Logger::TAG_FD, "recv failed for fd " + toString(fd));
		close_connection(fd);
        return true;
    }

    client.last_activity_tick = _tick_counter;

    Client::ParsingState st = client.handle_recv_data(buf, n);

    if (st == Client::PARSING_COMPLETED)
    {
		Logger::info(Logger::TAG_REQ,"got request from FD " + toString(fd));
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

    if (sent == 0 && !buf.empty())
    {
        size_t lineEnd = buf.find("\r\n");
        std::string statusLine = (lineEnd != std::string::npos) ? buf.substr(0, lineEnd) : buf;
        std::string statusCode = "unknown";
        size_t sp1 = statusLine.find(' ');
        if (sp1 != std::string::npos)
        {
            size_t sp2 = statusLine.find(' ', sp1 + 1);
            if (sp2 != std::string::npos)
                statusCode = statusLine.substr(sp1 + 1, sp2 - sp1 - 1);
            else
                statusCode = statusLine.substr(sp1 + 1);
        }
        const http_request &req = client.get_request();
        Logger::info(Logger::TAG_EVENT,
            "Responding FD " + toString(client_fd) +
            " status " + statusCode +
            " ← " + req.get_method() + " " + req.get_path());
    }

    if (sent >= total)
    {
        bool keep = client.get_request().keep_alive() && client.get_error_code() == 0;
        if (keep)
        {
            client.reset();
            client.last_activity_tick = _tick_counter;
            update_poll_events(client_fd, POLLIN);
            client.update_state(Client::RECVING_REQUEST);
            return true;
        }
        client.update_state(Client::CONNECTION_CLOSE);
        close_connection(client_fd);
        return true;
    }

    ssize_t bytes_sent = send(client_fd, buf.data() + sent, total - sent, 0);

    if (bytes_sent < 0)
    {
        Logger::warn(Logger::TAG_FD, "send() failed for FD " + toString(client_fd));
        close_connection(client_fd);
        return true;
    }
    if (bytes_sent == 0)
        return false;

    sent += bytes_sent;

    if (sent >= total)
    {
        bool keep = client.get_request().keep_alive() && client.get_error_code() == 0;
        if (keep)
        {
            client.reset();
            client.last_activity_tick = _tick_counter;
            update_poll_events(client_fd, POLLIN);
            client.update_state(Client::RECVING_REQUEST);
        }
        else
        {
            client.update_state(Client::CONNECTION_CLOSE);
            close_connection(client_fd);
        }
        return true;
    }

    return false;
}



/* ************************************************************************** */
/*                              Main loop                                     */
/* ************************************************************************** */

void Server_Manager::run()
{
	Logger::info(Logger::TAG_CORE, "ServerManager main loop started.");

	while (g_running)
	{
		if (_poll_fds.empty())
		{
			Logger::error(Logger::TAG_POLL,"No poll fds left. Exiting loop.");
			break;
		}
		int ret = poll(&_poll_fds[0], _poll_fds.size(), TIMEOUT_MS);
		if (ret < 0)
		{
			if (errno == EINTR)
			{
				if (!g_running)
				{
					Logger::info(Logger::TAG_CORE,"ctrl + c situation. Exiting loop."); 
					break;
				}
				continue;
			}
			Logger::error(Logger::TAG_POLL,"poll() failed: " + std::string(strerror(errno)));
		break;
		}

		++_tick_counter;

        if (ret == 0)
		{
			check_idle_clients();
            continue;
		}

        for (size_t i = 0; i < _poll_fds.size(); ++i)
        {
            struct pollfd &pfd = _poll_fds[i];
            int fd = pfd.fd;

            if (pfd.revents == 0)
                continue;

            bool closed = false;

			std::map<int, CgiFdInfo>::iterator cgi_it = _cgi_fd_map.find(fd);
			if (cgi_it != _cgi_fd_map.end())
			{
				bool removed = handle_cgi_poll_event(pfd);
				if (removed && i > 0)
					--i;
				continue;
			}

            if (is_listening_fd(fd))
            {
                if (pfd.revents & POLLIN)
                    accept_new_client(fd);
                continue;
            }

            if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                Logger::warn(Logger::TAG_POLL,"poll error/hup on client FD " + toString(fd));
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

                    if (client.get_state() == Client::REQUEST_COMPLETE ||
                        client.get_state() == Client::ERROR)
                    {
			client.build_response();
			Client::ClientState state = client.get_state();
			if (state == Client::SENDING_RESPONSE)
			{
				update_poll_events(fd, POLLOUT);
			}
			else if (state == Client::CGI_SENDING_BODY || state == Client::CGI_READING_OUTPUT)
			{
				update_poll_events(fd, 0);
				register_cgi_fds(client);
			}
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
		check_idle_clients();
    }
}
