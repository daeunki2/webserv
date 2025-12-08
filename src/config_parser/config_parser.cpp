/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:13:38 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/05 17:57:31 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_parser.hpp"
#include "Utils.hpp"

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
            _servers.push_back(s);
            return;
        }
        else if (t == "listen")
        {
            next();
            std::string p = next();
            expect(";");
            if (!isNumber(p))
                throw Error("Invalid port: " + p, __FILE__, __LINE__);
            s.setPort(toInt(p));
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
            if (!isNumber(size))
                throw Error("Invalid client_max_body_size", __FILE__, __LINE__);
            
			long long v =toLLong(size);
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
			if (!isNumber(size))
				throw Error("Invalid client_max_body_size", __FILE__, __LINE__);
			long long v = toLLong(size);
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
