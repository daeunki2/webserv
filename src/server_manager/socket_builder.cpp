/* ************************************************************************** */
/*	*/
/*	:::	  ::::::::   */
/*   socket_builder.cpp	 :+:	  :+:	:+:   */
/*	+:+ +:+	 +:+	 */
/*   By: daeunki2 <daeunki2@student.42.fr>	  +#+  +:+	   +#+	*/
/*	+#+#+#+#+#+   +#+	   */
/*   Created: 2025/11/02 14:56:12 by daeunki2	  #+#	#+#	 */
/*   Updated: 2025/11/02 15:22:11 by daeunki2	 ###   ########.fr	   */
/*	*/
/* ************************************************************************** */

#include "socket_builder.hpp"


// ******************************************************
//	               Constructer 
// ******************************************************
socket_builder::socket_builder(const socket_builder& other)
{

};

socket_builder& socket_builder::operator=(const socket_builder& other)
{

};

socket_builder::socket_builder(int port, const std::string& ip)
{

};

socket_builder::~socket_builder()
{

};

// ******************************************************
//	               Private Builder Steps
// ******************************************************

void	socket_builder::createSocket()
{
	m_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_fd < 0)
	{
		throw std::runtime_error(get_error_message("socket creation"));
	}
};

void	socket_builder::setSocketOptions()
{
	int opt = 1; 
	if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(m_fd); 
		throw std::runtime_error(get_error_message("setsockopt SO_REUSEADDR"));
	}
};

void	socket_builder::bindSocket()
{
	struct sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(m_port);

	if (bind(m_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		close(m_fd); 
		throw std::runtime_error(get_error_message("bind"));
	}
};

void	socket_builder::startListening()
{
	if (listen(m_fd, 1024) < 0)
	{ 
	close(m_fd);
	throw std::runtime_error(get_error_message("listen"));
	}
};


void	socket_builder::setNonBlocking()
{
	if (fcntl(m_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(m_fd);
		throw std::runtime_error(get_error_message("fcntl O_NONBLOCK"));
	}
}

// ******************************************************
//	                Public Interface
// ******************************************************

socket_builder::socket_builder(int port, const std::string& ip)
: m_fd(-1), m_port(port), m_ip(ip) 
{
	createSocket();
	setSocketOptions();
	bindSocket();
	startListening();
	setNonBlocking();
}

socket_builder::~socket_builder()
{
	if (m_fd != -1)
	{
		close(m_fd); 
	}
}

// ******************************************************
//                      Getter
// ******************************************************


int socket_builder::getFd() const
{
	return m_fd;
};

int socket_builder::getPort() const
{
	return m_port;
};

std::string socket_builder::getIp() const
{
	return m_ip;
};


// ******************************************************
//	                  error control
// ******************************************************

static std::string get_error_message(const std::string& msg)
{
	return "[SocketBuilder] " + msg + " failed: " + std::string(strerror(errno));
};