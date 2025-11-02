/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_maker.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 09:48:04 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/30 13:18:35 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_MAKER_HPP
#define RESPONSE_MAKER_HPP

#include "response.hpp"

class http_request;
class ServerConfigData;
class LocationData;

enum BuildState
{
	BUILD_PENDING,
	BUILD_STATIC_FILE,
	BUILD_CGI_EXEC,
	BUILD_COMPLETE,
	BUILD_ERROR
};

class response_maker
{
	private:
		BuildState m_state;

    // 헬퍼 함수
    const LocationData* find_matching_location(const http_request& req, const ServerConfigData& config);
    void build_static_file_response(const http_request& req, response& res, const LocationData& loc);
    void build_directory_listing(const http_request& req, response& res, const LocationData& loc);
    void handle_error(int status_code, response& res, const ServerConfigData& config);
    // CGI 로직은 CgiExecutor 클래스를 호출하여 처리될 것입니다.
    
public:
    response_maker();
    response_maker(const response_maker& src);
    response_maker& operator=(const response_maker& src);
    ~response_maker();

    BuildState build_response(const http_request& req, response& res, const ServerConfigData& config);
    bool send_response(int client_fd, response& res);

    // Getter
    BuildState get_state() const;
};

#endif