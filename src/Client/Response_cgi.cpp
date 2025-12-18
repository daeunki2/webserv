/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_cgi.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 18:37:46 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/11 16:19:53 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response_Builder.hpp"
#include "Logger.hpp"
#include <sys/stat.h>

bool Response_Builder::isCgiRequest(const Location* loc, const std::string& path) const
{
    if (!loc || !loc->hasCgi())
        return false;

    std::string clean = path;
    size_t q = clean.find('?');
    if (q != std::string::npos)
        clean = clean.substr(0, q);

    size_t dot = clean.rfind('.');
    if (dot == std::string::npos)
        return false;

    std::string ext = clean.substr(dot);

    if (ext != loc->getCgiExtension())
        return false;

    std::string fsPath = applyRoot(loc, clean);
    struct stat st;

    if (stat(fsPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
        return false;

    return true;
}


std::string Response_Builder::buildHttpResponseFromCgi(const std::string& cgiOutput)
{
    std::ostringstream res;

    std::string headers;
    std::string body;

    size_t pos  = cgiOutput.find("\r\n\r\n");
    size_t skip = 4;

    if (pos == std::string::npos)
    {
        pos  = cgiOutput.find("\n\n");
        skip = 2;
    }

    if (pos != std::string::npos)
    {
        headers = cgiOutput.substr(0, pos);
        body    = cgiOutput.substr(pos + skip);
    }
    else
    {
        headers.clear();
        body = cgiOutput;
    }

    std::string status = "200 OK";
    bool hasContentLength = false;
    std::ostringstream forwardHeaders;

    std::istringstream hs(headers);
    std::string line;

    while (std::getline(hs, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (line.empty())
            continue;

        if (line.find("Status:") == 0)
        {
            status = line.substr(7);
            while (!status.empty() && status[0] == ' ')
                status.erase(0, 1);
        }
        else if (line.find("Content-Length:") == 0)
        {
            hasContentLength = true;
            forwardHeaders << line << "\r\n";
        }
        else
        {
            forwardHeaders << line << "\r\n";
        }
    }

    res << "HTTP/1.1 " << status << "\r\n";
    res << forwardHeaders.str();

    if (!hasContentLength)
    {
        res << "Content-Length: " << body.size() << "\r\n";
    }

    std::string connection = _req.keep_alive() ? "keep-alive" : "close";
    res << "Connection: " << connection << "\r\n";
    res << "\r\n";

    res << body;

    return res.str();
}

std::string Response_Builder::buildCgiResponse(const std::string& cgiOutput)
{
	return buildHttpResponseFromCgi(cgiOutput);
}
