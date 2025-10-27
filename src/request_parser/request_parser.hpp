/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_parser.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 14:30:48 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/27 17:30:53 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*       take the raw request, parse it, and store in http_request class      */
/*============================================================================*/


#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include "http_request.hpp"
#include "libft.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cerrno>  

class http_request;

enum ParsingState
{
    REQUEST_LINE,
    HEADERS,
    HEADERS_DONE,
    READING_BODY,
    READING_CHUNKED_BODY,
    PARSING_COMPLETED,
    PARSING_ERROR
};

class RequestParser
{
	private:
	ParsingState	m_state;
	std::string		m_buffer;
	http_request	m_request;
	size_t          m_current_body_size;
	
	ParsingState parseRequestLine();
    ParsingState parseHeaders();
    ParsingState determineBodyParsing(); // 헤더 완료 후 본문 방식 결정
    ParsingState parseBody(); // Content-Length 기반
    ParsingState parseChunkedBody(); // Transfer-Encoding: chunked 기반
    
    // 헬퍼 함수
    std::string extract_line(); // 버퍼에서 \r\n을 포함하는 한 줄을 추출하고 버퍼에서 제거
	void trim(std::string &str);
	
public:
    RequestParser();
    RequestParser(const RequestParser& src);
    RequestParser& operator=(const RequestParser& src);
    ~RequestParser();

    ParsingState load_data(const char* data, size_t size); 
    
    ParsingState get_state() const;
    const http_request& get_request() const;

    void reset();
};
#endif

/*
HTTP protocall

POST /upload_file HTTP/1.1 >>>  Request Line
//header stasrt.
Host: webserv.com >> parseHeaders
User-Agent: Mozilla/5.0
Content-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YW
Content-Length: 1258900
Connection: keep-alive
//headers fini
//body start
1258900 bite later...
//body fini
*/