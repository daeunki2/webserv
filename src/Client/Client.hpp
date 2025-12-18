/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 12:28:23 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/08 13:23:31 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <cstddef>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>

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
		CGI_SENDING_BODY,
		CGI_READING_OUTPUT,
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
	int             _listen_port;
	RequestParser   _parser;            // HTTP parser
	ClientState     _state;             // state os client
	std::string     _response_buffer;   // full response "HTTP"
	size_t          _sent_bytes;        // buffer_that i use
	int             _error_code;        // HTTP error code  (0 if is normal)
	bool            _keep_alive;        //
	long long       _default_body_limit;
	bool            _location_limit_applied;

	struct CgiState
	{
		bool        active;
		int         stdin_fd;
		int         stdout_fd;
		pid_t       pid;
		size_t      body_sent;
		bool        stdin_closed;
		bool        stdout_closed;
		std::string output;

		CgiState()
		: active(false), stdin_fd(-1), stdout_fd(-1), pid(-1), body_sent(0), stdin_closed(false), stdout_closed(false), output()
		{}
	};

	CgiState        _cgi;

	public:
	unsigned long long last_activity_tick;

	public:
	// ---------------- Canonical Form ----------------
	Client();
	Client(int fd, Server* server, int listen_port, const std::string &remote_addr, const std::string &remote_port);
	Client(const Client& other);
	Client& operator=(const Client& other);
	~Client();

	// ---------------- basic info ----------------
	int                 get_fd() const;
	ClientState         get_state() const;
	const http_request& get_request() const;
	int 				get_error_code() const;
	bool                is_sending_response() const;
	bool                has_pending_response_bytes() const;
	
	const std::string&  get_response_buffer() const;
	size_t              get_response_length() const;
	size_t&             get_sent_bytes();

	bool                has_active_cgi() const;
	int                 get_cgi_stdin_fd() const;
	int                 get_cgi_stdout_fd() const;
	bool                is_cgi_body_complete() const;
	bool                is_cgi_stdout_closed() const;
	bool                handle_cgi_stdin_event();
	bool                handle_cgi_stdout_event();
	void                finalize_cgi_response();
	void                handle_cgi_completion();
	void                abort_cgi();

	// ---------------- status control ----------------
	void                reset();
	void                update_state(ClientState new_state);

	// send data from rcv
	ParsingState        handle_recv_data(const char* data, size_t size);

	// more like send 
	void                build_response();
	bool                start_cgi_process(const Location* loc, const std::string& script_path);

private:
	char**              buildCgiEnv(const std::string& abs_script,const std::string& script_path, const Location* loc) const;
	void                freeEnv(char **env) const;
	void                reset_cgi_state();
	void                send_simple_error_response(int status);
	void                apply_location_body_limit();
	std::string         resolve_document_root(const Location* loc) const;
	void                append_http_headers_to_env(std::vector<std::string> &env, const http_request &req) const;

	std::string         _remote_addr;
	std::string         _remote_port;

};

#endif
