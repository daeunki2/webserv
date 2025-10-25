/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_parser.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 14:30:48 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/25 20:48:48 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*       take the raw request, parse it, and store in http_request class      */
/*============================================================================*/


#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include "http_request.hpp"
#include <string>
#include <sstream>
#include <iostream>

class http_request;

enum ParsingState
{
	REQUEST_LINE_START,
	READING_REQUEST_LINE,
	READING_HEADERS,
	READING_BODY,
	PARSING_COMPLETED,
	PARSING_ERROR
};

class RequestParser
{
	private:
	ParsingState	m_state;
	std::string		m_buffer;

	http_request	m_request;

	ParsingState parseRequestLine(std::string &data);
	ParsingState parseHeaders(std::string &data);
	ParsingState parseBody(std::string &data);
	
	void trim(std::string &str);

	public:
	RequestParser();
	RequestParser(const RequestParser& src);
	RequestParser& operator=(const RequestParser& src);
	~RequestParser();

	ParsingState load_data(const char* data, size_t size); 
	
	ParsingState get_state() const { return m_state; }
	const http_request& get_request() const { return m_request; }

	void reset(); 
};
#endif


// > POST /upload HTTP/1.1
// > Host: 127.0.0.1:8080
// > User-Agent: curl/7.81.0
// > Accept: */* 
// > Content-Length: 21
// > Content-Type: application/x-www-form-urlencoded