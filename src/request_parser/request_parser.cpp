/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_parser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:13:45 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/27 17:32:31 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_parser.hpp" 

// ******************************************************
//              Constructors & Destructor
// ******************************************************

RequestParser::RequestParser()
: m_state(REQUEST_LINE), m_buffer(""), m_request(), m_current_body_size(0)
{

};

RequestParser::RequestParser(const RequestParser& src)
: m_state(src.m_state), m_buffer(src.m_buffer), m_request(src.m_request), m_current_body_size(src.m_current_body_size)
{
    
};

RequestParser& RequestParser::operator=(const RequestParser& src)
{
    if (this != &src) {
        m_state = src.m_state;
        m_buffer = src.m_buffer;
        m_request = src.m_request;
        m_current_body_size = src.m_current_body_size;
    }
    return *this;
};

RequestParser::~RequestParser()
{
    
};

// ******************************************************
//                 Getter and reset
// ******************************************************
ParsingState RequestParser::get_state() const
{
	return m_state;
};

const http_request& RequestParser::get_request() const
{
	return m_request;
};

void RequestParser::reset()
{
	m_state = REQUEST_LINE;
	m_buffer.clear();
	m_request.reset();
	m_current_body_size = 0;
}
// ******************************************************
//                 Parser unit
// ******************************************************

ParsingState RequestParser::parseRequestLine()
{
    std::string line = extract_line();
    
    if (line.empty())
        return REQUEST_LINE;

    std::vector<std::string> tokens = split(line, ' ');
    
    if (tokens.size() != 3)
        return PARSING_ERROR;

    const std::string& method = tokens[0];
    if (method != "GET" && method != "POST" && method != "DELETE" && method != "HEAD")
        return PARSING_ERROR;
    
    m_request.set_method(method);

    const std::string& uri = tokens[1];
    size_t pos = uri.find('?');
    
    if (pos == std::string::npos)
	{
        m_request.set_uri_path(uri);
        m_request.set_query_string("");
    } 
    else
	{
        m_request.set_uri_path(uri.substr(0, pos));
        m_request.set_query_string(uri.substr(pos + 1));
    }
    
    const std::string& version = tokens[2];
    if (version != "HTTP/1.1")
        return PARSING_ERROR;
    
    m_request.set_version(version);

    return HEADERS;
}

ParsingState RequestParser::parseHeaders()
{
    std::string line = extract_line();

    if(line.empty())
	{
        return HEADERS_DONE;
    }
        
    size_t colon_pos = line.find(':');

    if (colon_pos == std::string::npos || colon_pos == 0)
	{
        return PARSING_ERROR;
    }


    std::string key = line.substr(0, colon_pos);
    std::string value = line.substr(colon_pos + 1);
    
    trim(value); 
    
    m_request.add_header(key, value);
    return HEADERS;
}

ParsingState RequestParser::determineBodyParsing()//4
{
	
};

ParsingState RequestParser::parseBody()//5
{
	
};

ParsingState RequestParser::parseChunkedBody()//6
{
	
};

std::string RequestParser::extract_line()
{
    size_t position = m_buffer.find("\r\n");
    
    if (position == std::string::npos)
        return "";

    std::string line = m_buffer.substr(0, position);
    
    m_buffer.erase(0, position + 2);
    
    return line;
}

void RequestParser::trim(std::string &str)
{
    const std::string whitespace = " \t\n\v\f\r";

    size_t end = str.find_last_not_of(whitespace);
    
    if (end != std::string::npos)
	{
        str.erase(end + 1);
    }
    else
	{
        str.clear();
        return;
    }

    size_t start = str.find_first_not_of(whitespace);

    if (start != std::string::npos)
	{
        str.erase(0, start);
    }
    else
	{
        str.clear();
    }
}

// ******************************************************
//                   Load parsed data
// ******************************************************
ParsingState RequestParser::load_data(const char* data, size_t size);//7
