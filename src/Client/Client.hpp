/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 12:28:23 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/01 14:51:00 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <ctime>        // time_t
#include <sys/socket.h>

#include "../config_parser/Server.hpp"
#include "http_request.hpp"
#include "request_parser.hpp"
#include "Utils.hpp"

class Client
{
public:
    // 클라이언트의 전체 상태머신
    enum ClientState
    {
        RECVING_REQUEST,     // 요청을 받는 중 (recv + parser.feed)
        REQUEST_COMPLETE,    // 하나의 요청이 완전히 파싱됨
        SENDING_RESPONSE,    // 응답을 보내는 중 (send)
        CONNECTION_CLOSE,    // 연결 종료 예정 / 종료
        ERROR_STATE,          // 에러가 발생한 상태 (에러 응답 생성용)
		ERROR
    };

    // Server_Manager에서 보기 편하도록 파서 결과를 래핑
    enum ParsingState
    {
        PARSING_IN_PROGRESS,
        PARSING_COMPLETED,
        PARSING_ERROR
    };

private:
    int             _fd;                // clien FD
    Server*         _server;            // server settint

    RequestParser   _parser;            // HTTP parser
    ClientState     _state;             // state os client

    std::string     _response_buffer;   // full response "HTTP"
    size_t          _sent_bytes;        // buffer_that i use

    int             _error_code;        // HTTP error code  (0 if is normal)
    bool            _keep_alive;        //

public:
    time_t          last_active_time;

public:
    // ---------------- Canonical Form ----------------
    Client();
    Client(int fd, Server* server);
    Client(const Client& other);
    Client& operator=(const Client& other);
    ~Client();

    // ---------------- 기본 정보 ----------------
    int                 get_fd() const;
    ClientState         get_state() const;
    const http_request& get_request() const;
	int 				get_error_code() const;
	
    const std::string&  get_response_buffer() const;
    size_t              get_response_length() const;
    size_t&             get_sent_bytes();

    // ---------------- 상태 관리 ----------------
    void                reset();
    void                update_state(ClientState new_state);

    // recv로 받은 데이터를 파서에 넘김
    ParsingState        handle_recv_data(const char* data, size_t size);

    // Request → 간단한 응답 생성 (나중에 Response_Builder로 교체 가능)
    void                build_response();
};

#endif
