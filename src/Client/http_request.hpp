/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_request.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 19:03:30 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/20 10:23:08 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*              store important information from raw request                  */
/*============================================================================*/

/*
Request Line	Method (GET/POST/DELETE)	// define what server need to perform.
Request Line	URI Path (/index.html)	//define what kinds of resources asked server 
Request Line	HTTP Version (HTTP/1.1)	//varify the http version if not, 505error 버전이 
Header	Host	HTTP/1.1에서 필수 헤더입니다. 어떤 가상 서버(Server Block)로 요청을 보낼지 결정합니다.
Header	Content-Length	POST/PUT 요청 시, 본문 데이터의 정확한 길이를 파서에게 알려줍니다.
Header	Transfer-Encoding	Content-Length 대신 청크 처리를 해야 하는지 알려줍니다. (대부분 chunked만 처리)
Header	Connection	연결을 유지(keep-alive)할지 요청 처리 후 즉시 닫을지(close) 결정합니다.
*/

#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include <string>
# include <map>

class HttpRequest
{
public:
    HttpRequest();
    HttpRequest(const HttpRequest &o);
    HttpRequest &operator=(const HttpRequest &o);
    ~HttpRequest();

    /* setters */
    void setMethod(const std::string &m);
    void setUri(const std::string &u);
    void setPath(const std::string &p);
    void setQuery(const std::string &q);
    void setVersion(const std::string &v);
    void addHeader(const std::string &key, const std::string &value);
    void setBody(const std::string &b);
    void appendBody(const std::string &b);
    void setKeepAlive(bool k);

    /* getters */
    const std::string &getMethod() const;
    const std::string &getUri() const;
    const std::string &getPath() const;
    const std::string &getQuery() const;
    const std::string &getVersion() const;
    const std::map<std::string, std::string> &getHeaders() const;
    const std::string &getBody() const;
    bool getKeepAlive() const;

    /* helpers */
    bool hasHeader(const std::string &key) const;
    std::string getHeader(const std::string &key) const;

private:
    std::string                           _method;
    std::string                           _uri;
    std::string                           _path;
    std::string                           _query;
    std::string                           _version;
    std::map<std::string, std::string>    _headers;
    std::string                           _body;
    bool                                  _keepAlive;
};

#endif

