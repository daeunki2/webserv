/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 15:40:04 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/26 12:48:11 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "../config_parser/Server.hpp"
#include "../etc/Logger.hpp"
#include "../etc/Utils.hpp"
#include "Response_Builder.hpp"
#include <sstream>
#include <cstring>
#include <cerrno>

/* ************************************************************************** */
/*                         Canonical form                                      */
/* ************************************************************************** */

Client::Client()
: _fd(-1), _server(0), _parser(), _state(RECVING_REQUEST), _response_buffer(), _sent_bytes(0), _error_code(0), _keep_alive(false), last_active_time(time(NULL))
{
}

Client::Client(int fd, Server* server)
: _fd(fd), _server(server),_parser(),_state(RECVING_REQUEST),_response_buffer(),_sent_bytes(0),_error_code(0),_keep_alive(false),last_active_time(time(NULL))
{
}

Client::Client(const Client &o)
{
    *this = o;
}

Client &Client::operator=(const Client &o)
{
	if (this != &o)
	{
		_fd              = o._fd;
		_server          = o._server;
		_parser          = o._parser;
		_state           = o._state;
		_response_buffer = o._response_buffer;
		_sent_bytes      = o._sent_bytes;
		_error_code      = o._error_code;
		_keep_alive      = o._keep_alive;
		last_active_time = o.last_active_time;
	}
	return *this;
}

Client::~Client()
{
	
}

/* ************************************************************************** */
/*                               getters                                       */
/* ************************************************************************** */

int Client::get_fd() const
{
    return _fd;
}

Client::ClientState Client::get_state() const
{
    return _state;
}

const http_request& Client::get_request() const
{
    return _parser.getRequest();
}

const std::string& Client::get_response_buffer() const
{
    return _response_buffer;
}

size_t Client::get_response_length() const
{
    return _response_buffer.size();
}

size_t& Client::get_sent_bytes()
{
    return _sent_bytes;
}

int 	Client::get_error_code() const
{
	return _error_code;
}



/* ************************************************************************** */
/*                          state / reset                                      */
/* ************************************************************************** */

void Client::update_state(ClientState st)
{
    _state = st;
}

void Client::reset()
{
    _parser.reset();
    _response_buffer.clear();
    _sent_bytes  = 0;
    _error_code  = 0;
    _keep_alive  = false;
    _state       = RECVING_REQUEST;
    last_active_time = time(NULL);
}

/* ************************************************************************** */
/*                     recv data → RequestParser                              */
/* ************************************************************************** */

Client::ParsingState
Client::handle_recv_data(const char* data, size_t size)
{
    last_active_time = time(NULL);

    RequestParser::ParsingState st = _parser.feed(data, size);

    if (st == RequestParser::PARSING_ERROR)
    {
		//todo - set_error. 
        Logger::warn("Parsing error on FD " + toString(_fd));
        _error_code = _parser.get_error_code(); 
        _state      = ERROR_STATE;
        return PARSING_ERROR;
    }
    else if (st == RequestParser::PARSING_COMPLETED)
    {
        _state = REQUEST_COMPLETE;
        return PARSING_COMPLETED;
    }

    return PARSING_IN_PROGRESS;
}

/* ************************************************************************** */
/*                          ResponseBuilder 연결                                */
/* ************************************************************************** */

void Client::build_response()
{
    const http_request& req = _parser.getRequest();

    Response_Builder builder(_server, req, this);

    std::string response;

	response = builder.build();

    _response_buffer = response;
    _sent_bytes      = 0;
    _state           = SENDING_RESPONSE;
}

