/* ************************************************************************** */
/*			    */
/*			        :::      ::::::::   */
/*   Server_Manager.cpp			         :+:      :+:    :+:   */
/*			    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*			+#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 15:36:32 by daeunki2          #+#    #+#			 */
/*   Updated: 2025/11/05 12:34:41 by daeunki2         ###   ########.fr       */
/*			    */
/* ************************************************************************** */

#include "Server_Manager.hpp"

// ******************************************************
//            constructer 
// ******************************************************

Server_Manager::Server_Manager() {}

Server_Manager::Server_Manager(const std::vector<Server>& servers)
: servers(servers) 
{
    init_sockets();
}

Server_Manager::~Server_Manager() {}

// ******************************************************
//                    logics (Utility)
// ******************************************************

void Server_Manager::set_fd_non_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        throw error_control(RED, "Server_Manager","fcntl(F_GETFL) failed: " + std::string(strerror(errno)));

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        throw error_control(RED, "Server_Manager","fcntl(F_SETFL, O_NONBLOCK) failed: " + std::string(strerror(errno)));
};

void Server_Manager::init_sockets()
{
    int server_num = servers.size();
    int temp_fd;
    int optval = 1;
    
    for (int i = 0; i < server_num; i++)
    {
        temp_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (temp_fd < 0)
            throw error_control(RED, "Server_Manager", "socket() failed: " + std::string(strerror(errno)));
        if (setsockopt(temp_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        {
            close(temp_fd);
            throw error_control(RED, "Server_Manager","setsockopt(SO_REUSEADDR) failed: " + std::string(strerror(errno)));
        }
        try
        {
            set_fd_non_blocking(temp_fd);
        }
        catch (const error_control& e)
        {
            close(temp_fd);
            throw;
        }
        
        struct sockaddr_in server_addr;
        std::memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
        server_addr.sin_port = htons(servers[i].getPort());
        
        if (bind(temp_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        {
            close(temp_fd);
            throw error_control(RED, "Server_Manager", "bind() failed on port " + std::to_string(servers[i].getPort()) + ": " + std::string(strerror(errno)));
        }
        if (listen(temp_fd, SOMAXCONN) < 0)
        { 
            close(temp_fd);
            throw error_control(RED, "Server_Manager","listen() failed on port " + std::to_string(servers[i].getPort()) + ": " + std::string(strerror(errno)));
        }
        listening_fds.push_back(temp_fd);

        fd_to_server[temp_fd] = &servers[i];

        struct pollfd pfd;
        pfd.fd = temp_fd;
        pfd.events = POLLIN;
        pfd.revents = 0;
        poll_fds.push_back(pfd);
    }
};

// ******************************************************
//                      Utility 
// ******************************************************

bool Server_Manager::is_listening_fd(int fd) const
{
    std::vector<int>::const_iterator it = std::find(listening_fds.begin(), listening_fds.end(), fd);
    return (it != listening_fds.end());
}

Server* Server_Manager::get_server_by_fd(int fd)
{
    std::map<int, Server*>::iterator it = fd_to_server.find(fd);
    if (it == fd_to_server.end())
        return NULL;
    else
        return (it->second);
};

// ******************************************************
//                  CLIENT CONTOL
// ******************************************************

void Server_Manager::check_idle_clients()
{
    time_t current_time = time(NULL);
    
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); )
    {
        int client_fd = it->first;
        time_t last_active = it->second.last_active_time;

        if (current_time - last_active > IDLE_TIMEOUT_SECONDS)
        {
            close_connection(client_fd);
            it = clients.find(client_fd);
            if (it == clients.end()) {
                it = clients.begin();
            }
        }
        else
        {
            ++it;
        }
    }

};

void Server_Manager::close_connection(int client_fd)
{
    if (close(client_fd) < 0)
    {   
        std::cerr << "Error: close() failed for FD " << client_fd << ": " << strerror(errno) << std::endl;
    }
    for (std::vector<struct pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); ++it)
    {
        if (it->fd == client_fd)
        {
            poll_fds.erase(it);
            break;
        }
    }
};

void Server_Manager::accept_new_client(int server_fd)
{
    struct sockaddr_storage client_addr;
    socklen_t               addr_size = sizeof(client_addr);
    int                     client_fd;

    while (true)
    {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size);

        if (client_fd < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return;
            std::cerr << "Error: accept() failed for server FD " << server_fd << ": " << strerror(errno) << std::endl;
            return;
        }

        if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
        {
            std::cerr << "Error: fcntl() O_NONBLOCK failed for client FD " << client_fd  << ": " << strerror(errno) << std::endl;
            close(client_fd);
            continue;
        }

        Server* config = get_server_by_fd(server_fd); 
        if (!config)
        {
            std::cerr << "Error: Could not find server config for FD " << server_fd << std::endl;
            close(client_fd);
            continue;
        }

        try
        {
            clients.insert(std::make_pair(client_fd, Client(client_fd, config)));
            clients[client_fd].last_active_time = time(NULL);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: Client object creation failed: " << e.what() << std::endl;
            close(client_fd);
            continue;
        }

        struct pollfd new_fd;
        new_fd.fd = client_fd;
        new_fd.events = POLLIN;
        new_fd.revents = 0;
        poll_fds.push_back(new_fd);
        std::cout << "[+] New connection (FD " << client_fd << ")" << std::endl;
    }
}

// ******************************************************
//                 Request & Response
// ******************************************************

bool Server_Manager::receive_request(int client_fd)
{
    Client& client = clients.at(client_fd);
    char buffer[RECV_BUFFER_SIZE];
    
    ssize_t bytes_read = recv(client_fd, buffer, RECV_BUFFER_SIZE, 0);
    if (bytes_read <= 0)
    {

        if (bytes_read == 0 || (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK))
        {
            close_connection(client_fd);
            return true; 
        }
        return false; 
    }

    client.m_last_active_time = time(NULL);

    Client::ParsingState state = client.handle_recv_data(buffer, (size_t)bytes_read); 

    if (state == Client::PARSING_ERROR)
    {
        close_connection(client_fd);
        return true; 
    }
    else if (state == Client::PARSING_COMPLETED)
    {
        client.update_state(REQUEST_COMPLETE);
    }
    
    return false;
}

bool Server_Manager::send_response(int client_fd)
{
    Client& client = clients.at(client_fd);
    
    const std::string& buffer = client.get_response_buffer();
    size_t& sent = client.get_sent_bytes();
    size_t total_length = buffer.length();
    
    if (total_length == 0 || sent >= total_length)
	{
        client.update_state(CONNECTION_CLOSE);
    }

    const char* data_to_send = buffer.c_str() + sent;
    size_t remaining = total_length - sent;
    
    ssize_t bytes_sent = send(client_fd, data_to_send, remaining, 0);

    if (bytes_sent < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return false;
        }
        close_connection(client_fd);
        return true; 
    }

    sent += (size_t)bytes_sent;

    if (sent == total_length)
    {

        client.update_state(CONNECTION_CLOSE); 

        if (client.get_state() == CONNECTION_CLOSE)
        {
            close_connection(client_fd);
            return true;
        } 
        else 
        {
            client.reset(); 
            update_poll_events(client_fd, POLLIN);
        }
    }

    return false;
}

// ******************************************************
//                    Main Loop
// ******************************************************

void Server_Manager::run()
{
    check_idle_clients();

    if (poll_fds.empty())
        return;

    int poll_count = poll(&poll_fds[0], poll_fds.size(), TIMEOUT_MS);

    if (poll_count < 0)
    {
        if (errno != EINTR)
        {
            std::cerr << "Fatal Error: poll() failed: " << strerror(errno) << std::endl;
        }
        return;
    }
    
    if (poll_count == 0)
        return;

    for (size_t i = 0; i < poll_fds.size(); ++i)
    {
        int current_fd = poll_fds[i].fd;


        if (!clients.count(current_fd))
        {
            if (poll_fds[i].revents & POLLIN)
            {
                accept_new_client(current_fd);
            }
            continue;
        }
        
        bool closed = false;
        Client& client = clients.at(current_fd);

        if (poll_fds[i].revents & (POLLERR | POLLHUP))
        {
            close_connection(current_fd);
            closed = true;
        }

        if (!closed && (poll_fds[i].revents & POLLIN))
        {
            if (receive_request(current_fd))
                closed = true;
        }


        if (!closed && client.get_state() == REQUEST_COMPLETE)
        {
            client.build_response(); 
            
            client.update_state(SENDING_RESPONSE);
            update_poll_events(current_fd, POLLOUT); 
        }

        if (!closed && (poll_fds[i].revents & POLLOUT))
        {
            if (send_response(current_fd))
                closed = true;
        }

        if (closed)
            --i;
    }
}