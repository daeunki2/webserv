/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_parser.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 14:30:48 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/23 19:38:12 by daeunki2         ###   ########.fr       */
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

class http_request;

class RequestParser
{
	public:
	http_request parse(const std::string &raw_request);

	private:
	// 내부 파싱 단계별 함수
	void parseRequestLine(http_request &req, std::istringstream &stream);
	void parseHeaders(http_request &req, std::istringstream &stream);
	void parseBody(http_request &req, std::istringstream &stream);
	void trim(std::string &str);
};

#endif


// > POST /upload HTTP/1.1
// > Host: 127.0.0.1:8080
// > User-Agent: curl/7.81.0
// > Accept: */* 
// > Content-Length: 21
// > Content-Type: application/x-www-form-urlencoded