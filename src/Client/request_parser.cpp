/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_parser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:13:45 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/29 17:27:02 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_parser.hpp"
#include <cctype>
#include <cstdlib>

// -----------------------------------------------------------
// utils
// -----------------------------------------------------------


static bool parse_decimal_ll(const std::string& s, long long& out)
{
    if (s.empty()) return false;

    size_t i = 0;
    long long sign = 1;

    if (s[0] == '+') i = 1;
    else if (s[0] == '-') { sign = -1; i = 1; }

    if (i >= s.size()) return false;

    long long v = 0;
    for (; i < s.size(); ++i)
    {
        char c = s[i];
        if (c < '0' || c > '9') return false;
        v = v * 10 + (c - '0');
    }

    out = v * sign;
    return true;
}

// -----------------------------------------------------------
// canonical
// -----------------------------------------------------------

RequestParser::RequestParser()
: _state(REQUEST_LINE),_error_code(0), _buffer(),_request(),_content_to_read(0),_chunk_size(0),_is_chunked(false)
{
	
}

RequestParser::RequestParser(const RequestParser& o)
{
	*this = o;
}

RequestParser& RequestParser::operator=(const RequestParser& o)
{
    if (this != &o)
    {
        _state           = o._state;
		_error_code		 = o._error_code;
        _buffer          = o._buffer;
        _request         = o._request;
        _content_to_read = o._content_to_read;
        _chunk_size      = o._chunk_size;
        _is_chunked      = o._is_chunked;
    }
    return *this;
}

RequestParser::~RequestParser() {}

void RequestParser::reset()
{
    _state = REQUEST_LINE;
	_error_code = 0;
    _buffer.clear();
    _request.reset();
    _content_to_read = 0;
    _chunk_size = 0;
    _is_chunked = false;
}

// -----------------------------------------------------------
// feed()
// -----------------------------------------------------------

RequestParser::ParsingState
RequestParser::feed(const char* data, size_t len)
{
    if (data && len > 0)
        _buffer.append(data, len);

    while (true)
    {
        ParsingState st;

        if (_state == REQUEST_LINE)
        {
            st = parse_request_line();

            if (st != PARSING_IN_PROGRESS)
                return st;

            if (_state == REQUEST_LINE)
                return PARSING_IN_PROGRESS;
        }
        else if (_state == HEADERS)
        {
            st = parse_headers();

            if (st != PARSING_IN_PROGRESS)
                return st;

            if (_state == HEADERS)
                return PARSING_IN_PROGRESS;
        }
        else if (_state == BODY)
        {
            st = parse_body();

            if (st != PARSING_IN_PROGRESS)
                return st;

            if (_state == BODY)
                return PARSING_IN_PROGRESS;
        }
        else if (_state == CHUNK_SIZE)
        {
            st = parse_chunk_size();

            if (st != PARSING_IN_PROGRESS)
                return st;

            if (_state == CHUNK_SIZE)
                return PARSING_IN_PROGRESS;
        }
        else if (_state == CHUNK_DATA)
        {
            st = parse_chunk_data();

            if (st != PARSING_IN_PROGRESS)
                return st;

            if (_state == CHUNK_DATA)
                return PARSING_IN_PROGRESS;
        }
        else if (_state == COMPLETE)
        {
            return PARSING_COMPLETED;
        }
        else if (_state == ERROR)
        {
            return PARSING_ERROR;
        }
        else
        {
            _state = ERROR;
            return PARSING_ERROR;
        }
    }
}


// -----------------------------------------------------------
// extract line
// -----------------------------------------------------------

bool RequestParser::extract_line(std::string& line)
{
    size_t pos = _buffer.find("\r\n");
    if (pos == std::string::npos)
        return false;

    line = _buffer.substr(0, pos);
    _buffer.erase(0, pos + 2);
    return true;
}

int	RequestParser::get_error_code() const
{
	return _error_code;
}


// -----------------------------------------------------------
// request-line
// -----------------------------------------------------------

RequestParser::ParsingState
RequestParser::parse_request_line()
{
    std::string line;
    if (!extract_line(line))
        return PARSING_IN_PROGRESS;

    std::vector<std::string> tok;
    std::string tmp;

    for (size_t i = 0; i < line.size(); ++i)
    {
        if (line[i] == ' ')
        {
            if (!tmp.empty()) { tok.push_back(tmp); tmp.clear(); }
        }
        else
            tmp.push_back(line[i]);
    }
    if (!tmp.empty()) tok.push_back(tmp);

	if (tok.size() != 3)
	{
		_state = ERROR;
		_error_code = 400; // bad request
		return PARSING_ERROR;
	}

    _request.set_method(tok[0]);
    _request.set_uri(tok[1]);
    _request.set_version(tok[2]);

    _state = HEADERS;
    return PARSING_IN_PROGRESS;
}

// -----------------------------------------------------------
// headers
// -----------------------------------------------------------

RequestParser::ParsingState
RequestParser::parse_headers()
{
    while (true)
    {
        std::string line;
        if (!extract_line(line))
            return PARSING_IN_PROGRESS;

        if (line.empty())
        {
            if (_is_chunked)
                _state = CHUNK_SIZE;
            else if (_request.has_content_length())
            {
                _content_to_read = _request.get_content_length();
                if (_content_to_read < 0)
                {
                    _state = ERROR;
					_error_code = 400; // bad_request
                    return PARSING_ERROR;
                }
                _state = BODY;
            }
            else
			{
			//	Logger::info(Logger::TAG_REQ, "HTTP request parsed: " + _request.get_method() + " " + _request.get_uri());
                _state = COMPLETE;
			}
            return PARSING_IN_PROGRESS;
        }

        parse_header_line(line);
        if (_state == ERROR)
            return PARSING_ERROR;
    }
}

// -----------------------------------------------------------
// single header
// -----------------------------------------------------------

void RequestParser::parse_header_line(const std::string& line)
{
    size_t pos = line.find(':');
    if (pos == std::string::npos)
    {
        _state = ERROR;
		_error_code = 400;
        return;
    }

    std::string name  = trim(line.substr(0, pos));
    std::string value = trim(line.substr(pos + 1));

    if (name.empty())
    {
        _state = ERROR;
		_error_code = 400;
        return;
    }

    _request.add_header(name, value);

    std::string lname;
    for (size_t i = 0; i < name.size(); ++i)
        lname.push_back(std::tolower(name[i]));

    if (lname == "content-length")
    {
        long long len = 0;
        if (!parse_decimal_ll(value, len) || len < 0)
        {
            _state = ERROR;
			_error_code = 400;
            return;
        }
        _request.set_content_length(len);
    }
    else if (lname == "transfer-encoding")
    {
        std::string lv;
        for (size_t i = 0; i < value.size(); ++i)
            lv.push_back(std::tolower(value[i]));

        if (lv == "chunked")
        {
            _is_chunked = true;
            _request.set_chunked(true);
        }
    }
    else if (lname == "connection")
    {
        std::string lv;
        for (size_t i = 0; i < value.size(); ++i)
            lv.push_back(std::tolower(value[i]));

        if (lv == "keep-alive")
            _request.set_keep_alive(true);
        else if (lv == "close")
            _request.set_keep_alive(false);
    }
}

// -----------------------------------------------------------
// body (Content-Length)
// -----------------------------------------------------------

RequestParser::ParsingState
RequestParser::parse_body()
{
    if (_content_to_read <= 0)
    {
        _state = COMPLETE;
        return PARSING_IN_PROGRESS;
    }

    if (_buffer.empty())
        return PARSING_IN_PROGRESS;

    if (_buffer.size() < (size_t)_content_to_read)
    {
        _request.append_body(_buffer);
        _content_to_read -= _buffer.size();
        _buffer.clear();
        return PARSING_IN_PROGRESS;
    }

    std::string chunk = _buffer.substr(0, (size_t)_content_to_read);
    _request.append_body(chunk);
    _buffer.erase(0, (size_t)_content_to_read);

    _content_to_read = 0;
    _state = COMPLETE;
//	Logger::info(Logger::TAG_REQ, "HTTP request parsed: "+ _request.get_method()+ " " + _request.get_uri());
    return PARSING_IN_PROGRESS;
}

// -----------------------------------------------------------
// chunk-size
// -----------------------------------------------------------

RequestParser::ParsingState
RequestParser::parse_chunk_size()
{
    std::string line;
    if (!extract_line(line))
        return PARSING_IN_PROGRESS;

    line = trim(line);
    if (line.empty())
    {
        _state = ERROR;
		_error_code = 400;
        return PARSING_ERROR;
    }

    long long sz = 0;
    for (size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];
        sz *= 16;

        if (c >= '0' && c <= '9') sz += (c - '0');
        else if (c >= 'a' && c <= 'f') sz += (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') sz += (c - 'A' + 10);
        else
        {
            _state = ERROR;
			_error_code = 400;
            return PARSING_ERROR;
        }
    }

    _chunk_size = sz;

    if (_chunk_size == 0)
    {
        _state = COMPLETE;
//		Logger::info(Logger::TAG_REQ, "HTTP request parsed (chunked): " + _request.get_method() + " " + _request.get_uri());
        return PARSING_IN_PROGRESS;
    }

    _state = CHUNK_DATA;
    return PARSING_IN_PROGRESS;
}

// -----------------------------------------------------------
// chunk-data
// -----------------------------------------------------------

RequestParser::ParsingState
RequestParser::parse_chunk_data()
{
    if (_buffer.size() < (size_t)(_chunk_size + 2))
        return PARSING_IN_PROGRESS;

    std::string data = _buffer.substr(0, (size_t)_chunk_size);
    _request.append_body(data);

    if (_buffer[_chunk_size] != '\r' || _buffer[_chunk_size + 1] != '\n')
    {
        _state = ERROR;
		_error_code = 400;
        return PARSING_ERROR;
    }

    _buffer.erase(0, (size_t)(_chunk_size + 2));
    _chunk_size = 0;

    _state = CHUNK_SIZE;
    return PARSING_IN_PROGRESS;
}

// -----------------------------------------------------------
// getRequest
// -----------------------------------------------------------

const http_request& RequestParser::getRequest() const
{
    return _request;
}

