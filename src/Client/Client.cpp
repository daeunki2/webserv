/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 15:40:04 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/20 10:19:39 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

#include "Client.hpp"
#include "Logger.hpp"

Client::Client()
: _fd(-1),
  _server(0),
  _parser(),
  _request(),
  _responseBuffer(""),
  _sentBytes(0),
  _state(READING)
{
    last_active_time = time(NULL);
}

Client::Client(int fd, Server *serverConfig)
: _fd(fd),
  _server(serverConfig),
  _parser(),
  _request(),
  _responseBuffer(""),
  _sentBytes(0),
  _state(READING)
{
    last_active_time = time(NULL);
}

Client::Client(const Client &o)
{
    *this = o;
}

Client &Client::operator=(const Client &o)
{
    if (this != &o)
    {
        _fd             = o._fd;
        _server         = o._server;
        _parser         = o._parser;
        _request        = o._request;
        _responseBuffer = o._responseBuffer;
        _sentBytes      = o._sentBytes;
        _state          = o._state;
        last_active_time= o.last_active_time;
    }
    return *this;
}

Client::~Client()
{}

int Client::getFd() const
{
    return _fd;
}

Client::ParsingState Client::handle_recv_data(const char *data, size_t len)
{
    last_active_time = time(NULL);

    int res = _parser.feed(data, len);
    if (res == -1)
    {
        Logger::error("Request parsing error");
        _state = CONNECTION_CLOSE; // ServerManager가 정리하게
        return PARSING_ERROR;
    }
    else if (res == 1)
    {
        _request = _parser.getRequest();
        _state = REQUEST_COMPLETE;
        return PARSING_COMPLETED;
    }
    return PARSING_INCOMPLETE;
}

void Client::build_response()
{
    if (_state != REQUEST_COMPLETE)
        return;

    Response_Builder builder(_server, _request);
    _responseBuffer = builder.build();
    _sentBytes = 0;
}

const std::string &Client::get_response_buffer() const
{
    return _responseBuffer;
}

size_t &Client::get_sent_bytes()
{
    return _sentBytes;
}

Client::State Client::get_state() const
{
    return _state;
}

void Client::update_state(State s)
{
    _state = s;
}

void Client::reset()
{
    _parser.reset();
    _request = HttpRequest();
    _responseBuffer.clear();
    _sentBytes = 0;
    _state = READING;
    last_active_time = time(NULL);
}
