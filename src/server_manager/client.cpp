/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 15:40:04 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/02 16:05:14 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"


static std::string get_error_message(const std::string& msg)
{
    return "[Client] " + msg + " error: " + std::string(strerror(errno));
}

// ******************************************************
//                 생성자 및 소멸자
// ******************************************************

client::client(int fd, const Server& config)
: m_fd(fd), m_state(WAITING_FOR_REQUEST),m_server_config(config) 
{
	std::memset(m_read_buffer, 0, READ_BUFFER_SIZE); 
	std::cout << "Client " << m_fd << " connected. State: WAITING_FOR_REQUEST" << std::endl;
}

client::~client()
{
	if (m_fd >= 0)
	{
		close(m_fd);
	}
	std::cout << "Client " << m_fd << " disconnected." << std::endl;
}

// ******************************************************
//                 내부 헬퍼 함수
// ******************************************************

void client::handleRequestProcessing()
{
	std::cout << "-> FD " << m_fd << ": Request parsed. Response ready." << std::endl;
    // m_response.prepare_response("..."); // 실제 응답 생성 로직
}


// ******************************************************
//                 Public 인터페이스
// ******************************************************

client::ClientState client::handleRead()
{
    ssize_t bytes_read = read(m_fd, m_read_buffer, READ_BUFFER_SIZE);

    if (bytes_read == 0)
    {
        std::cout << "Client FD " << m_fd << " closed connection gracefully." << std::endl;
        return CLOSED; 
    }

    if (bytes_read < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return m_state;
        }
        std::cerr << get_error_message("read fatal") << std::endl;
        return CLOSED; 
    }

    m_parser.load_data(m_read_buffer, bytes_read);

    if (m_parser.get_state() == PARSING_COMPLETED)
    {
        handleRequestProcessing(); 
        m_state = WRITING_RESPONSE;
        std::cout << "-> FD " << m_fd << ": State changed to WRITING_RESPONSE." << std::endl;
    }
    else if (m_parser.get_state() == PARSING_ERROR)
    {
        std::cerr << "Request parsing error on FD " << m_fd << ". Closing connection." << std::endl;
        return CLOSED; 
    }
    return m_state;
}

client::ClientState client::handleWrite()
{

    std::string response_data = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello Webserv!";
    ssize_t bytes_written = write(m_fd, response_data.c_str(), response_data.length());

    if (bytes_written < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
            return m_state;
        }
        std::cerr << get_error_message("write fatal") << std::endl;
        return CLOSED;
    }    
    std::cout << "-> FD " << m_fd << ": Response sent completely. Closing connection." << std::endl;
    return CLOSED;
}


void client::updatePollEvents(struct pollfd& pfd)
{
    pfd.events = 0;
    
    switch (m_state)
    {
        case WAITING_FOR_REQUEST:
        case READING_BODY:
            pfd.events = POLLIN;
            break;
        case WRITING_RESPONSE:
            pfd.events = POLLOUT;
            break;
        case CLOSED:
            break;
    }
}

// ******************************************************
//                      Getter
// ******************************************************

int client::getFd() const
{	
	return m_fd;
};

client::ClientState client::getState() const
{
	return m_state;
};