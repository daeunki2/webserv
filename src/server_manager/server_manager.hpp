/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_manager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:16:11 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/29 14:26:52 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*              store important information from raw request                  */
/*============================================================================*/

#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include <vector>
#include <map>
#include <poll.h>
#include <sys/socket.h>

#include "request_parser.hpp" 
//#include "config_parser.hpp"

class ServerManager
{
	private:
    // std::vector<ServerConfig> m_configs; (on_going)
    std::vector<pollfd> m_fds; //for the socket
    
    // 활성화된 클라이언트 연결 관리 (fd를 키로 사용)
    std::map<int, Client> m_clients; 
    
    // Listen 소켓 FD들을 저장합니다.
    std::vector<int> m_listen_fds;

private:
    void setup_listen_sockets();
    void handle_new_connection(int listen_fd);
    void handle_client_data(int client_fd);
    void close_client_connection(int client_fd);

public:
    ServerManager();
    // Config 파싱 후 초기화하는 생성자가 필요할 수 있음
    
    void run(); // 메인 이벤트 루프
};

#endif