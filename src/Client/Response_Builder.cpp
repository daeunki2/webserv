/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_Builder.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 11:28:29 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/20 10:50:06 by daeunki2         ###   ########.fr       */
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

Response_Builder::Response_Builder(Server *server, const HttpRequest &req)
: _server(server), _req(req)
{}

Response_Builder::Response_Builder(const Response_Builder &o)
{
    *this = o;
}

Response_Builder &Response_Builder::operator=(const Response_Builder &o)
{
    if (this != &o)
    {
        _server = o._server;
        _req    = o._req;
    }
    return *this;
}

Response_Builder::~Response_Builder() {}

std::string Response_Builder::statusMessage(int status) const
{
    if (status == 200) return "OK";
    if (status == 201) return "Created";
    if (status == 204) return "No Content";
    if (status == 301) return "Moved Permanently";
    if (status == 302) return "Found";
    if (status == 400) return "Bad Request";
    if (status == 403) return "Forbidden";
    if (status == 404) return "Not Found";
    if (status == 405) return "Method Not Allowed";
    if (status == 413) return "Payload Too Large";
    if (status == 500) return "Internal Server Error";
    if (status == 501) return "Not Implemented";
    if (status == 502) return "Bad Gateway";
    return "Unknown";
}

const Location *Response_Builder::matchLocation(const std::string &path) const
{
    const std::vector<Location> &locs = _server->getLocations();
    const Location *best = 0;
    size_t bestLen = 0;

    for (size_t i = 0; i < locs.size(); ++i)
    {
        const std::string &lp = locs[i].getPath(); // ex) "/upload"
        if (lp.empty())
            continue;
        if (path.compare(0, lp.size(), lp) == 0 &&
            (lp.size() > bestLen))
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
        return true; // server level에서는 제한 X

    const std::vector<std::string> &methods = loc->getMethods();
    if (methods.empty())
        return true;

    const std::string &m = _req.getMethod();
    for (size_t i = 0; i < methods.size(); ++i)
    {
        if (methods[i] == m)
            return true;
    }
    return false;
}

std::string Response_Builder::getMimeType(const std::string &path) const
{
    std::string::size_type pos = path.rfind('.');
    if (pos == std::string::npos)
        return "application/octet-stream";

    std::string ext = path.substr(pos + 1);
    if (ext == "html" || ext == "htm")
        return "text/html";
    if (ext == "css")
        return "text/css";
    if (ext == "js")
        return "application/javascript";
    if (ext == "png")
        return "image/png";
    if (ext == "jpg" || ext == "jpeg")
        return "image/jpeg";
    if (ext == "gif")
        return "image/gif";
    if (ext == "txt")
        return "text/plain";
    return "application/octet-stream";
}

std::string Response_Builder::applyRoot(const Location *loc, const std::string &path) const
{
    std::string root = _server->getRoot();
    if (loc && !loc->getRoot().empty())
        root = loc->getRoot();

    if (root.size() && root[root.size() - 1] == '/')
        root.erase(root.size() - 1);

    std::string full = root + path;
    return full;
}

std::string Response_Builder::findErrorPage(int status) const
{
    const std::vector<std::pair<int, std::string> > &errs = _server->getErrorPages();
    for (size_t i = 0; i < errs.size(); ++i)
    {
        if (errs[i].first == status)
            return errs[i].second;
    }
    return "";
}

std::string Response_Builder::buildSimpleResponse(int status, const std::string &body)
{
    std::ostringstream oss;
    std::string msg = statusMessage(status);

    oss << "HTTP/1.1 " << status << " " << msg << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Content-Type: text/html\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";
    oss << body;

    return oss.str();
}

std::string Response_Builder::buildErrorResponse(int status, const std::string &msg)
{
    std::string custom = findErrorPage(status);
    if (!custom.empty())
    {
        std::string fsPath = applyRoot(0, custom);
        std::ifstream f(fsPath.c_str(), std::ios::in | std::ios::binary);
        if (f)
        {
            std::ostringstream body;
            body << f.rdbuf();
            std::string content = body.str();

            std::ostringstream oss;
            oss << "HTTP/1.1 " << status << " " << statusMessage(status) << "\r\n";
            oss << "Content-Length: " << content.size() << "\r\n";
            oss << "Content-Type: text/html\r\n";
            oss << "Connection: close\r\n";
            oss << "\r\n";
            oss << content;
            return oss.str();
        }
    }

    std::ostringstream body;
    body << "<html><head><title>" << status << " " << statusMessage(status)
         << "</title></head><body><h1>" << status << " " << statusMessage(status)
         << "</h1><p>" << msg << "</p></body></html>";
    return buildSimpleResponse(status, body.str());
}

std::string Response_Builder::buildRedirectResponse(int status, const std::string &url)
{
    std::ostringstream oss;
    std::string msg = statusMessage(status);

    std::string body = "<html><head><title>" + toString(status) + " " + msg +
                       "</title></head><body><h1>" + msg +
                       "</h1><p><a href=\"" + url + "\">" + url +
                       "</a></p></body></html>";

    oss << "HTTP/1.1 " << status << " " << msg << "\r\n";
    oss << "Location: " << url << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Content-Type: text/html\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";
    oss << body;

    return oss.str();
}

std::string Response_Builder::buildAutoindexResponse(const std::string &fsPath,
                                                     const std::string &urlPath)
{
    DIR *dir = opendir(fsPath.c_str());
    if (!dir)
        return buildErrorResponse(403, "Autoindex forbidden");

    std::ostringstream body;
    body << "<html><head><title>Index of " << urlPath
         << "</title></head><body><h1>Index of " << urlPath << "</h1><ul>";

    struct dirent *entry;
    while ((entry = readdir(dir)) != 0)
    {
        std::string name = entry->d_name;
        if (name == "." || name == "..")
            continue;
        body << "<li><a href=\"" << urlPath;
        if (urlPath.size() && urlPath[urlPath.size() - 1] != '/')
            body << "/";
        body << name << "\">" << name << "</a></li>";
    }
    closedir(dir);
    body << "</ul></body></html>";

    return buildSimpleResponse(200, body.str());
}

std::string Response_Builder::buildFileResponse(const std::string &fsPath, int status)
{
    std::ifstream f(fsPath.c_str(), std::ios::in | std::ios::binary);
    if (!f)
        return buildErrorResponse(404, "File not found");

    std::ostringstream body;
    body << f.rdbuf();
    std::string content = body.str();

    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << " " << statusMessage(status) << "\r\n";
    oss << "Content-Length: " << content.size() << "\r\n";
    oss << "Content-Type: " << getMimeType(fsPath) << "\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";
    oss << content;

    return oss.str();
}

std::string Response_Builder::build()
{
    const std::string &method = _req.getMethod();
    const std::string &path   = _req.getPath();

    if (method != "GET" && method != "POST" && method != "DELETE")
        return buildErrorResponse(405, "Method not allowed");

    const Location *loc = matchLocation(path);

    if (!isMethodAllowed(loc))
        return buildErrorResponse(405, "Method not allowed");

    // redirect
    if (loc && loc->isRedirect())
        return buildRedirectResponse(loc->getRedirectCode(), loc->getRedirectUrl());

    // body size 제한
    if (_req.getBody().size() > _server->getClientMaxBodySize())
        return buildErrorResponse(413, "Payload too large");

    // 파일 시스템 경로 계산
    std::string fsPath = applyRoot(loc, path);

    // 디렉토리인지 확인
    struct stat st;
    if (stat(fsPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
    {
        std::string indexFile;
        if (loc && !loc->getIndex().empty())
            indexFile = loc->getIndex();
        else
            indexFile = "index.html";

        if (fsPath.size() && fsPath[fsPath.size() - 1] != '/')
            fsPath += "/";
        std::string indexPath = fsPath + indexFile;

        if (stat(indexPath.c_str(), &st) == 0 && S_ISREG(st.st_mode))
            return buildFileResponse(indexPath, 200);

        // autoindex
        bool autoindex = false;
        if (loc)
            autoindex = loc->getAutoindex();
        if (autoindex)
            return buildAutoindexResponse(fsPath, path);
        else
            return buildErrorResponse(403, "Directory listing forbidden");
    }

    // 메소드별 처리
    if (method == "GET")
    {
        return buildFileResponse(fsPath, 200);
    }
    else if (method == "POST")
    {
        // 업로드 처리
        const std::string *uploadPath;
		uploadPath = 0;
        std::string up;
        if (loc && !loc->getUploadPath().empty())
            up = loc->getUploadPath();
        if (!up.empty())
        {
            if (up[up.size() - 1] != '/')
                up += "/";
            std::string filename = "upload_" + toString((int)time(0)) + ".bin";
            std::string fullPath = up + filename;

            std::ofstream out(fullPath.c_str(), std::ios::out | std::ios::binary);
            if (!out)
                return buildErrorResponse(500, "Cannot open upload file");

            out << _req.getBody();
            out.close();

            return buildSimpleResponse(201, "<html><body><h1>Uploaded</h1></body></html>");
        }
        else
        {
            // 업로드 설정 없으면 그냥 200 OK 텍스트 리턴
            return buildSimpleResponse(200, "<html><body><h1>POST OK</h1></body></html>");
        }
    }
    else if (method == "DELETE")
    {
        if (stat(fsPath.c_str(), &st) != 0)
            return buildErrorResponse(404, "File not found");

        if (std::remove(fsPath.c_str()) != 0)
            return buildErrorResponse(500, "Delete failed");

        return buildSimpleResponse(200, "<html><body><h1>Deleted</h1></body></html>");
    }

    return buildErrorResponse(500, "Unhandled case");
}
