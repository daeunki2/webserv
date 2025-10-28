/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_parser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:13:45 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/28 16:45:11 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_parser.hpp" 

// ******************************************************
//              Constructors & Destructor
// ******************************************************

RequestParser::RequestParser()
: m_state(REQUEST_LINE), m_chunk_state(READING_SIZE), m_buffer(""), m_request(), m_current_body_size(0), m_current_chunk_size(0)
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
		m_chunk_state = src.m_chunk_state;
        m_buffer = src.m_buffer;
        m_request = src.m_request;
        m_current_body_size = src.m_current_body_size;
		m_current_chunk_size = src.m_current_chunk_size;
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
	while (true)
	{
		if (m_buffer.size() >= 4 && m_buffer.substr(0, 4) == "\r\n\r\n")
		{
			m_buffer.erase(0, 4);
			return HEADERS_DONE;
		}

		std::string line = extract_line();

		if (line.empty())
		{
			if (m_buffer.empty())
			{
				return HEADERS_DONE;
			}
			if (m_buffer.find("\r\n") == std::string::npos)
			{
				return HEADERS;
			}
			continue; 
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
	}
}

ParsingState RequestParser::determineBodyParsing()
{

	std::string content_len = m_request.get_header_value("content-length");
	std::string transfer_encount = m_request.get_header_value("transfer-encoding");

	if (!content_len.empty() && !transfer_encount.empty())
	{
		return PARSING_ERROR;
	}

	if (!transfer_encount.empty() && transfer_encount == "chunked")
	{
		return READING_CHUNKED_BODY;
	}
	if (!content_len.empty())
	{
		char *endptr;
		long long length = std::strtoll(content_len.c_str(), &endptr, 10);

		if (*endptr != '\0' || length < 0)
		{
			return PARSING_ERROR;
		}

		m_request.set_content_length(static_cast<size_t>(length)); 
		return READING_BODY;
	}

	return PARSING_COMPLETED;
}

ParsingState RequestParser::parseBody()
{
	size_t target_length = m_request.get_content_length(); 

	while (m_buffer.size() > 0 && m_current_body_size < target_length)
	{
		size_t remaining_needed = target_length - m_current_body_size;
		size_t chunk_size = std::min(m_buffer.size(), remaining_needed); 
		std::string body_chunk = m_buffer.substr(0, chunk_size);

		m_request.append_body(body_chunk); 

		m_buffer.erase(0, chunk_size);

		m_current_body_size += chunk_size;
	}
	if (m_current_body_size == target_length)
		return PARSING_COMPLETED; 
	else
		return READING_BODY;
}
ParsingState RequestParser::parseChunkedBody()
{
	while (m_chunk_state != FINISHED)
	{
		if (m_chunk_state == READING_SIZE)
		{
			std::string line = extract_line();
			if (line.empty())
			{
				return READING_CHUNKED_BODY;
			}

			char *end;
			long long size_ll = std::strtoll(line.c_str(), &end, 16);

			if (*end != '\0' && *end != ';')
			{
				return PARSING_ERROR;
			}
			m_current_chunk_size = static_cast<size_t>(size_ll);
			if (m_current_chunk_size == 0)
			{
				m_chunk_state = FINISHED;
				continue;
			}
			m_chunk_state = READING_DATA;
		}
		if (m_chunk_state == READING_DATA)
		{
			if (m_buffer.size() < m_current_chunk_size)
			{
				return READING_CHUNKED_BODY;
			}
			std::string data_chunk = m_buffer.substr(0, m_current_chunk_size);
			m_request.append_body(data_chunk);
			m_buffer.erase(0, m_current_chunk_size);
			m_chunk_state = CONSUMING_CRLF;
		}

		if (m_chunk_state == CONSUMING_CRLF)
		{
			if (m_buffer.size() < 2 || m_buffer.substr(0, 2) != "\r\n")
			{
				return PARSING_ERROR;
			}
			m_buffer.erase(0, 2);
			m_chunk_state = READING_SIZE;
		}	
	}

	if (m_chunk_state == FINISHED)
	{
		if (m_buffer.size() >= 2 && m_buffer.substr(0, 2) == "\r\n")
		{
			m_buffer.erase(0, 2);
			return PARSING_COMPLETED;
		} 
		else if (m_buffer.size() > 0)
		{
			return PARSING_ERROR;
		}
		return READING_CHUNKED_BODY; 
	}
	return PARSING_ERROR;
}

// ******************************************************
//                   String control
// ******************************************************

std::string RequestParser::extract_line()
{
    size_t position = m_buffer.find("\r\n");
    
    if (position == std::string::npos)
        return std::string();

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
std::vector<std::string> RequestParser::split(const std::string &str, char delimiter)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end;

    while ((end = str.find(delimiter, start)) != std::string::npos)
    {
        if (end != start)
            result.push_back(str.substr(start, end - start));
        start = end + 1;
    }

    if (start < str.size())
        result.push_back(str.substr(start));

    return result;
}

std::string RequestParser::line_extracter(std::string separator)
{
    size_t position = m_buffer.find(separator);
    
    if (position == std::string::npos)
        return "";

    std::string line = m_buffer.substr(0, position);
    
    m_buffer.erase(0, position + 2);
    
    return line;
}
// ******************************************************
//                   Load parsed data
// ******************************************************


ParsingState RequestParser::load_data(const char* data, size_t size)
{
	m_buffer.append(data, size); 

	while (m_state != PARSING_COMPLETED && m_state != PARSING_ERROR)
	{
		ParsingState old_state = m_state; 

		if (m_state == REQUEST_LINE )
			m_state = parseRequestLine();
		else if (m_state == HEADERS)
			m_state = parseHeaders();
		else if (m_state == HEADERS_DONE)
			m_state = determineBodyParsing();
		else if (m_state == READING_BODY)
			m_state = parseBody();
		else if (m_state == READING_CHUNKED_BODY)
			m_state = parseChunkedBody();
		if (m_state == old_state)
		{
			break; // 💡 1번 수정으로 인해 이 break는 이제 데이터가 부족할 때만 발생해야 합니다.
		}
	}
    // ...
	return m_state;
}