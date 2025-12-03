/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_Builder.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 11:31:43 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/03 09:49:31 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef RESPONSE_BUILDER_HPP
#define RESPONSE_BUILDER_HPP

#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <sys/stat.h>
#include "Server.hpp"
#include "Location.hpp"
#include "Utils.hpp"
#include "http_request.hpp"
#include "Client.hpp"

class Response_Builder
{
private:
    Server*               _server;
    const http_request&   _req;     // reference → 반드시 초기화 리스트에서만 설정 가능
	Client*				  _client;
    // 복사 금지 (C++98 방식)
    Response_Builder(const Response_Builder&);
    Response_Builder& operator=(const Response_Builder&);

private:
    // 내부 유틸리티
    std::string statusMessage(int status) const;
    const Location* matchLocation(const std::string &path) const;
    bool isMethodAllowed(const Location *loc) const;
    std::string getMimeType(const std::string &path) const;
    std::string applyRoot(const Location *loc, const std::string &path) const;
    std::string findErrorPage(int status) const;
	std::string sanitizeFilename(const std::string &name);
    std::string buildSimpleResponse(int status, const std::string &body);
    std::string buildErrorResponse(int status, const std::string &msg);
    std::string buildRedirectResponse(int status, const std::string &url);
    std::string buildAutoindexResponse(const std::string &fsPath, const std::string &urlPath);
    std::string buildFileResponse(const std::string &fsPath, int status);
	/* Method handlers */
	std::string handleGet(const Location *loc);
	std::string handlePost(const Location *loc, const std::string &path);
	std::string handleDelete(const Location *loc, const std::string &path);

	/* POST helpers */
	std::string parseMultipart(const std::string &body,const std::string &boundary,const std::string &uploadDir);
	/*CGI*/
	bool		isCgiRequest(const Location* loc, const std::string& path) const;
	char**		buildCgiEnv( const std::string& script_path) const;
	std::string handleCgi(const Location* loc, const std::string& script_path);
	std::string buildHttpResponseFromCgi(const std::string& cgiOutput);
	void		freeEnv(char **env) const;

	public:
    Response_Builder(Server* server, const http_request& req, Client* client);
    ~Response_Builder();

    std::string build();
};

#endif
