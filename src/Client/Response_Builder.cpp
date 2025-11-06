/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_Builder.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 11:28:29 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/06 12:12:10 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response_Builder.hpp"
#include "Server.hpp" // Server 설정 구조체 포함
#include "http_request.hpp" // http_request 구조체 포함
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <map>
#include <iostream>
#include <cstdlib>
#include <algorithm> // transform 사용

// ******************************************************
//              Constructors & Destructor
// ******************************************************

Response_Builder::Response_Builder(const http_request& request, const Server* config)
: m_request(request), m_config(config)
{}

Response_Builder::~Response_Builder()
{}

// ******************************************************
//              Internal Helper Functions
// ******************************************************

void Response_Builder::build_status_line(int status_code, const std::string& reason_phrase)
{
    std::ostringstream oss;
    oss << "HTTP/1.1 " << status_code << " " << reason_phrase << "\r\n";
    m_response_content += oss.str();
}

void Response_Builder::add_header(const std::string& key, const std::string& value)
{
    m_response_content += key + ": " + value + "\r\n";
}

void Response_Builder::add_body(const std::string& body_content)
{
    m_response_content += "\r\n"; 
    m_response_content += body_content;
}

std::string Response_Builder::get_mime_type(const std::string& path) const
{
    size_t dot_pos = path.rfind('.');
    if (dot_pos == std::string::npos) return "application/octet-stream";
    
    std::string ext = path.substr(dot_pos);
    if (ext == ".html" || ext == ".htm") return "text/html";
    if (ext == ".css") return "text/css";
    if (ext == ".js") return "application/javascript";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".png") return "image/png";
    if (ext == ".gif") return "image/gif";
    if (ext == ".ico") return "image/x-icon";
    return "text/plain";
}

std::string Response_Builder::get_error_page_content(int status_code)
{

    std::string error_uri = m_config->get_error_page_path(status_code); 
    
    if (error_uri.empty())
    {
        std::ostringstream oss;
        oss << "<html><head><title>Error " << status_code << "</title></head>";
        oss << "<body><h1>" << status_code << " Error</h1><p>Webserv Default Error Page</p></body></html>";
        return oss.str();
    }

    std::string full_path = m_config->get_root() + error_uri; 

    std::ifstream ifs(full_path.c_str());
    if (!ifs.is_open())
    {
        return "<html><body><h1>500 Internal Server Error</h1><p>Failed to load custom error page.</p></body></html>";
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    return buffer.str();
}


// ******************************************************
//             Method Handling (핵심 라우팅 로직)
// ******************************************************

std::string Response_Builder::handle_get()
{
    std::string root_path = m_config->get_root(); 
    std::string full_path = root_path + m_request.get_uri();
    
    struct stat file_info;
    if (stat(full_path.c_str(), &file_info) != 0)
        throw std::runtime_error("404"); // Not Found

    if (S_ISREG(file_info.st_mode))
    {
        std::ifstream ifs(full_path.c_str());
        if (!ifs.is_open())
            throw std::runtime_error("403"); // Forbidden
        
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        return buffer.str();
    }
    
    if (S_ISDIR(file_info.st_mode))
    {
        std::string index_path = full_path;
        if (index_path.back() != '/')
            index_path += '/';
        index_path += "index.html";
        
        if (stat(index_path.c_str(), &file_info) == 0 && S_ISREG(file_info.st_mode))
        {
            std::ifstream ifs(index_path.c_str());
            if (!ifs.is_open())
                throw std::runtime_error("403");
            
            std::stringstream buffer;
            buffer << ifs.rdbuf();
            return buffer.str();
        }
        
        if (m_config->is_autoindex_enabled())
        {
            throw std::runtime_error("403"); 
        }
        
        throw std::runtime_error("403"); 
    }

    throw std::runtime_error("403"); 
}

std::string Response_Builder::handle_post()
{
    std::string len_str = m_request.get_header_value("Content-Length");
    if (len_str.empty())
        throw std::runtime_error("411"); 
    size_t content_len = std::atol(len_str.c_str());
    if (content_len > m_config->get_max_body_size())
        throw std::runtime_error("413"); 

    std::string upload_path = m_config->get_upload_path();
    std::string filename = "/post_data_" + std::to_string(time(NULL)) + ".dat";
    
    std::ofstream ofs((upload_path + filename).c_str());
    if (!ofs.is_open())
        throw std::runtime_error("500");
    
    ofs << m_request.get_body();
    
    return "Resource created successfully at " + filename;
}

std::string Response_Builder::handle_delete()
{
    std::string root_path = m_config->get_root(); 
    std::string full_path = root_path + m_request.get_uri();
    
    if (std::remove(full_path.c_str()) != 0)
    {
        if (errno == ENOENT) 
            throw std::runtime_error("404");
        else if (errno == EACCES) 
            throw std::runtime_error("403");
        else
            throw std::runtime_error("500"); 
    }
    
    throw std::runtime_error("204"); 
}


// ******************************************************
//             Main Control Function
// ******************************************************

const std::string& Response_Builder::build_response()
{
    std::string body_content;
    std::string content_type = "text/plain";
    int status_code = 200;
    std::string reason_phrase = "OK";
    bool connection_close = true;

    try
    {
        // 1. 메서드 분기 및 처리
        if (m_request.get_method() == "GET") {
            body_content = handle_get();
            content_type = get_mime_type(m_request.get_uri());
        } else if (m_request.get_method() == "POST") {
            body_content = handle_post();
            status_code = 201; reason_phrase = "Created";
            content_type = "text/plain";
        } else if (m_request.get_method() == "DELETE") {
            handle_delete(); 
        } else {
            throw std::runtime_error("405"); // Method Not Allowed
        }
        
        std::string conn_header = m_request.get_header_value("Connection");
        std::transform(conn_header.begin(), conn_header.end(), conn_header.begin(), ::tolower);
        
        if (conn_header == "keep-alive")
            connection_close = false;

    }
    catch (const std::runtime_error& e)
    {
        std::string err_code_str = e.what();
        status_code = std::atoi(err_code_str.c_str());

        if (status_code == 204)
        {
            reason_phrase = "No Content";
            body_content = "";
        }
        else 
        {
            reason_phrase = "Error"; 
            body_content = get_error_page_content(status_code);
            content_type = get_mime_type(".html"); 
            connection_close = true; 
        }
    }
    
    m_response_content.clear();
    build_status_line(status_code, reason_phrase);
    
    if (status_code != 204) 
    {
        add_header("Content-Type", content_type);
        add_header("Content-Length", std::to_string(body_content.length()));
    }
    
    if (connection_close)
        add_header("Connection", "close"); 
    else
        add_header("Connection", "keep-alive"); 

    add_header("Server", "Webserv/1.0");

    if (status_code != 204)
        add_body(body_content);

    return m_response_content;
}