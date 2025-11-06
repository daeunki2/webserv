/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 12:28:23 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/06 12:10:29 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <ctime>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include "request_parser.hpp" 
#include "http_request.hpp" 
#include "Response_Builder.hpp"

class Server; 

enum ClientState
{
    READING_REQUEST,
    REQUEST_COMPLETE,
    CGI_PENDING,
    CGI_WRITING,
    CGI_READING,
    SENDING_RESPONSE,
    CONNECTION_CLOSE
};

class Client 
{
private:
    int             m_fd;
    ClientState     m_state;
    time_t          m_last_active_time;

    Server*			m_server_config;

    RequestParser   m_request_parser; 
    http_request    m_current_request; 
    
    std::string     m_response_buffer; 
    size_t          m_sent_bytes;     
    
    
public:
    Client(int fd, Server* config);
    ~Client();

    int                             get_fd() const;
    ClientState                     get_state() const;
    time_t                          get_last_active_time() const;
    const http_request&             get_request() const;
    const std::string&              get_response_buffer() const;
    size_t                          get_response_length() const;
    size_t&                         get_sent_bytes(); 
    
    
    void reset(); 
    void update_state(ClientState new_state);
    

    RequestParser::ParsingState handle_recv_data(const char* data, size_t size); 

    void build_response(); 
};

#endif