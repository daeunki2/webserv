/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_cgi.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 18:37:46 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/03 17:22:59 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response_Builder.hpp"
#include <unistd.h>
#include <sys/wait.h>



bool Response_Builder::isCgiRequest(const Location* loc, const std::string& path) const
{
	if (!loc || !loc->hasCgi())
		return false;

	std::string::size_type dot = path.rfind('.');
	if (dot == std::string::npos)
		return false;

	if (path.substr(dot) == loc->getCgiExtension())
		return true;
	return false;
}


char **Response_Builder::buildCgiEnv(const std::string& script_path) const
{
    const int ENV_SIZE = 10;
    char **env = new char*[ENV_SIZE];
    if (!env)
        return NULL;

    int i = 0;

    env[i++] = ft_strdup(std::string("REQUEST_METHOD=" + _req.get_method()).c_str());

    env[i++] = ft_strdup(std::string("QUERY_STRING=" + _req.get_query()).c_str());

    if (_req.get_method() == "POST")
    {
        env[i++] = ft_strdup(std::string("CONTENT_TYPE=" +_req.get_header("content-type")).c_str());
    }

    if (_req.get_method() == "POST" && _req.has_content_length())
    {
        env[i++] = ft_strdup(std::string("CONTENT_LENGTH=" +toString(_req.get_content_length())).c_str());
    }

    env[i++] = ft_strdup(
        std::string("SCRIPT_FILENAME=" + script_path).c_str());

    env[i++] = ft_strdup("PATH_INFO=");

    env[i++] = ft_strdup(
        std::string("SCRIPT_NAME=" + _req.get_path()).c_str());

    env[i++] = ft_strdup("SERVER_PROTOCOL=HTTP/1.1");
    env[i++] = ft_strdup("GATEWAY_INTERFACE=CGI/1.1");
    env[i++] = ft_strdup("REDIRECT_STATUS=200");

    env[i] = NULL;
    return env;
}





void Response_Builder::freeEnv(char **envp) const
{
    if (!envp)
        return;

    for (int i = 0; envp[i] != NULL; ++i)
        delete [] envp[i];

    delete [] envp;
}

std::string Response_Builder::handleCgi(const Location* loc, const std::string& script_path)
{
    int in_pipe[2];
    int out_pipe[2];

    if (pipe(in_pipe) < 0 || pipe(out_pipe) < 0)
        return buildErrorResponse(500, "pipe failed");

    pid_t pid = fork();
    if (pid < 0)
        return buildErrorResponse(500, "fork failed");

    if (pid == 0)
    {
        dup2(in_pipe[0], STDIN_FILENO);
        dup2(out_pipe[1], STDOUT_FILENO);

        close(in_pipe[1]);
        close(out_pipe[0]);
        close(in_pipe[0]);
        close(out_pipe[1]);

		char *argv[3];
		argv[0] = const_cast<char*>(loc->getCgiPath().c_str()); // /usr/bin/python3
		argv[1] = const_cast<char*>(script_path.c_str());       // ./cgi-bin/baguette.py
		argv[2] = NULL;

        char **envp = buildCgiEnv( script_path);
        if (!envp)
        {
            kill(getpid(), SIGTERM);
			return "";
        }
        execve(argv[0], argv, envp);

        freeEnv(envp);
        kill(getpid(), SIGTERM);
		return "";
    }

    close(in_pipe[0]);
    close(out_pipe[1]);


	const std::string &body = _req.get_body();
if (_req.get_method() == "POST" && !body.empty())
{
    const char *data = body.c_str();
    size_t      len  = body.size();
    size_t      sent = 0;

    while (sent < len)
    {
        ssize_t written = write(in_pipe[1], data + sent, len - sent);
        if (written < 0)
        {
            close(in_pipe[1]);
            close(out_pipe[0]);
            return buildErrorResponse(500, "CGI write failed");
        }
        sent += static_cast<size_t>(written);
    }
}

    close(in_pipe[1]);

    std::string cgi_output;
    char buffer[4096];
    ssize_t r;

    while ((r = read(out_pipe[0], buffer, sizeof(buffer))) > 0)
        cgi_output.append(buffer, r);

    close(out_pipe[0]);
    waitpid(pid, NULL, 0);

    return buildHttpResponseFromCgi(cgi_output);
}



std::string Response_Builder::buildHttpResponseFromCgi(const std::string& cgiOutput)
{
    std::ostringstream response;
    std::string body;
    std::string headers;

    size_t header_end = cgiOutput.find("\r\n\r\n");
    size_t header_len = 4;

    if (header_end == std::string::npos)
    {
        header_end = cgiOutput.find("\n\n");
        header_len = 2;
    }

    if (header_end != std::string::npos)
    {
        headers = cgiOutput.substr(0, header_end);
        body    = cgiOutput.substr(header_end + header_len);
    }
    else
    {
        body = cgiOutput;
    }

    while (!body.empty() && (body[0] == '\n' || body[0] == '\r'))
        body.erase(0, 1);

    response << "HTTP/1.1 200 OK\r\n";

    if (!headers.empty())
        response << headers << "\r\n";
    else
        response << "Content-Type: text/plain\r\n";

    response << "Content-Length: " << body.size() << "\r\n";
    response << "Connection: " << (_req.keep_alive() ? "keep-alive" : "close") << "\r\n";
    response << "\r\n";
    response << body;

    return response.str();
}

/*
std::string Response_Builder::buildHttpResponseFromCgi(
    const std::string& cgiOutput)
{
    std::ostringstream response;


    std::string headers;
    std::string body;

    size_t header_end = cgiOutput.find("\r\n\r\n");
    size_t header_len = 4;

    if (header_end == std::string::npos)
    {
        header_end = cgiOutput.find("\n\n");
        header_len = 2;
    }

    if (header_end != std::string::npos)
    {
        headers = cgiOutput.substr(0, header_end);
        body    = cgiOutput.substr(header_end + header_len);
    }
    else
    {
        body = cgiOutput;
    }

    while (!body.empty() && (body[0] == '\n' || body[0] == '\r'))
        body.erase(0, 1);


    std::string status_code = "200 OK";
    std::ostringstream forwarded_headers;

    std::istringstream header_stream(headers);
    std::string line;

    while (std::getline(header_stream, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.erase(line.size() - 1);

        if (line.find("Status:") == 0)
        {
            status_code = line.substr(7); // " xyz ..."
            while (!status_code.empty() && status_code[0] == ' ')
                status_code.erase(0, 1);
        }
        else if (!line.empty())
        {
            forwarded_headers << line << "\r\n";
        }
    }

    int code = 200;
    std::istringstream(status_code) >> code;

    bool no_content = (code == 204 || code == 304);


    response << "HTTP/1.1 " << status_code << "\r\n";
    response << forwarded_headers.str();

    if (!no_content)
    {
        if (forwarded_headers.str().find("Content-Type") == std::string::npos)
            response << "Content-Type: text/plain\r\n";

        response << "Content-Length: " << body.size() << "\r\n";
    }

    response << "Connection: "
            << (_req.keep_alive() ? "keep-alive" : "close") << "\r\n";
    response << "\r\n";

    if (!no_content)
        response << body;

    return response.str();
}
*/