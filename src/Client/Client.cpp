/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 15:40:04 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/04 12:34:57 by daeunki2         ###   ########.fr       */
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
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <limits.h>

/* ************************************************************************** */
/*                         Canonical form                                      */
/* ************************************************************************** */

Client::Client()
: _fd(-1), _server(0), _parser(), _state(RECVING_REQUEST), _response_buffer(), _sent_bytes(0), _error_code(0), _keep_alive(false), _default_body_limit(0), _location_limit_applied(false), _cgi(), last_active_time(time(NULL))
{
}

Client::Client(int fd, Server* server)
: _fd(fd), _server(server),_parser(),_state(RECVING_REQUEST),_response_buffer(),_sent_bytes(0),_error_code(0),_keep_alive(false), _default_body_limit(0), _location_limit_applied(false), _cgi(), last_active_time(time(NULL))
{
    if (_server)
    {
        long long limit = _server->getClientMaxBodySize();
        if (limit > 0)
        {
            _default_body_limit = limit;
            _parser.set_max_body_size(limit);
        }
    }
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
	_default_body_limit = o._default_body_limit;
	_location_limit_applied = o._location_limit_applied;
	_cgi             = CgiState();
		last_active_time = o.last_active_time;
	}
	return *this;
}

Client::~Client()
{
	abort_cgi();
}

/* ************************************************************************** */
/*                              CGI Helpers                                   */
/* ************************************************************************** */

void Client::reset_cgi_state()
{
	if (_cgi.stdin_fd >= 0)
		close(_cgi.stdin_fd);
	if (_cgi.stdout_fd >= 0)
		close(_cgi.stdout_fd);
	_cgi = CgiState();
}

void Client::send_simple_error_response(int status)
{
	std::string reason = (status == 500) ? "Internal Server Error" : "Bad Gateway";
	std::ostringstream oss;
	oss << "HTTP/1.1 " << status << " " << reason << "\r\n";
	oss << "Content-Length: 0\r\n";
	oss << "Connection: close\r\n\r\n";
	_response_buffer = oss.str();
	_sent_bytes = 0;
	_keep_alive = false;
	_state = SENDING_RESPONSE;
}

void Client::apply_location_body_limit()
{
	if (_location_limit_applied || !_server)
		return;

	const std::string &path = _parser.getRequest().get_path();
	if (path.empty())
		return;

	const Location *loc = _server->findLocation(path);
	if (!loc || !loc->hasClientMaxBodySize())
		return;

	_parser.set_max_body_size(loc->getClientMaxBodySize());
	_location_limit_applied = true;
}

char **Client::buildCgiEnv(const std::string& abs_script,
	const std::string& script_path,
	const Location* loc) const
{
	(void)loc;
	const http_request &req = _parser.getRequest();
	char **env = new char*[40];
	int i = 0;

	env[i++] = ft_strdup(("REQUEST_METHOD=" + req.get_method()).c_str());
	env[i++] = ft_strdup(("QUERY_STRING=" + req.get_query()).c_str());
	env[i++] = ft_strdup(("SCRIPT_FILENAME=" + abs_script).c_str());
	env[i++] = ft_strdup(("PATH_TRANSLATED=" + abs_script).c_str());
	env[i++] = ft_strdup(("PATH_INFO=" + script_path).c_str());

	long long contentLen = req.has_content_length()
		? req.get_content_length()
		: static_cast<long long>(req.get_body().size());
	env[i++] = ft_strdup(("CONTENT_LENGTH=" + toString(contentLen)).c_str());
	env[i++] = ft_strdup(("CONTENT_TYPE=" + req.get_header("Content-Type")).c_str());

	env[i++] = ft_strdup("SERVER_PROTOCOL=HTTP/1.1");
	env[i++] = ft_strdup("GATEWAY_INTERFACE=CGI/1.1");
	env[i++] = ft_strdup("REDIRECT_STATUS=200");

	const std::string &secret = req.get_header("X-Secret-Header-For-Test");
	if (!secret.empty())
		env[i++] = ft_strdup(("HTTP_X_SECRET_HEADER_FOR_TEST=" + secret).c_str());

	env[i] = NULL;
	return env;
}

void Client::freeEnv(char **env) const
{
	if (!env)
		return;
	for (int i = 0; env[i] != NULL; ++i)
		delete [] env[i];
	delete [] env;
}

bool Client::start_cgi_process(const Location* loc, const std::string& script_path)
{
	if (!loc)
		return false;

	abort_cgi();

	if (script_path.empty())
		return false;

	std::string abs_script = script_path;
	std::string script_dir = ".";
	size_t slash = abs_script.rfind('/');
	if (slash != std::string::npos)
	{
		if (slash == 0)
			script_dir = "/";
		else
			script_dir = abs_script.substr(0, slash);
	}
	std::string script_arg = abs_script;
	std::string cgiExecPath = loc->getCgiPath();
	if (!cgiExecPath.empty() && cgiExecPath[0] != '/')
	{
		char cwd_exec[PATH_MAX];
		if (getcwd(cwd_exec, sizeof(cwd_exec)) == NULL)
			return false;
		cgiExecPath = std::string(cwd_exec) + "/" + cgiExecPath;
	}

	int stdin_pipe[2];
	int stdout_pipe[2];
	if (pipe(stdin_pipe) < 0 || pipe(stdout_pipe) < 0)
		return false;

	if (fcntl(stdin_pipe[1], F_SETFL, O_NONBLOCK) == -1 ||
		fcntl(stdout_pipe[0], F_SETFL, O_NONBLOCK) == -1)
	{
		close(stdin_pipe[0]);
		close(stdin_pipe[1]);
		close(stdout_pipe[0]);
		close(stdout_pipe[1]);
		return false;
	}

	pid_t pid = fork();
	if (pid < 0)
	{
		close(stdin_pipe[0]);
		close(stdin_pipe[1]);
		close(stdout_pipe[0]);
		close(stdout_pipe[1]);
		return false;
	}

	if (pid == 0)
	{
		if (chdir(script_dir.c_str()) < 0)
		{
			const char msg[] = "chdir failed\n";
			write(2, msg, sizeof(msg) - 1);
			_exit(1);
		}
		if (dup2(stdin_pipe[0], STDIN_FILENO) < 0 ||
			dup2(stdout_pipe[1], STDOUT_FILENO) < 0)
		{
			const char msg[] = "dup2 failed\n";
			write(2, msg, sizeof(msg) - 1);
			kill(getpid(), SIGKILL);
		}

		close(stdin_pipe[0]);
		close(stdin_pipe[1]);
		close(stdout_pipe[0]);
		close(stdout_pipe[1]);

		char **envp = buildCgiEnv(abs_script, abs_script, loc);

		char *argv[3];
		argv[0] = const_cast<char*>(cgiExecPath.c_str());
		argv[1] = const_cast<char*>(script_arg.c_str());
		argv[2] = NULL;

		execve(cgiExecPath.c_str(), argv, envp);

		freeEnv(envp);
		const char msg[] = "execve failed\n";
		write(2, msg, sizeof(msg) - 1);
		kill(getpid(), SIGKILL);
	}

	close(stdin_pipe[0]);
	close(stdout_pipe[1]);

	_cgi.active = true;
	_cgi.stdin_fd = stdin_pipe[1];
	_cgi.stdout_fd = stdout_pipe[0];
	_cgi.pid = pid;
	_cgi.body_sent = 0;
	_cgi.stdin_closed = false;
	_cgi.stdout_closed = false;
	_cgi.output.clear();

	_state = CGI_SENDING_BODY;

	const std::string &body = _parser.getRequest().get_body();
	if (body.empty())
	{
		close(_cgi.stdin_fd);
		_cgi.stdin_fd = -1;
		_cgi.stdin_closed = true;
		_state = CGI_READING_OUTPUT;
	}

	return true;
}

bool Client::has_active_cgi() const
{
	return _cgi.active;
}

int Client::get_cgi_stdin_fd() const
{
	return _cgi.stdin_fd;
}

int Client::get_cgi_stdout_fd() const
{
	return _cgi.stdout_fd;
}

bool Client::is_cgi_body_complete() const
{
	return !_cgi.active || _cgi.stdin_closed;
}

bool Client::is_cgi_stdout_closed() const
{
	return !_cgi.active || _cgi.stdout_closed;
}

bool Client::handle_cgi_stdin_event()
{
	if (!_cgi.active || _cgi.stdin_closed || _cgi.stdin_fd < 0)
		return true;

	const std::string &body = _parser.getRequest().get_body();
	size_t remaining = body.size() - _cgi.body_sent;
	if (remaining == 0)
	{
		close(_cgi.stdin_fd);
		_cgi.stdin_fd = -1;
		_cgi.stdin_closed = true;
		_state = CGI_READING_OUTPUT;
		return true;
	}

	ssize_t w = write(_cgi.stdin_fd, body.data() + _cgi.body_sent, remaining);
	if (w < 0)
	{
		abort_cgi();
		send_simple_error_response(500);
		return false;
	}

	_cgi.body_sent += static_cast<size_t>(w);
	if (_cgi.body_sent >= body.size())
	{
		close(_cgi.stdin_fd);
		_cgi.stdin_fd = -1;
		_cgi.stdin_closed = true;
		_state = CGI_READING_OUTPUT;
	}
	return true;
}

bool Client::handle_cgi_stdout_event()
{
	if (!_cgi.active || _cgi.stdout_fd < 0)
		return true;

	char buf[8192];
	ssize_t r = read(_cgi.stdout_fd, buf, sizeof(buf));
	if (r < 0)
	{
		abort_cgi();
		send_simple_error_response(500);
		return false;
	}
	if (r == 0)
	{
		close(_cgi.stdout_fd);
		_cgi.stdout_fd = -1;
		_cgi.stdout_closed = true;
		return true;
	}

	_cgi.output.append(buf, r);
	return true;
}

void Client::finalize_cgi_response()
{
	Response_Builder builder(_server, _parser.getRequest(), this);
	_response_buffer = builder.buildCgiResponse(_cgi.output);
	_sent_bytes = 0;
	_keep_alive = _parser.getRequest().keep_alive();
	_state = SENDING_RESPONSE;
	Logger::info(Logger::TAG_CGI, "CGI finished for FD " + toString(_fd) +
		" body=" + toString(_cgi.output.size()) + " bytes");
}

void Client::handle_cgi_completion()
{
	if (!_cgi.active || !_cgi.stdout_closed)
		return;
	if (_cgi.pid > 0)
		waitpid(_cgi.pid, NULL, 0);
	finalize_cgi_response();
	reset_cgi_state();
	_cgi.active = false;
}

void Client::abort_cgi()
{
	if (!_cgi.active)
		return;
	if (_cgi.pid > 0)
	{
		kill(_cgi.pid, SIGKILL);
		waitpid(_cgi.pid, NULL, 0);
	}
	reset_cgi_state();
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
	abort_cgi();
    _parser.reset();
	bool had_location_limit = _location_limit_applied;
	_location_limit_applied = false;
	if (_default_body_limit > 0)
		_parser.set_max_body_size(_default_body_limit);
	else if (had_location_limit)
		_parser.set_max_body_size(0);

    _error_code = 0;
    _response_buffer.clear();
    _sent_bytes = 0;

    _state = RECVING_REQUEST;
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
	apply_location_body_limit();

	while (st == RequestParser::PARSING_IN_PROGRESS)
	{
		RequestParser::ParsingState cont = _parser.feed(NULL, 0);
		if (cont == RequestParser::PARSING_IN_PROGRESS)
			break;
		st = cont;
		apply_location_body_limit();
	}

    if (st == RequestParser::PARSING_ERROR)
    {
        Logger::warn(Logger::TAG_REQ, "Parsing error on FD " + toString(_fd) +
            " code=" + toString(_parser.get_error_code()));
        _error_code = _parser.get_error_code();
        _state = Client::ERROR;
        _keep_alive = false;
        return PARSING_COMPLETED;
    }
    else if (st == RequestParser::PARSING_COMPLETED)
    {
        _state = Client::REQUEST_COMPLETE;
        _keep_alive = _parser.getRequest().keep_alive();
        return PARSING_COMPLETED;
    }
    return PARSING_IN_PROGRESS;
}


/* ************************************************************************** */
/*                          ResponseBuilder                                   */
/* ************************************************************************** */

void Client::build_response()
{
    const http_request& req = _parser.getRequest();
	Logger::info(Logger::TAG_CORE, "Building response for FD " + toString(_fd) + " (" + req.get_method() + " " + req.get_path() + ")");

	Response_Builder builder(_server, req, this);
	std::string response;

	response = builder.build();// the real build logic

	if (_state == CGI_SENDING_BODY || _state == CGI_READING_OUTPUT)
		return;

    _response_buffer = response;
    _sent_bytes      = 0;
    _state           = SENDING_RESPONSE;
}
