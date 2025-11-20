/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_Builder.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 11:31:43 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/20 19:15:47 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef RESPONSE_BUILDER_HPP
#define RESPONSE_BUILDER_HPP

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include "Server.hpp"
#include "Location.hpp"
#include "http_request.hpp"

class Response_Builder
{
private:
    Server*               _server;
    const http_request&   _req;     // reference → 반드시 초기화 리스트에서만 설정 가능

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

    std::string buildSimpleResponse(int status, const std::string &body);
    std::string buildErrorResponse(int status, const std::string &msg);
    std::string buildRedirectResponse(int status, const std::string &url);
    std::string buildAutoindexResponse(const std::string &fsPath, const std::string &urlPath);
    std::string buildFileResponse(const std::string &fsPath, int status);

public:
    Response_Builder(Server* server, const http_request& req);
    ~Response_Builder();

    std::string build();  // 최종 응답 생성
};

#endif
