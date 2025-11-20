/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_parser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:13:45 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/20 10:18:43 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_parser.hpp"
#include "Utils.hpp"
#include "Logger.hpp"

RequestParser::RequestParser()
: _state(START_LINE),
  _buffer(""),
  _req(),
  _hasContentLength(false),
  _contentLength(0),
  _chunked(false)
{}

RequestParser::RequestParser(const RequestParser &o)
{
    *this = o;
}

RequestParser &RequestParser::operator=(const RequestParser &o)
{
    if (this != &o)
    {
        _state           = o._state;
        _buffer          = o._buffer;
        _req             = o._req;
        _hasContentLength= o._hasContentLength;
        _contentLength   = o._contentLength;
        _chunked         = o._chunked;
    }
    return *this;
}

RequestParser::~RequestParser() {}

void RequestParser::reset()
{
    _state = START_LINE;
    _buffer.clear();
    _req   = HttpRequest();
    _hasContentLength = false;
    _contentLength    = 0;
    _chunked          = false;
}

RequestParser::State RequestParser::getState() const
{
    return _state;
}

const HttpRequest &RequestParser::getRequest() const
{
    return _req;
}

std::string RequestParser::toLower(const std::string &s) const
{
    std::string r = s;
    for (size_t i = 0; i < r.size(); ++i)
    {
        if (r[i] >= 'A' && r[i] <= 'Z')
            r[i] = r[i] - 'A' + 'a';
    }
    return r;
}

/* data를 추가로 공급받아 상태에 따라 파싱 */
int RequestParser::feed(const char *data, size_t len)
{
    if (_state == DONE || _state == ERROR_STATE)
        return (_state == DONE ? 1 : -1);

    _buffer.append(data, len);

    bool progress = true;
    while (progress)
    {
        progress = false;

        if (_state == START_LINE)
        {
            if (parseRequestLine())
            {
                _state = HEADERS;
                progress = true;
            }
        }
        else if (_state == HEADERS)
        {
            if (parseHeaders())
            {
                if (_chunked)
                {
                    // 아직 chunked 미지원 → 400으로 처리하기 위해 ERROR_STATE 유지
                    Logger::warn("Chunked request received but not supported yet.");
                    _state = ERROR_STATE;
                    return -1;
                }
                else if (_hasContentLength && _contentLength > 0)
                {
                    _state = BODY;
                    progress = true;
                }
                else
                {
                    _state = DONE;
                    return 1;
                }
            }
        }
        else if (_state == BODY)
        {
            if (parseBody())
            {
                _state = DONE;
                return 1;
            }
        }
    }

    if (_state == ERROR_STATE)
        return -1;
    return 0; // 아직 미완
}

bool RequestParser::parseRequestLine()
{
    std::string::size_type pos = _buffer.find("\r\n");
    if (pos == std::string::npos)
        return false;

    std::string line = _buffer.substr(0, pos);
    _buffer.erase(0, pos + 2);

    if (line.empty())
    {
        _state = ERROR_STATE;
        return false;
    }

    std::vector<std::string> tokens = split(line, ' ');
    if (tokens.size() != 3)
    {
        _state = ERROR_STATE;
        return false;
    }

    const std::string &method  = tokens[0];
    const std::string &uri     = tokens[1];
    const std::string &version = tokens[2];

    if (version != "HTTP/1.1" && version != "HTTP/1.0")
    {
        _state = ERROR_STATE;
        return false;
    }

    _req.setMethod(method);
    _req.setUri(uri);
    _req.setVersion(version);

    // path / query 분리
    std::string::size_type qpos = uri.find('?');
    if (qpos == std::string::npos)
    {
        _req.setPath(uri);
        _req.setQuery("");
    }
    else
    {
        _req.setPath(uri.substr(0, qpos));
        _req.setQuery(uri.substr(qpos + 1));
    }

    return true;
}

bool RequestParser::parseHeaders()
{
    std::string::size_type pos = _buffer.find("\r\n\r\n");
    if (pos == std::string::npos)
        return false;

    std::string headersStr = _buffer.substr(0, pos);
    _buffer.erase(0, pos + 4);

    std::string line;
    std::string::size_type start = 0;
    while (true)
    {
        std::string::size_type end = headersStr.find("\r\n", start);
        if (end == std::string::npos)
            break;
        line = headersStr.substr(start, end - start);
        start = end + 2;

        if (line.empty())
            break;

        std::string::size_type colon = line.find(':');
        if (colon == std::string::npos)
        {
            _state = ERROR_STATE;
            return false;
        }

        std::string key = trim(line.substr(0, colon));
        std::string value = trim(line.substr(colon + 1));

        std::string lowerKey = toLower(key);
        _req.addHeader(lowerKey, value);
    }

    // Host 필수 (간단 체크)
    if (!_req.hasHeader("host"))
    {
        _state = ERROR_STATE;
        return false;
    }

    // keep-alive 여부
    std::string connection = toLower(_req.getHeader("connection"));
    if (connection == "keep-alive")
        _req.setKeepAlive(true);
    else
        _req.setKeepAlive(false);

    // Content-Length
    std::string cl = _req.getHeader("content-length");
    if (!cl.empty())
    {
        if (!isNumber(cl))
        {
            _state = ERROR_STATE;
            return false;
        }
        _hasContentLength = true;
        _contentLength = (size_t)toInt(cl);
    }

    // Transfer-Encoding
    std::string te = toLower(_req.getHeader("transfer-encoding"));
    if (!te.empty())
    {
        if (te == "chunked")
            _chunked = true;
    }

    // 둘 다 동시에 설정되면 에러 (RFC 규칙)
    if (_hasContentLength && _chunked)
    {
        _state = ERROR_STATE;
        return false;
    }

    return true;
}

bool RequestParser::parseBody()
{
    if (!_hasContentLength)
        return true;

    if (_buffer.size() < _contentLength)
        return false;

    std::string body = _buffer.substr(0, _contentLength);
    _buffer.erase(0, _contentLength);
    _req.setBody(body);

    return true;
}
