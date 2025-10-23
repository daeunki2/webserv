/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_request.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 19:03:30 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/23 19:47:07 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*              store important information from raw request                  */
/*============================================================================*/

#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <map>

class http_request
{
	private:
	std::string m_method;//GET, POST, DELETE
	std::string m_path;//index.html
	std::string m_version;// HTTP/1.1
	std::map<std::string, std::string> m_headers; // 
	std::string m_body;

public:
	http_request();
	http_request(const http_request& src);
	http_request& operator=(const http_request& src);
	~http_request();

	// Getter
	const std::string &get_method() const;
	const std::string &get_path() const;
	const std::string &get_version() const;
	const std::map<std::string, std::string> &get_headers() const;
	const std::string &get_body() const;

	// Setter
	void set_method(const std::string &method);
	void set_path(const std::string &path);
	void set_version(const std::string &version);
	void add_header(const std::string &key, const std::string &value);
	void set_body(const std::string &body);

};

#endif