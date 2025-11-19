/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Manager.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 13:40:10 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/19 14:47:07 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server_Manager.hpp"

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
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        throw Error("fcntl(F_GETFL) failed: " + std::string(strerror(errno)),
                    __FILE__, __LINE__);

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        throw Error("fcntl(F_SETFL, O_NONBLOCK) failed: " + std::string(strerror(errno)),
                    __FILE__, __LINE__);
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
            std::string msg = "bind() failed on port " +
                              toString(_servers[i].getPort()) + ": " + std::string(strerror(errno));
            close(fd);
            throw Error(msg, __FILE__, __LINE__);
        }

        if (listen(fd, SOMAXCONN) < 0)
        {
            std::string msg = "listen() failed on port " +
                              toString(_servers[i].getPort()) + ": " + std::string(strerror(errno));
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
    std::map<int, Client>::iterator it = _clients.find(client_fd);
    if (it != _clients.end())
    {
        Logger::info("Closing client FD " + toString(client_fd));
        _clients.erase(it);
    }

    for (std::vector<struct pollfd>::iterator p = _poll_fds.begin();
         p != _poll_fds.end(); ++p)
    {
        if (p->fd == client_fd)
        {
            _poll_fds.erase(p);
            break;
        }
    }

    if (close(client_fd) < 0)
        Logger::error("close() failed for FD " + toString(client_fd) +
                      ": " + std::string(strerror(errno)));
}

void Server_Manager::check_idle_clients()
{
    time_t now = time(NULL);

    for (std::map<int, Client>::iterator it = _clients.begin();
         it != _clients.end(); )
    {
        int fd = it->first;
        time_t last = it->second.last_active_time; // Client 안에 이 필드가 있다고 가정

        if (now - last > IDLE_TIMEOUT_SECONDS)
        {
            Logger::warn("Client FD " + toString(fd) + " timed out (idle).");
            ++it;              // 먼저 iterator를 옮겨놓고
            close_connection(fd); // 내부에서 _clients.erase(fd) 수행
        }
        else
            ++it;
    }
}

void Server_Manager::accept_new_client(int server_fd)
{
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    int client_fd;

    while (true)
    {
        addr_size = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size);

        if (client_fd < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return;
            Logger::error("accept() failed on FD " + toString(server_fd) +
                          ": " + std::string(strerror(errno)));
            return;
        }

        if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
        {
            Logger::error("fcntl(O_NONBLOCK) failed for client FD " +
                          toString(client_fd) + ": " + std::string(strerror(errno)));
            close(client_fd);
            continue;
        }

        Server *config = get_server_by_fd(server_fd);
        if (!config)
        {
            Logger::error("No server config found for listening FD " +
                          toString(server_fd));
            close(client_fd);
            continue;
        }

        try
        {
            Client c(client_fd, config);
            c.last_active_time = time(NULL);
            _clients.insert(std::make_pair(client_fd, c));
        }
        catch (const std::exception &e)
        {
            Logger::error(std::string("Client creation failed: ") + e.what());
            close(client_fd);
            continue;
        }

        struct pollfd pfd;
        pfd.fd      = client_fd;
        pfd.events  = POLLIN;
        pfd.revents = 0;
        _poll_fds.push_back(pfd);

        Logger::info("New client connected, FD " + toString(client_fd));
    }
}

/* ************************************************************************** */
/*                         Request / Response                                 */
/* ************************************************************************** */

bool Server_Manager::receive_request(int client_fd)
{
    std::map<int, Client>::iterator it = _clients.find(client_fd);
    if (it == _clients.end())
        return true;

    Client &client = it->second;

    char buffer[RECV_BUFFER_SIZE];
    ssize_t bytes_read = recv(client_fd, buffer, RECV_BUFFER_SIZE, 0);

    if (bytes_read <= 0)
    {
        if (bytes_read == 0)
            Logger::info("Client FD " + toString(client_fd) + " closed the connection.");
        else if (errno != EAGAIN && errno != EWOULDBLOCK)
            Logger::error("recv() failed on FD " + toString(client_fd) +
                          ": " + std::string(strerror(errno)));

        close_connection(client_fd);
        return true; // closed
    }

    client.last_active_time = time(NULL);

    Client::ParsingState st = client.handle_recv_data(buffer, (size_t)bytes_read);

    if (st == Client::PARSING_ERROR)
    {
        Logger::error("Parsing error on client FD " + toString(client_fd));
        close_connection(client_fd);
        return true;
    }
    else if (st == Client::PARSING_COMPLETED)
    {
        client.update_state(REQUEST_COMPLETE);
    }

    return false; // keep connection
}

bool Server_Manager::send_response(int client_fd)
{
    std::map<int, Client>::iterator it = _clients.find(client_fd);
    if (it == _clients.end())
        return true;

    Client &client = it->second;

    const std::string &buf = client.get_response_buffer();
    size_t &sent           = client.get_sent_bytes();
    size_t total           = buf.size();

    if (total == 0 || sent >= total)
    {
        client.update_state(CONNECTION_CLOSE);
    }

    size_t remaining = total - sent;
    const char *data = buf.c_str() + sent;

    ssize_t bytes_sent = send(client_fd, data, remaining, 0);
    if (bytes_sent < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return false; // 나중에 다시 시도
        Logger::error("send() failed on FD " + toString(client_fd) +
                      ": " + std::string(strerror(errno)));
        close_connection(client_fd);
        return true;
    }

    sent += (size_t)bytes_sent;

    if (sent == total)
    {
        client.update_state(CONNECTION_CLOSE);

        if (client.get_state() == CONNECTION_CLOSE)
        {
            close_connection(client_fd);
            return true;
        }
        else
        {
            client.reset();                     // keep-alive 등
            update_poll_events(client_fd, POLLIN);
        }
    }
    return false;
}

/* ************************************************************************** */
/*                              Main loop                                     */
/* ************************************************************************** */

void Server_Manager::run()
{
    check_idle_clients();

    if (_poll_fds.empty())
        return;

    int ret = poll(&_poll_fds[0], _poll_fds.size(), TIMEOUT_MS);
    if (ret < 0)
    {
        if (errno != EINTR)
            Logger::error("poll() failed: " + std::string(strerror(errno)));
        return;
    }
    if (ret == 0)
        return; // timeout

    for (size_t i = 0; i < _poll_fds.size(); ++i)
    {
        struct pollfd &pfd = _poll_fds[i];
        if (pfd.revents == 0)
            continue;

        int fd = pfd.fd;

        // 1) 리스닝 소켓 → accept
        if (is_listening_fd(fd))
        {
            if (pfd.revents & POLLIN)
                accept_new_client(fd);
            continue;
        }

        // 2) 클라이언트 소켓
        bool closed = false;

        // 에러/HUP
        if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL))
        {
            Logger::warn("poll error/hup on FD " + toString(fd));
            close_connection(fd);
            closed = true;
        }

        // 읽기
        if (!closed && (pfd.revents & POLLIN))
        {
            if (receive_request(fd))
                closed = true;
        }

        // 요청 완료 → 응답 생성
        if (!closed)
        {
            std::map<int, Client>::iterator it = _clients.find(fd);
            if (it != _clients.end())
            {
                Client &client = it->second;
                if (client.get_state() == REQUEST_COMPLETE)
                {
                    client.build_response();
                    client.update_state(SENDING_RESPONSE);
                    update_poll_events(fd, POLLOUT);
                }
            }
        }

        // 쓰기
        if (!closed && (pfd.revents & POLLOUT))
        {
            if (send_response(fd))
                closed = true;
        }

        // poll_fds에서 fd가 erase되었을 수 있으므로 인덱스 조정
        if (closed && i > 0)
            --i;
    }
}
