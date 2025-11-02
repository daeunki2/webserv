/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_builder.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 14:56:07 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/02 15:19:50 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_BUILDER_HPP
# define SOCKET_BUILDER_HPP

# include <string>
# include <sys/socket.h> // socket, bind, listen
# include <netinet/in.h> // sockaddr_in
# include <fcntl.h>      // fcntl, O_NONBLOCK
# include <unistd.h>     // close
# include <stdexcept>    // std::runtime_error를 사용하기 위해 포함
#include <cstring>      // strerror, memset
#include <errno.h>      // errno
#include <arpa/inet.h>  // INADDR_ANY, htons
#include <stdexcept>    // std::runtime_error

class socket_builder
{
private:
	int         m_fd;
	int         m_port;
	std::string m_ip;

	void    createSocket();
	void    setSocketOptions();
	void    bindSocket();
	void    startListening();
	void    setNonBlocking();

	socket_builder(const socket_builder& other);
	socket_builder& operator=(const socket_builder& other);

public:
    socket_builder(int port, const std::string& ip);

    ~socket_builder();

    int getFd() const; 
    int getPort() const;
    std::string getIp() const;
};

#endif