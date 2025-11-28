/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_Builder.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 11:28:29 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/28 19:42:46 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Response_Builder.hpp"
#include "Utils.hpp"
#include "Logger.hpp"

#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <cstdio>

/* ************************************************************************** */
/*                               Constructor                                  */
/* ************************************************************************** */

Response_Builder::Response_Builder(Server *server, const http_request &req, Client* clinet)
: _server(server), _req(req), _client(clinet)
{}

Response_Builder::~Response_Builder()
{}

/* ************************************************************************** */
/*                               Helpers                                      */
/* ************************************************************************** */

std::string Response_Builder::statusMessage(int status) const
{
	if (status == 200)
		return "OK";
	if (status == 201)
		return "Created";
	if (status == 204)
		return "No Content";
	if (status == 301)
		return "Moved Permanently";
	if (status == 302)
		return "Found";
	if (status == 400)
		return "Bad Request";
	if (status == 403)
		return "Forbidden";
	if (status == 404)
		return "Not Found";
	if (status == 405)
		return "Method Not Allowed";
	if (status == 413)
		return "Payload Too Large";
	if (status == 500)
		return "Internal Server Error";
	return "Unknown";
}

const Location* Response_Builder::matchLocation(const std::string &path) const
{
    const std::vector<Location> &locs = _server->getLocations();
    const Location *best = 0;
    size_t bestLen = 0;

    for (size_t i = 0; i < locs.size(); ++i)
    {
        const std::string &lp = locs[i].getPath();

        if (lp.size() == 0)
            continue;

        if (path.compare(0, lp.size(), lp) == 0 && lp.size() > bestLen)
        {
            best = &locs[i];
            bestLen = lp.size();
        }
    }
    return best;
}

bool Response_Builder::isMethodAllowed(const Location *loc) const
{
    if (!loc)
        return true;

    const std::vector<std::string> &methods = loc->getMethods();
    if (methods.empty())
        return true;

    const std::string &m = _req.get_method();

    for (size_t i = 0; i < methods.size(); ++i)
        if (methods[i] == m)
            return true;

    return false;
}

std::string Response_Builder::getMimeType(const std::string &path) const
{
    std::string::size_type pos = path.rfind('.');
    if (pos == std::string::npos) return "application/octet-stream";

    std::string ext = path.substr(pos + 1);
    if (ext == "html" || ext == "htm") return "text/html";
    if (ext == "css") return "text/css";
    if (ext == "js") return "application/javascript";
    if (ext == "png") return "image/png";
    if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
    if (ext == "gif") return "image/gif";
    if (ext == "txt") return "text/plain";

    return "application/octet-stream";
}

std::string Response_Builder::applyRoot(const Location *loc, const std::string &path) const
{
    std::string root = (loc && !loc->getRoot().empty()) ? loc->getRoot() : _server->getRoot();

    std::string url = path;

    // Remove prefix only if location matches
    if (loc && path.compare(0, loc->getPath().size(), loc->getPath()) == 0)
        url = path.substr(loc->getPath().size());

    // Ensure root does not end with slash
    if (!root.empty() && root[root.size() - 1] == '/')
        root.erase(root.size() - 1);

    // Ensure url starts with slash
    if (url.empty() || url[0] != '/')
        url = "/" + url;

    return root + url;
}


std::string Response_Builder::findErrorPage(int status) const
{
    const std::vector<std::pair<int, std::string> > &pages =_server->getErrorPages();

    for (size_t i = 0; i < pages.size(); ++i)
        if (pages[i].first == status)
            return pages[i].second;

    return "";
}

std::string Response_Builder::buildSimpleResponse(int status, const std::string &body)
{
    std::ostringstream oss;

    oss << "HTTP/1.1 " << status << " " << statusMessage(status) << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Content-Type: text/html\r\n";
	oss << "Connection: " << (_req.keep_alive() ? "keep-alive" : "close") << "\r\n\r\n";
    oss << body;

    return oss.str();
}

std::string Response_Builder::buildErrorResponse(int status, const std::string &msg)
{
    std::string custom = findErrorPage(status);

	   Logger::warn("[RESP] FD " + toString(_client->get_fd()) + " -> " + toString(status) + " " + statusMessage(status) + " (" + msg + ")");

    if (!custom.empty())
    {
        std::string fsPath = applyRoot(NULL, custom);

        std::ifstream f(fsPath.c_str(), std::ios::binary);
        if (f)
        {
            std::ostringstream buf;
            buf << f.rdbuf();
            std::string content = buf.str();

            std::ostringstream oss;
            oss << "HTTP/1.1 " << status << " " << statusMessage(status) << "\r\n";
            oss << "Content-Length: " << content.size() << "\r\n";
            oss << "Content-Type: text/html; charset=UTF-8\r\n";
			oss << "Connection: " << (_req.keep_alive() ? "keep-alive" : "close") << "\r\n\r\n";
            oss << content;

            return oss.str();
        }
    }

    std::ostringstream defaultBody;
    defaultBody << "<html><body><h1>" << status << " "
                << statusMessage(status)
                << "</h1><p>" << msg << "</p></body></html>";

    return buildSimpleResponse(status, defaultBody.str());
}

std::string Response_Builder::buildRedirectResponse(int status, const std::string &url)
{
    std::ostringstream oss;

    std::string body =
        "<html><body><h1>Redirect</h1>"
        "<p><a href=\"" + url + "\">" + url + "</a></p>"
        "</body></html>";

    oss << "HTTP/1.1 " << status << " " << statusMessage(status) << "\r\n";
    oss << "Location: " << url << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Content-Type: text/html\r\n";
		oss << "Connection: " << (_req.keep_alive() ? "keep-alive" : "close") << "\r\n\r\n";
    oss << body;

    return oss.str();
}

std::string Response_Builder::buildAutoindexResponse(const std::string &fsPath, const std::string &urlPath)
{
	DIR *dir = opendir(fsPath.c_str());
	if (!dir)
		return buildErrorResponse(403, "Autoindex forbidden");

    std::ostringstream body;

    body << "<html><body><h1>Index of " << urlPath << "</h1><ul>";

    struct dirent *e;
    while ((e = readdir(dir)))
    {
        std::string name = e->d_name;

        if (name == "." || name == "..")
            continue;

        body << "<li><a href=\"" << urlPath;
        if (!urlPath.empty() && urlPath[urlPath.size() - 1] != '/')
            body << "/";
        body << name << "\">" << name << "</a></li>";
    }

    closedir(dir);

    body << "</ul></body></html>";

    return buildSimpleResponse(200, body.str());
}

std::string Response_Builder::buildFileResponse(const std::string &fsPath, int status)
{
    std::ifstream f(fsPath.c_str(), std::ios::binary);

	Logger::info("[RESP] FD " + toString(_client->get_fd()) + " -> " + toString(status) + " " + statusMessage(status) + " file=" + fsPath);

    if (!f)
	{
        return buildErrorResponse(404, "File not found");
	}
    std::ostringstream buf;
    buf << f.rdbuf();
    std::string content = buf.str();

    std::ostringstream oss;

    oss << "HTTP/1.1 " << status << " " << statusMessage(status) << "\r\n";
    oss << "Content-Length: " << content.size() << "\r\n";
    oss << "Content-Type: " << getMimeType(fsPath) << "\r\n";
	oss << "Connection: " << (_req.keep_alive() ? "keep-alive" : "close") << "\r\n\r\n";
    oss << content;

    return oss.str();
}




std::string Response_Builder::handleDelete(const Location *loc, const std::string &path)
{
    std::string fsPath = applyRoot(loc, path);
    struct stat st;

    if (stat(fsPath.c_str(), &st) != 0)// 파일이 존재하지 않을때
        return buildErrorResponse(404, "File not found");

    if (S_ISDIR(st.st_mode))
        return buildErrorResponse(403, "Cannot delete directory");

    if (remove(fsPath.c_str()) != 0)
        return buildErrorResponse(500, "Delete failed");

    return buildSimpleResponse(200, "<html><body><h1>Deleted</h1></body></html>");
}

std::string Response_Builder::sanitizeFilename(const std::string &name)
{

    size_t pos = name.find_last_of("/\\");
    std::string base = (pos == std::string::npos) ? name : name.substr(pos + 1);

    if (base == "." || base == "..")
        return "";

    if (base.find("..") != std::string::npos)
        return "";

    return base;
}

std::string Response_Builder::parseMultipart(const std::string &body,const std::string &boundary,const std::string &uploadDir)
{
    std::string sep = "--" + boundary;
    size_t pos = 0;

    while (true)
    {
        size_t start = body.find(sep, pos);
        if (start == std::string::npos)
            return "400";

        start += sep.size();

        if (body.compare(start, 2, "--") == 0)
            break;

        if (body.compare(start, 2, "\r\n") == 0)
            start += 2;

        size_t headerEnd = body.find("\r\n\r\n", start);
        if (headerEnd == std::string::npos)
            return "400";

        std::string headerBlock = body.substr(start, headerEnd - start);

        std::string filename;
        {
            size_t fnPos = headerBlock.find("filename=\"");
            if (fnPos != std::string::npos)
            {
                fnPos += 10;
                size_t end = headerBlock.find("\"", fnPos);
                if (end != std::string::npos)
                    filename = headerBlock.substr(fnPos, end - fnPos);
            }
        }

        filename = sanitizeFilename(filename);

        size_t dataStart = headerEnd + 4;
        size_t nextSep = body.find(sep, dataStart);
        if (nextSep == std::string::npos)
            return "400";

        size_t dataEnd = nextSep - 2;

        if (filename.empty())
        {
            Logger::warn("Skipping multipart part (no filename). Only text field or empty file.");
            pos = nextSep;
            continue;
        }

        std::string filePath = uploadDir + "/" + filename;
        std::ofstream ofs(filePath.c_str(), std::ios::binary);

        if (!ofs.is_open())
            return "500";

        ofs.write(body.data() + dataStart, dataEnd - dataStart);
        ofs.close();

        pos = nextSep;
    }

    return "";
}

std::string Response_Builder::handleGet(const Location *loc)
{
    std::string fsPath = applyRoot(loc, _req.get_path());

    struct stat st;
    if (stat(fsPath.c_str(), &st) < 0)
        return buildErrorResponse(404, "Not Found");

    if (S_ISDIR(st.st_mode))
    {
        const std::string &reqPath = _req.get_path();

        if (!reqPath.empty() && reqPath[reqPath.size() - 1] != '/')
        {
            std::string location = reqPath + "/";
            return "HTTP/1.1 301 Moved Permanently\r\n"
                   "Location: " + location + "\r\n"
                   "Content-Length: 0\r\n"
                   "Connection: " + std::string(_req.keep_alive() ? "keep-alive" : "close") +
                   "\r\n\r\n";
        }

        if (loc && !loc->getIndex().empty())
        {
            std::string idx = fsPath + "/" + loc->getIndex();
            if (stat(idx.c_str(), &st) == 0 && S_ISREG(st.st_mode))
                return buildFileResponse(idx, 200);
        }

        // autoindex
		if (loc && loc->getAutoindex())
    		return buildAutoindexResponse(fsPath, _req.get_path());
        return buildErrorResponse(403, "Forbidden");
    }

    return buildFileResponse(fsPath, 200);
}

std::string Response_Builder::handlePost(const Location *loc, const std::string &path)
{
    (void)path;

    if (!loc || loc->getUploadPath().empty())
        return buildSimpleResponse(200, "<html><body><h1>POST OK</h1></body></html>");

    std::string uploadDir = loc->getUploadPath();

    std::string ctype = _req.get_header("Content-Type");
    size_t bpos = ctype.find("boundary=");
    if (bpos == std::string::npos)
    {
        Logger::warn("Missing multipart boundary");
        return buildErrorResponse(400, "Missing multipart boundary");
    }

    std::string boundary = ctype.substr(bpos + 9);
    const std::string &body = _req.get_body();

    std::string err = parseMultipart(body, boundary, uploadDir);

    if (!err.empty())
    {
        if (err == "500")
        {
            Logger::warn("Multipart upload failed (server error)");
            return buildErrorResponse(500, "Upload failed");
        }

        Logger::warn("Malformed multipart request");
        return buildErrorResponse(400, "Malformed multipart body");
    }

    return buildSimpleResponse(201, "<html><body><h1>Uploaded</h1></body></html>");
}




/* ************************************************************************** */
/*                                  build()                                   */
/* ************************************************************************** */

std::string Response_Builder::build()
{
    const std::string &method = _req.get_method();
    const std::string &path   = _req.get_path();
    Logger::info("[REQ] FD " + toString(_client->get_fd()) + " " + method + " " + path);

    if (_client->get_error_code() != 0)
    {
        int code = _client->get_error_code();
        return buildErrorResponse(code, statusMessage(code));
    }

    const Location *loc = matchLocation(path);

    if (!isMethodAllowed(loc))
	{
		Logger::warn("Method Not Allowed");
        return buildErrorResponse(405, "Method Not Allowed");
	}
    if (loc && loc->isRedirect())
    {
        return buildRedirectResponse(loc->getRedirectCode(),loc->getRedirectUrl());
    }

    if (method == "POST" &&
        _req.get_body().size() > _server->getClientMaxBodySize())
    {
		Logger::warn("Payload Too Large");
        return buildErrorResponse(413, "Payload Too Large");
    }

    if (method == "GET")
        return handleGet(loc);

    if (method == "POST")
        return handlePost(loc, path);

    if (method == "DELETE")
        return handleDelete(loc, path);

    return buildErrorResponse(501, "Not Implemented");
}

