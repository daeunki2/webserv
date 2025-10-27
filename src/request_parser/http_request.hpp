/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_request.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 19:03:30 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/27 13:40:04 by daeunki2         ###   ########.fr       */
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
#define HTTP_REQUEST_HPP

#include <string>
#include <map>
#include <algorithm> // for std::transform
#include <cctype>    // for ::tolower

class http_request
{
private:
    std::string m_method;
    std::string m_uri_path;
    std::string m_query_string; // 쿼리 스트링 (? 이후)
    std::string m_version;      // HTTP/1.1
    std::map<std::string, std::string> m_headers; // 헤더 키는 소문자로 저장
    std::string m_body;

    long long   m_content_length;
    bool        m_is_chunked;

    // Helper: 문자열을 소문자로 변환
    static std::string to_lower(const std::string& str);

public:
    http_request();
    http_request(const http_request& src);
    http_request& operator=(const http_request& src);
    ~http_request();

    // Getter
    const std::string &get_method() const;
    const std::string &get_uri_path() const;
    const std::string &get_query_string() const;
    const std::string &get_version() const;
    const std::map<std::string, std::string> &get_headers() const;
    const std::string &get_body() const;
    long long get_content_length() const;    
    const std::string get_header_value(const std::string &key) const;

    // Setter
    void set_method(const std::string &method);
    void set_uri_path(const std::string &path);
    void set_query_string(const std::string &query);
    void set_version(const std::string &version);
    void set_content_length(long long length);
    void set_is_chunked(bool chunked);

	void add_header(const std::string &key, const std::string &value);
    void append_body(const std::string &data); // body는 누적되어 추가될 수 있음
    bool is_chunked() const;
    void reset();
};

#endif