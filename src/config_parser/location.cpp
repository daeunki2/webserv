/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:56:12 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/30 14:16:28 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "location.hpp"

// ******************************************************
//              Constructors & Destructor
// ******************************************************
location::location() :
    m_path(""),
    m_root(""),
    m_autoindex(false),
    m_upload_enable(false),
    m_upload_store(""),
    m_cgi_pass(""),
    m_auth_basic("off"),
    m_auth_user_file("")
{
    // 기본 허용 메서드 설정
    m_allowed_methods.push_back("GET");
    m_allowed_methods.push_back("POST");
    m_allowed_methods.push_back("DELETE");
}

location::location(const location& src) :
    m_path(src.m_path),
    m_root(src.m_root),
    m_index_files(src.m_index_files),
    m_allowed_methods(src.m_allowed_methods),
    m_autoindex(src.m_autoindex),
    m_upload_enable(src.m_upload_enable),
    m_upload_store(src.m_upload_store),
    m_cgi_extensions(src.m_cgi_extensions),
    m_cgi_pass(src.m_cgi_pass),
    m_auth_basic(src.m_auth_basic),
    m_auth_user_file(src.m_auth_user_file)
{}

location& location::operator=(const location& src)
{
    if (this != &src) {
        m_path = src.m_path;
        m_root = src.m_root;
        m_index_files = src.m_index_files;
        m_allowed_methods = src.m_allowed_methods;
        m_autoindex = src.m_autoindex;
        m_upload_enable = src.m_upload_enable;
        m_upload_store = src.m_upload_store;
        m_cgi_extensions = src.m_cgi_extensions;
        m_cgi_pass = src.m_cgi_pass;
        m_auth_basic = src.m_auth_basic;
        m_auth_user_file = src.m_auth_user_file;
    }
    return *this;
}

location::~location() {}



// ******************************************************
//                       Setter
// ******************************************************
void location::set_Path(const std::string& p)
{
	m_path = p;
};

void location::set_Root(const std::string& r)
{
	m_root = r;
};

void location::set_Autoindex(bool state)
{
	m_autoindex = state;
};

void location::set_UploadEnable(bool state)
{
	m_upload_enable = state;
};

void location::set_UploadStore(const std::string& path)
{
	m_upload_store = path;
};

void location::set_CgiPass(const std::string& pass)
{
	m_cgi_pass = pass;
};

void location::set_AuthBasic(const std::string& state)
{
	m_auth_basic = state;
};

void location::set_AuthUserFile(const std::string& file)
{
	m_auth_user_file = file;
};


// ******************************************************
//                   Status & add
// ******************************************************
void location::add_AllowedMethod(const std::string& method)
{
	m_allowed_methods.push_back(method);
};

void location::add_CgiExtension(const std::string& ext, const std::string& path)
{
	m_cgi_extensions.insert(std::make_pair(ext, path));
};

void location::addIndexFile(const std::string& f)
{
	m_index_files.push_back(f);
};

bool location::is_AutoindexEnabled() const
{
	return m_autoindex;
};

bool location::is_UploadEnabled() const
{
	return m_upload_enable;
};


// ******************************************************
//                       Getter
// ******************************************************
const std::string& location::get_Path() const
{
	return m_path;
};

const std::string& location::get_Root() const
{
	return m_root;
};

const std::vector<std::string>& location::get_IndexFiles() const
{
	return m_index_files;
};

const std::vector<std::string>& location::get_AllowedMethods() const
{
	return m_allowed_methods;
};

const std::string& location::get_UploadStore() const
{
	return m_upload_store;
};

const std::map<std::string, std::string>& location::get_CgiExtensions() const
{
	return m_cgi_extensions;
};

const std::string& location::get_CgiPass() const
{
	return m_cgi_pass;
};

const std::string& location::get_AuthBasic() const
{
	return m_auth_basic;
};

const std::string& location::get_AuthUserFile() const
{
	return m_auth_user_file;
};
