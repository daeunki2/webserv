/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 12:35:59 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/30 12:43:31 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include <vector>
#include <sstream>

class response
{
	private:
	int									m_status_code;
	std::map<std::string, std::string>	m_headers;
	std::vector<char>					m_body;              // 응답 본문 데이터 (바이너리 안전성을 위해 vector<char> 사용)
	std::vector<char>					m_raw_response;      // 최종적으로 소켓에 쓸 헤더 + 본문 데이터
	size_t								m_bytes_sent;        // 논블로킹 전송 시, 현재까지 전송된 바이트 수 추적

	public:
	response();
	response(const response& src);
	response& operator=(const response& src);
	~response();

	// Getter
	int											get_status_code() const;
	const std::map<std::string, std::string>&	get_headers() const;
	const std::vector<char>&					get_body() const;
	const std::vector<char>&					get_raw_response() const;
	size_t										get_bytes_sent() const;
	
	// Setter
	void		set_status_code(int code);
	void		add_header(const std::string& key, const std::string& value);
	void		set_body(const std::vector<char>& body);
	void		append_body(const std::vector<char>& data);
	void		set_bytes_sent(size_t sent_count);

	void 		reset();
	void		clear_raw_response();
	void		finalize_response(); 
};

#endif