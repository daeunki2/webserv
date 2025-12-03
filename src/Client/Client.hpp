/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 12:28:23 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/03 10:35:15 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <ctime>        // time_t
#include <sys/socket.h>

#include "../config_parser/Server.hpp"
#include "http_request.hpp"
#include "request_parser.hpp"
#include "Utils.hpp"

class Client
{
	public:
	// client atate
	enum ClientState
	{
		RECVING_REQUEST,     // (recv + parser.feed)
		REQUEST_COMPLETE,
		SENDING_RESPONSE,
		CONNECTION_CLOSE,
		ERROR_STATE,
		ERROR
	};

	// for Server_Manager
	enum ParsingState
	{
		PARSING_IN_PROGRESS,
		PARSING_COMPLETED,
		PARSING_ERROR
	};

	private:
    int             _fd;                // clien FD
    Server*         _server;            // server settint
    RequestParser   _parser;            // HTTP parser
    ClientState     _state;             // state os client
    std::string     _response_buffer;   // full response "HTTP"
    size_t          _sent_bytes;        // buffer_that i use
	int             _error_code;        // HTTP error code  (0 if is normal)
	bool            _keep_alive;        //

	public:
	time_t          last_active_time;

	public:
	// ---------------- Canonical Form ----------------
	Client();
	Client(int fd, Server* server);
	Client(const Client& other);
	Client& operator=(const Client& other);
	~Client();

	// ---------------- basic info ----------------
	int                 get_fd() const;
	ClientState         get_state() const;
	const http_request& get_request() const;
	int 				get_error_code() const;
	
	const std::string&  get_response_buffer() const;
	size_t              get_response_length() const;
	size_t&             get_sent_bytes();

	// ---------------- status control ----------------
	void                reset();
	void                update_state(ClientState new_state);

	// send data from rcv
	ParsingState        handle_recv_data(const char* data, size_t size);

	// more like send 
	void                build_response();
	//cgi

};

#endif
