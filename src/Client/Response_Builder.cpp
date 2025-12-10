/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_Builder.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 11:28:29 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/05 21:09:46 by daeunki2         ###   ########.fr       */
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
#include <cctype>

/* ************************************************************************** */
/*                               Constructor                                  */
/* ************************************************************************** */

Response_Builder::Response_Builder(Server *server, const http_request &req, Client* clinet)
: _server(server), _req(req), _client(clinet)
{}

Response_Builder::~Response_Builder()
{}

bool Response_Builder::isAbsolutePath(const std::string &path)
{
    return (!path.empty() && path[0] == '/');
}

std::string Response_Builder::trimTrailingSlashes(const std::string &path)
{
    if (path.size() <= 1)
        return path;
    size_t end = path.size();
    while (end > 1 && path[end - 1] == '/')
        --end;
    return path.substr(0, end);
}

std::string Response_Builder::resolveRootPath(const Location *loc) const
{
    std::string root = (loc && !loc->getRoot().empty())
                        ? loc->getRoot()
                        : _server->getRoot();

    if (root.empty())
        root = ".";

    std::string trimmed = trimTrailingSlashes(root);
    if (trimmed.empty())
        trimmed = ".";
    return trimmed;
}

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
    std::string base = resolveRootPath(loc);

    std::string url = path;
    if (loc)
    {
        const std::string& lp = loc->getPath();
        if (!lp.empty() && path.compare(0, lp.size(), lp) == 0)
            url = path.substr(lp.size());
    }

    if (!url.empty() && url[0] == '/')
        url.erase(0, 1);

    if (url.empty())
        return base;

    if (base == "/")
        return "/" + url;

    return base + "/" + url;
}

// std::string Response_Builder::applyRoot(const Location *loc, const std::string &path) const
// {
//     std::string root = (loc && !loc->getRoot().empty()) ? loc->getRoot() : _server->getRoot();

//     std::string url = path;

//     if (loc && path.compare(0, loc->getPath().size(), loc->getPath()) == 0)
//         url = path.substr(loc->getPath().size());

//     if (!root.empty() && root[root.size() - 1] == '/')
//         root.erase(root.size() - 1);

//     if (url.empty() || url[0] != '/')
//         url = "/" + url;

//     return root + url;
// }


std::string Response_Builder::findErrorPage(int status) const
{
    const std::vector<std::pair<int, std::string> > &pages =_server->getErrorPages();

    for (size_t i = 0; i < pages.size(); ++i)
        if (pages[i].first == status)
            return pages[i].second;

    return "";
}
/* ************************************************************************** */
/*                            Response builders                                */
/* ************************************************************************** */

std::string Response_Builder::buildSimpleResponse(int status, const std::string &body)
{
    std::ostringstream oss;

    oss << "HTTP/1.1 " << status << " " << statusMessage(status) << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";

    if (!body.empty())
        oss << "Content-Type: text/html\r\n";

    bool keep = _req.keep_alive();

    if (_client && _client->get_error_code() != 0)
        keep = false;

    if (status == 413)
        keep = false;

    oss << "Connection: " << (keep ? "keep-alive" : "close") << "\r\n\r\n";
    oss << body;

    return oss.str();
}

std::string Response_Builder::buildErrorResponse(int status, const std::string &msg)
{
    std::string custom = findErrorPage(status);

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

            bool keep = _req.keep_alive();

            if (_client && _client->get_error_code() != 0)
                keep = false;
            if (status == 413)
                keep = false;

            oss << "Connection: " << (keep ? "keep-alive" : "close") << "\r\n\r\n";
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

    oss << "HTTP/1.1 " << status << " " << statusMessage(status) << "\r\n";
    oss << "Location: " << url << "\r\n";
    oss << "Content-Length: 0\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";

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

    if (stat(fsPath.c_str(), &st) != 0)
        return buildErrorResponse(404, "File not found");

    if (S_ISDIR(st.st_mode))
        return buildErrorResponse(403, "Cannot delete directory");

    if (remove(fsPath.c_str()) != 0)
        return buildErrorResponse(500, "Delete failed");

    return buildSimpleResponse(200, "");
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
//            Logger::warn("Skipping multipart part (no filename). Only text field or empty file.");
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
    std::string reqPath = _req.get_path();
    std::string fsPath  = applyRoot(loc, reqPath);

    struct stat st;
    if (stat(fsPath.c_str(), &st) < 0)
        return buildErrorResponse(404, "Not Found");

    if (S_ISDIR(st.st_mode))
    {
        bool autoindexEnabled = (loc && loc->getAutoindex());
        bool needsSlash = (!reqPath.empty() && reqPath[reqPath.size() - 1] != '/');
        bool hasIndexFile = false;
        std::string idxPath;

        if (loc && !loc->getIndex().empty())
        {
            idxPath = fsPath;
            if (idxPath[idxPath.size() - 1] != '/')
                idxPath += "/";
            idxPath += loc->getIndex();

            struct stat idxStat;
            if (stat(idxPath.c_str(), &idxStat) == 0 && S_ISREG(idxStat.st_mode))
                hasIndexFile = true;
        }

        if (needsSlash)
        {
            // Redirect only if the directory could actually be served
            if (hasIndexFile || autoindexEnabled)
                return buildRedirectResponse(301, reqPath + "/");
            return buildErrorResponse(404, "Not Found");
        }

        if (hasIndexFile)
            return buildFileResponse(idxPath, 200);

        if (autoindexEnabled)
            return buildAutoindexResponse(fsPath, reqPath);

        return buildErrorResponse(404, "Not Found");
    }

    return buildFileResponse(fsPath, 200);
}



std::string Response_Builder::handlePost(const Location *loc, const std::string &path)
{
    (void)path;
    if (_client->get_error_code() == 413)
    {
        return buildErrorResponse(413, "Payload Too Large");
    }
    if (!loc )
        return buildSimpleResponse(200, "<html><body><h1>POST OK</h1></body></html>");

    std::string uploadDir = loc->getUploadPath();

    if (uploadDir.empty())
        return buildSimpleResponse(200, "<html><body><h1>POST OK</h1></body></html>");

    std::string ctype = _req.get_header("Content-Type");
    size_t bpos = ctype.find("boundary=");
    if (bpos == std::string::npos)
    {
        return buildErrorResponse(400, "Missing multipart boundary");
    }

    std::string boundary = ctype.substr(bpos + 9);
    const std::string &body = _req.get_body();

    std::string err = parseMultipart(body, boundary, uploadDir);

    if (!err.empty())
    {
        if (err == "500")
        {
//            Logger::warn("Multipart upload failed (server error)");
            return buildErrorResponse(500, "Upload failed");
        }

//        Logger::warn("Malformed multipart request");
        return buildErrorResponse(400, "Malformed multipart body");
    }

    return buildSimpleResponse(201, "<html><body><h1>Uploaded</h1></body></html>");
}




/* ************************************************************************** */
/*                                  build()                                   */
/* ************************************************************************** */


std::string Response_Builder::build413Response() const
{
    std::ostringstream oss;
    oss << "HTTP/1.1 413 Payload Too Large\r\n";
    oss << "Content-Length: 0\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";
    return oss.str();
}

std::string Response_Builder::build()
{
    const std::string &method = _req.get_method();
    const std::string &path   = _req.get_path();
    size_t body_size          = _req.get_body().size();

    Logger::info(Logger::TAG_REQ,
        "FD " + toString(_client->get_fd()) +
        " build response for " + method + " " + path
    );

    // ---------------- parse error ----------------
    if (_client->get_error_code() != 0)
    {
        Logger::warn(Logger::TAG_REQ, "Returning parse error " +
            toString(_client->get_error_code()) + " for FD " + toString(_client->get_fd()));
        return buildErrorResponse(_client->get_error_code(),
                                  statusMessage(_client->get_error_code()));
    }

    // ---------------- location ----------------
    const Location *loc = (_server ? _server->findLocation(path) : NULL);
    if (loc)
        Logger::info(Logger::TAG_REQ, "Matched location: " + loc->getPath());
    else
        Logger::info(Logger::TAG_REQ, "Matched location: <none>");

    // ---------------- method check ----------------
    if (!isMethodAllowed(loc))
    {
        Logger::warn(Logger::TAG_REQ, "Method not allowed: " + method + " " + path);
        return buildErrorResponse(405, "Method Not Allowed");
    }

    // ---------------- redirect ----------------
    if (loc && loc->isRedirect())
    {
        Logger::info(Logger::TAG_REQ, "Redirecting " + path + " -> " + loc->getRedirectUrl());
        return buildRedirectResponse(loc->getRedirectCode(),
                                     loc->getRedirectUrl());
    }

    // ---------------- body limit ----------------
    if (method == "POST" && loc && loc->hasClientMaxBodySize())
    {
        long long limit = loc->getClientMaxBodySize();

        if (limit > 0 && body_size > (size_t)limit)
        {
            Logger::warn(Logger::TAG_REQ, "Body exceeds limit (" + toString(body_size) + " > " + toString(limit) + ")");
            return build413Response();
        }
    }

    // ============================================================
    //                     ★ CGI 조건 검사 ★
    // ============================================================
    bool isCgi = isCgiRequest(loc, path);

    if (isCgi)
    {
        std::string script_path = applyRoot(loc, path);
        Logger::info(Logger::TAG_CGI, "Launching CGI for " + method + " " + path);
        if (!_client->start_cgi_process(loc, script_path))
            return buildErrorResponse(500, "CGI start failed");

        return "";
    }

    // ---------------- methods ----------------
    if (method == "HEAD")
    {
        std::string res = handleGet(loc);
        size_t pos = res.find("\r\n\r\n");
        if (pos != std::string::npos)
            res.erase(pos + 4); // remove body
        return res;
    }

    if (method == "GET")
    {
        return handleGet(loc);
    }

    if (method == "POST")
    {
        return handlePost(loc, path);
    }

    if (method == "DELETE")
    {
        return handleDelete(loc, path);
    }

    return buildErrorResponse(501, "Not Implemented");
}
