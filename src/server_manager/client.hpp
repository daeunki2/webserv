/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 15:40:08 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/02 16:36:39 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>
# include <unistd.h>     // close
# include <poll.h>       // pollfd 구조체 갱신용
#include <unistd.h>     // read, write, close
#include <sys/socket.h> // read, write
#include <errno.h>      // errno, EAGAIN, EWOULDBLOCK
#include <cstring>      // strerror, memset
#include <stdexcept>

#include "request_parser.hpp" 
# include "HttpResponse.hpp"  // (추후 구현될) 응답 클래스를 포함

# define READ_BUFFER_SIZE 4096 

// 전방 선언
class Server;
class HttpResponse; 

class client
{
	public:
	enum ClientState 
	{
		WAITING_FOR_REQUEST, 
		READING_BODY,        
		WRITING_RESPONSE,   
		CLOSED              
	};
	
	private:
	int                 m_fd;//form sicket
	ClientState         m_state;
	char                m_read_buffer[READ_BUFFER_SIZE];

	RequestParser		m_parser;
	HttpResponse        m_response;
	const Server&       m_server_config;
    client(const client& other);
    client& operator=(const client& other);

public:
    client(int fd, const Server& config);
    
    ~client();
    ClientState handleRead();
    ClientState handleWrite();
    void        updatePollEvents(struct pollfd& pfd);
    int         getFd() const;
    ClientState getState() const;
};

#endif