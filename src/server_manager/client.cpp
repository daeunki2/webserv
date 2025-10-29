/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:36:30 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/29 14:36:50 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

// ******************************************************
//              Constructors & Destructor
// ******************************************************

Client::Client(int fd)
: m_fd(fd), m_request_parser(), m_status(RECEIVING_REQUEST)
{
    // Note: The socket should be set to non-blocking mode here or in ServerManager::handle_new_connection.
}

Client::Client(const Client& src)
: m_fd(src.m_fd), m_request_parser(src.m_request_parser), m_status(src.m_status)
{
    
}

Client& Client::operator=(const Client& src)
{
    if (this != &src) {
        m_fd = src.m_fd;
        m_request_parser = src.m_request_parser;
        m_status = src.m_status;
    }
    return *this;
}

Client::~Client()
{
    // Actual connection closure is handled by the ServerManager.
}