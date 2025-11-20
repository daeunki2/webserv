/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_Builder.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 11:31:43 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/20 10:23:27 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_BUILDER_HPP
# define RESPONSE_BUILDER_HPP

# include <string>
# include "http_request.hpp"
# include "Server.hpp"
# include "Location.hpp"

class Response_Builder
{
public:
    Response_Builder(Server *server, const HttpRequest &req);
    Response_Builder(const Response_Builder &o);
    Response_Builder &operator=(const Response_Builder &o);
    ~Response_Builder();

    std::string build();

private:
    Server      *_server;
    HttpRequest _req;

    /* helpers */
    const Location *matchLocation(const std::string &path) const;
    std::string     buildErrorResponse(int status, const std::string &msg);
    std::string     buildRedirectResponse(int status, const std::string &url);
    std::string     buildAutoindexResponse(const std::string &fsPath, const std::string &urlPath);
    std::string     buildFileResponse(const std::string &fsPath, int status);
    std::string     buildSimpleResponse(int status, const std::string &body);

    bool            isMethodAllowed(const Location *loc) const;
    std::string     getMimeType(const std::string &path) const;
    std::string     statusMessage(int status) const;
    std::string     applyRoot(const Location *loc, const std::string &path) const;
    std::string     findErrorPage(int status) const;
};

#endif
