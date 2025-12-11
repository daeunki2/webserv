/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:13:38 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/11 15:12:58 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_parser.hpp"
#include "Utils.hpp"
#include <cerrno>
#include <climits>
#include <cstdlib>


/*
Server 구조부터 확장하세요. _host/_port 대신 std::vector<ListenTarget> 같은 컨테이너를 두고, ListenTarget은 {std::string host; int port;} 형태로 만듭니다. setHost/setPort를 쓰는 현재 로직을 addListenTarget(host, port)로 갈아타고, 기존 getter들도 필요하다면 첫 번째 항목을 돌려주거나 새 getter(getListens())를 추가합니다.

config_parser.cpp의 parseServerBlock()에서 listen을 만날 때마다 split_listen_target()으로 host·port를 분리한 뒤 server.addListenTarget(host, toInt(portStr));를 호출하세요. 지금처럼 한 번 읽고 덮어쓰지 말고, 지시어가 반복될 때마다 push 하도록 합니다. listen이 단 한 번도 나오지 않았다면 기본값(예: 0.0.0.0 (line 8080))을 넣어 주는 검사도 추가하세요.

Server_Manager::init_sockets()에서는 for each server 안에서 다시 for each listen target을 돌며 소켓을 열어야 합니다. 즉, _servers[i].getListens()를 순회하면서 socket → setsockopt → fcntl(O_NONBLOCK) → bind → listen을 반복적으로 호출하고, 성공한 FD마다 _listening_fds.push_back(fd)와 _fd_to_server[fd] = & _servers[i];를 수행합니다. host 문자열이 비어 있으면 INADDR_ANY, 아니면 기존처럼 getaddrinfo를 사용하세요.

Server_Manager 다른 부분(리스닝 FD 판별 등)은 _listening_fds만 제대로 채워지면 그대로 동작합니다. 마지막으로, 디폴트 설정 파일이나 기존 config.txt에도 listen이 여러 번 나오는 예시를 추가해서 테스트해 보면 정상적으로 여러 포트를 동시에 열 수 있을 거예요.

*/

static long long parse_body_size_value(const std::string &token)
{
    errno = 0;
    char *end = NULL;
    long long value = std::strtoll(token.c_str(), &end, 10);
    if (*token.c_str() == '\0' || end == token.c_str() || *end != '\0' || errno == ERANGE || value < 0)
        throw Error("Invalid client_max_body_size", __FILE__, __LINE__);
    return value;
}

static void split_listen_target(const std::string &token, std::string &hostOut, std::string &portOut)
{
    std::string value = trim(token);
    if (value.empty())
        throw Error("Invalid listen directive", __FILE__, __LINE__);

    if (value[0] == '[')
    {
        size_t closing = value.find(']');
        if (closing == std::string::npos || closing + 1 >= value.size() || value[closing + 1] != ':')
            throw Error("Invalid listen directive", __FILE__, __LINE__);
        hostOut = value.substr(1, closing - 1);
        portOut = value.substr(closing + 2);
    }
    else
    {
        size_t colon = value.find(':');
        if (colon == std::string::npos)
        {
            hostOut.clear();
            portOut = value;
        }
        else
        {
            hostOut = value.substr(0, colon);
            portOut = value.substr(colon + 1);
        }
    }

    hostOut = trim(hostOut);
    portOut = trim(portOut);

    if (portOut.empty())
        throw Error("Invalid listen directive", __FILE__, __LINE__);

    if (hostOut == "*" || hostOut == "0.0.0.0")
        hostOut.clear();
}

ConfigParser::ConfigParser() : _i(0)
{}
ConfigParser::ConfigParser(const std::string &file)
{
	parse(file);
}

ConfigParser::ConfigParser(const ConfigParser &o)
: _lines(o._lines), _tokens(o._tokens), _i(o._i)
{}

ConfigParser &ConfigParser::operator=(const ConfigParser &o)
{
    if (this != &o)
    {
        _lines = o._lines;
        _tokens = o._tokens;
        _i = o._i;
    }
    return *this;
}

ConfigParser::~ConfigParser()
{}

/****************************************
 * PARSE ENTRY
 ****************************************/

void ConfigParser::parse(const std::string &path)
{
    _lines.clear();
    _tokens.clear();
    _servers.clear();
    _i = 0;
	
    std::ifstream file(path.c_str());
    if (!file.is_open())
        throw Error("Cannot open config: " + path, __FILE__, __LINE__);

    Logger::info(Logger::TAG_CONF,"Loading config: " + path);

    std::string line;
    while (std::getline(file, line))
        _lines.push_back(line);

    tokenize();

	_servers.clear();
    
	while (hasNext())
    {
        std::string t = peek();
        if (t == "server")
            parseServerBlock();
        else
            throw Error("Unexpected token: " + t, __FILE__, __LINE__);
    }
}

/****************************************
 * TOKENIZER
 ****************************************/

void ConfigParser::tokenize()
{
    for (size_t i = 0; i < _lines.size(); i++)
    {
        std::string l = trim(_lines[i]);
        if (l.empty() || l[0] == '#')
            continue;

        std::string token;
        for (size_t j = 0; j < l.size(); j++)
        {
            char c = l[j];

            if (c == '{' || c == '}' || c == ';')
            {
                if (!token.empty())
                {
                    _tokens.push_back(token);
                    token.clear();
                }
                _tokens.push_back(std::string(1, c));
            }
            else if (std::isspace(c))
            {
                if (!token.empty())
                {
                    _tokens.push_back(token);
                    token.clear();
                }
            }
            else
                token += c;
        }
        if (!token.empty())
            _tokens.push_back(token);
    }
    _i = 0;
}

bool ConfigParser::hasNext()
{ return _i < _tokens.size(); }

const std::string &ConfigParser::peek()
{
    if (!hasNext())
        throw Error("Unexpected EOF", __FILE__, __LINE__);
    return _tokens[_i];
}

const std::string &ConfigParser::next()
{
    const std::string &t = peek();
    _i++;
    return t;
}

void ConfigParser::expect(const std::string &t)
{
    if (!hasNext() || next() != t)
        throw Error("Expected '" + t + "'", __FILE__, __LINE__);
}

bool ConfigParser::consume(const std::string &t)
{
    if (hasNext() && peek() == t)
    {
        _i++;
        return true;
    }
    return false;
}

/****************************************
 * SERVER BLOCK
 ****************************************/

void ConfigParser::parseServerBlock()
{
    expect("server");
    expect("{");

    Server s;

    while (hasNext())
    {
        const std::string &t = peek();

        if (t == "}")
        {
            expect("}");
			if (s.getListenTargets().empty())
				throw Error("Server block missing listen directive", __FILE__, __LINE__);
            _servers.push_back(s);
            return;
        }
        else if (t == "listen")
        {
            next();
            std::string p = next();
            expect(";");
            std::string host;
            std::string portStr;
            split_listen_target(p, host, portStr);
            if (!isNumber(portStr))
                throw Error("Invalid port: " + portStr, __FILE__, __LINE__);
			int listenPort = toInt(portStr);
			if (s.hasListenTarget(host, listenPort))
				throw Error("Duplicate listen directive for " + (host.empty() ? std::string("*") : host) + ":" + portStr, __FILE__, __LINE__);
            s.addListenTarget(host, listenPort);
        }
        else if (t == "server_name")
        {
            next();
            s.setServerName(next());
            expect(";");
        }
        else if (t == "root")
        {
            next();
            s.setRoot(next());
            expect(";");
        }
        else if (t == "client_max_body_size")
        {
            next();
            std::string size = next();
            expect(";");
			long long v = parse_body_size_value(size);
			if (v == 0)
				v = -1;
			s.setClientMaxBodySize(v);
        }
        else if (t == "error_page")
        {
            next();
            std::string code = next();
            std::string path = next();
            expect(";");
            if (!isNumber(code))
                throw Error("Invalid error code", __FILE__, __LINE__);
            s.addErrorPage(toInt(code), path);
        }
        else if (t == "location")
            parseLocationBlock(s);
        else
            throw Error("Unexpected token inside server: " + t, __FILE__, __LINE__);
    }
}

/****************************************
 * LOCATION BLOCK
 ****************************************/

void ConfigParser::parseLocationBlock(Server &srv)
{
    expect("location");

    std::string path = next();
    Location loc(path);

    expect("{");

    while (true)
    {
        const std::string &t = peek();

        if (t == "}")
        {
            expect("}");
            srv.addLocation(loc);
            return;
        }
        else if (t == "root")
        {
            next();
            loc.setRoot(next());
            expect(";");
        }
        else if (t == "index")
        {
            next();
            loc.setIndex(next());
            expect(";");
        }
        else if (t == "autoindex")
        {
            next();
            std::string v = next();
            expect(";");
            loc.setAutoindex(v == "on");
        }
        else if (t == "upload")
        {
            next();
            loc.setUploadPath(next());
            expect(";");
        }
        else if (t == "allowed_methods")
        {
            next();
            while (hasNext() && peek() != ";")
                loc.addMethod(next());
            expect(";");
        }
        else if (t == "return")
        {
            next();
            std::string code = next();
            std::string url = next();
            expect(";");
            if (!isNumber(code))
                throw Error("Invalid redirect code", __FILE__, __LINE__);
            loc.setRedirect(toInt(code), url);
        }
        else if (t == "cgi")
        {
            next();
            std::string ext = next();
            std::string path = next();
            expect(";");
            loc.setCgi(ext, path);
        }
		else if (t == "client_max_body_size")
		{
			next();
			std::string size = next();
			expect(";");
			long long v = parse_body_size_value(size);
			if (v == 0)
				v = -1;    // -1 = unlimited
			loc.setClientMaxBodySize(v);
		}
        else
		{
            throw Error("Unexpected token inside location: " + t, __FILE__, __LINE__);
		}
    }
}

const std::vector<Server> &ConfigParser::getServers() const
{
    return _servers;
}
