/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_parser.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 14:30:48 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/20 18:43:01 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*       take the raw request, parse it, and store in http_request class      */
/*============================================================================*/

#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <string>
#include <vector>
#include "../Client/http_request.hpp"

class RequestParser
{
public:
	enum ParsingState
	{
		PARSING_IN_PROGRESS,
		PARSING_COMPLETED,
		PARSING_ERROR
	};

	enum State
	{
		REQUEST_LINE,
		HEADERS,
		BODY,
		CHUNK_SIZE,
		CHUNK_DATA,
		COMPLETE,
		ERROR
	};

private:
	State        _state;
	std::string  _buffer;
	http_request _request;

	long long _content_to_read;
	long long _chunk_size;
	bool      _is_chunked;

private:
	// internal helpers
	bool extract_line(std::string& line);

	ParsingState parse_request_line();
	ParsingState parse_headers();
	void         parse_header_line(const std::string& line);

	ParsingState parse_body();
	ParsingState parse_chunk_size();
	ParsingState parse_chunk_data();

public:
    // canonical form
	RequestParser();
	RequestParser(const RequestParser& other);
	RequestParser& operator=(const RequestParser& other);
	~RequestParser();

    // main API
	ParsingState feed(const char* data, size_t len);

    // completed request
	const http_request& getRequest() const;

    // reset for keep-alive
	void reset();
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