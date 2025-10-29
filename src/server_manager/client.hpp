/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:33:37 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/29 14:38:25 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*              store important information of a browser                 */
/*============================================================================*/


#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "request_parser.hpp" 
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cerrno>

enum clientStatus
{
        RECEIVING_REQUEST,  // Receiving request data and parsing (waiting for POLLIN)
        REQUEST_READY,      // Request parsing complete, ready for response processing
        SENDING_RESPONSE,   // Sending response data (waiting for POLLOUT)
        CLOSE_CONNECTION    // Connection scheduled for closure
};

class client
{
	private:
    int             m_fd;               // Client socket file descriptor
    RequestParser   m_request_parser;   // Request parsing object
    clientStatus    m_status;           // Current state of the client
	ResponseBuilder m_response_builder; //
    

	public:
    Client(int fd);
    Client(const Client& src);
    Client& operator=(const Client& src);
    ~Client();
};

#endif