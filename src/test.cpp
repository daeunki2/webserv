/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 18:14:04 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/20 10:46:53 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include "config_parser.hpp"
#include "Server_Manager.hpp"
#include "Logger.hpp"
#include "Error.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./webserv <config_file>" << std::endl;
        return 1;
    }

    try
    {
        std::string configFile = argv[1];

        Logger::info("Loading config: " + configFile);

        ConfigParser parser(configFile);
        const std::vector<Server> &servers = parser.getServers();

        Logger::info("Config loaded. Initializing server manager...");

        Server_Manager manager(servers);     // 🔥 서버 설정을 바로 전달
        manager.run();                       // 🔥 poll 루프 시작
    }
    catch (const Error &e)
    {
        Logger::error("Fatal error: " + std::string(e.what()));
        return 1;
    }
    catch (const std::exception &e)
    {
        Logger::error("Unhandled std::exception: " + std::string(e.what()));
        return 1;
    }
    catch (...)
    {
        Logger::error("Unknown fatal error occurred.");
        return 1;
    }

    return 0;
}



// #include<iostream>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <unistd.h>
// #include <cstring>
// #include <cstdio>
// #include <string> // std::string을 사용하기 위해 추가

// int main()
// {
//     int server_fd, client_fd;
//     struct sockaddr_in address;
//     int addrlen = sizeof(address);
//     const int PORT = 8080;
//     int opt = 1;

//     // 1. 소켓 생성 및 REUSEADDR 설정
//     server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd < 0) { perror("socket failed"); return 1; }
//     if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
//         perror("setsockopt");
//         return 1;
//     }

//     // 2. 주소 바인딩
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(PORT);

//     if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) { perror("bind failed"); return 1; }

//     // 3. 리슨
//     if (listen(server_fd, 3) < 0) { perror("listen failed"); return 1; }

//     std::cout << "Server listening on port " << PORT << std::endl;

//     // 4. 클라이언트 연결 처리
//     client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
//     if (client_fd < 0) { perror("accept failed"); close(server_fd); return 1; }
//     std::cout << "Client connected!" << std::endl;


//     // 5. Request 수신 루프 (요청 전체를 받기 위해)
//     char temp_buffer[1024];
//     ssize_t valread;
//     std::string request_buffer;
//     const std::string EOH = "\r\n" "\r\n\r\n"; // End of Headers

//     std::cout << "[INFO] Waiting for full request..." << std::endl;

//     while (true)
//     {
//         valread = recv(client_fd, temp_buffer, sizeof(temp_buffer), 0);
        
//         if (valread <= 0)
// 		{
//             if (valread == 0) std::cout << "[INFO] Client closed connection." << std::endl;
//             else perror("recv failed");
//             break; 
//         }

//         // 수신된 데이터를 전체 버퍼에 추가
//         request_buffer.append(temp_buffer, valread);

//         // 헤더 종료 시그널(\r\n\r\n)이 있는지 확인
//         if (request_buffer.find(EOH) != std::string::npos)
// 		{
//             std::cout << "[INFO] End of Headers found." << std::endl;
//             break;
//         }
        
//         // 간단한 GET 요청은 바디가 없으므로 EOH를 발견하면 break
//         // POST 요청의 경우, Content-Length 등을 확인하여 바디를 더 읽어야 하지만
//         // 지금은 브라우저 GET 요청을 확인하는 것이 목적이므로 EOH까지만 받음
//     }


//     // 6. 요청 출력
//     if (!request_buffer.empty()) {
//         std::cout << "\n----- Actual Browser Request Received -----\n";
//         std::cout << request_buffer; // 전체 요청을 출력
//         std::cout << "\n-----------------------------------------\n";
//     }

//     // 7. 최소 HTTP 응답
//     const char* response =
//         "HTTP/1.1 200 OK\r\n"
//         "Content-Type: text/plain\r\n" // 텍스트로 변경하여 브라우저 출력을 명확하게 함
//         "Content-Length: 5\r\n"
//         "\r\n"
//         "DONE\n";

//     send(client_fd, response, strlen(response), 0);
//     std::cout << "Response sent!" << std::endl;

//     // 8. 소켓 종료
//     close(client_fd);
//     close(server_fd);

//     return 0;
// }


//    http://127.0.0.1:8080

/*
how to get a msg from client.
1. open 2 terminal
2. run server in terminal a
3. connect to a server and send a request by run this cmd in terminal b
for GET "curl -v http://127.0.0.1:8080/index.html"
for POST "curl -v -X POST -d "username=manoi&age=29" http://127.0.0.1:8080/upload"
for HEAD "curl -v -I http://127.0.0.1:8080/"
*/


/*
서버 실행: ./(프로그램 이름)

다른 터미널에서 접속: nc localhost 8080

결과:

당신이 터미널에 GET /test HTTP/1.1을 입력하고 엔터를 치면, 서버는 recv를 호출하고 잠시 대기합니다.

이 상태에서 엔터를 한 번 더 쳐서 \r\n\r\n을 전송하여 요청을 마감하면, recv가 해제되고 서버 콘솔에 당신이 입력한 내용만 정확히 출력됩니다.*/