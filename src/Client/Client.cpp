/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 15:40:04 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/06 11:30:32 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"


// ******************************************************
//                 Constructer 
// ******************************************************
Client::Client(int fd, Server* config)
: m_fd(fd), m_state(READING_REQUEST), m_last_active_time(time(NULL)),m_server_config(config), m_request_parser(), m_current_request(), m_response_buffer(), m_sent_bytes(0)
{
	
}

Client::~Client()
{

}


// ******************************************************
//                 Setting
// ******************************************************
void Client::reset()
{
    m_state = READING_REQUEST; 
    m_last_active_time = time(NULL);
    m_request_parser.reset();
    m_response_buffer.clear(); 
    m_sent_bytes = 0;
};
void Client::update_state(ClientState new_state)
{
	m_state = new_state;
};


// ******************************************************
//                      Getter
// ******************************************************
int		Client::get_fd() const
{
	return m_fd;
}

ClientState			Client:: get_state() const
{
	return m_state;
}

time_t							Client::get_last_active_time() const
{
	return m_last_active_time;
}

const http_request&             Client::get_request() const
{
	return m_current_request;
}

const std::string&              Client::get_response_buffer() const
{
	return m_response_buffer;
}

size_t                          Client::get_response_length() const
{
	return m_response_buffer.length();
}

size_t&                         Client::get_sent_bytes()
{
	return m_sent_bytes;
}


// ******************************************************
//                      Request
// ******************************************************
RequestParser::ParsingState Client::handle_recv_data(const char* data, size_t size)
{
	RequestParser::ParsingState state = m_request_parser.load_data(data, size);

    if (state == RequestParser::PARSING_COMPLETED || state == RequestParser::PARSING_ERROR)
    {
		m_current_request = m_request_parser.get_request();
    }
    return state;
};

// ******************************************************
//                      Response
// ******************************************************
void Client::build_response()
{
    if (m_request_parser.get_state() == RequestParser::PARSING_ERROR)
    {
        m_response_buffer = "HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Length: 17\r\n\r\n400 Bad Request";
        update_state(CONNECTION_CLOSE); 
        return;
    }

    try
    {
        Response_Builder builder(m_current_request, m_server_config);
        
        m_response_buffer = builder.build_response();

        update_state(SENDING_RESPONSE); 
    }
    catch (const std::exception& e)
    {
        std::cerr << "Response Build Error: " << e.what() << std::endl;
        m_response_buffer = "HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\nContent-Length: 26\r\n\r\n500 Internal Server Error";
        update_state(CONNECTION_CLOSE);
    }
}