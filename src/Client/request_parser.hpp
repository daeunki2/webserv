/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_parser.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 14:30:48 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/20 10:23:20 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*       take the raw request, parse it, and store in http_request class      */
/*============================================================================*/

#ifndef REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP

# include <string>
# include "http_request.hpp"

class RequestParser
{
public:
    enum State
    {
        START_LINE,
        HEADERS,
        BODY,
        DONE,
        ERROR_STATE
    };

    RequestParser();
    RequestParser(const RequestParser &o);
    RequestParser &operator=(const RequestParser &o);
    ~RequestParser();

    int feed(const char *data, size_t len);

    const HttpRequest &getRequest() const;
    void reset();

    State getState() const;

private:
    State       _state;
    std::string _buffer;
    HttpRequest _req;

    bool        _hasContentLength;
    size_t      _contentLength;
    bool        _chunked;

    bool parseRequestLine();
    bool parseHeaders();
    bool parseBody();

    std::string toLower(const std::string &s) const;
};

#endif

/*
HTTP protocall

POST /upload_file HTTP/1.1 >>>  Request Line it is always the first line
//header stasrt. all header line fini with "\r\n"
Host: webserv.com \r\n
User-Agent: Mozilla/5.0
Content-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YW
Content-Length: 1258900
Connection: keep-alive
//headers fini with "\r\n\r\n"
//body start
1258900 bite later...
//body fini
*/