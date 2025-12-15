/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 15:40:04 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/11 16:15:44 by daeunki2         ###   ########.fr       */
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
#include <cctype>
#include <map>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

static size_t local_strlen(const char *s)
{
	size_t len = 0;

	if (!s)
		return 0;
	while (s[len] != '\0')
		++len;
	return len;
}

static void terminate_cgi_child(const char *msg)
{
	if (msg)
	{
		size_t len = local_strlen(msg);
		if (len > 0)
			(void)write(2, msg, len);
	}
	(void)signal(SIGPIPE, SIG_DFL);
	int suicide_pipe[2];
	if (pipe(suicide_pipe) == 0)
	{
		close(suicide_pipe[0]);
		const char dummy = '\0';
		(void)write(suicide_pipe[1], &dummy, 1);
		close(suicide_pipe[1]);
	}
	while (true)
		;
}

/* ************************************************************************** */
/*                         Canonical form                                      */
/* ************************************************************************** */

Client::Client()
: _fd(-1), _server(0), _listen_port(0), _parser(), _state(RECVING_REQUEST), _response_buffer(), _sent_bytes(0), _error_code(0), _keep_alive(false), _default_body_limit(0), _location_limit_applied(false), _cgi(), last_activity_tick(0), _remote_addr(), _remote_port()
{
}

Client::Client(int fd, Server* server, int listen_port, const std::string &remote_addr, const std::string &remote_port)
: _fd(fd), _server(server), _listen_port(listen_port), _parser(),_state(RECVING_REQUEST),_response_buffer(),_sent_bytes(0),_error_code(0),_keep_alive(false), _default_body_limit(0), _location_limit_applied(false), _cgi(), last_activity_tick(0), _remote_addr(remote_addr), _remote_port(remote_port)
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
		_listen_port     = o._listen_port;
		_parser          = o._parser;
		_state           = o._state;
	_response_buffer = o._response_buffer;
	_sent_bytes      = o._sent_bytes;
	_error_code      = o._error_code;
	_keep_alive      = o._keep_alive;
	_default_body_limit = o._default_body_limit;
	_location_limit_applied = o._location_limit_applied;
	_cgi             = CgiState();
		last_activity_tick = o.last_activity_tick;
		_remote_addr    = o._remote_addr;
		_remote_port    = o._remote_port;
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

static std::string trim_trailing_slash(const std::string &path)
{
	if (path.size() <= 1)
		return path;
	size_t end = path.size();
	while (end > 1 && path[end - 1] == '/')
		--end;
	return path.substr(0, end);
}

std::string Client::resolve_document_root(const Location* loc) const
{
	std::string root;
	if (loc && !loc->getRoot().empty())
		root = loc->getRoot();
	else if (_server)
		root = _server->getRoot();

	if (root.empty())
		root = ".";

	return trim_trailing_slash(root);
}

void Client::append_http_headers_to_env(std::vector<std::string> &env, const http_request &req) const
{
	const std::map<std::string, std::string> &headers = req.get_headers();

	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
	{
		const std::string &key = it->first;
		if (key == "content-length" || key == "content-type")
			continue;

		std::string formatted;
		for (size_t i = 0; i < key.size(); ++i)
		{
			char c = key[i];
			if (c == '-')
				formatted.push_back('_');
			else
				formatted.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
		}

		env.push_back("HTTP_" + formatted + "=" + it->second);
	}
}

char **Client::buildCgiEnv(const std::string& abs_script,
	const std::string& script_path,
	const Location* loc) const
{
	const http_request &req = _parser.getRequest();
	std::vector<std::string> entries;
	entries.reserve(48);

	std::string scriptName = script_path.empty() ? req.get_path() : script_path;
	if (scriptName.empty())
		scriptName = "/";

	entries.push_back("REQUEST_METHOD=" + req.get_method());
	entries.push_back("QUERY_STRING=" + req.get_query());
	entries.push_back("SCRIPT_FILENAME=" + abs_script);
	entries.push_back("PATH_TRANSLATED=" + abs_script);
	entries.push_back("PATH_INFO=" + scriptName);
	entries.push_back("SCRIPT_NAME=" + scriptName);
	entries.push_back("REQUEST_URI=" + req.get_uri());

	long long contentLen = req.has_content_length()
		? req.get_content_length()
		: static_cast<long long>(req.get_body().size());
	entries.push_back("CONTENT_LENGTH=" + toString(contentLen));

	std::string contentType = req.get_header("Content-Type");
	if (!contentType.empty())
		entries.push_back("CONTENT_TYPE=" + contentType);

	std::string protocol = req.get_version().empty() ? "HTTP/1.1" : req.get_version();
	entries.push_back("SERVER_PROTOCOL=" + protocol);
	entries.push_back("GATEWAY_INTERFACE=CGI/1.1");
	entries.push_back("REDIRECT_STATUS=200");
	entries.push_back("SERVER_SOFTWARE=webserv/1.0");
	entries.push_back("REQUEST_SCHEME=http");

	std::string serverName;
	if (_server && !_server->getServerName().empty())
		serverName = _server->getServerName();
	else
	{
		serverName = req.get_header("Host");
		size_t colon = serverName.find(':');
		if (colon != std::string::npos)
			serverName = serverName.substr(0, colon);
	}
	if (serverName.empty())
		serverName = "localhost";
	entries.push_back("SERVER_NAME=" + serverName);

	std::string serverPort = toString(_listen_port);
	entries.push_back("SERVER_PORT=" + serverPort);

	std::string documentRoot = resolve_document_root(loc);
	entries.push_back("DOCUMENT_ROOT=" + documentRoot);

	if (!_remote_addr.empty())
		entries.push_back("REMOTE_ADDR=" + _remote_addr);
	if (!_remote_port.empty())
		entries.push_back("REMOTE_PORT=" + _remote_port);

	append_http_headers_to_env(entries, req);

	char **env = new char*[entries.size() + 1];
	for (size_t i = 0; i < entries.size(); ++i)
		env[i] = ft_strdup(entries[i].c_str());
	env[entries.size()] = NULL;
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
	if (cgiExecPath.empty())
		return false;

	int base_dir_fd = -1;
	std::string exec_full_path = cgiExecPath;
	if (!cgiExecPath.empty() && cgiExecPath[0] != '/')
	{
		base_dir_fd = open(".", O_RDONLY);
		if (base_dir_fd < 0)
			return false;
		if (fcntl(base_dir_fd, F_SETFD, FD_CLOEXEC) == -1)
		{
			close(base_dir_fd);
			return false;
		}
		exec_full_path = "/proc/self/fd/" + toString(base_dir_fd) + "/" + cgiExecPath;
	}

	int stdin_pipe[2];
	int stdout_pipe[2];
	if (pipe(stdin_pipe) < 0 || pipe(stdout_pipe) < 0)
	{
		if (base_dir_fd >= 0)
			close(base_dir_fd);
		return false;
	}

	if (fcntl(stdin_pipe[1], F_SETFL, O_NONBLOCK) == -1 ||
		fcntl(stdout_pipe[0], F_SETFL, O_NONBLOCK) == -1)
	{
		close(stdin_pipe[0]);
		close(stdin_pipe[1]);
		close(stdout_pipe[0]);
		close(stdout_pipe[1]);
		if (base_dir_fd >= 0)
			close(base_dir_fd);
		return false;
	}

	pid_t pid = fork();
	if (pid < 0)
	{
		close(stdin_pipe[0]);
		close(stdin_pipe[1]);
		close(stdout_pipe[0]);
		close(stdout_pipe[1]);
		if (base_dir_fd >= 0)
			close(base_dir_fd);
		return false;
	}

	if (pid == 0)
	{
		if (chdir(script_dir.c_str()) < 0)
		{
			if (base_dir_fd >= 0)
				close(base_dir_fd);
			terminate_cgi_child("chdir failed\n");
		}
		if (dup2(stdin_pipe[0], STDIN_FILENO) < 0 ||
			dup2(stdout_pipe[1], STDOUT_FILENO) < 0)
		{
			if (base_dir_fd >= 0)
				close(base_dir_fd);
			terminate_cgi_child("dup2 failed\n");
		}

		close(stdin_pipe[0]);
		close(stdin_pipe[1]);
		close(stdout_pipe[0]);
		close(stdout_pipe[1]);

		char **envp = buildCgiEnv(abs_script, _parser.getRequest().get_path(), loc);

		char *argv[3];
		argv[0] = const_cast<char*>(exec_full_path.c_str());
		argv[1] = const_cast<char*>(script_arg.c_str());
		argv[2] = NULL;

		execve(exec_full_path.c_str(), argv, envp);

		freeEnv(envp);
		if (base_dir_fd >= 0)
			close(base_dir_fd);
		terminate_cgi_child("execve failed\n");
	}

	close(stdin_pipe[0]);
	close(stdout_pipe[1]);
	if (base_dir_fd >= 0)
		close(base_dir_fd);

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
	Logger::info(Logger::TAG_CGI, "CGI finished for FD " + toString(_fd) + " body=" + toString(_cgi.output.size()) + " bytes");
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

bool Client::is_sending_response() const
{
	return _state == SENDING_RESPONSE;
}

bool Client::has_pending_response_bytes() const
{
	if (_state != SENDING_RESPONSE)
		return false;
	return _sent_bytes < _response_buffer.size();
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
    last_activity_tick = 0;
}

/* ************************************************************************** */
/*                     recv data → RequestParser                              */
/* ************************************************************************** */

Client::ParsingState
Client::handle_recv_data(const char* data, size_t size)
{
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
        Logger::warn(Logger::TAG_REQ, "Parsing error on FD " + toString(_fd) + " code=" + toString(_parser.get_error_code()));
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
